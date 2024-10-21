/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:34 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/03 11:50:19 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Host::Host() { return ;}

Host::Host(ServerConf & src): ServerConf(src), _nfds(0) {

	struct epoll_event	event;
	std::ostringstream	oss;

	// Create listening socket
	if ((_fdSetSock = socket(_address.sin_family, SOCK_STREAM, 0)) <= 0) {
		oss << "Error in the socket creation for server " << _nbServer;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Set socket options for reusing the address
	int	opt = 1;
	if (setsockopt(_fdSetSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the setsockopt for server " << _nbServer << " and socket " << _fdSetSock;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Set socket details (bind)
	if (bind(_fdSetSock, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the bind for server " << _nbServer << "with port " << _port << " and socket " << _fdSetSock;
			throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Listen on the socket
	if (listen(_fdSetSock, MAX_CONNECTIONS) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the listen", ERR_CODE_INTERNAL_ERROR);
	}
	
	// Make the socket non-blocking
	if (fcntl(_fdSetSock, F_SETFL, O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the listen for server " << _nbServer << " and socket " <<_fdSetSock;
			throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Create epoll instance
	_fdEpoll = epoll_create(1);
	if (_fdEpoll < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the epoll_create for server " << _nbServer << " and socket " <<_fdSetSock;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	event.events = EPOLLIN;
	event.data.fd = _fdSetSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdSetSock, &event) < 0)
		throw ErrorFdManipulation("Error in epoll_ctl for listening socket: " + static_cast<std::string>(strerror(errno)), ERR_CODE_INTERNAL_ERROR);

	// Resize the events vector
	_events.resize(MAX_CONNECTIONS);
}
Host::~Host() { return ; }

void	Host::json_update(void) {
	std::ostringstream	oss;
	std::string			json;

	oss << "[";
	for (size_t i = 0; i < _files.size(); ++i) {
		oss << "\"" << _files[i] << "\"";
		if (i < _files.size() - 1)
			oss << ",";
	}
	oss << "]";
	json = oss.str();

	std::ofstream file((_rootPath + "/uploads/files.json").c_str());
	if (!file.is_open())
		throw ErrorFdManipulation("Error in the opening of the file", ERR_CODE_INTERNAL_ERROR);
	file << json;
	file.close();
}

void	Host::new_connection(void) {
	int					res;
	std::ostringstream	oss;
	std::cout << YELLOW BOLD "New connection detected" RESET << std::endl;
	
	// Accept the connection
	res = accept(_fdSetSock, (struct sockaddr *)&_address, (socklen_t*)&_address_len);
	if (res < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the accept for server " << _nbServer << " and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	_fdAcceptSock.push_back(res);

	// Make the connection non-blocking
	if (fcntl(_fdAcceptSock.back(), F_SETFL, O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		ft_close(_fdAcceptSock.back());
		oss << "Error in the fcntl for server " << _nbServer << " and socket " << _fdAcceptSock.back() << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Add new connection to epoll if possible
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _fdAcceptSock.back();
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdAcceptSock.back(), &event) < 0)
		throw ErrorFdManipulation("Error in the epoll_ctl: " + static_cast<std::string>(strerror(errno)), ERR_CODE_INTERNAL_ERROR);

	// Manage number of kept connections
	_keep_alive_time = time(NULL);
	_nb_keepalive++;
}

void	Host::parse_request(int i) {
	char								buffer[BUFFER_SIZE] = { 0 };
	int									valread, fd = _events[i].data.fd;
	std::map<int, Request>::iterator	it = _requests.find(fd);

	// Read data from client
	try {
		valread = read(fd, buffer, BUFFER_SIZE - 1);
		if (valread < 0)
			throw ErrorFdManipulation("Error in the read", ERR_CODE_INTERNAL_ERROR);
	} catch (const ErrorFdManipulation & e) {
		return send_error_page(*this, i, e, NULL);
	}

	if (it != _requests.end())
		it->second.append(buffer, valread);
	else {
		Request tmp(*this, _events[i], static_cast<std::string>(buffer));
		_requests.insert(std::pair<int, Request>(fd, tmp));
		it = _requests.find(fd);
	}

	// Parse the partial read of the request
	try {
		it->second._eof = recv(fd, buffer, 2, MSG_PEEK);
		it->second.parse();
	} catch (const ErrorRequest & e) {
		send_error_page(*this, i, e, NULL);
	}
}

void	Host::act_on_request(int i) {
	bool								done = false;
	int									fd = _events[i].data.fd;
	std::map<int, Response>::iterator	it_resp = _responses.find(fd);
	std::map<int, Request>::iterator	it_req = _requests.find(fd);

	// Build the response if it doesn't exist
	if (it_resp == _responses.end()) {
		Response tmp(it_req->second, *this);
		_responses.insert(std::pair<int, Response>(fd, tmp));
		it_resp = _responses.find(fd);
	}
	try {
		if (!it_resp->second._response_ready) {
			if (it_resp->second._request_line["method"] == "GET")
				it_resp->second.buildGet();
			else if (it_resp->second._request_line["method"] == "POST") {
				it_resp->second.buildPost();
				_files.push_back(it_resp->second._filename);
			}
			else if (it_resp->second._request_line["method"] == "DELETE") {
				it_resp->second.buildDelete();
				if (it_resp->second._filename != "is a directory")
					_files.erase(std::remove(_files.begin(), _files.end(), it_resp->second._filename), _files.end());
			}
			else
				throw ErrorResponse("Error in the request: method not implemented", ERR_CODE_MET_NOT_ALLOWED);
		}

		// Send the response
		if (it_resp->second._response_ready) {
			it_resp->second.send_response(fd, &done);
			if (!done)
				return ;
		}
		
		// Update files list JSON
		json_update();

		// Close the connection if needed
		if (!it_req->second._headers["Connection"].compare("close") || _nb_keepalive >= _max_keepalive
			|| (difftime(time(NULL), _keep_alive_time) > KEEP_ALIVE && !it_req->second._headers["Connection"].compare("keep-alive"))) {
			epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, fd, NULL);
			ft_close(fd);
			std::cout << "Closing connection" << std::endl;
			_nb_keepalive--;
		} else {
			if (it_req->second._headers["Connection"].compare("keep-alive"))
				_keep_alive_time = time(NULL);
			_events[i].events = EPOLLIN;
			epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, fd, &_events[i]);
			std::cout << "Connection kept alive" << std::endl;
		}
		_requests.erase(fd);
		_responses.erase(fd);

	} catch (const ErrorResponse & e) {
		send_error_page(*this, i, e, &_nb_keepalive);
	}
	
	std::cout << YELLOW "---> Request answered" RESET << std::endl << std::endl;
}

void	Host::run_server(void) {
	std::map<int, Request>::iterator	it;

	// Wait for events
	_nfds = epoll_wait(_fdEpoll, _events.data(), _events.size(), 0);
	if (_nfds < 0) {
		std::ostringstream oss;
		oss << "Error in the epoll_wait for server " << _nbServer << " and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Handle events
	for (int i = 0; i < _nfds; ++i) {
		if (_events[i].events == EPOLLIN) {
			// New connection
			if (_events[i].data.fd == _fdSetSock)
				try {
					new_connection();
				} catch (std::exception & e) {
					ft_perror(e.what());
				}

			// Existing connection
			else {
				parse_request(i);
				// Change the event to EPOLLOUT
				it = _requests.find(_events[i].data.fd);
				if (it != _requests.end() && it->second._stage == BODY_DONE) {
					std::cout << YELLOW "New request cought" RESET << std::endl;
					print_request(it->second._request_line, it->second._headers, it->second._body);
					std::cout << std::endl;
					_events[i].events = EPOLLOUT;
					epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, _events[i].data.fd, &_events[i]);
				}
			}
		} else if (_requests.find(_events[i].data.fd) != _requests.end() && _events[i].events == EPOLLOUT)
			act_on_request(i);
		else
			std::cout << "Unknown event detected" << std::endl;
	}
	std::fill(_events.begin(), _events.end(), epoll_event());
}

void	Host::close_everything(void) {
	for (size_t i = 0; i < _fdAcceptSock.size(); i++)
		ft_close(_fdAcceptSock[i]);
	ft_close(_fdSetSock);
	ft_close(_fdEpoll);
	for (size_t i = 0; i < _events.size(); i++)
		ft_close(_events[i].data.fd);
}
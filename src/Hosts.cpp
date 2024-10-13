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

	// printVector(src._IndexFile);
	// Create listening socket
	if ((_fdSetSock = socket(_address.sin_family, SOCK_STREAM, 0)) <= 0) {
		oss << "Error in the socket creation for server " << _nbServer;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Set socket options for reusing the address
	int	opt = 1;
	if (setsockopt(_fdSetSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the setsockopt for server " << _nbServer << "and socket " << _fdSetSock;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Set socket details (bind)
	if (bind(_fdSetSock, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the bind for server " << _nbServer << "with port " << _port << "and socket " << _fdSetSock;
			throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Listen on the socket
	if (listen(_fdSetSock, MAX_CONNECTIONS) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the listen", ERR_CODE_INTERNAL_ERROR);
	}
	
	// Make the socket non-blocking
	if (fcntl(_fdSetSock, F_SETFL, fcntl(_fdSetSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the listen for server " << _nbServer << "and socket " <<_fdSetSock;
			throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Create epoll instance
	_fdEpoll = epoll_create(1);
	if (_fdEpoll < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the epoll_create for server " << _nbServer << "and socket " <<_fdSetSock;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	event.events = EPOLLIN;
	event.data.fd = _fdSetSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdSetSock, &event) < 0)
		throw ErrorFdManipulation("Error in epoll_ctl for listening socket: " + static_cast<std::string>(strerror(errno)), ERR_CODE_INTERNAL_ERROR);

	// il faut resizer 
	_events.resize(MAX_CONNECTIONS);
}
Host::~Host() { return ; }

void	Host::new_connection(void) {
	std::ostringstream oss;
	std::cout << YELLOW "New connection detected" RESET << std::endl;
	
	// Accept the connection
	_fdAcceptSock = accept(_fdSetSock, (struct sockaddr *)&_address, (socklen_t*)&_address_len);
	if (_fdAcceptSock < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the accept for server " << _nbServer << "and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Make the connection non-blocking
	if (fcntl(_fdAcceptSock, F_SETFL, fcntl(_fdAcceptSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		ft_close(_fdAcceptSock);
		oss << "Error in the fcntl for server " << _nbServer << "and socket " << _fdAcceptSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Add new connection to epoll
	struct epoll_event new_event;
	new_event.events = EPOLLIN;
	new_event.data.fd = _fdAcceptSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdAcceptSock, &new_event) < 0)
		throw ErrorFdManipulation("Error in the epoll_ctl: " + static_cast<std::string>(strerror(errno)), ERR_CODE_INTERNAL_ERROR);
}

void	Host::parse_request(int fd) {
	char		buffer[BUFFER_SIZE] = { 0 };
	int			valread;
	std::string	raw;

	// Read data from client
	try {
		valread = read(_events[fd].data.fd, buffer, BUFFER_SIZE - 1);
		buffer[valread] = '\0';
		if (valread < 0) {
			ft_close(_events[fd].data.fd);
			epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, _events[fd].data.fd, NULL);
			throw ErrorFdManipulation("Error in the read", ERR_CODE_INTERNAL_ERROR);
		}
	} catch (const ErrorFdManipulation & e) {
		ft_perror(e.what());
		send_error_page(*this, fd, e._code);
		return ;
	}

	// Append the data to the request
	raw = buffer;
	if (_requests.find(_events[fd].data.fd) != _requests.end())
		_requests[_events[fd].data.fd].append(raw);
	else {
		Request tmp(*this, _events[fd], raw);
		_requests.insert(std::pair<int, Request>(_events[fd].data.fd, tmp));
	}

	// Parse the partial read of the request
	try {
		_requests[_events[fd].data.fd]._eof = recv(_events[fd].data.fd, buffer, 2, MSG_PEEK);
		_requests[_events[fd].data.fd].parse();
	} catch (const ErrorRequest & e) {
		ft_perror(e.what());
		send_error_page(*this, fd, e._code);
	}
}

void	Host::act_on_request(int fd) {

	std::cout << YELLOW "Acting on request" RESET << std::endl;

	// Send data to client
	Response response(_requests[_events[fd].data.fd], *this);
	if (response._request_line["method"] == "GET")
		response.BuildGet(_events[fd].data.fd, _events[fd]);
	else {
		send_error_page(*this, fd, ERR_CODE_MET_NOT_ALLOWED);
		return ;
	}
	
	// Close the connection if needed
	if (!_requests[_events[fd].data.fd]._headers["Connection"].compare("close")) {
		std::cout << "Closing connection" << std::endl;
		epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, _events[fd].data.fd, NULL);
		ft_close(_events[fd].data.fd);
		_requests.erase(_events[fd].data.fd);
	} else {
		std::cout << "Connection kept alive" << std::endl;
		_events[fd].events = EPOLLIN;
		epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, _events[fd].data.fd, &_events[fd]);
	}

	std::cout << YELLOW "---> Request answered" RESET << std::endl << std::endl;
}

void	Host::run_server(void) {

	// Wait for events
	_nfds = epoll_wait(_fdEpoll, this->_events.data(), MAX_CONNECTIONS, 0);
	if (_nfds < 0) {
		std::ostringstream oss;
		oss << "Error in the epoll_wait for server " << _nbServer << "and socket " << _fdSetSock << ": " << strerror(errno);
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
				if (_requests[_events[i].data.fd]._stage == BODY_DONE) {
					std::cout << YELLOW "New request cought" RESET << std::endl;
					print_request(_requests[_events[i].data.fd]._request_line, _requests[_events[i].data.fd]._headers, _requests[_events[i].data.fd]._body);
					std::cout << std::endl;
					_events[i].events = EPOLLOUT;
					epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, _events[i].data.fd, &_events[i]);
				}
			}
		}
		else if (_events[i].events == EPOLLOUT)
			act_on_request(i);
		// else
			// std::cout << "Unknown event detected" << std::endl;
	}
}

void	Host::close_everything(void) {
	ft_close(_fdSetSock);
	ft_close(_fdEpoll);
	for (size_t i = 0; i < _events.size(); i++)
		ft_close(_events[i].data.fd);
}
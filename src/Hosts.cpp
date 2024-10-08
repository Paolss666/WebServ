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

Host::Host(ServerConf & src): ServerConf(src) {
	struct epoll_event	event;
	std::ostringstream	oss;

	// Initialize variables
	_nfds = 0;
	_b_completed = false;
	_b_partial = false;

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
	char	buffer[BUFFER_SIZE] = { 0 };
	int		valread;

	try {

		// Read data from client
		valread = read(_events[fd].data.fd, buffer, BUFFER_SIZE - 1);
		buffer[valread] = '\0';
		if (valread < 0) {
			ft_close(_events[fd].data.fd);
			epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, _events[fd].data.fd, NULL);
			throw ErrorFdManipulation("Error in the read", ERR_CODE_INTERNAL_ERROR);
		}

		// Parse the request
		std::string raw(buffer);
		if (_requests.find(_events[fd].data.fd) != _requests.end())
			_requests[_events[fd].data.fd].append(raw);
		else {
			Request tmp(*this, _events[fd], raw);
			_requests.insert(std::pair<int, Request>(_events[fd].data.fd, tmp));
		}
		if (valread == 0 || recv(_events[fd].data.fd, buffer, BUFFER_SIZE - 1, MSG_PEEK) <= 0) {
			try {
				_requests[_events[fd].data.fd].parse();
			} catch (const ErrorRequest & e) {
				ft_perror(e.what());
				send_error_page(*this, fd, e._code);
			}
			_b_completed = true;
		}
	} catch (const ErrorFdManipulation & e) {
		ft_perror(e.what());
		send_error_page(*this, fd, e._code);
	}
}

void	Host::send_response(int fd) {
	send(_events[fd].data.fd, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!\n", 53, 0);
}

void	Host::act_on_request(int fd) {

	std::cout << CYAN "Acting on request" RESET << std::endl;

	// Send data to client
	Response response(_requests[_events[fd].data.fd]);
	send_response(fd);
	
	// Close the connection if needed
	if (_requests[_events[fd].data.fd]._headers["Connection"].compare("close")) {
		epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, _events[fd].data.fd, NULL);
		ft_close(_events[fd].data.fd);
		_requests.erase(_events[fd].data.fd);
	}
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
				if (_b_completed) {
					_events[i].events = EPOLLOUT;
					epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, _events[i].data.fd, &_events[i]);
					_b_completed = false;
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
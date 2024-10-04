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

#include "../incl/webserv.hpp"

Host::Host() { 
	std::cout << "I START TO CONSTRUCT jhfjkasdlfadjskfjdsh" << std::endl;
 }
Host::Host(ServerConf &src): ServerConf(src) {
	struct epoll_event event; 

	std::cout << "I START TO CONSTRUCT : " << _nbServer << std::endl;

	// Create listening socket
	if ((_fdSetSock = socket(_address.sin_family, SOCK_STREAM, 0)) <= 0) {
		throw ErrorFdManipulation("Error in the socket creation");
	}
	
	// Set socket options for reusing the address
	int	opt = 1;
	if (setsockopt(_fdSetSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the setsockopt");
	}
	
	// Set socket details (bind)
	if (bind(_fdSetSock, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the bind");
	}
	
	// Listen on the socket
	if (listen(_fdSetSock, MAX_CONNECTIONS) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the listen");
	}
	
	// Make the socket non-blocking
	if (fcntl(_fdSetSock, F_SETFL, fcntl(_fdSetSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the fcntl");
	}
	
	// Create epoll instance
	// std::cout << "_fdsocket -> " << _fdSetSock << std::endl;
	_fdEpoll = epoll_create(1);
	// std::cout << "pfEpoll --> " << _fdEpoll << std::endl;
	if (_fdEpoll < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the epoll_create");
	}
	event.events = EPOLLIN;
	event.data.fd = _fdSetSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdSetSock, &event) < 0)
		throw ErrorFdManipulation("Error in epoll_ctl for listening socket: " + static_cast<std::string>(strerror(errno)));

	// il faut resizer 
	events.resize(MAX_CONNECTIONS);
}
Host::~Host() {
	std::cout << "DESTRUC host\n";
    // for (size_t i = 0; i < hosts.size(); i++) {
		// ft_close(_fdSetSock);
		// ft_close(_fdEpoll);
    return ;
}

void	Host::new_connection(void) {
	std::ostringstream oss;
	std::cout << "New connection detected" << std::endl;
	
	// Accept the connection
	_fdAcceptSock = accept(_fdSetSock, (struct sockaddr *)&_address, (socklen_t*)&_address_len);
	if (_fdAcceptSock < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the accept for server " << _nbServer << "and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str());
	}

	// Make the connection non-blocking
	if (fcntl(_fdAcceptSock, F_SETFL, fcntl(_fdAcceptSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		ft_close(_fdAcceptSock);
		oss << "Error in the fcntl for server " << _nbServer << "and socket " << _fdAcceptSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str());
	}

	// Add new connection to epoll
	struct epoll_event new_event;
	new_event.events = EPOLLIN;
	new_event.data.fd = _fdAcceptSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdAcceptSock, &new_event) < 0)
		throw ErrorFdManipulation("Error in the epoll_ctl: " + static_cast<std::string>(strerror(errno)));
}

void	Host::parse_request(int fd) {
	char	buffer[BUFFER_SIZE] = { 0 };
	int		valread;

	std::cout << "Message received" << std::endl;

	// Read data from client
	valread = read(events[fd].data.fd, buffer, BUFFER_SIZE);
	std::cout << "valread: " << valread << std::endl;
	
	// Error in the read
	if (valread < 0) {
		ft_close(events[fd].data.fd);
		epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, events[fd].data.fd, NULL);
		throw ErrorFdManipulation("Error in the read");
	}

	// Parse the request
	else {
		std::string raw(buffer);
		if (requests.find(events[fd].data.fd) != requests.end())
			requests[events[fd].data.fd].append(raw);
		else {
			Request tmp(raw);
			requests.insert(std::pair<int, Request>(events[fd].data.fd, tmp));
		}
	}
	if (valread == 0 || valread < BUFFER_SIZE) {
		try {
			requests[events[fd].data.fd].parse();
		} catch (std::exception & e) {
			ft_perror(e.what());
		}
	}
}


void	Host::build_response(int fd) {
	std::ifstream	file("../www/index.html");

	// if (!file.good())
	// 	throw
	send(events[fd].data.fd, "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!\n", 53, 0);
	
	// Line under to be removed
	if (requests.find(events[fd].data.fd) == requests.end())
		return ;
}

void	Host::act_on_request(int fd) {

	// Change the event to EPOLLOUT
	events[fd].events = EPOLLOUT;
	epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, events[fd].data.fd, &events[fd]);

	// Send data to client
	build_response(fd);
	
	// Close the connection if needed
	epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, events[fd].data.fd, NULL);
	ft_close(events[fd].data.fd);
	requests.erase(events[fd].data.fd);
}

void	Host::run_server(void) {

	// Wait for events
	// std::cout << "FDEpoll->" << _fdEpoll<< std::endl;
	nfds = epoll_wait(_fdEpoll, this->events.data(), MAX_CONNECTIONS, EPOLL_TIMEOUT);
	if (nfds < 0) {
		std::ostringstream oss;
		oss << "Error in the epoll_wait for server " << _nbServer << "and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str());
	}

	// Handle events
	for (int j = 0; j < nfds; ++j) {

		if (events[j].events == EPOLLIN) {
			// New connection
			if (events[j].data.fd == _fdSetSock)
				new_connection();
			
			// Existing connection
			else {
				parse_request(j);
				act_on_request(j);
			}

		} else if (events[j].events == EPOLLOUT) {
			std::cout << "EPOLLOUT event detected" << std::endl;
		} else {
			std::cout << "Unknown event detected" << std::endl;
		}		
	}
}

void	Host::close_everything(void) {
	ft_close(_fdSetSock);
	ft_close(_fdEpoll);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:34 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/03 11:50:19 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/webserv.hpp"

Hosts::Hosts() { return ; }

void Hosts::ft_conf_init(const char *fileName) {
    std::string	line;
    int     i = 0;

    Check_extension(fileName, ".conf");
    std::ifstream file(fileName);
    if (!file.is_open())
        throw ErrorConfFile("Can't open the file");
	
    while (std::getline(file, line)) {
        if (line == "server {") {
            ServerConf virtualServer;	
            std::cout << " ======> "<< line << "<============================== \n";
             virtualServer.initWServer(file);
            std::cout << "FILE DONE\n";
			hosts.push_back(virtualServer);
            virtualServer.setNbServer(i);
            std::cout << "nbserver " << i << "\n";
			i++;
		}
        else if (line.empty())
            continue;
        else
			throw ErrorConfFile("Error in the config file");
    }
	events.resize(MAX_CONNECTIONS);
}

void	ft_server_init(std::vector<ServerConf> & hosts, std::vector<epoll_event> & events) {
	// Initiate string buffer
	
	for (size_t i = 0; i < hosts.size(); ++i) {
		std::ostringstream oss;
		
		// Create listening socket
		if ((hosts[i].fdSetSock = socket(hosts[i]._address.sin_family, SOCK_STREAM, 0)) <= 0) {
			oss << "Error in the socket creation for server " << i;
			throw ErrorFdManipulation(oss.str());
		}
		
		// Set socket options for reusing the address
		int	opt = 1;
		if (setsockopt(hosts[i].fdSetSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0) {
			ft_close(hosts[i].fdSetSock);
			oss << "Error in the setsockopt for server " << i << "and socket " << hosts[i].fdSetSock;
			throw ErrorFdManipulation(oss.str());
		}
		
		// Set socket details (bind)
		if (bind(hosts[i].fdSetSock, (struct sockaddr *)&hosts[i]._address, sizeof(hosts[i]._address)) < 0) {
			ft_close(hosts[i].fdSetSock);
			oss << "Error in the bind for server " << i << "with port " << hosts[i]._port << "and socket " << hosts[i].fdSetSock;
			throw ErrorFdManipulation(oss.str());
		}
		
		// Listen on the socket
		if (listen(hosts[i].fdSetSock, MAX_CONNECTIONS) < 0) {
			ft_close(hosts[i].fdSetSock);
			oss << "Error in the listen for server " << i << "and socket " << hosts[i].fdSetSock;
			throw ErrorFdManipulation(oss.str());
		}
		
		// Make the socket non-blocking
		if (fcntl(hosts[i].fdSetSock, F_SETFL, fcntl(hosts[i].fdSetSock, F_GETFL) | O_NONBLOCK) < 0) {
			ft_close(hosts[i].fdSetSock);
			oss << "Error in the fcntl for server " << i << "and socket " << hosts[i].fdSetSock;
			throw ErrorFdManipulation(oss.str());
		}
		
		// Create epoll instance
		if ((hosts[i].fdEpoll = epoll_create(1)) < 0) {
			ft_close(hosts[i].fdSetSock);
			oss << "Error in the epoll_create for server " << i << "and socket " << hosts[i].fdSetSock;
			throw ErrorFdManipulation(oss.str());
		}
        events[i].events = EPOLLIN;
    	events[i].data.fd = hosts[i].fdSetSock;
		if (epoll_ctl(hosts[i].fdEpoll, EPOLL_CTL_ADD, hosts[i].fdSetSock, &events[i]) < 0)
			throw ErrorFdManipulation("Error in epoll_ctl for listening socket: " + static_cast<std::string>(strerror(errno)));
	}
}

void	ft_new_connection(ServerConf & host, std::vector<epoll_event> & events , int nb_host) {
	std::ostringstream oss;
	std::cout << "New connection detected" << std::endl;
	
	// Accept the connection
	host.fdAcceptSock = accept(host.fdSetSock, (struct sockaddr *)&host._address, (socklen_t*)&host.address_len);
	if (host.fdAcceptSock < 0) {
		ft_close(host.fdSetSock);
		oss << "Error in the accept for server " << nb_host << "and socket " << host.fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str());
	}

	// Make the connection non-blocking
	if (fcntl(host.fdAcceptSock, F_SETFL, fcntl(host.fdAcceptSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(host.fdSetSock);
		ft_close(host.fdAcceptSock);
		oss << "Error in the fcntl for server " << nb_host << "and socket " << host.fdAcceptSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str());
	}

	// Add new connection to epoll
	struct epoll_event new_event;
	new_event.events = EPOLLIN;
	new_event.data.fd = host.fdAcceptSock;
	if (epoll_ctl(host.fdEpoll, EPOLL_CTL_ADD, host.fdAcceptSock, &new_event) < 0)
		throw ErrorFdManipulation("Error in the epoll_ctl: " + static_cast<std::string>(strerror(errno)));
	events.push_back(new_event);
}

void	ft_parse_request(ServerConf & host, epoll_event & event) {
	char	buffer[BUFFER_SIZE] = { 0 };
	int		valread;

	std::cout << "Message received" << std::endl;

	// Read data from client
	valread = read(event.data.fd, buffer, BUFFER_SIZE);
	std::cout << "valread: " << valread << std::endl;
	
	// Error in the read
	if (valread < 0) {
		ft_close(event.data.fd);
		epoll_ctl(host.fdEpoll, EPOLL_CTL_DEL, event.data.fd, NULL);
		throw ErrorFdManipulation("Error in the read");
	}

	// Parse the request
	else {
		std::string raw(buffer);
		if (host.requests.find(event.data.fd) != host.requests.end())
			host.requests[event.data.fd].append(raw);
		else {
			Request tmp(raw);
			host.requests.insert(std::pair<int, Request>(event.data.fd, tmp));
		}
	}
	if (valread == 0 || valread <= BUFFER_SIZE) {
		try {
			host.requests[event.data.fd].parse();
		} catch (std::exception & e) {
			ft_perror(e.what());
		}
	}
}

void	ft_act_on_request(ServerConf & host, epoll_event & event) {
	// Send data to client & ft_close connection
	send(event.data.fd, "Hello, client!", 14, 0);
	if (host.requests.find(event.data.fd) != host.requests.end())
		std::cout << std::endl;
}

void	Hosts::loopServer(void) {
	int		nfds;

	// Initiate the servers
	ft_server_init(this->hosts, this->events);

	// Print the details of the hosts and epoll watchlist
	std::cout << "Detail of hosts and epoll watchlist:" << std::endl;
	for (size_t i = 0; i < hosts.size(); ++i)
		std::cout << CYAN << std::setw(8) << "Server " << WHITE << std::setw(2) << i 
			<< CYAN << " | Port: " << WHITE << std::setw(1) << hosts[i]._port 
			<< CYAN << " | IP: " << WHITE << std::setw(9) << hosts[i]._ip 
			<< CYAN << " | Name: " << WHITE << std::setw(10) << hosts[i]._name 
			<< CYAN << " | Fd: " << WHITE << std::setw(1) << this->events[i].data.fd 
			<< CYAN << " | Epoll_fd: " << WHITE << std::setw(1) << hosts[i].fdEpoll 
			<< CYAN << " | Events: " << WHITE << std::setw(2) << this->events[i].events 
			<< RESET << std::endl;

	std::cout << CYAN BOLD "\n----------------- Server is running -----------------\n" RESET << std::endl;
	while (!g_sig) {
		for (size_t i = 0; i < hosts.size(); ++i) {

			// Wait for events
			nfds = epoll_wait(hosts[i].fdEpoll, this->events.data(), MAX_CONNECTIONS / hosts.size(), EPOLL_TIMEOUT);
			if (nfds < 0) {
				std::ostringstream oss;
				oss << "Error in the epoll_wait for server " << i << "and socket " << hosts[i].fdSetSock << ": " << strerror(errno);
				throw ErrorFdManipulation(oss.str());
			}

			// Handle events
			for (int j = 0; j < nfds; ++j) {

				if (this->events[j].events == EPOLLIN) {
					// New connection
					if (this->events[j].data.fd == hosts[i].fdSetSock)
						ft_new_connection(hosts[i], this->events, i);
					
					// Existing connection
					else {
						ft_parse_request(hosts[i], this->events[j]);
						ft_act_on_request(hosts[i], this->events[j]);
					}

				} else if (this->events[j].events == EPOLLOUT) {
					std::cout << "EPOLLOUT event detected" << std::endl;
				} else {
					std::cout << "Unknown event detected" << std::endl;
				}		
			}
		}
	}
}

Hosts::~Hosts() {
    for (size_t i = 0; i < hosts.size(); i++) {
		ft_close(hosts[i].fdSetSock);
		ft_close(hosts[i].fdEpoll);
	}
    return ;
}
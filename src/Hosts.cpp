/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:34 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/01 18:48:07 by bdelamea         ###   ########.fr       */
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

void	Hosts::loopServer(void) {
	char	buffer[1024] = { 0 };
	int		nfds;
	int		valread;

	ft_server_init(this->hosts, this->events);

	std::cout << "Detail of hosts and epoll watchlist:" << std::endl;
	for (size_t i = 0; i < hosts.size(); ++i)
		std::cout << "Server " << i << " is running on port " << hosts[i]._port << ", ip " << hosts[i]._ip << ", name " << hosts[i]._name << ", file descriptor: " << this->events[i].data.fd << ", epoll fd: " << hosts[i].fdEpoll << ", event: " << this->events[i].events << std::endl;

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
				std::ostringstream oss;

				if (this->events[j].events & EPOLLIN) {
					
					// New connection
					if (this->events[j].data.fd == hosts[i].fdSetSock) {
						std::cout << "New connection detected" << std::endl;

						// Accept the connection
						if ((hosts[i].fdAcceptSock = accept(hosts[i].fdSetSock, (struct sockaddr *)&hosts[i]._address, (socklen_t*)&hosts[i].address_len)) < 0) {
							ft_close(hosts[i].fdSetSock);
							oss << "Error in the accept for server " << i << "and socket " << hosts[i].fdSetSock << ": " << strerror(errno);
							throw ErrorFdManipulation(oss.str());
						}

						// Make the connection non-blocking
						if (fcntl(hosts[i].fdAcceptSock, F_SETFL, fcntl(hosts[i].fdAcceptSock, F_GETFL) | O_NONBLOCK) < 0) {
							ft_close(hosts[i].fdSetSock);
							ft_close(hosts[i].fdAcceptSock);
							oss << "Error in the fcntl for server " << i << "and socket " << hosts[i].fdAcceptSock << ": " << strerror(errno);
							throw ErrorFdManipulation(oss.str());
						}

						// Add new connection to epoll
						struct epoll_event new_event;
						new_event.events = EPOLLIN;
						new_event.data.fd = hosts[i].fdAcceptSock;
						if (epoll_ctl(hosts[i].fdEpoll, EPOLL_CTL_ADD, hosts[i].fdAcceptSock, &new_event) < 0)
							throw ErrorFdManipulation("Error in the epoll_ctl: " + static_cast<std::string>(strerror(errno)));
						this->events.push_back(new_event);
					}
					// Existing connection
					else {
						std::cout << "Existing connection detected" << std::endl;
						
						// Read data from client
						valread = read(this->events[j].data.fd, buffer, 1024);
						if (valread < 0) {

							// Error in the read
							ft_close(this->events[j].data.fd);
							throw ErrorFdManipulation("Error in the read");

						} else if (valread == 0) {

							// Connection ft_closed by client
							ft_close(this->events[j].data.fd);
							std::cout << "Client disconnected, fd: " << this->events[j].data.fd << std::endl;
						}

						std::cout << "Message received: " << buffer << std::endl;

						// Send data to client & ft_close connection
						send(this->events[j].data.fd, "Hello, client!", 14, 0);
						ft_close(this->events[j].data.fd);
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
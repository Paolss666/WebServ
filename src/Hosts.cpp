/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:34 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 19:05:29 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/webserv.hpp"

Hosts::Hosts() { return ; }

void Hosts::initServer(const char *fileName) {
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
}

void	Hosts::loopServer(void) {
	for (size_t i = 0; i < hosts.size(); i++) {
		if ((hosts[i]._FdSocket = socket(hosts[i]._address.sin_family, SOCK_STREAM, 0)) <= 0)
			throw ErrorFdManipulation("Error in the socket creation");
		if (bind(hosts[i]._FdSocket, (struct sockaddr *)&hosts[i]._address, sizeof(hosts[i]._address)) < 0)
			throw ErrorFdManipulation("Error in the bind");
		if (listen(hosts[i]._FdSocket, MAX_CONNECTIONS) < 0)
			throw ErrorFdManipulation("Error in the listen");
		if ((hosts[i]._FdEpoll = epoll_create(1) < 0))
			throw ErrorFdManipulation("Error in the epoll_create");
		if (epoll_ctl(hosts[i]._FdEpoll, EPOLL_CTL_ADD, hosts[i]._FdSocket, NULL) < 0)
			throw ErrorFdManipulation("Error in the epoll_ctl");
	}
	while (!g_sig) {
		sleep(1);
		std::cout << "waiting\n";
	}
}

Hosts::~Hosts() {
    for (size_t i = 0; i < hosts.size(); i++) {
		close(hosts[i]._FdSocket);
		close(hosts[i]._FdEpoll);
	}
    return ;
}
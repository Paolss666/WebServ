/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:34 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 18:43:28 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
		if ((hosts[i]._Fdsocket = socket(hosts[i]._address.sin_family, SOCK_STREAM, 0)) <= 0)
			throw ErrorFdManipulation("Error in the socket creation");
	}
}

Hosts::~Hosts() {
    for (size_t i = 0; i < hosts.size(); i++)
		close (hosts[i]._Fdsocket);
    return ;
}
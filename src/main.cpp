/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:20 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/23 11:12:30 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	g_sig;

std::vector<ServerConf>	ft_conf_init(const char *fileName) {
    std::string				line;
	std::vector<ServerConf> server;
    int     				i = 0;

    Check_extension(fileName, ".conf");
    std::ifstream file(fileName);
    if (!file.is_open())
        throw ErrorConfFile("Can't open the file");
	
    while (std::getline(file, line)) {
        if (line == "server {") {
            ServerConf virtualServer;	
            // std::cout << " ======> "<< line << "<============================== \n";
             virtualServer.initWServer(file);
            // std::cout << "FILE DONE\n";
            virtualServer.setNbServer(i);
			server.push_back(virtualServer);
            // std::cout << "nbserver " << i << "\n";
			i++;
		}
        else if (line.empty())
            continue;
        else
			throw ErrorConfFile("In the conf file");
    }
	for (size_t i = 0; i < server.size() - 1; i++)
		if (server[i].getIp() == server[i + 1].getIp() && server[i].getPort() == server[i + 1].getPort())
			throw ErrorConfFile("In conf file: server : same Ip/Port");

	return server;
}

void	WebServe(std::vector<ServerConf> & server, std::vector<Host> & hosts) {

	// Initiate the servers
	for (size_t i = 0; i < server.size(); i++) {
		Host tmp(server[i]);
		hosts.push_back(tmp);
	}
	// Print the details of the hosts and epoll watchlist
	std::cout << "Detail of hosts and epoll watchlist:" << std::endl;
	for (size_t i = 0; i < hosts.size(); ++i)
		std::cout << CYAN << std::setw(8) << "Server " << WHITE << std::setw(2) << i 
			<< CYAN << " | Port: " << WHITE << std::setw(1) << hosts[i]._port 
			<< CYAN << " | IP: " << WHITE << std::setw(9) << hosts[i]._ip 
			<< CYAN << " | Name: " << WHITE << std::setw(10) << hosts[i]._name
			<< CYAN << " | Epoll_fd: " << WHITE << std::setw(1) << hosts[i]._fdEpoll  
			<< RESET << std::endl;

	std::cout << CYAN BOLD "\n----------------- Server is running -----------------\n" RESET << std::endl;
	while (!g_sig)
		for (size_t i = 0; i < hosts.size(); ++i)
			hosts[i].run_server();
	for (size_t i = 0; i < server.size(); i++)
		hosts[i].close_everything();
}

std::map<std::string, std::string>	CONTENT_TYPES;

int main(int ac, char **av) {
	std::vector<ServerConf> server;
	std::vector<Host> hosts;

	signal(SIGINT, sig_handler);
	try {
		if (ac != 2)
			server = ft_conf_init("conf/basicgood.conf");
		else
			server = ft_conf_init(av[1]);
		WebServe(server, hosts);
	} catch(std::exception & c) {
		ft_perror(c.what());
		for (size_t i = 0; i < server.size(); i++)
			hosts[i].close_everything();
		return EXIT_FAILURE;
	}
	std::cout << GREEN BOLD "Server is done running" RESET << std::endl;
	return EXIT_SUCCESS;
}

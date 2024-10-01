/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:20 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/01 17:48:34 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/webserv.hpp"

int	g_sig;

int main(int ac, char **av) {
	Hosts  server;

	signal(SIGINT, sig_handler);
	try {
		if (ac < 2)
			return (std::cerr << "ERREUR\n", 1);
		server.ft_conf_init(av[1]);
		server.loopServer();
	}
	catch(std::exception & c) {
		std::cerr << c.what() << std::endl;
		return 1;
	}
	std::cout << "Server is done running\n";
}

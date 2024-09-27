/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:20 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 19:05:50 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	g_sig;

int main(int ac, char **av) {
	Hosts  server;
	try {
		if (ac < 2)
			return (std::cerr << "ERREUR\n", 1);
		server.initServer(av[1]);
		server.loopServer();
	}
	catch(std::exception & c) {
		std::cerr << c.what() << std::endl;
		return 1;
	}
}

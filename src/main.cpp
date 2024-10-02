/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:20 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/02 08:09:44 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/webserv.hpp"

int	g_sig;

int main(int ac, char **av) {
	Hosts  server;

	signal(SIGINT, sig_handler);
	try {
		if (ac < 2)
			return (ft_perror("ERREUR"), EXIT_FAILURE);
		server.ft_conf_init(av[1]);
		server.loopServer();
	}
	catch(std::exception & c) {
		ft_perror(c.what());
		return EXIT_FAILURE;
	}
	std::cout << GREEN BOLD "Server is done running" RESET << std::endl;
}

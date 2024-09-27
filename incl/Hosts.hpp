/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 17:51:48 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 18:02:47 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOSTS_HPP
# define HOSTS_HPP

# include "webserv.hpp"

class Hosts {
	public:
		Hosts(void);
		~Hosts(void);

		std::vector<ServerConf>	hosts;

		void					initServer(const char * fileName);
		void					loopServer(void);
};

#endif
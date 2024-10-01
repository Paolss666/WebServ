/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 17:51:48 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/01 17:48:34 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOSTS_HPP
# define HOSTS_HPP

# include "webserv.hpp"

class Hosts {
	public:
		Hosts(void);
		~Hosts(void);

		std::vector<ServerConf>				hosts;
		std::vector<struct epoll_event>		events;

		void					ft_conf_init(const char * fileName);
		void					loopServer(void);
};

#endif
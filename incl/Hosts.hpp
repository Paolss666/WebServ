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

class Host: public ServerConf {
	public:
		Host(void);
		Host(ServerConf &src);
		~Host(void);

		std::vector<struct epoll_event>		events;
		std::map<int, Request>				requests;
		int									nfds;

		void								new_connection(void);
		void								parse_request(int fd);
		void								act_on_request(int fd);
		void								build_response(int fd);
		void								run_server(void);
		void								close_everything(void);
};

#endif
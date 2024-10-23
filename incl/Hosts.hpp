/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hosts.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 17:51:48 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/21 19:25:18 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HOSTS_HPP
# define HOSTS_HPP

# include "webserv.hpp"

class Request;
class Response;

class Host: public ServerConf {
	public:
		Host(void);
		Host(ServerConf & src);
		~Host(void);

		std::vector<struct epoll_event>	_events;
		std::vector<std::string>		_files;
		std::map<int, Request>			_requests;
		std::map<int, Response>			_responses;
		std::vector<int>				_partials;
		int								_nfds;
		time_t							_keep_alive_time;

		void							new_connection(void);
		void							parse_request(int fd);
		void							act_on_request(int fd);
		void							run_server(void);
		void							close_everything(void);
		void							json_update(void);
};

#endif
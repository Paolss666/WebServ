/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:58:06 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/08 19:29:17 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Host;

class Request {
	public:
		Request(void);
		Request(Host & host, struct epoll_event & event, std::string const & raw);
		Request(Request const & src);
		~Request(void);

		void								parse(void);
		void								append(std::string const & data);
		void								pnc_request_line(std::istringstream & iss);
		void								pnc_headers(std::istringstream & iss);
		void								pnc_body(std::istringstream & iss);

		Host &								_host;
		struct epoll_event &				_event;
		std::string							_raw;
		std::map<std::string, std::string>	_request_line;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		bool								_b_content_length;
		int									_stage;
		
};

#endif
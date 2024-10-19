/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:58:06 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/19 01:28:59 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Host;

class Request {
	public:
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
		int									_stage;
		int									_eof;	
};

#endif
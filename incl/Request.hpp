/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:58:06 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/06 18:29:39 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Host;

class Request {
	public:
		Request(void);
		Request(std::string const & raw);
		~Request(void);

		void								parse(Host & host);
		void								append(std::string const & data);
		void								check_request_line(void);
		void								check_headers(Host & host);
		void								check_body(void);
		std::string							trim(std::string & str);

		std::string							_raw;
		std::map<std::string, std::string>	_request_line;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		bool								_b_keepalive;
		bool								_b_content_length;
		
};

#endif
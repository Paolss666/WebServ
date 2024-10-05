/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:58:06 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/05 20:16:29 by benoit           ###   ########.fr       */
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
		void								ft_check_request_line(void);
		void								ft_check_headers(Host & host);
		void								ft_check_body(void);

		std::string							raw;
		std::map<std::string, std::string>	request_line;
		std::map<std::string, std::string>	headers;
		std::string							body;
		bool								b_keepalive;
		bool								b_content_length;
};

#endif
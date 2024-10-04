/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 08:58:06 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/03 11:13:42 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "webserv.hpp"

class Request {
	public:
		Request(void);
		Request(std::string const & raw);
		~Request(void);

		void								parse(void);
		void								append(std::string const & data);

		std::string							raw;
		std::map<std::string, std::string>	request_line;
		std::map<std::string, std::string>	headers;
		std::string							body;
		int									p_step;
};

#endif
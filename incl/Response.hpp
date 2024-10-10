/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:22:37 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/06 18:35:24 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Response: public Request {
	public:
		Response(const Request & src, const Host &host);
		~Response(void);

		// void			createGET();


		std::map<std::string, Location>  _Location;
		std::vector<std::string>		_indexPages;
		std::string						_path_file;
		std::string						_root;
		std::map<int, std::string>		_returnPages;
		std::map<int, std::string>		_pagesError;
		int								_err; // Error code 4 the pages -- need to be setup
		bool							_autoIndex; // if is true->on else flase->off;
};

#endif
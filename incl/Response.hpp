/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:22:37 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/13 13:00:32 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Response: public Request {
	public:
		Response(const Request & src, const Host &host);
		~Response(void);

		void							BuildGet(int fd, struct epoll_event & event);
		void							BuildPage(int fd, struct epoll_event & event);
		void							buildAutoindex(int fd, struct epoll_event & event);
		void							buildErrorPage(int fd, int statusCode, struct epoll_event & event);
		std::string						_Ip;
		std::string						_Port;
		std::map<std::string, Location>  _Location;
		std::string						_serverName;
		std::vector<std::string>		_indexPages;
		std::string						_path_file;
		std::string						_root;
		std::map<int, std::string>		_returnPages;
		std::map<int, std::string>		_pagesError;
		int								_err; // Error code 4 the pages -- need to be setup
		bool							_autoIndex; // if is true->on else flase->off;
		int								_autoInxPrint;
		std::string						_tmpForInx;
		std::string						_finalUri;
		std::string						_startUri;
		int								_found; // found the good Uri;
		size_t							_maxBodySize;
		int								_statusCode;
};

#endif
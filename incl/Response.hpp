/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:22:37 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/27 14:35:49 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

class Response: public Request {
	public:
		Response(const Request & src, const Host &host);
		~Response(void);

		// CGI functions
		std::vector<char *> 				MakeEnvCgi(std::string & cgi, std::vector<std::string> & storage);
		void								exportENV(std::vector<std::string> & storage, std::vector<char *> & env, std::string key, std::string value);
		void								exec_cgi(int *fd, int cgiFdOut, int cgiFdIn, std::string script);
		void								buildCgi(void);
	
		// Response functions
		void								buildGet(void);
		void								buildPost(void);
		void								buildDelete(void);
		void								buildPage(void);
		void								buildAutoindex(void);
		void								buildReturnPage(void);
		void								send_response(int fd, bool *done);

		// Variables
		std::map<std::string, std::string>	_response_header;
		std::string							_response_body;
		std::string							_response_message;
		std::string							_Ip;
		std::string							_Port;
		std::string							_serverName;
		std::vector<std::string>			_indexPages;
		std::string							_path_file;
		std::string							_root;
		std::map<int, std::string>			_returnPages;
		std::map<int, std::string>			_pagesError;
		int									_err; // Error code 4 the pages -- need to be setup
		bool								_autoIndex; // if is true->on else flase->off;
		bool								_response_ready;
		int									_autoInxPrint;
		std::string							_tmpForInx;
		std::string							_finalUri;
		std::string							_startUri;
		int									_found; // found the good Uri;
		size_t								_maxBodySize;
		int									_statusCode;
};

#endif
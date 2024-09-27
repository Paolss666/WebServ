/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 09:41:36 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 18:42:45 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

#include "webserv.hpp"

class ServerConf {
	public:
		ServerConf(void);
		~ServerConf(void);

		struct sockaddr_in			_address;
		std::map<int, std::string>	_PageError;
		std::map<int, std::string>	_CodeReturn;
		std::vector<std::string>	_IndexFile;
		int							_Fdsocket;
		int							_NotBind;
		int							_port;
		int							_IndexPages;
		int							_nbServer;
		size_t						_maxBodySize;
		std::string					_PortString;
		std::string					_ip;
		std::string					_name;
		std::string					_rootPath;
		bool						_maxBodyState;
		bool						_DefaultPort;
		bool						_StateListen;
		bool						_IpDefault;
		bool						_isServerName;
		bool						_rootFlag;
		bool						_Autoindex;
		bool						_errorFlag;
		bool						_Default_server;
		bool						_CheckDefaultServer;
		bool						_ReturnFlag;

		// P means parsing;
		void						printServerNames(void)const;
		void						initWServer(std::istream & file);
		void						setNbServer(int nb);
		void						p_Listen(std::istringstream & iss);
		void						p_IpAddrs(void);
		void						p_name(std::istringstream & iss);
		void						p_MaxClientBodySize(std::istringstream &iss);
		void						p_Root(std::istringstream & iss);
		void						p_AutoIndex(std::istringstream & iss);
		void						p_ErrorPages(std::istringstream & iss);
		void						p_Index(std::istringstream & iss);
		void						p_DefaultServer(std::istringstream & iss);
		void						p_CodeReturn(std::istringstream & iss);
		void						p_Location(std::istringstream& iss, std::string kw);
		int							p_errorCodes(std::string & pgError);
		int							p_Return(std::string & codeRetrn);
};

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 17:51:45 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/26 12:32:44 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"
// #include "ServerConf.hpp"
/* 
	La directive location dans un fichier de configuration Nginx
	est utilisée pour définir comment Nginx doit traiter les requêtes HTTP pour des URL spécifiques.
	En fonction du chemin de la requête (le URI), Nginx peut décider de servir des fichiers, de rediriger,
	de proxyfier les requêtes vers un autre serveur, ou d'appliquer d'autres règles.
*/

class Location {
	private:
		std::string					_Uri;
		std::string					_ServerName;
		std::string					_Root;
		std::string					_CgiPath;
		std::map<int,std::string>	_Retourn;
		std::map<int,std::string>	_PageError;
		std::vector<std::string>	_Methods;
		std::vector<std::string>	_Indx;
		bool						_indexFlag;
		bool						_CgiFlag;
		bool						_ReturnFlag;
		bool						_ErPages;
		bool						_AutoFlag;
		bool						_AutoIndex;
		bool						_MetFlag;
		bool						_rootflag;
		int							_flagPost;
		int							_flagGet;
		int							_flagDelete;

	public:
		Location(/*args*/);
		~Location();
		void						InLoc_Index(std::istringstream & iss);
		void						InLoc_Cgi(std::istringstream & iss);
		void						InLoc_Return(std::istringstream & iss);
		int							InLoc_p_Return(std::string & codeRetrn);
		void						InLoc_ErPages(std::istringstream & iss);
		int							InLoc_p_errorCodes(std::string & pgError);
		void						InLoc_AutoIndex(std::istringstream & iss);
		void						InLoc_Methos(std::istringstream & iss);
		void						InLoc_root(std::istringstream & iss);
		void						setUri(std::string uri);
		std::string					getUri(void);
		void						ParseLocation(std::istream&  file);
		int							getFlagAutoInx(void);
		int							getFlagIndex(void);
		int							getReturnFlag(void);
		int							getFlagErrorPages(void);
		int							getRootFlag(void);
		int							getFlagCgi(void);
		std::map<int,std::string>&	getReturnPages();
		std::map<int,std::string>&	getPagesError();
		std::vector<std::string> &	getIndexPages();
		std::string					getRoot();
		bool						getAutoIndex();
		std::string					getCgiPath();
		int							getFlagGet();
		int							getFlagPost();
		int							getFlagDelete();
	std::vector<std::string>		getMtods();
};

#endif
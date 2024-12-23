/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 17:51:45 by bdelamea          #+#    #+#             */
/*   Updated: 2024/11/04 17:09:52 by bdelamea         ###   ########.fr       */
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
	public:
		std::string							_Uri;
		std::string							_ServerName;
		std::string							_Root;
		std::string							_CgiPath;
		std::map<int,std::string>			_Retourn;
		std::map<int,std::string>			_PageError;
		std::vector<std::string>			_Methods;
		std::vector<std::string>			_Indx;
		bool								_indexFlag;
		bool								_CgiFlag;
		bool								_ReturnFlag;
		bool								_ErPages;
		bool								_AutoFlag;
		bool								_AutoIndex;
		bool								_MetFlag;
		bool								_rootflag;
		int									_flagPost;
		int									_flagGet;
		int									_flagDelete;
		int									_CgiAllow;

		Location(/*args*/);
		~Location();
		void								InLoc_Index(std::istringstream & iss);
		void								InLoc_Cgi(std::istringstream & iss);
		void								InLoc_Return(std::istringstream & iss);
		int									InLoc_p_Return(std::string & codeRetrn);
		void								InLoc_ErPages(std::istringstream & iss);
		int									InLoc_p_errorCodes(std::string & pgError);
		void								InLoc_AutoIndex(std::istringstream & iss);
		void								InLoc_Methos(std::istringstream & iss);
		void								InLoc_root(std::istringstream & iss);
		void								setUri(std::string uri);
		void								ParseLocation(std::istream&  file);
				
		std::string							getUri(void) const;
		int									getFlagAutoInx(void) const;
		int									getFlagIndex(void) const;
		int									getReturnFlag(void) const;
		int									getFlagErrorPages(void) const;
		int									getRootFlag(void) const;
		int									getFlagCgi(void) const;
		std::map<int,std::string> const &	getReturnPages() const;
		std::map<int,std::string> const &	getPagesError() const;
		std::vector<std::string> const &	getIndexPages() const;
		std::string							getRoot() const;
		bool								getAutoIndex() const;
		std::string							getCgiPath() const;
		int									getCgiAllow() const;
		int									getFlagGet() const;
		int									getFlagPost() const;
		int									getFlagDelete() const;
	std::vector<std::string>				getMtods() const;
};

#endif
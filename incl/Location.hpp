/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 17:51:45 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 17:56:48 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "webserv.hpp"

class Location {
	public:
		Location(void);
		~Location(void);

		std::string					_Uri;
		std::string					_ServerName;
		std::string					_Root;
		std::string					_CgiPath;
		std::map<int, std::string>	_Retourn;
		std::map<int, std::string>	_PageError;
		std::vector<std::string>	_Methods;
		std::vector<std::string>	_Indx;
		bool						_MetFlag;
		bool						_indexFlag;
		bool						_CgiFlag;

		void						p_Methos(std::istringstream & iss);
		void						p_Index(std::istringstream & iss);
		void						p_Cgi(std::istringstream & iss);
		void						setUri(std::string uri);
		std::string					getUri(void);
		void						ParseLocation(std::istream & file);	
};

#endif
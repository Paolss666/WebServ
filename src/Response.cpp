/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/06 18:28:25 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Response::Response(const Request & src, const Host &host): Request(src) {
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_b_keepalive = src._b_keepalive;
	_b_content_length = src._b_content_length;
	_indexPages = host._IndexFile;
	_Location =  host._Location;
	_root = host._rootPath;
	return ;
}

Response::~Response(void) { return ; }

void		Response::createGET()
{
	
	std::string 	uri = _request_line["uri"];

	
	std::cout  << "uri -> " << uri << std::endl;
	 
	while (true)
	{
		bool found = false;
        for (std::map<std::string, Location>::iterator it = _Location.begin(); it != _Location.end(); ++it) {
            if (it->first == uri)
			{                
				std::cout << "Found matching URI: " << it->first << std::endl;
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
        std::size_t pos = uri.find_last_of('/');
        if (pos == std::string::npos || pos == 0) {
            uri = "/";
        } else {
            uri = uri.substr(0, pos);
        }
	}
	if (_Location[uri].getFlagIndex())
		_indexPages = _Location[uri].getIndexPages();
	if (_Location[uri].getRootFlag())
		_root = _Location[uri].getRoot();
	std::string		file_path = _root + uri;
	
	// if (file_path.back() == '/')
		file_path += _indexPages[0];
	// else
	// {
	// 	file_path += '/';
	// 	file_path += _indexPages[0];
	// }
	std::cout << file_path << " << ===== file_path " << std::endl;
	_path_file = file_path;
}
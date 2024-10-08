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
	_pagesError = host._PageError;
	_returnPages = host._CodeReturn;

	if (_root[0] == '/')
		_root = _root.substr(1,_root.size() - 1);
	if (_root[_root.size() - 1] == '/')
		_root = _root.substr(0,_root.size() - 1);
	
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
			break;
        } else {
            uri = uri.substr(0, pos);
        }
	}
	std::cout  << "uri -> " << uri << std::endl;
	if (_Location[uri].getFlagIndex())
		_indexPages = _Location[uri].getIndexPages();
	if (_Location[uri].getRootFlag())
		_root = _Location[uri].getRoot();
	if (_Location[uri].getReturnFlag())
		_returnPages = _Location[uri].getReturnPages();
	if (_Location[uri].getFlagErrorPages())
		_pagesError = _Location[uri].getPagesError();
	
	std::string		file_path = _root + uri;
	std::cout << file_path << " << ===== file_path " << std::endl;
	if (file_path == "www/home" && !file_path.empty() && file_path[file_path.size() - 1] != '/') {
	    file_path += '/';
	}
	struct stat buf;
	if (stat(file_path.c_str(), &buf) < 0 || file_path == "www/")
		_path_file = "www/index.html";
	else
		_path_file = file_path + _indexPages[0];
}

Response::~Response(void) { return ; }
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/09 16:16:57 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Response::Response(const Request & src, const Host &host): Request(src) {
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_b_content_length = src._b_content_length;
	_indexPages = host._IndexFile;
	_Location =  host._Location;
	_root = host._rootPath;
	_pagesError = host._PageError;
	_returnPages = host._CodeReturn;
	_err = 0; // 4 error 404 etc etc 
	_autoInxPrint = 0;
	_autoIndex = host._Autoindex;

	if (_root[0] == '/')
		_root = _root.substr(1,_root.size() - 1);
	if (_root[_root.size() - 1] == '/')
		_root = _root.substr(0,_root.size() - 1);

	std::string 	uri = _request_line["uri"];
	std::string		tmpUri = uri;

	std::cout  << "uri -> " << uri << std::endl; 
	std::string test_path = _root + uri;
	std::cout  << "test_path -> " << test_path << std::endl;
	if (test_path.find(".") != std::string::npos && _err == 0)
		_err = IsARepertory(test_path);
	while (true)
	{
		bool found = false;
        for (std::map<std::string, Location>::iterator it = _Location.begin(); it != _Location.end(); ++it) {
			std::cout << "it->first" << it->first << std::endl;
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
		uri = uri.substr(0, uri.find_last_of('/'));
		std::cout << " --------->  uri    ->" << uri << std::endl;
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
	if (_Location[uri].getFlagAutoInx())
		_autoIndex = _Location[uri].getAutoIndex();
	if (_Location[uri].getRootFlag())
		_root = _Location[uri].getRoot();
	if (_Location[uri].getReturnFlag())
		_returnPages = _Location[uri].getReturnPages();
	if (_Location[uri].getFlagErrorPages())
		_pagesError = _Location[uri].getPagesError();
	
	// _finalUri
	std::cout << host._Autoindex << "autoindex HOST \n";
	std::string		file_path = _root + uri;
	_finalUri = file_path;
	std::cout << "TMP URI --> " << tmpUri << std::endl;
	std::size_t posIndex = tmpUri.find_last_of('/') + 1;
	std::string foundindx = tmpUri.substr(posIndex, tmpUri[tmpUri.size() - 1]);
	std::cout << "foundindx --> " << foundindx << std::endl;
	std::cout << "_autoIndex --> " << _autoIndex << std::endl;
	if (foundindx == "autoindex.html" && _autoIndex)
	{
		_autoInxPrint = 1;
		_tmpForInx = tmpUri;
	}
	else
		_autoInxPrint = 0;
	// if (file_path)
	std::cout << "_autoInxPrint --> " << _autoInxPrint << std::endl;
	std::cout << file_path <<  "< ----- filepath befor" << std::endl;
	// IsARepertory(file_path);
	std::cout << file_path << " << ===== file_path " << std::endl;
	if (file_path == "www/home" && !file_path.empty() && file_path[file_path.size() - 1] != '/') {
	    file_path += '/';
	}
	struct stat buf;
	if (stat(file_path.c_str(), &buf) < 0 || file_path == "www/")
		_path_file = "www/index.html";
	else
	{
		for (size_t i = 0; i < _indexPages.size() ; i++)
		{
			if (file_path[file_path.size() - 1] != '/')
				_path_file = file_path + "/" + _indexPages[i];
			else 
				_path_file = file_path + _indexPages[i];
		}
	}
}

Response::~Response(void) { return ; }
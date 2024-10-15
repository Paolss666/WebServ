/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/15 19:48:32 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Response::Response(const Request & src, const Host &host): Request(src) {
	fillContentTypes();
	_response_ready = false;
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_indexPages = host._IndexFile;
	_Location =  host._Location;
	_root = host._rootPath;
	_pagesError = host._PageError;
	_returnPages = host._CodeReturn;
	_err = 0; // 4 error 404 etc etc 
	_autoInxPrint = 0;
	_autoIndex = host._Autoindex;
	_found = 0;
	_serverName = host._name;
	_maxBodySize= host._maxBodySize;
	_Ip = host._ip;
	_Port = host._port;
	_statusCode = 200;
	_startUri = _request_line["uri"];

	if (_root[0] == '/')
		_root = _root.substr(1,_root.size() - 1);
	if (_root[_root.size() - 1] == '/')
		_root = _root.substr(0,_root.size() - 1);
}

bool	Response::send_response(int fd) {
	int	sent;

	if (_response_message.size() > BUFFER_SIZE)
		sent = send(fd, _response_message.c_str(), BUFFER_SIZE, MSG_MORE | MSG_NOSIGNAL);
	else
		sent = send(fd, _response_message.c_str(), _response_message.size(), MSG_NOSIGNAL);
	
	if (sent == -1)
		return (send_error_page(_host, fd, ERR_CODE_INTERNAL_ERROR), true);
	else
		_response_message = _response_message.substr(sent);
	
	if (_response_message.empty())
		return true;
	else
		return false;
}

void	Response::buildAutoindex(int fd) {
	std::cout << "Building autoindex" << std::endl;
	std::vector<std::string> filesList;
	DIR *dir = opendir(_startUri.c_str());
	if (!dir)
		return send_error_page(_host, fd, ERR_CODE_INTERNAL_ERROR);

	struct dirent *fileRead;
	_startUri  = _root + _startUri;
	while ((fileRead = readdir(dir)) != NULL)
		if (strcmp(fileRead->d_name, ".") != 0 || (strcmp(fileRead->d_name, "..") != 0 && _startUri != "/"))
			filesList.push_back(fileRead->d_name);

	_headers["content-type"] = "text/html";
	this->_body = "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"<meta charset=\"UTF-8\">\n"
			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"<link href=\"style_autoindex.css\" rel=\"stylesheet\">\n"
			"<link href='../home/css/style.css' rel='stylesheet' type='text/css'>\n"
			"<link href=\"../../style_autoindex.css\" rel=\"stylesheet\" />\n"
			"<title>Auto index</title>\n"
			"</head>\n"
			"<body>\n"
			"<h1 class=\"title1\"> Auto index </h1>\n"
			"<h2 class=\"autoindex\">\n" + _startUri + "\n"
			"</h2>\n"
			"</body>\n"
			"</html>";

	for (std::vector<std::string>::iterator it = filesList.begin(); it != filesList.end(); it++) {
		if (*it == ".")
			continue ;
		std::string hyperlink("");
		std::string filename("");
		if (*it == "..")
			filename = "<< COME BACK TO HOME ";
		else
			filename = (*it);
		hyperlink = (*it);
		this->_body += "<div class=\"button-container\"><a class=\"link-button\" href=" + hyperlink + ">" + filename + "</a></div>\n";
	}

	this->_body += "</body>\n";
	this->_body += "</html>";

	closedir(dir);

	_response_ready = true;
}

void	Response::buildPage(int fd) {
	std::stringstream	buffer;
	std::string			resourceName, fileExtension;
	size_t				pos;
	std::ostringstream 	oss;
	
	// Open the requested file
	_startUri = _root + _startUri;
	std::ifstream fileRequested(_startUri.c_str());
	if (fileRequested.good() == false)
		return (send_error_page(_host, fd, ERR_CODE_NOT_FOUND));
	
	buffer << fileRequested.rdbuf();
	
	_response_body = buffer.str();
	if (_response_body.size() > _maxBodySize) {
		_response_body = "";
		return  (send_error_page(_host, fd, ERR_CODE_NOT_FOUND));
	}

	pos = _startUri.find_last_of("/");
	if (pos != std::string::npos && (_startUri.begin() + pos + 1) != _startUri.end()) {
		resourceName = _startUri.substr(pos + 1);
		pos = resourceName.find_last_of(".");
		if (pos != std::string::npos && (resourceName.begin() + pos + 1) != resourceName.end()) {
			fileExtension = resourceName.substr(pos + 1);
			if (CONTENT_TYPES.find(fileExtension) != CONTENT_TYPES.end())		
				_response_header.insert(std::make_pair("Content-Type", CONTENT_TYPES[fileExtension]));
		}
	}

	// Add remainig headers
	oss << _response_body.size();
	_response_header.insert(std::make_pair("Content-Length",oss.str()));
	if (_response_header.find("Content-Type") == _response_header.end())
		_response_header.insert(std::make_pair("Content-Type", "text/html"));
	
	// Merge everything in the final message to send
	oss.str("");
	oss << "HTTP/1.1 " << _statusCode << " OK" << "\r\n";
	_response_message = oss.str();
	_response_message += "Server: " + _serverName + "\r\n";
	for (std::map<std::string, std::string>::iterator it = _response_header.begin(); it != _response_header.end(); it++)
		_response_message += it->first + ": " + it->second + "\r\n";
	_response_message += "\r\n";
	_response_message += _response_body;

	// Set the response ready
	_response_ready = true;
}

void	Response::buildGet(int fd) {
	std::string	index, path;
	
	// Set the default values for the response
	if (_Location[_startUri].getFlagIndex())
		_indexPages = _Location[_startUri].getIndexPages();
	if (_Location[_startUri].getFlagAutoInx())
		_autoIndex = _Location[_startUri].getAutoIndex();
	if (_Location[_startUri].getRootFlag())
		_root = _Location[_startUri].getRoot();
	if (_Location[_startUri].getReturnFlag())
		_returnPages = _Location[_startUri].getReturnPages();
	if (_Location[_startUri].getFlagErrorPages())
		_pagesError = _Location[_startUri].getPagesError();
	
	// Check if the URI is a directory
	if (isRepertory(_root, _startUri) == 3) {
		if (_startUri[_startUri.size() -1] != '/')
			send_error_page(_host, fd, ERR_CODE_MOVE_PERM);
		else {
			if (!_indexPages.empty()) {
				for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++) {
					index = (*it)[0] == '/' ? (*it).substr(1, std::string::npos) : (*it);
					path = _startUri + index;
					if (isRepertory(_root, path) == 1) {
						_startUri = path;
						buildPage(fd);
					}
				}
			}
			else if (_autoIndex)
				buildAutoindex(fd);
			else
				send_error_page(_host, fd, ERR_CODE_FORBIDDEN);
		}
	} else if (isRepertory(_root, _startUri) == 1)
		buildPage(fd);
	else
		send_error_page(_host, fd, ERR_CODE_NOT_FOUND);
}

Response::~Response(void) {	return ; }
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/23 11:12:12 by benoit           ###   ########.fr       */
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

void	Response::send_response(int fd, bool *done) {
	int	sent;

	if (_response_message.size() > BUFFER_SIZE)
		sent = send(fd, _response_message.c_str(), BUFFER_SIZE, MSG_MORE | MSG_NOSIGNAL);
	else
		sent = send(fd, _response_message.c_str(), _response_message.size(), MSG_NOSIGNAL);

	if (sent == -1)
		throw ErrorResponse("In the send of the response", ERR_CODE_INTERNAL_ERROR);
	else
		_response_message = _response_message.substr(sent);
	
	if (_response_message.empty())
		*done = true;
	else
		return ;
}

void	Response::buildAutoindex(void) {
	std::cout << "Building autoindex" << std::endl;
	std::vector<std::string> filesList;
	DIR *dir = opendir(_startUri.c_str());
	if (!dir)
		throw ErrorResponse("In the opening of the directory pointer by the URI", ERR_CODE_INTERNAL_ERROR);

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

void	Response::buildPage(void) {
	std::stringstream	buffer;
	std::string			resourceName, fileExtension;
	size_t				pos;
	std::ostringstream 	oss;
	
	// Open the requested file
	_startUri = _root + _startUri;
	std::ifstream fileRequested(_startUri.c_str());
	if (fileRequested.good() == false)
		throw ErrorResponse("In the opening of the file requested", ERR_CODE_NOT_FOUND);
	
	buffer << fileRequested.rdbuf();
	
	_response_body = buffer.str();
	if (_response_body.size() > _maxBodySize) {
		_response_body = "";
		throw ErrorResponse("In the size of the file requested", ERR_CODE_NOT_FOUND);
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

void	Response::buildGet(void) {
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
			throw ErrorResponse("In the response: URI is not correctly written", ERR_CODE_MOVE_PERM);
		else {
			if (!_indexPages.empty()) {
				for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++) {
					index = (*it)[0] == '/' ? (*it).substr(1, std::string::npos) : (*it);
					path = _startUri + index;
					if (isRepertory(_root, path) == 1) {
						_startUri = path;
						buildPage();
					}
				}
			}
			else if (_autoIndex)
				buildAutoindex();
			else
				throw ErrorResponse("In the response: URI points nowhere", ERR_CODE_FORBIDDEN);
		}
	} else if (isRepertory(_root, _startUri) == 1)
		buildPage();
	else
		throw ErrorResponse("In the response: URI is not a directory", ERR_CODE_NOT_FOUND);
}

void	Response::buildPost(void) {
	std::istringstream	iss;
	std::string			line, path;
	size_t 				pos, start = 0;

	// Get the boundary
	iss.str(_headers["Content-Type"]);
	if (!std::getline(iss, _boundary, '=') || !std::getline(iss, _boundary) || _boundary.empty())
		throw ErrorResponse("In the response: content-type not well formatted", ERR_CODE_BAD_REQUEST);
	_boundary += "--";
	
	// Get the end of preliminary binary information
	for (std::size_t i = 0; i < _binary_body.size(); i++) {
		line += _binary_body[i];
		if (line.size() >= 4 && line.size() < _binary_body.size() && line.substr(line.size() - 4) == "\r\n\r\n") {
			start = i + 1;
			break;
		}
	}
	if (!start)
		throw ErrorResponse("In the response: missing opening body information", ERR_CODE_BAD_REQUEST);

	// Parse the multipart form data
	pos = line.find("filename=\"");
	if (pos != std::string::npos) {
		pos += 10; // Move past 'filename="'
		_filename = line.substr(pos, line.find("\"", pos) - pos);
		if (_filename.empty())
			throw ErrorResponse("In the response: missing filename information", ERR_CODE_BAD_REQUEST);
	} else
		throw ErrorResponse("In the response: missing filename information", ERR_CODE_BAD_REQUEST);
	
	// Check the end of file string is present
	line.clear();
	for (size_t i = _binary_body.size() - _boundary.size() - 2; i < _binary_body.size(); i++)
		line += _binary_body[i];
	if (line.find(_boundary) == std::string::npos)
		throw ErrorResponse("In the response: missing closing body information", ERR_CODE_BAD_REQUEST);

	// Save the file
	path = _host._rootPath + "/uploads/";

	// Check if the file exists
	if (access((path + _filename).c_str(), F_OK) != -1)
		throw ErrorResponse("In the response: file already exists", ERR_CODE_CONFLICT);

	std::ofstream outfile((path + _filename).c_str(), std::ios::binary);
	if (!outfile.is_open())
		throw ErrorResponse("In the response: cannot open the file", ERR_CODE_INTERNAL_ERROR);
	for (size_t i = start; i < _binary_body.size() - (_boundary.size() + 4); i++)
		outfile.put(_binary_body[i]);
	outfile.close();
	_host._files.push_back(_filename);

	// Send a response to the client
	_response_message = "HTTP/1.1 200 OK\r\n";
	_response_message += "Content-Type: text/plain\r\n";
	_response_message += "Content-Length: 19\r\n";
	_response_message += "\r\n";
	_response_message += "File uploaded successfully!";

	_response_ready = true;
}

void Response::buildDelete(void) {
	int			res;
	struct stat	check;
	std::string	file;

	// Check if the URI exists
	file = _root + _request_line["uri"];
	res = stat(file.c_str(), &check);
	if (res == -1)
		throw ErrorResponse("In the response: file not found", ERR_CODE_NOT_FOUND);
	
	// Check if the URI is a directory
	if (S_ISDIR(check.st_mode)) {

		// Check if the URI is a directory with correct ending
		if (_request_line["uri"].substr(_request_line["uri"].size() - 1) != "/")
			throw ErrorResponse("In the response: URI for directory does not end with /", ERR_CODE_CONFLICT);
		
		// Check for write permission
		if (access(file.c_str(), W_OK) == -1)
			throw ErrorResponse("In the response: no write permission", ERR_CODE_FORBIDDEN);

		// Attempt to remove the directory
		if (rmdir(file.c_str()) == -1)
			throw ErrorResponse("In the response: directory not removed", ERR_CODE_INTERNAL_ERROR);
	} else {
		
		// // Check for write permission
		if (access(file.c_str(), W_OK) == -1)
			throw ErrorResponse("In the response: no write permission", ERR_CODE_FORBIDDEN);

		// Attempt to remove the file
		if (unlink(file.c_str()) == -1)
			throw ErrorResponse("In the response: file not removed", ERR_CODE_INTERNAL_ERROR);

		_host._files.erase(std::remove(_host._files.begin(), _host._files.end(), _request_line["uri"].substr(9)), _host._files.end());
	}
	
	// Send a response to the client
	_response_message = "HTTP/1.1 204 No Content\r\n";
	_response_message += "Server: " + _serverName + "\r\n";
	_response_message += "\r\n";

	_response_ready = true;
}

Response::~Response(void) {	return ; }
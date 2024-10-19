/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/19 12:16:00 by bdelamea         ###   ########.fr       */
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
		throw ErrorResponse("Error in the send of the response", ERR_CODE_INTERNAL_ERROR);
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
		throw ErrorResponse("Error in the opening of the directory pointer by the URI", ERR_CODE_INTERNAL_ERROR);

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
		throw ErrorResponse("Error in the opening of the file requested", ERR_CODE_NOT_FOUND);
	
	buffer << fileRequested.rdbuf();
	
	_response_body = buffer.str();
	if (_response_body.size() > _maxBodySize) {
		_response_body = "";
		throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
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
			throw ErrorResponse("Error in the response: URI is not correctly written", ERR_CODE_MOVE_PERM);
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
				throw ErrorResponse("Error in the response: URI points nowhere", ERR_CODE_FORBIDDEN);
		}
	} else if (isRepertory(_root, _startUri) == 1)
		buildPage();
	else
		throw ErrorResponse("Error in the response: URI is not a directory", ERR_CODE_NOT_FOUND);
}

void	Response::buildPost(void) {
	std::istringstream	iss(_headers["Content-Type"]);
	std::string			boundary_start, boundary_end, line, filename, path;
	std::size_t 		pos;
	std::ostringstream	file_data;

	if (!std::getline(iss, boundary_start, '=') || !std::getline(iss, boundary_start) || boundary_start.empty())
		throw ErrorResponse("Error in the response: content-type not well formatted", ERR_CODE_INTERNAL_ERROR);
	boundary_start = "--" + boundary_start;
	boundary_end = boundary_start + "--";
	
	// Go to the start of the boundary
	pos = _body.find(boundary_start);
	iss.clear();
	iss.str(_body);
	iss.seekg(pos + boundary_start.size());

	// Parse the multipart form data
	while (std::getline(iss, line) && line.find("Content-Type:") == std::string::npos) {
		if (line.find("Content-Disposition: form-data;") != std::string::npos) {
			// Extract filename
			pos = line.find("filename=\"");
			if (pos != std::string::npos) {
				pos += 10; // Move past 'filename="'
				filename = line.substr(pos, line.find("\"", pos) - pos);
			}
		}
	}

	// Go to the start of the file data
	pos = _body.find("\r\n\r\n");
	_body = _body.substr(pos + 4);

	std::cout << YELLOW "Body: " << _body << RESET << std::endl;

	// Extract file data
	for (std::size_t i = 0; i < _body.size(); i++)
		file_data << _body[i];

	// Save the file
	// filename = _host._rootPath + "Uploads/medias" + filename;
	// path = "../www/uploads/medias/" + filename; // Change at 42 !!
	// std::cout << "Saving file: " << filename << " to location: " << path << std::endl;
	filename = "test.png";
	path = "../www/uploads/" + filename; // Change at 42 !!
	std::cout << "Saving file: " << filename << " to location: " << path << std::endl;

	std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary);
	if (!outfile.is_open())
		throw ErrorResponse("Error in the response: cannot open the file", ERR_CODE_INTERNAL_ERROR);
	outfile << file_data.str();
	outfile.close();

	// Send a response to the client
	_response_body = "HTTP/1.1 200 OK\r\n";
	_response_body += "Content-Length: 0\r\n";
	_response_body += "Content-Type: text/plain\r\n";
	_response_body += "\r\n";
	_response_body += "File uploaded successfully.\n";
    _response_body += "File URL: " + path + "\n";

	_response_ready = true;

	if (line.find(boundary_end) == std::string::npos)
		throw ErrorResponse("Error in the response: body not complete", ERR_CODE_BAD_REQUEST);
}

Response::~Response(void) {	return ; }
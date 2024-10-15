/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/13 17:24:27 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"



Response::Response(const Request & src, const Host &host): Request(src) {
	fillContentTypes();
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

	if (_root[0] == '/')
		_root = _root.substr(1,_root.size() - 1);
	if (_root[_root.size() - 1] == '/')
		_root = _root.substr(0,_root.size() - 1);

	std::string 	uri = _request_line["uri"];
	
	_startUri	=	uri;

	// if (_startUri[_startUri.size () - 1] != '/')
	// 	_err = 301;
	
	std::string		tmpUri = uri;
	std::string		checkUri = uri;

	std::cout  << "uri -> " << uri << std::endl; 
	std::string test_path = _root + uri;
	std::cout  << "test_path -> " << test_path << std::endl;
	// if (test_path.find(".") != std::string::npos && _err == 0)
	// 	_err = /* IsARepertory(test_path) */404;
	std::cout << "_err -> " << _err << std::endl;
	
	std::cout << host._Autoindex << "autoindex HOST \n";
/* 	std::string		file_path = _root + uri;
	_startUri = file_path;
	std::cout << "TMP URI --> " << tmpUri << std::endl;
	std::size_t posIndex = tmpUri.find_last_of('/') + 1;
	std::string foundindx = tmpUri.substr(posIndex, tmpUri[tmpUri.size() - 1]);
	std::cout << "foundindx --> " << foundindx << std::endl;
	std::cout << "_autoIndex --> " << _autoIndex << std::endl;
	if (foundindx == "autoindex.html" && _autoIndex) {
		_autoInxPrint = 1;
		_tmpForInx = tmpUri;
	}
	else
		_autoInxPrint = 0;

	std::cout << "_autoInxPrint --> " << _autoInxPrint << std::endl;
	std::cout << file_path <<  "< ----- filepath befor" << std::endl;

	std::cout << file_path << " << ===== file_path " << std::endl;
	if (file_path == "www/home" && !file_path.empty() && file_path[file_path.size() - 1] != '/')
	    file_path += '/';
	struct stat buf;
	if (stat(file_path.c_str(), &buf) < 0 || file_path == "www/")
		_path_file = "www/index.html";
	else {
		for (size_t i = 0; i < _indexPages.size() ; i++) {
			if (file_path[file_path.size() - 1] != '/')
				_path_file = file_path + "/" + _indexPages[i];
			else 
				_path_file = file_path + _indexPages[i];
		}
	} */
}

void	Response::buildErrorPage(int fd, int statusCode,  struct epoll_event & event)
{
	_statusCode  = statusCode;
	std::string	errorPageUri("");
	
	if (!this->_Location.empty())
	{
		// if (RESPONSE)
		// 	std::cout << "STATUSCODE = " << _statusCode << std::endl;
		std::map<int, std::string>::iterator loc = _pagesError.find(statusCode);
		if (loc != _pagesError.end())
		{
			errorPageUri = loc->second;
			std::cout << "STATUSCODE = " << errorPageUri << std::endl;
		}
	}
	if (errorPageUri.empty() || ! readContent(errorPageUri, _body)) 
	{
		// if (RESPONSE)
		// 	std::cerr << "PAS DE PAGE ERROR RECORDED\n";
		std::string errorMsg;
		std::map<int, std::string>::iterator it = _pagesError.find(statusCode);
		if (it != _pagesError.end())
			errorMsg = convertToStr(statusCode) + " " + _pagesError[statusCode];
		else
			errorMsg = "Unknown error " + convertToStr(statusCode);

		_body = "<!DOCTYPE html>\
				<html lang=\"en\">\
				<head>\
				<meta charset=\"UTF-8\">\
				<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
				<link href=\"style.css\" rel=\"stylesheet\">\
				<link href=\"../../style.css\" rel=\"stylesheet\">\
				<title>Error</title>\
				</head>\
				<body>\
				<div class=\"title1\">" + errorMsg + "</div>\
				<div class=\"index\">\
				<a class=\"indexButton\" href=\"/\">go back to home page</a>\
				</div>\
				</body>\
				</html>";

	}
	_headers["content-type"] = "text/html";
	_headers["content-length"] = convertToStr(_body.size());
	SendUltra(fd, event, *this);

}

void	Response::buildAutoindex(int fd, struct epoll_event & event)
{
		std::vector<std::string> filesList;
		DIR *dir = opendir(_startUri.c_str());
		if (!dir)
			ft_perror("ERROR");

		struct dirent *fileRead;
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
                "<style>\n"
                "    body {\n"
                "        font-family: 'Arial', sans-serif;\n"
                "        display: flex;\n"
                "        flex-direction: column;\n"
                "        justify-content: center;\n"
                "        align-items: center;\n"
                "        height: 100vh;\n"
                "        margin: 0;\n"
                "        background-color: #f4f7f6;\n"
                "    }\n"
                "    h1 {\n"
                "        font-size: 48px;\n"
                "        color: #333;\n"
                "        margin-bottom: 20px;\n"
                "    }\n"
				"    h2 {\n"
                "        font-size: 48px;\n"
                "        color: #330;\n"
                "        margin-bottom: 10px;\n"
                "    }\n"
                "    p {\n"
                "        font-size: 18px;\n"
                "        color: #777;\n"
                "        margin-bottom: 40px;\n"
                "    }\n"
                "    .button-container {\n"
                "        display: flex;\n"
                "        gap: 20px;\n"
                "    }\n"
                "    button, .link-button {\n"
                "        padding: 15px 30px;\n"
                "        font-size: 18px;\n"
                "        cursor: pointer;\n"
                "        border: none;\n"
                "        border-radius: 50px;\n"
                "        transition: background-color 0.3s, box-shadow 0.3s;\n"
                "    }\n"
                "    button {\n"
                "        background-color: #3498db;\n"
                "        color: white;\n"
                "    }\n"
                "    button:hover {\n"
                "        background-color: #2980b9;\n"
                "        box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);\n"
                "    }\n"
                "    .link-button {\n"
                "        background-color: #2ecc71;\n"
                "        color: white;\n"
                "        text-decoration: none;\n"
                "        display: inline-block;\n"
                "        line-height: 1.5;\n"
                "    }\n"
                "    .link-button:hover {\n"
                "        background-color: #03eb63;\n"
                "        box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);\n"
                "    }\n"
                "</style>\n"
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
		SendUltra(fd, event, *this);
}


void	Response::BuildPage(int fd, struct epoll_event & event)
{
	// std::map<std::string, std::string>	CONTENT_TYPES;
	// fillContentTypes();
	
	std::ifstream fileRequested(_startUri.c_str());
	if (fileRequested.good() == false)
	{
		// if (RESPONSE)
		// 	std::cerr << "file not good\n";
		return(buildErrorPage(fd, 404, event));
	}
	std::stringstream buffer;
	buffer << fileRequested.rdbuf();
	_body = buffer.str();
	if (_body.size() > _maxBodySize)
	{
		_body = "";
		return (buildErrorPage(fd, 403, event));
	}
	_headers["content-length"] = convertToStr(_body.size());

	size_t pos = _startUri.find_last_of("/");
	if (pos != std::string::npos && (_startUri.begin() + pos + 1) != _startUri.end())
	{
		std::string resourceName = _startUri.substr(pos + 1);
		pos = resourceName.find_last_of(".");
		if (pos != std::string::npos && (resourceName.begin() + pos + 1) != resourceName.end())
		{
			std::string fileExtension = resourceName.substr(pos + 1);
			if (CONTENT_TYPES.find(fileExtension) != CONTENT_TYPES.end())		
			{
				_headers["content-type"] = CONTENT_TYPES[fileExtension];
/* 				continue; */
			}
		}
	}

	SendUltra(fd, event, *this);
}

void	Response::BuildGet(int fd, struct epoll_event & event) {
	/* struct stat buffer; */
	// int			error = 0;
	
	/* std::cout << _path_file << "<----------\n";
 */
/* 	std::string uri = foundUriInLoc(_startUri, this->_Location); */

	/* std::cout  << "uri -> " << uri << std::endl; */
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
	std::cout << "FINAL URI -> " << _startUri << std::endl;
	if (IsARepertory(_startUri) == 3)
	{
		if (_startUri[_startUri.size() -1] != '/')
		{
			// request.statusCode = STATUS_MOVED_PERMANENTLY;
			_statusCode = 301;
			std::string serverName(_serverName);
			if (serverName.empty())
				serverName = _Ip + ":" + convertToStr(_Port);
			else
				serverName += ":" + convertToStr(_Port);
			_headers["location"] = "http://" + serverName + _startUri + "/"; //A mettre ici ou dans builHeaders ?
			return ;
		}
		else
		{
			if (_indexPages.empty())
			{
				for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++)
					{
						std::string index = (*it)[0] == '/' ? (*it).substr(1, std::string::npos) : (*it);
						std::string path;
						path = _startUri + index;
						// if (RESPONSE)
						// 	std::cerr << "path = " << path << std::endl;
						if (IsARepertory(path) == 1)
						{
							// if (RESPONSE)
							// 	std::cerr << "path regular"<< std::endl;
							_startUri = path;
							return (std::cout << "IM HERE 2" << std::endl, BuildPage(fd, event));
						}

					}
			}
			if (_autoInxPrint)
				buildAutoindex(fd, event);
			return(std::cout << "IM HERE" << std::endl, buildErrorPage(fd, 404, event));
		}
	}
	if (IsARepertory(_path_file) == 1)
		return (std::cout << "IM HERE" << std::endl, BuildPage(fd, event));
	else
	{
		std::cout << "IM HERE 404" << std::endl;
		buildErrorPage(fd, 404, event);
	}
}

Response::~Response(void) {	return ; }
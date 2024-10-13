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
	while (true) {
		bool found = false;
        for (std::map<std::string, Location>::iterator it = _Location.begin(); it != _Location.end(); ++it) {
			std::cout << "it->first" << it->first << std::endl;
            if (it->first == uri) {                
				std::cout << "Found matching URI: " << it->first << std::endl;
                found = true;
                break;
            }
        }
        if (found)
            break;

        std::size_t pos = uri.find_last_of('/');
		uri = uri.substr(0, uri.find_last_of('/'));
		std::cout << " --------->  uri    ->" << uri << std::endl;
        if (pos == std::string::npos || pos == 0) {
            uri = "/";
			break;
        } else
            uri = uri.substr(0, pos);
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
	
	std::cout << host._Autoindex << "autoindex HOST \n";
	std::string		file_path = _root + uri;
	_finalUri = file_path;
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
	}
}

void	Response::BuildGet(int fd, struct epoll_event & event) {
	struct stat buffer;
	int			error = 0;
	
	std::cout << _path_file << "<----------\n";
	
	std::size_t pos  = _path_file.find_last_of('/') + 1;
	std::string tmp = _path_file.substr(pos, _path_file.size() -1);
	std::cout  << "tmp -> " << tmp << std::endl;
	size_t check = 0;
	std::cout << "AUTOINDEXPRINT --> " << _autoInxPrint << std::endl;
	if (_autoInxPrint) {
		std::vector<std::string> filesList;
		DIR *dir = opendir(_finalUri.c_str());
		if (!dir)
			ft_perror("ERROR");

		struct dirent *fileRead;
		while ((fileRead = readdir(dir)) != NULL)
			if (strcmp(fileRead->d_name, ".") != 0 || (strcmp(fileRead->d_name, "..") != 0 && _finalUri != "/"))
				filesList.push_back(fileRead->d_name);

		_headers["content-type"] = "text/html";
		_body = "<!DOCTYPE html>\n"
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
                "<h2 class=\"autoindex\">\n" + _finalUri + "\n"
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
			_body += "<div class=\"button-container\"><a class=\"link-button\" href=" + hyperlink + ">" + filename + "</a></div>\n";
		}

		_body += "</body>\n";
		_body += "</html>";

		closedir(dir);
		 
    	std::ostringstream oss;  // Crea un flusso di output
    	oss << _body.size();           // Scrivi il numero nel flusso
		std::string response_header = "HTTP/1.1 200 OK\r\n";
		response_header += "Content-Length: " + oss.str() + "\r\n";
    	response_header += "Content-Type: text/html\r\n"; // forma html
    	// response_header += "Content-Type: text/css\r\n"; // forma html
    	response_header += "\r\n";  // Fine dell'header
		error = send(fd, response_header.c_str(), response_header.length(), 0);
    	if (error == -1) {
			error_send(fd, event, "Error in send for header");
    	    return;
    	}

    	error = send(fd, _body.c_str(), _body.length(), 0);
    	if (error == -1) {
    	    error_send(fd, event, "Error in send for the content of the fd");
			return ;
    	}
    }
	if (_indexPages.size() == 0)
		check = 404;

	for (size_t i = 0; i < _indexPages.size(); i++) {
		if (tmp != _indexPages[i]) {
			check = 404;
			break;
		}
	}
	if (stat(_path_file.c_str(), &buffer) != 0 || (_err == 404) 
		|| (_err == 1 && check == 404)) {
		// std::cout << "BAD FILE PATH\n;
		std::ifstream file(_host._PageError[404].c_str());
		std::string file_con;
		if (file.good()) {
			std::ostringstream ss;
			ss << file.rdbuf();
			file_con = ss.str();
			std::ostringstream os;
			os << file_con.length();
    		std::string response_header = "HTTP/1.1 404 OK\r\n";
			response_header += "Content-Length: " + os.str() + "\r\n";
    		response_header += "Content-Type: text/html\r\n"; // forma html
    		response_header += "\r\n";  // Fine dell'header
			error = send(fd, response_header.c_str(), response_header.length(), 0);
    		if (error == -1) {
				error_send(fd, event, "Error in send for header");
    		    return;
			}
    		error = send(fd, file_con.c_str(), file_con.length(), 0);
    		if (error == -1) {
				error_send(fd, event, "Error in send for the content of the fd");
				return;
			}
        return;
		}
	}
	std::ifstream file(_path_file.c_str());
    std::string file_content;
	std::cout << "path --> " << _path_file << std::endl;
    if (file.good()) {
        std::ostringstream ss;
        ss << file.rdbuf();
        file_content = ss.str();
    } else {
        std::string error_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 23\r\n\r\n<h1>404 Not Found</h1>";
        error = send(fd, error_response.c_str(), error_response.length(), 0);
		if (error == -1)
			error_send(fd, event, "Error in send for error");
        return;
    }

    std::ostringstream oss;
    oss << file_content.length();

    std::string response_header = "HTTP/1.1 200 OK\r\n";
    response_header += "Content-Length: " + oss.str() + "\r\n";
    response_header += "Content-Type: text/html\r\n"; // forma html
    response_header += "\r\n";  // Fine dell'header

    error = send(fd, response_header.c_str(), response_header.length(), 0);
    if (error == -1) {
		error_send(fd, event, "Error in send for header");
        return;
    }

    error = send(fd, file_content.c_str(), file_content.length(), MSG_NOSIGNAL);
    if (error == -1)
		error_send(fd, event, "Error in send for the content of the fd");
}

Response::~Response(void) {	return ; }
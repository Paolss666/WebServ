/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:51 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/24 15:12:46 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

ErrorConfFile::ErrorConfFile(std::string msg) throw(): _errorMsg(msg) { return ; }
ErrorConfFile::~ErrorConfFile() throw() { return ; }
const char *ErrorConfFile::what() const throw() { return _errorMsg.c_str(); }

ErrorFdManipulation::ErrorFdManipulation(std::string msg, int code) throw(): _errorMsg(msg), _code(code) { return ; }
ErrorFdManipulation::~ErrorFdManipulation() throw() { return ; }
const char *ErrorFdManipulation::what() const throw() {
	std::ostringstream	oss;

	oss << _code;
	_errorMsg += " | Response code: ";
	_errorMsg += oss.str();
	return _errorMsg.c_str();
}

ErrorRequest::ErrorRequest(std::string msg, int code) throw(): _errorMsg(msg), _code(code) { return ; }
ErrorRequest::~ErrorRequest() throw() { return ; }
const char *ErrorRequest::what() const throw() {
	std::ostringstream oss;

	oss << _code;
	_errorMsg += " | Response code: ";
	_errorMsg += oss.str();
	return _errorMsg.c_str();
}

ErrorResponse::ErrorResponse(std::string msg, int code) throw(): _errorMsg(msg), _code(code) { return ; }
ErrorResponse::~ErrorResponse() throw() { return ; }
const char *ErrorResponse::what() const throw() {
	std::ostringstream oss;

	oss << _code;
	_errorMsg += " | Response code: ";
	_errorMsg += oss.str();
	return _errorMsg.c_str();
}

void	ft_perror(const char * message) { std::cerr << BOLD RED "Error: " RESET RED << message << RESET << std::endl; }

std::string build_error_page(int code, std::string image) {
	std::string line, body;
	std::ostringstream oss;
	
	std::ifstream file("www/error.html");
	if (!file.is_open())
		ft_perror("In the opening of the error page");
	
	oss << code;

	while (std::getline(file, line)) {
		if (line.find("<!-- status -->") != std::string::npos)
			line.replace(line.find("<!-- status -->"), 15, oss.str());
		if (line.find("<!-- image -->") != std::string::npos)
			line.replace(line.find("<!-- image -->"), 14, image);
		body += line;
	}
	file.close();
	return body;
}

template <typename T>
void	send_error_page(Host & host, int i, const T & e, int *_nb_keepalive, std::string uri) {
	std::string status, response, body, line, image;
	std::fstream file;
	std::ostringstream oss, str_code, str_port;
	ft_perror(e.what());
	if (_nb_keepalive)
		*_nb_keepalive -= 1;

	// Set the status
	switch (e._code) {
		case ERR_CODE_MOVE_PERM:
			status = ERR_NAME_MOVE_PERM;
			break;
		case ERR_CODE_BAD_REQUEST:
			status = ERR_NAME_BAD_REQUEST;
			break;
		case ERR_CODE_FORBIDDEN:
			status = ERR_NAME_FORBIDDEN;
			break;
		case ERR_CODE_NOT_FOUND:
			status = ERR_NAME_NOT_FOUND;
			break;
		case ERR_CODE_MET_NOT_ALLOWED:
			status = ERR_NAME_MET_NOT_ALLOWED;
			break;
		case ERR_CODE_TIMEOUT:
			status = ERR_NAME_TIMEOUT;
			break;
		case ERR_CODE_CONFLICT:
			status = ERR_NAME_CONFLICT;
			break;
		case ERR_CODE_LENGTH_REQUIRED:
			status = ERR_NAME_LENGTH_REQUIRED;
			break;
		case ERR_CODE_PAYLOAD_TOO_LARGE:
			status = ERR_NAME_PAYLOAD_TOO_LARGE;
			break;
		case ERR_CODE_URI_TOO_LONG:
			status = ERR_NAME_URI_TOO_LONG;
			break;
		case ERR_CODE_UNSUPPORTED_MEDIA:
			status = ERR_NAME_UNSUPPORTED_MEDIA;
			break;
		case ERR_CODE_REQ_HEADER_FIELDS:
			status = ERR_NAME_REQ_HEADER_FIELDS;
			break;
		case ERR_CODE_INTERNAL_ERROR:
			status = ERR_NAME_INTERNAL_ERROR;
			break;
		case ERR_CODE_SERVICE_UNAVAIL:
			status = ERR_NAME_SERVICE_UNAVAIL;
			break;
		case ERR_CODE_HTTP_VERSION:
			status = ERR_NAME_HTTP_VERSION;
			break;
		default:
			status = "Unknown";
			break;
	}
	std::string found = foundGoodUri(host, uri);
	// std::cout << "FOUND  --> " << found << std::endl;
	if(host._errorFlag)
	{
		if (host._Location[found].getFlagErrorPages())
		{
			const std::map<int, std::string>& pagesError = host._Location[found].getPagesError();  // Store the map in a local variable
			std::map<int, std::string>::const_iterator it = pagesError.find(e._code);  // Use the map here
			// std::map<int, std::string>::iterator it = host._Location[found].getPagesError().find(e._code);
			if (it != host._Location[found].getPagesError().end()) {
				std::ifstream fileRequested(it->second.c_str());
				if (fileRequested.good() == false)
					throw ErrorResponse("In the opening of the file requested", ERR_CODE_NOT_FOUND);

				std::stringstream	 buffer;
				buffer << fileRequested.rdbuf();

				body = buffer.str(); // Print the value // Print the value
				
			} 
		}
		else
		{
			const std::map<int, std::string>& pagesError = host.getPagesError();  // Store the map in a local variable
			std::map<int, std::string>::const_iterator it = pagesError.find(e._code);  // Use the map here
			// std::map<int, std::string>::iterator it = host.getPagesError().find(e._code);
			if (it != host.getPagesError().end()) {
			    // std::cout << it->second << " <-- found file\n"; 
				std::ifstream fileRequested(it->second.c_str());
				if (fileRequested.good() == false)
					throw ErrorResponse("In the opening of the file requested", ERR_CODE_NOT_FOUND);

				std::stringstream	 buffer;
				buffer << fileRequested.rdbuf();

				body = buffer.str(); // Print the value
			} 
		}

	}
	else if (host._Location[found].getFlagErrorPages())
	{
		const std::map<int, std::string>& pagesError = host._Location[found].getPagesError();  // Store the map in a local variable
		std::map<int, std::string>::const_iterator it = pagesError.find(e._code);  // Use the map here
			// std::map<int, std::string>::iterator it= host._Location[found].getPagesError().find(e._code);
		if (it != host._Location[found].getPagesError().end()) {
		    // std::cout << it->second << " <-- found file\n";
				std::ifstream fileRequested(it->second.c_str());
				if (fileRequested.good() == false)
					throw ErrorResponse("In the opening of the file requested", ERR_CODE_NOT_FOUND);

				std::stringstream	 buffer;
				buffer << fileRequested.rdbuf();

				body = buffer.str();  // Print the value
		}
		else
		{
			str_code << e._code;
			if (status == "Unkown")
				image = "<img src=\"https://http.cat/450.jpg\">";
			else
				image = "<img src=\"https://http.cat/" + str_code.str() + ".jpg\">";	
			body = "<!DOCTYPE html>\
					<html lang=\"en\">\
					<head>\
					<meta charset=\"UTF-8\">\
					<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
					<title>" + status + "</title>\
					</head>\
					<body>\
					<div class=\"img\">" + image + "</div>\
					<div class=\"index\">\
					<a class=\"indexButton\" href=\"/index.html\">go back to home page</a>\
					</div>\
					</body>\
					</html>";
		}
	}
	else
	{
		str_code << e._code;
		if (status == "Unkown")
			image = "<img src=\"https://http.cat/450.jpg\">";
		else
			image = "<img src=\"https://http.cat/" + str_code.str() + ".jpg\">";	
		body = body = build_error_page(e._code, image);
	}
		// Set the response
	oss << "HTTP/1.1 " << e._code << " " << status << "\r\n";
	if (host._name.empty()) {
		str_port << host._port;
		oss << "Server: " << host._raw_ip << ":" << str_port.str() << "\r\n";	
	} else
		oss << "Server: " << host._name << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << body.size() << "\r\n";
	oss << "Connection: close\r\n";
	oss << "\r\n";
	oss << body;
	response = oss.str();
	// Send the response
	if (send(host._events[i].data.fd, response.c_str(), response.size(), 0) < 0)
		ft_perror(("In the send of error page: " + str_code.str()).c_str());
	// Close the connection
	ft_close(host._events[i].data.fd);
	epoll_ctl(host._fdEpoll, EPOLL_CTL_DEL, host._events[i].data.fd, NULL);
	if (host._requests.find(host._events[i].data.fd) != host._requests.end()) {
		host._requests.erase(host._events[i].data.fd);
		host._responses.erase(host._events[i].data.fd);
	}
}

template void send_error_page<ErrorFdManipulation>(Host&, int, const ErrorFdManipulation&, int*, std::string);
template void send_error_page<ErrorRequest>(Host&, int, const ErrorRequest&, int*, std::string);
template void send_error_page<ErrorResponse>(Host&, int, const ErrorResponse&, int*, std::string);


std::string foundGoodUri(Host & host, std::string uri)
{
	// std::cout << "URI -> " << uri << std::endl;
	int i = 1;
	while (i)
	{
        for (std::map<std::string, Location>::iterator it = host._Location.begin(); it != host._Location.end(); ++it) {
			// std::cout << "it->first" << it->first << std::endl;
            if (it->first == uri)
			{                
				// std::cout << "Found matching URI: " << uri << std::endl;
            	i = 0;
				break;
			}
        }
        	std::size_t pos = uri.find_last_of('/');
			uri = uri.substr(0, uri.find_last_of('/'));
			// std::cout << " --------->  uri    ->" << uri << std::endl;
        	if (pos == std::string::npos || pos == 0) {
        	    uri = "/";
				break;
			}
	}
	return (uri);
}

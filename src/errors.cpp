/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:51 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/15 19:44:47 by bdelamea         ###   ########.fr       */
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

void	ft_perror(const char * message) { std::cerr << BOLD RED "Error: " RESET RED << message << RESET << std::endl; }

void	send_error_page(Host & host, int fd, int code) {
	std::string status, response, body, line, image;
	std::fstream file;
	std::ostringstream oss, str_code, str_port;

	// Set the status
	switch (code) {
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

	str_code << code;
	
	// Set the body
	if (status == "Unkown")
		image = "<img src=\"https://http.cat/450.jpg\">";
	else
		image = "<img src=\"https://http.cat/" + str_code.str() + ".jpg\">";	
	body = "<!DOCTYPE html>\
			<html lang=\"en\">\
			<head>\
			<meta charset=\"UTF-8\">\
			<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
			<link href=\"style.css\" rel=\"stylesheet\">\
			<link href=\"../../style.css\" rel=\"stylesheet\">\
			<title>" + status + "</title>\
			</head>\
			<body>\
			<div class=\"img\">" + image + "</div>\
			<div class=\"index\">\
			<a class=\"indexButton\" href=\"/\">go back to home page</a>\
			</div>\
			</body>\
			</html>";

	// Set the response
	oss << "HTTP/1.1 " << code << " " << status << "\r\n";
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
	if (send(host._events[fd].data.fd, response.c_str(), response.size(), 0) < 0)
		ft_perror(("Error in the send of error page: " + str_code.str()).c_str());
	
	// Close the connection
	epoll_ctl(host._fdEpoll, EPOLL_CTL_DEL, host._events[fd].data.fd, NULL);
	ft_close(host._events[fd].data.fd);
	host._requests.erase(host._events[fd].data.fd);
	host._responses.erase(host._events[fd].data.fd);
}

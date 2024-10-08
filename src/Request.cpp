/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 09:43:56 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/08 17:08:24 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Request::Request(void): _host(*(Host*)NULL), _event(*(epoll_event*)NULL), _raw(""), _request_line(), _headers(), _body(""), _b_content_length(false) { return ; }

Request::Request(Host & host, struct epoll_event & event, std::string const & raw): _host(host), _event(event), _raw(raw) { return ; }

Request::Request(Request const & src): _host(src._host), _event(src._event) {
	_raw = src._raw;
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_b_content_length = src._b_content_length;
}

Request::~Request(void) { return ; }

void	Request::append(std::string const & data) { this->_raw.append(data); }

void	Request::check_request_line(void) {

	// Check the method
	if (!(_request_line["method"] == "GET" || _request_line["method"] == "POST" || _request_line["method"] == "DELETE"))
		throw ErrorRequest("Error in the request: method not supported", 666);
	
	// Check the protocol
	if (_request_line["protocol"] != "HTTP/1.1\r")
		throw ErrorRequest("Error in the request: protocol not supported", 666);

	// Check the uri
	if (_request_line["uri"].empty())
		throw ErrorRequest("Error in the request: uri not found", 666);
}

void	Request::check_headers(void) {

	// Check host
	std::ostringstream	oss;
	oss << _host._port;
	
	if (!_host._name.empty()
		&& (_headers.find("Host") == _headers.end()
			|| (_headers["Host"] != _host._name && _headers["Host"] != _host._raw_ip + ":" + oss.str())))
	{
		throw ErrorRequest("Error in the request: host error", 666);
	}

	// Check content for POST
	if (_request_line["method"] == "POST") {
		if (_headers.find("Content-Length") == _headers.end() || _headers.find("Content-Type") == _headers.end())
			throw ErrorRequest("Error in the request: content-length not found", 666);
		if (_headers["Content-Length"].empty() || _headers["Content-Length"].find_first_not_of("0123456789") != std::string::npos)
			throw ErrorRequest("Error in the request: content-length mismatch", 666);
		if (_headers["Content-Type"].empty())
			throw ErrorRequest("Error in the request: content-type not found", 666);
	}

	return ;
}

void	Request::check_body(void) { return ; }

std::string	Request::trim(std::string & str) {
	size_t	start = str.find_first_not_of(" \t");
	size_t	end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}

void	Request::parse(void) {
	std::istringstream					iss(this->_raw);
	std::string							line, buffer, method, uri, protocol, key, value;
	char								ch;
	long								len, max_content_len;

	// Skip the empty lines
	while (std::getline(iss, line, '\n') && line == "\r");

	// Parse the request line
	std::istringstream	iss_line(line);
	if (!(std::getline(iss_line, method, ' ') && std::getline(iss_line, uri, ' ') && std::getline(iss_line, protocol, ' ')))
		throw ErrorRequest("Error in the request: method not well formatted", 666);
	_request_line["method"] = method;
	_request_line["uri"] = uri;
	_request_line["protocol"] = protocol;
	
	// Parse the headers
	while (std::getline(iss, line, '\n') && line != "\r") {
		if (line[line.size() - 1] == '\r') {
			buffer.append(line);
		} else {
			buffer = line;
			continue ;
		}
		
		std::istringstream	iss_line(line.erase(buffer.size() - 1));
		
		if (!(std::getline(iss_line, key, ':') && std::getline(iss_line, value)))
			throw ErrorRequest("Error in the request: header not well formatted", 666);
		// Remove leading and trailing whitespaces
		key = trim(key);
		value = trim(value);
		_headers[key] = value;
		buffer.clear();
	}
	if (_headers.empty())
		throw ErrorRequest("Error in the request: end of headers not found", 666);

	// Parse the body
	if (_headers.find("Content-Length") != _headers.end()) {
		if (_request_line["method"] == "POST") {
			max_content_len = long(std::atof(_headers["Content-Length"].c_str()));
			len = 0;
			while (iss.get(ch)) {
				len++;
				if (len > max_content_len || len > long(_host._maxBodySize))
					throw ErrorRequest("Error in the request: body too long", 666);
				_body.append(1, ch);
			}
			if (len != max_content_len)
				throw ErrorRequest("Error in the request: body not complete", 666);	
		}
	}

	// Check the request
	check_request_line();
	check_headers();
	check_body();

	// Display the request
	print_request(_request_line, _headers, _body);
}
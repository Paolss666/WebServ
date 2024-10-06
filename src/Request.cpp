/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 09:43:56 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/06 18:29:39 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// include "webserv.hpp"

Request::Request(void): _b_keepalive(false), _b_content_length(false) { return ; }

Request::Request(std::string const & raw): _raw(raw), _b_keepalive(false), _b_content_length(false) { return ; }

Request::~Request(void) { return ; }

void	Request::append(std::string const & data) { this->_raw.append(data); }

void	Request::check_request_line(void) {

	// Check the method
	if (!(_request_line["method"] == "GET" || _request_line["method"] == "POST" || _request_line["method"] == "DELETE"))
		throw ErrorRequest("Error in the request: method not supported");
	
	// Check the protocol
	if (_request_line["protocol"] != "HTTP/1.1\r")
		throw ErrorRequest("Error in the request: protocol not supported");

	// Check the uri
	if (_request_line["uri"].empty())
		throw ErrorRequest("Error in the request: uri not found");
}

void	Request::check_headers(Host & host) {

	// Check host
	std::ostringstream	oss;
	oss << host._port;
	
	if (!host._name.empty()
		&& (_headers.find("Host") == _headers.end()
			|| (_headers["Host"] != host._name && _headers["Host"] != host._raw_ip + ":" + oss.str())))
	{
		throw ErrorRequest("Error in the request: host error");
	}

	// Check content for POST
	if (_request_line["method"] == "POST") {
		if (_headers.find("Content-Length") == _headers.end() || _headers.find("Content-Type") == _headers.end())
			throw ErrorRequest("Error in the request: content-length not found");
		if (_headers["Content-Length"].empty() || _headers["Content-Length"].find_first_not_of("0123456789") != std::string::npos)
			throw ErrorRequest("Error in the request: content-length mismatch");
		if (_headers["Content-Type"].empty())
			throw ErrorRequest("Error in the request: content-type not found");
	}

	// Check keep alive
	if (_headers.find("Connection") != _headers.end() && _headers["Connection"] == "keep-alive"
		&& host._n_b_keepalive < host._max_keepalive) {
			if (_b_keepalive)
				host._n_b_keepalive++;
			_b_keepalive = true;
	}
	return ;
}

void	Request::check_body(void) { return ; }

std::string	Request::trim(std::string & str) {
	size_t	start = str.find_first_not_of(" \t");
	size_t	end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}

void	Request::parse(Host & host) {
	std::istringstream					iss(this->_raw);
	std::string							line, buffer, method, uri, protocol, key, value;
	char								ch;
	long								len, max_content_len;

	// Skip the empty lines
	while (std::getline(iss, line, '\n') && line == "\r");

	// Parse the request line
	std::istringstream	iss_line(line);
	if (!(std::getline(iss_line, method, ' ') && std::getline(iss_line, uri, ' ') && std::getline(iss_line, protocol, ' ')))
		throw ErrorRequest("Error in the request: method not well formatted");
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
			throw ErrorRequest("Error in the request: header not well formatted");
		// Remove leading and trailing whitespaces
		key = trim(key);
		value = trim(value);
		_headers[key] = value;
		buffer.clear();
	}
	if (_headers.empty())
		throw ErrorRequest("Error in the request: end of headers not found");

	// Parse the body
	if (_headers.find("Content-Length") != _headers.end()) {
		if (_request_line["method"] == "POST") {
			max_content_len = long(std::atof(_headers["Content-Length"].c_str()));
			len = 0;
			while (iss.get(ch)) {
				len++;
				if (len > max_content_len || len > long(host._maxBodySize))
					throw ErrorRequest("Error in the request: body too long");
				_body.append(1, ch);
			}
			if (len != max_content_len)
				throw ErrorRequest("Error in the request: body not complete");	
		}
	}

	// Check the request
	check_request_line();
	check_headers(host);
	check_body();

	// Display the request
	print_request(_request_line, _headers, _body);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 09:43:56 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/09 17:38:01 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Request::Request(void): _host(*(Host*)NULL), _event(*(epoll_event*)NULL), _raw(""), _request_line(), _headers(), _body(""), _b_content_length(false), _stage(-1), _eof(1) { return ; }

Request::Request(Host & host, struct epoll_event & event, std::string const & raw): _host(host), _event(event), _raw(raw), _stage(-1), _eof(1) { return ; }

Request::Request(Request const & src): _host(src._host), _event(src._event), _stage(-1), _eof(1) {
	_raw = src._raw;
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_b_content_length = src._b_content_length;
}

Request::~Request(void) { return ; }

void	Request::append(std::string const & data) { _raw.append(data); }

void	Request::pnc_request_line(std::istringstream & iss) {
	std::string	line, method, uri, protocol;

	// Skip the empty lines
	while (std::getline(iss, line, '\n') && line == "\r");

	// Check again if there is a whole line
	if (line.find("\r") == std::string::npos)
		return ;

	// Parse the request line
	std::istringstream	iss_line(line);
	if (!(std::getline(iss_line, method, ' ') && std::getline(iss_line, uri, ' ') && std::getline(iss_line, protocol, ' ')))
		throw ErrorRequest("Error in the request: method not well formatted", ERR_CODE_BAD_REQUEST);
	_request_line["method"] = method;
	_request_line["uri"] = uri;
	_request_line["protocol"] = protocol;

	// Check the method
	if (!(_request_line["method"] == "GET" || _request_line["method"] == "POST" || _request_line["method"] == "DELETE"))
		throw ErrorRequest("Error in the request: method not supported", ERR_CODE_MET_NOT_ALLOWED);
	
	// Check the protocol
	if (_request_line["protocol"] != "HTTP/1.1\r")
		throw ErrorRequest("Error in the request: protocol not supported", ERR_CODE_HTTP_VERSION);

	// Check the uri
	if (_request_line["uri"].empty())
		throw ErrorRequest("Error in the request: uri not found", ERR_CODE_BAD_REQUEST);
}

void	Request::pnc_headers(std::istringstream & iss) {
	std::ostringstream	oss;
	std::string			line, buffer, key, value;

	while (std::getline(iss, line, '\n') && line != "\r") {
		if (line[line.size() - 1] == '\r') {
			buffer.append(line);
		} else {
			buffer = line;
			continue ;
		}
		
		std::istringstream	iss_line(line.erase(buffer.size() - 1));
		
		if (!(std::getline(iss_line, key, ':') && std::getline(iss_line, value)))
			throw ErrorRequest("Error in the request: header not well formatted", ERR_CODE_BAD_REQUEST);
		// Remove leading and trailing whitespaces
		key = trim(key);
		value = trim(value);
		_headers[key] = value;
		buffer.clear();
	}
	
	if (_headers.empty())
		throw ErrorRequest("Error in the request: end of headers not found", ERR_CODE_BAD_REQUEST);

	// Check host
	oss << _host._port;
	if (!_host._name.empty() && (_headers.find("Host") == _headers.end() || (_headers["Host"] != _host._name && _headers["Host"] != _host._raw_ip + ":" + oss.str())))
		throw ErrorRequest("Error in the request: host error", 666);

	// Check content for POST
	if (_request_line["method"] == "POST") {
		if (_headers.find("Content-Length") == _headers.end())
			throw ErrorRequest("Error in the request: content-length not found", ERR_CODE_LENGTH_REQUIRED);
		if (_headers.find("Content-Type") == _headers.end() || _headers["Content-Type"].empty())
			throw ErrorRequest("Error in the request: content-length not found", ERR_CODE_UNSUPPORTED_MEDIA);
		if (_headers["Content-Length"].empty() || _headers["Content-Length"].find_first_not_of("0123456789") != std::string::npos)
			throw ErrorRequest("Error in the request: content-length mismatch", ERR_CODE_BAD_REQUEST);
	}
}

void	Request::pnc_body(std::istringstream & iss) {
	char			ch;
	long			len, max_content_len;
	std::string		line;
	std::streampos	last_pos;

	// Check the body size
	if (iss.str().size() > MAX_BODY_SIZE || iss.str().size() > _host._maxBodySize
		|| iss.str().size() > size_t(std::atof(_headers["Content-Length"].c_str())))
		throw ErrorRequest("Error in the request: body too long", ERR_CODE_BAD_REQUEST);

	if (_eof > 0)
		return ;

	// Skip the empty lines and get back at the start of the body
	last_pos = iss.tellg();
	while (std::getline(iss, line, '\n') && line == "\r")
		last_pos = iss.tellg();
	iss.seekg(last_pos);

	if (_headers.find("Content-Length") != _headers.end()) {
		if (_request_line["method"] == "POST") {
			max_content_len = long(std::atof(_headers["Content-Length"].c_str()));
			len = 0;
			while (iss.get(ch)) {
				len++;
				_body.append(1, ch);
			}
			if (len != max_content_len)
				throw ErrorRequest("Error in the request: body not complete", ERR_CODE_BAD_REQUEST);	
		}
	}

	_stage = BODY_DONE;
}

void	Request::parse() {
	std::istringstream	iss(_raw);
	long				header_len;
	std::string			line;

	// Parse the request line
	if (_stage == -1) {

		// Check if there is a whole line
		if (_raw.find("\r\n") == std::string::npos && _raw.length() < MAX_URI_SIZE + 16)
			return ;
		else if (_raw.length() >= MAX_URI_SIZE + 16)
			throw ErrorRequest("Error in the request: request line too long", ERR_CODE_URI_TOO_LONG);
		
		// Parse & Check the request line
		pnc_request_line(iss);

		// Mark the request line as done & store the end of the request line
		_stage = RL_DONE;
		if (iss.tellg() < 0)
			return ;
		_raw = _raw.substr(static_cast<std::string::size_type>(iss.tellg()));
	}
	
	// Parse the headers
	if (_stage == RL_DONE) {
		iss.clear();
		iss.str(_raw);

		// Check if there is a whole header
		header_len = 0;
		while (std::getline(iss, line, '\n') && line != "\r") {
			header_len += line.size();
			if (header_len > MAX_HEADER_SIZE)
				throw ErrorRequest("Error in the request: headers too long", ERR_CODE_REQ_HEADER_FIELDS);
		}
		if (header_len == 0 && line == "\r")
			throw ErrorRequest("Error in the request: end of headers not found", ERR_CODE_BAD_REQUEST);
		if (line != "\r")
			return ;
		
		// Parse & Check the headers
		iss.clear();
		iss.str(_raw);
		pnc_headers(iss);

		// Mark the headers as done & store the end of the request line
		_stage = HEADERS_DONE;
		if (iss.tellg() < 0)
			return ;
		_raw = _raw.substr(static_cast<std::string::size_type>(iss.tellg()));
	}

	// Parse the body
	if (_stage == HEADERS_DONE) {
		iss.clear();
		iss.str(_raw);

		// Check if a body is expected
		if (_request_line["method"] == "POST")
			pnc_body(iss);
		else
			_stage = BODY_DONE;
	}
}
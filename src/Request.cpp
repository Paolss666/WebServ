/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 09:43:56 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/06 16:49:55 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// include "webserv.hpp"

Request::Request(void): b_keepalive(false), b_content_length(false) { return ; }

Request::Request(std::string const & raw): raw(raw), b_keepalive(false), b_content_length(false) { return ; }

Request::~Request(void) { return ; }

void	Request::append(std::string const & data) { this->raw.append(data); }

void	Request::ft_check_request_line(void) {

	// Check the method
	if (!(request_line["method"] == "GET" || request_line["method"] == "POST" || request_line["method"] == "DELETE"))
		throw ErrorRequest("Error in the request: method not supported");
	
	// Check the protocol
	if (request_line["protocol"] != "HTTP/1.1\r")
		throw ErrorRequest("Error in the request: protocol not supported");

	// Check the uri
	if (request_line["uri"].empty())
		throw ErrorRequest("Error in the request: uri not found");
}

void	Request::ft_check_headers(Host & host) {

	// Check host
	if (!host._name.empty() && (headers.find("Host") == headers.end() || headers["Host"] != host._name))
		throw ErrorRequest("Error in the request: host error");

	// Check content for POST
	if (request_line["method"] == "POST") {
		if (headers.find("Content-Length") == headers.end() || headers.find("Content-Type") == headers.end())
			throw ErrorRequest("Error in the request: content-length not found");
		if (headers["Content-Length"].empty() || headers["Content-Length"].find_first_not_of("0123456789") != std::string::npos)
			throw ErrorRequest("Error in the request: content-length mismatch");
		if (headers["Content-Type"].empty())
			throw ErrorRequest("Error in the request: content-type not found");
	}

	// Check keep alive
	if (headers.find("Connection") != headers.end() && headers["Connection"] == "keep-alive"
		&& host._nb_keepalive < host._max_keepalive) {
			if (b_keepalive)
				host._nb_keepalive++;
			b_keepalive = true;
	}
	return ;
}

void	Request::ft_check_body(void) { return ; }

void	Request::parse(Host & host) {
	std::istringstream					iss(this->raw);
	std::string							line, buffer, method, uri, protocol, key, value;
	char								ch;
	long								len, max_content_len;

	// Skip the empty lines
	while (std::getline(iss, line, '\n') && line == "\r");

	// Parse the request line
	std::istringstream	iss_line(line);
	if (!(std::getline(iss_line, method, ' ') && std::getline(iss_line, uri, ' ') && std::getline(iss_line, protocol, ' ')))
		throw ErrorRequest("Error in the request: method not well formatted");
	request_line["method"] = method;
	request_line["uri"] = uri;
	request_line["protocol"] = protocol;
	
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
		
		if (key == "Content-Length" && !b_content_length)
			b_content_length = true;
		else if (key == "Content-Length" && b_content_length)
			throw ErrorRequest("Error in the request: multiple content-length headers");
		
		headers[key] = value;
		buffer.clear();
	}
	if (headers.empty())
		throw ErrorRequest("Error in the request: end of headers not found");

	// Parse the body
	if (headers.find("Content-Length") != headers.end()) {
		if (request_line["method"] == "POST") {
			max_content_len = long(std::atof(headers["Content-Length"].c_str()));
			len = 0;
			while (iss.get(ch)) {
				len++;
				if (len > max_content_len || len > long(host._maxBodySize))
					throw ErrorRequest("Error in the request: body too long");
				body.append(1, ch);
			}
			if (len != max_content_len)
				throw ErrorRequest("Error in the request: body not complete");	
		}
	}

	// Check the request
	ft_check_request_line();
	ft_check_headers(host);
	ft_check_body();

	// Display the request
	print_request(request_line, headers, body);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 09:43:56 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/03 12:03:24 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Request::Request(void): p_step(NOT_STARTED) { return ; }

Request::Request(std::string const & raw): raw(raw), p_step(NOT_STARTED) { return ; }

Request::~Request(void) { return ; }

void	Request::append(std::string const & data) { this->raw.append(data); }

std::map<std::string, std::string>	ft_get_method(std::istringstream & iss_line) {
	std::string							line, method, uri, protocol;
	std::map<std::string, std::string>	res;

	// Get the method, uri and protocol
	if (!(std::getline(iss_line, method, ' ') && std::getline(iss_line, uri, ' ') && std::getline(iss_line, protocol, ' ')))
		throw ErrorRequest("Error in the request: method not well formatted");
	
	std::cout << "Raw: ";
	print_with_hex(iss_line.str());
	std::cout << std::endl;

	std::cout << "Method: ";
	print_with_hex(method);
	std::cout << std::endl;

	std::cout << "URI: ";
	print_with_hex(uri);
	std::cout << std::endl;

	std::cout << "Protocol: ";
	print_with_hex(protocol);
	std::cout << " of length " << protocol.length() << std::endl;

	// Check the method
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw ErrorRequest("Error in the request: method not supported");
	
	// Check the protocol
	if (protocol != "HTTP/1.1\r")
		throw ErrorRequest("Error in the request: protocol not supported");
	
	// Save the method
    res["method"] = method;
    res["uri"] = uri;
	return res;
}

void	Request::parse(void) {
	std::istringstream					iss(this->raw);
	std::string							line, key, value;
	
	std::cout << CYAN BOLD "Raw request received -----------\n" RESET << raw << CYAN BOLD "End of raw -----------\n" RESET << std::endl;
	
	// Parse the method
	if (p_step == NOT_STARTED) {
		
		// Get the first line
		p_step =  METHOD;
		if (!(std::getline(iss, line, '\n') && line.find('\r')))
			throw ErrorRequest("Error in the request: method not found");
		std::istringstream	iss_line(line);
		request_line = ft_get_method(iss_line);
	}
	
	// Parse the headers
	else if (p_step == METHOD) {
		p_step = HEADERS;
		while (!std::getline(iss, line, '\n') && line != "\r") {
			std::istringstream	iss_line(line);

			if (!(std::getline(iss_line, key, ':') && std::getline(iss_line, value, ':')))
				throw ErrorRequest("Error in the request: header not well formatted");
			headers[key] = value;
		}
		if (line != "\r")
			throw ErrorRequest("Error in the request: end of headers not found");
	}
	
	// Parse the body
	else if (p_step == HEADERS) {
		p_step = BODY;
		while (!std::getline(iss, line, '\n'))
			body += line;
	}

	// Display the request
	std::cout << CYAN << "Method: " << WHITE << request_line["method"] << std::endl;
	std::cout << CYAN << "URI: " << WHITE << request_line["uri"] << std::endl;
	std::cout << CYAN << "Protocol: " << WHITE << "HTTP/1.1" << std::endl;
	std::cout << CYAN << "Headers:" << WHITE << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << CYAN << it->first << ": " << WHITE << it->second << std::endl;
	std::cout << CYAN << "Body: " << WHITE << body << std::endl;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 09:43:56 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/02 12:09:45 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Request::Request(std::string raw): raw(raw), p_step(NOT_STARTED) { return ; }

Request::~Request(void) { return ; }

std::map<std::string, std::string>	ft_get_method(std::istringstream & iss_raw) {
	std::string	line, method, uri, protocol;

	// Get the first line
	if (std::getline(iss_raw, line, '\n') == -1)
		throw ErrorRequest("Error in the request: method not found");
	std::istringstream	iss_line(line);
	
	// Get the method, uri and protocol
	if (!(std::getline(iss_line, method, ' ') && std::getline(iss_line, uri, ' ') == -1 && std::getline(iss_line, protocol, ' ')))
		throw ErrorRequest("Error in the request: method not well formatted");
	
	// Check the method
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw ErrorRequest("Error in the request: method not supported");
	
	// Check the protocol
	if (protocol != "HTTP/1.1")
		throw ErrorRequest("Error in the request: protocol not supported");
	
	// Save the method
	return std::map<std::string, std::string>{ { "method", method }, { "uri", uri } };
}

Request::parse(void) {
	std::istringstream	iss(this->raw);
	std::string			line;
	int					gnl_status;
	
	// Parse the method
	if (p_step == NOT_STARTED) {
		p_step = METHOD;
		request_line = ft_get_method(iss);
	}
}
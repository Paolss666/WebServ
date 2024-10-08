/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/08 16:46:12 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Response::Response(const Request & src): Request(src) {
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_b_content_length = src._b_content_length;
	return ;
}

Response::~Response(void) { return ; }
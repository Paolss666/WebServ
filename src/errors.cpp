/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:51 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/02 11:51:43 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

ErrorConfFile::ErrorConfFile(std::string msg) throw() : _errorMsg(msg) { return ; }
ErrorConfFile::~ErrorConfFile() throw() { return ; }
const char *ErrorConfFile::what() const throw() { return _errorMsg.c_str(); }

ErrorFdManipulation::ErrorFdManipulation(std::string msg) throw() : _errorMsg(msg) { return ; }
ErrorFdManipulation::~ErrorFdManipulation() throw() { return ; }
const char *ErrorFdManipulation::what() const throw() { return _errorMsg.c_str(); }

ErrorRequest::ErrorRequest(std::string msg) throw() : _errorMsg(msg) { return ; }
ErrorRequest::~ErrorRequest() throw() { return ; }
const char *ErrorRequest::what() const throw() { return _errorMsg.c_str(); }
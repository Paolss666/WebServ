/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:03:53 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/02 11:51:27 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_HPP
# define ERRORS_HPP

# include "webserv.hpp"

class ErrorConfFile: public std::exception {
	public:
		ErrorConfFile(std::string errorMsg) throw();
		~ErrorConfFile(void) throw();

		virtual const char* what() const throw();

		std::string	_errorMsg;
};

class ErrorFdManipulation: public std::exception {
	public:
		ErrorFdManipulation(std::string errorMsg) throw();
		~ErrorFdManipulation(void) throw();

		virtual const char* what() const throw();

		std::string	_errorMsg;
};

class ErrorRequest: public std::exception {
	public:
		ErrorRequest(std::string errorMsg) throw();
		~ErrorRequest(void) throw();

		virtual const char* what() const throw();

		std::string	_errorMsg;
};

#endif
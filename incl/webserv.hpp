/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 16:58:20 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/19 16:58:42 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// # include "WebServe.hpp"s
// # include <vector>
# include <iostream>
# include <exception>
# include <vector>
# include <map>
# include <string>
# include <errno.h>
# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <sstream>
# include <fstream>
# include <cstdlib>
# include <algorithm>
# include <ctime>
# include <cstring>
# include <csignal>


#define PORT 8080


// function ------------------------------

void	Check_extension(const std::string & str, const std::string & ext);
void	Print_map_code_errors(std::map<int, std::string> PagesError);
void	printVector(const std::vector<std::string>& vec);
void	Print_map_code_return(std::map<int, std::string> errorPages);
// Error try catch ---------------- //

class ErrorConfFile : public std::exception
{
	public:
		ErrorConfFile(std::string errorMsg) throw();
		~ErrorConfFile() throw();

		virtual const char* what() const throw();
		// std::string	errorMsg;
		
	private:
		std::string	_errorMsg;
};

#endif
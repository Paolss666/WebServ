/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 16:58:20 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/25 12:35:18 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// libs ------------------------------
# include <iostream>
# include <exception>
# include <vector>
# include <map>
# include <list>
# include <string>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
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
# include <iomanip>
# include <sys/stat.h>
// webserv specific
# include <netdb.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <arpa/inet.h>
# include <sys/epoll.h>

// global variables ------------------------------
extern int									g_sig;
extern std::map<std::string, std::string>	CONTENT_TYPES;

// hpp files ------------------------------
# include "Location.hpp"
# include "errors.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConf.hpp"
# include "Hosts.hpp"
# include "utils.hpp"

#endif
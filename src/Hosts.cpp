/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:34 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/03 11:50:19 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Host::Host() { return ;}

Host::Host(ServerConf & src): ServerConf(src), _nfds(0) {

	struct epoll_event	event;
	std::ostringstream	oss;

	// printVector(src._IndexFile);
	// Create listening socket
	if ((_fdSetSock = socket(_address.sin_family, SOCK_STREAM, 0)) <= 0) {
		oss << "Error in the socket creation for server " << _nbServer;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Set socket options for reusing the address
	int	opt = 1;
	if (setsockopt(_fdSetSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the setsockopt for server " << _nbServer << "and socket " << _fdSetSock;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Set socket details (bind)
	if (bind(_fdSetSock, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the bind for server " << _nbServer << "with port " << _port << "and socket " << _fdSetSock;
			throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Listen on the socket
	if (listen(_fdSetSock, MAX_CONNECTIONS) < 0) {
		ft_close(_fdSetSock);
		throw ErrorFdManipulation("Error in the listen", ERR_CODE_INTERNAL_ERROR);
	}
	
	// Make the socket non-blocking
	if (fcntl(_fdSetSock, F_SETFL, fcntl(_fdSetSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the listen for server " << _nbServer << "and socket " <<_fdSetSock;
			throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	
	// Create epoll instance
	_fdEpoll = epoll_create(1);
	if (_fdEpoll < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the epoll_create for server " << _nbServer << "and socket " <<_fdSetSock;
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}
	event.events = EPOLLIN;
	event.data.fd = _fdSetSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdSetSock, &event) < 0)
		throw ErrorFdManipulation("Error in epoll_ctl for listening socket: " + static_cast<std::string>(strerror(errno)), ERR_CODE_INTERNAL_ERROR);

	// il faut resizer 
	_events.resize(MAX_CONNECTIONS);
}
Host::~Host() { return ; }

void	Host::new_connection(void) {
	std::ostringstream oss;
	std::cout << YELLOW "New connection detected" RESET << std::endl;
	
	// Accept the connection
	_fdAcceptSock = accept(_fdSetSock, (struct sockaddr *)&_address, (socklen_t*)&_address_len);
	if (_fdAcceptSock < 0) {
		ft_close(_fdSetSock);
		oss << "Error in the accept for server " << _nbServer << "and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Make the connection non-blocking
	if (fcntl(_fdAcceptSock, F_SETFL, fcntl(_fdAcceptSock, F_GETFL) | O_NONBLOCK) < 0) {
		ft_close(_fdSetSock);
		ft_close(_fdAcceptSock);
		oss << "Error in the fcntl for server " << _nbServer << "and socket " << _fdAcceptSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Add new connection to epoll
	struct epoll_event new_event;
	new_event.events = EPOLLIN;
	new_event.data.fd = _fdAcceptSock;
	if (epoll_ctl(_fdEpoll, EPOLL_CTL_ADD, _fdAcceptSock, &new_event) < 0)
		throw ErrorFdManipulation("Error in the epoll_ctl: " + static_cast<std::string>(strerror(errno)), ERR_CODE_INTERNAL_ERROR);
}

void	Host::parse_request(int fd) {
	char		buffer[BUFFER_SIZE] = { 0 };
	int			valread;
	std::string	raw;

	// Read data from client
	try {
		valread = read(_events[fd].data.fd, buffer, BUFFER_SIZE - 1);
		buffer[valread] = '\0';
		if (valread < 0) {
			ft_close(_events[fd].data.fd);
			epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, _events[fd].data.fd, NULL);
			throw ErrorFdManipulation("Error in the read", ERR_CODE_INTERNAL_ERROR);
		}
	} catch (const ErrorFdManipulation & e) {
		ft_perror(e.what());
		send_error_page(*this, fd, e._code);
		return ;
	}

	// Append the data to the request
	raw = buffer;
	if (_requests.find(_events[fd].data.fd) != _requests.end())
		_requests[_events[fd].data.fd].append(raw);
	else {
		Request tmp(*this, _events[fd], raw);
		_requests.insert(std::pair<int, Request>(_events[fd].data.fd, tmp));
	}

	// Parse the partial read of the request
	try {
		_requests[_events[fd].data.fd]._eof = recv(_events[fd].data.fd, buffer, 2, MSG_PEEK);
		_requests[_events[fd].data.fd].parse();
	} catch (const ErrorRequest & e) {
		ft_perror(e.what());
		send_error_page(*this, fd, e._code);
	}
}


void	Host::BuildGet(int fd, Response &reponse) {
	
	struct stat buffer;
	
	std::cout << reponse._path_file << "<----------\n";
	
	std::size_t pos  = reponse._path_file.find_last_of('/') + 1;
	std::string tmp = reponse._path_file.substr(pos, reponse._path_file.size() -1);
	std::cout  << "tmp -> " << tmp << std::endl;
	size_t check = 0;
	std::cout << "AUTOINDEXPRINT --> " << reponse._autoInxPrint << std::endl;
	if (reponse._autoInxPrint)
	{
		std::vector<std::string> filesList;
		DIR *dir = opendir(reponse._finalUri.c_str());
		if (!dir)
			std::cerr << "ERROR\n";
			// std::vector<std::string> Response::doDirListing(DIR *dir)
// {

		struct dirent *fileRead;
		while ((fileRead = readdir(dir)) != NULL)
		{
			if (strcmp(fileRead->d_name, ".") != 0 || (strcmp(fileRead->d_name, "..") != 0 && reponse._finalUri != "/"))
				filesList.push_back(fileRead->d_name);
		}
	// return (filesList);
// }

		reponse._headers["content-type"] = "text/html";
		reponse._body = "<!DOCTYPE html>\n"
                "<html lang=\"en\">\n"
                "<head>\n"
                "<meta charset=\"UTF-8\">\n"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                "<link href=\"style_autoindex.css\" rel=\"stylesheet\">\n"
                "<link href='../home/css/style.css' rel='stylesheet' type='text/css'>\n"
                "<link href=\"../../style_autoindex.css\" rel=\"stylesheet\" />\n"
                "<title>Auto index</title>\n"
                "</head>\n"
                "<body>\n"
                "<style>\n"
                "    body {\n"
                "        font-family: 'Arial', sans-serif;\n"
                "        display: flex;\n"
                "        flex-direction: column;\n"
                "        justify-content: center;\n"
                "        align-items: center;\n"
                "        height: 100vh;\n"
                "        margin: 0;\n"
                "        background-color: #f4f7f6;\n"
                "    }\n"
                "    h1 {\n"
                "        font-size: 48px;\n"
                "        color: #333;\n"
                "        margin-bottom: 20px;\n"
                "    }\n"
				"    h2 {\n"
                "        font-size: 48px;\n"
                "        color: #330;\n"
                "        margin-bottom: 10px;\n"
                "    }\n"
                "    p {\n"
                "        font-size: 18px;\n"
                "        color: #777;\n"
                "        margin-bottom: 40px;\n"
                "    }\n"
                "    .button-container {\n"
                "        display: flex;\n"
                "        gap: 20px;\n"
                "    }\n"
                "    button, .link-button {\n"
                "        padding: 15px 30px;\n"
                "        font-size: 18px;\n"
                "        cursor: pointer;\n"
                "        border: none;\n"
                "        border-radius: 50px;\n"
                "        transition: background-color 0.3s, box-shadow 0.3s;\n"
                "    }\n"
                "    button {\n"
                "        background-color: #3498db;\n"
                "        color: white;\n"
                "    }\n"
                "    button:hover {\n"
                "        background-color: #2980b9;\n"
                "        box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);\n"
                "    }\n"
                "    .link-button {\n"
                "        background-color: #2ecc71;\n"
                "        color: white;\n"
                "        text-decoration: none;\n"
                "        display: inline-block;\n"
                "        line-height: 1.5;\n"
                "    }\n"
                "    .link-button:hover {\n"
                "        background-color: #03eb63;\n"
                "        box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);\n"
                "    }\n"
                "</style>\n"
                "<h1 class=\"title1\"> Auto index </h1>\n"
                "<h2 class=\"autoindex\">\n" + reponse._finalUri + "\n"
                "</h2>\n"
                "</body>\n"
                "</html>";

		for (std::vector<std::string>::iterator it = filesList.begin(); it != filesList.end(); it++)
		{
			if (*it == ".")
				continue ;
			std::string hyperlink("");
			std::string filename("");
			if (*it == "..")
				filename = "<< COME BACK TO HOME ";
			else
				filename = (*it);
			// hyperlink = _finalURI + (*it);
			hyperlink = (*it);
			// if (RESPONSE)
			// 	std::cerr << "hyperlink = " << hyperlink << std::endl;
			reponse._body += "<div class=\"button-container\"><a class=\"link-button\" href=" + hyperlink + ">" + filename + "</a></div>\n";
			// reponse._body += 
		}

		reponse._body += "</body>\n";
		reponse._body += "</html>";

		closedir(dir);
		 
    	std::ostringstream oss;  // Crea un flusso di output
    	oss << reponse._body.size();           // Scrivi il numero nel flusso
		std::string response_header = "HTTP/1.1 200 OK\r\n";
		response_header += "Content-Length: " + oss.str() + "\r\n";
    	response_header += "Content-Type: text/html\r\n"; // forma html
    	// response_header += "Content-Type: text/css\r\n"; // forma html
    	response_header += "\r\n";  // Fine dell'header
		int header_bytes_sent = send(fd, response_header.c_str(), response_header.length(), 0);
    	if (header_bytes_sent == -1) {
    	    std::cerr << "Error in send for header" << std::endl;
    	    close(fd);
    	    return;
    	}

    	int body_bytes_sent = send(fd, reponse._body.c_str(), reponse._body.length(), 0);
    	if (body_bytes_sent == -1) {
    	    std::cerr << "Error in send for the content of the fd" << std::endl;
    	}

    	close(fd);
		return ;
    }
	if (reponse._indexPages.size() == 0)
		check = 404;
	for (size_t i = 0; i < reponse._indexPages.size(); i++)
	{
		if (tmp != reponse._indexPages[i])
		{
			check = 404;
			break;
		}
	}
	if (stat(reponse._path_file.c_str(), &buffer) != 0 || (reponse._err == 404) 
		|| (reponse._err == 1 && check == 404))
	{
		// std::cout << "BAD FILE PATH\n;
		std::ifstream file(_PageError[404].c_str());
		std::string file_con;
		if (file.good())
		{
			std::ostringstream ss;
			ss << file.rdbuf();
			file_con = ss.str();
			std::ostringstream os;
			os << file_con.length();
    		std::string response_header = "HTTP/1.1 404 OK\r\n";
			response_header += "Content-Length: " + os.str() + "\r\n";
    		response_header += "Content-Type: text/html\r\n"; // forma html
    		response_header += "\r\n";  // Fine dell'header
			int header_bytes_sent = send(fd, response_header.c_str(), response_header.length(), 0);
    		if (header_bytes_sent == -1) {
    		    std::cerr << "Error in send for header" << std::endl;
    		    close(fd);
    		    return;
			}
    		int body_bytes_sent = send(fd, file_con.c_str(), file_con.length(), 0);
    		if (body_bytes_sent == -1) {
    		    std::cerr << "Error in send for the content of the fd" << std::endl;
    			close(fd);
				return;
			}
        return;
		}
	}
	// if (reponse._autoInxPrint)
	// 	reponse._path_file = reponse._tmpForInx;
	// std::cout << "ECCOLOOOOOOOOOOOOOOOOOo " << reponse._path_file << std::endl;
	std::ifstream file(reponse._path_file.c_str());
    std::string file_content;
	std::cout << "path --> " << reponse._path_file << std::endl;
    if (file.good()) {
        std::ostringstream ss;
        ss << file.rdbuf();
        file_content = ss.str();
    } else {
        std::string error_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 23\r\n\r\n<h1>404 Not Found</h1>";
        send(fd, error_response.c_str(), error_response.length(), 0);
        close(fd);
        return;
    }

    std::ostringstream oss;
    oss << file_content.length();

    std::string response_header = "HTTP/1.1 200 OK\r\n";
    response_header += "Content-Length: " + oss.str() + "\r\n";
    response_header += "Content-Type: text/html\r\n"; // forma html
    response_header += "\r\n";  // Fine dell'header

    int header_bytes_sent = send(fd, response_header.c_str(), response_header.length(), 0);
    if (header_bytes_sent == -1) {
        std::cerr << "Error in send for header" << std::endl;
        close(fd);
        return;
    }

    int body_bytes_sent = send(fd, file_content.c_str(), file_content.length(), 0);
    if (body_bytes_sent == -1) {
        std::cerr << "Error in send for the content of the fd" << std::endl;
    }
}

void	Host::act_on_request(int fd) {

	std::cout << YELLOW "Acting on request" RESET << std::endl;

	// Send data to client
	Response response(_requests[_events[fd].data.fd], *this);
	if (response._request_line["method"] == "GET")
		BuildGet(_events[fd].data.fd, response);
	else {
		send_error_page(*this, fd, ERR_CODE_MET_NOT_ALLOWED);
		return ;
	}
	
	// Close the connection if needed
	if (!_requests[_events[fd].data.fd]._headers["Connection"].compare("close")) {
		std::cout << "Closing connection" << std::endl;
		epoll_ctl(_fdEpoll, EPOLL_CTL_DEL, _events[fd].data.fd, NULL);
		ft_close(_events[fd].data.fd);
		_requests.erase(_events[fd].data.fd);
	} else {
		std::cout << "Connection kept alive" << std::endl;
		_events[fd].events = EPOLLIN;
		epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, _events[fd].data.fd, &_events[fd]);
	}

	std::cout << YELLOW "---> Request answered" RESET << std::endl << std::endl;
}

void	Host::run_server(void) {

	// Wait for events
	_nfds = epoll_wait(_fdEpoll, this->_events.data(), MAX_CONNECTIONS, 0);
	if (_nfds < 0) {
		std::ostringstream oss;
		oss << "Error in the epoll_wait for server " << _nbServer << "and socket " << _fdSetSock << ": " << strerror(errno);
		throw ErrorFdManipulation(oss.str(), ERR_CODE_INTERNAL_ERROR);
	}

	// Handle events
	for (int i = 0; i < _nfds; ++i) {
		if (_events[i].events == EPOLLIN) {
			// New connection
			if (_events[i].data.fd == _fdSetSock)
				try {
					new_connection();
				} catch (std::exception & e) {
					ft_perror(e.what());
				}

			// Existing connection
			else {
				parse_request(i);
				// Change the event to EPOLLOUT
				if (_requests[_events[i].data.fd]._stage == BODY_DONE) {
					std::cout << YELLOW "New request cought" RESET << std::endl;
					print_request(_requests[_events[i].data.fd]._request_line, _requests[_events[i].data.fd]._headers, _requests[_events[i].data.fd]._body);
					std::cout << std::endl;
					_events[i].events = EPOLLOUT;
					epoll_ctl(_fdEpoll, EPOLL_CTL_MOD, _events[i].data.fd, &_events[i]);
				}
			}
		}
		else if (_events[i].events == EPOLLOUT)
			act_on_request(i);
		// else
			// std::cout << "Unknown event detected" << std::endl;
	}
}

void	Host::close_everything(void) {
	ft_close(_fdSetSock);
	ft_close(_fdEpoll);
	for (size_t i = 0; i < _events.size(); i++)
		ft_close(_events[i].data.fd);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:36 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/13 18:07:43 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
// #include "Location.hpp"

void	fillContentTypes()
{
	CONTENT_TYPES["txt"] = "text/plain";
	CONTENT_TYPES["html"] = "text/html";
	CONTENT_TYPES["htm"] = "text/html";
	CONTENT_TYPES["png"] = "image/png";
	CONTENT_TYPES["jpg"] = "image/jpeg";
	CONTENT_TYPES["jpeg"] = "image/jpeg";
	CONTENT_TYPES["xpm"] = "image/x-xpixmap";
	CONTENT_TYPES["css"] = "text/css";
}

void	Check_extension(const std::string & str, const std::string & ext) {
	if (str.size() >= ext.size() && !str.compare(str.size() - ext.size(), ext.size(), ext))
		return ;	
	throw ErrorConfFile("Error : wrong conf extension");
}

void	Print_map_code_errors(std::map<int, std::string> errorPages) {
	for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		std::cout << "code -> " << it->first << " pages -> " << it ->second << "\n";
}

void	Print_map_code_return(std::map<int, std::string> errorPages) {
	for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		std::cout << "return -> " << it->first << " pages -> " << it ->second << "\n";
}

void printVector(const std::vector<std::string>& vec) {
	std::cout << "Vector content:" << std::endl;
	for (size_t i = 0; i!= vec.size(); ++i) {
		std::cout << " - " << vec[i] << std::endl;
	}
}

void sig_handler(int signal) {
	std::time_t currentTime = std::time(0);
	struct std::tm* localTime = std::localtime(&currentTime);
	char timeBuffer[80];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M:%S", localTime);
	std::cerr << "\rServer interrupted at time: " << timeBuffer << std::endl;
	if (signal == SIGINT)
		g_sig = 1;
}

void	ft_close(int fd) {
	if (fd >= 0)
		close(fd);
	fd = -1;
}

void	print_with_hex(const std::string & str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (isprint(*it))
			std::cout << *it;
		else
			std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)*it;
	}
	std::cout << std::dec; // Reset to decimal format
}


void	print_request(std::map<std::string, std::string> _request_line, std::map<std::string, std::string> headers, std::string body) {
	std::cout << CYAN << "Method: " << WHITE << _request_line["method"] << std::endl;
	std::cout << CYAN << "URI: " << WHITE << _request_line["uri"] << std::endl;
	std::cout << CYAN << "Protocol: " << WHITE << "HTTP/1.1" << std::endl;
	std::cout << CYAN << "Headers:" << WHITE << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << BLUE "---" << it->first << ": " << WHITE << it->second << std::endl;
	if (_request_line["method"] == "POST")
		std::cout << CYAN << "Body: " << WHITE << body << std::endl;
}

std::string FoundMatchLocation(std::string uri, std::map<std::string, Location> _Location)
{
	while (true) {
		bool found = false;
        for (std::map<std::string, Location>::iterator it = _Location.begin(); it != _Location.end(); ++it) {
			std::cout << "it->first" << it->first << std::endl;
            if (it->first == uri) {                
				std::cout << "Found matching URI: " << it->first << std::endl;
                found = true;
                break;
            }
        }
        if (found)
            break;

        std::size_t pos = uri.find_last_of('/');
		uri = uri.substr(0, uri.find_last_of('/'));
		std::cout << " --------->  uri    ->" << uri << std::endl;
        if (pos == std::string::npos || pos == 0) {
            uri = "/";
			break;
        } else
            uri = uri.substr(0, pos);
	}
	return (uri);
}

std::string foundUriInLoc(std::string uri, std::map<std::string, Location> _Location)
{

	while (true) {
	bool found = false;
    for (std::map<std::string, Location>::iterator it = _Location.begin(); it != _Location.end(); ++it) {
		std::cout << "it->first" << it->first << std::endl;
        if (it->first == uri) {                
			std::cout << "Found matching URI: " << it->first << std::endl;
            found = true;
			// _found = 1;
            break;
        }
    }
    if (found)
        break;
    std::size_t pos = uri.find_last_of('/');
	uri = uri.substr(0, uri.find_last_of('/'));
	std::cout << " --------->  uri    ->" << uri << std::endl;
    if (pos == std::string::npos || pos == 0) {
        uri = "/";
		break;
    } else
        uri = uri.substr(0, pos);
	}
	return (uri);
}


void	SendUltra(int fd, struct epoll_event &event, Response response)
{
	/* std::ifstream file(response._startUri.c_str()); */
    std::string file_content;
	std::cout << "path --> " << response._finalUri<< std::endl;
/*     if (file.good()) {
        std::ostringstream ss;
        ss << file.rdbuf();
        file_content = ss.str();
    } else {
		response.buildErrorPage(fd, 404, event);
        return;
    }

    std::ostringstream oss;
    oss << file_content.length(); */

	std::cout <<  " < ----, ====================" << std::endl;
	std::string response_header = "HTTP/1.1 " + convertToStr(response._statusCode) + " \r\n";
	response_header += "Content-Length: " + convertToStr(response._body.size()) + "\r\n";
    response_header += "Content-Type: text/html\r\n"; // forma html
    // response_header += "Content-Type: text/css\r\n"; // forma html
    response_header += "\r\n";  // Fine dell'header
	// std::cout << response_header << " < ----, ====================" << std::endl;
	// std::cout << response._body << ", ============"<< std::endl;
   
	response_header += response._body;
   
   int error = send(fd, response_header.c_str(), response_header.length(),MSG_NOSIGNAL | MSG_MORE );
    
	if (error == -1) {
		error_send(fd, event, "Error in send for header");
        return;
    }
	//  error = send(fd, response_header.c_str(), response_header.size(), MSG_NOSIGNAL | MSG_MORE);
    //     if (error == -1) {
    //         // Gestione dell'errore
    //         return ;
    //     }

    //     // Invio del corpo della risposta (_body)
    //     size_t bytesSent = 0;
    //     size_t tmpSent = 0;
    //     tmpSent = send(fd, response._body.c_str() + bytesSent, response._body.size() - bytesSent, MSG_NOSIGNAL);
    //     if (tmpSent <= 0) {
                
    //             return ;
    //         }
	
		close(fd);
		return;
/* 

    error = send(fd, file_content.c_str(), file_content.length(), MSG_NOSIGNAL);
    if (error == -1)
		error_send(fd, event, "Error in send for the content of the fd"); */
}

int	IsARepertory(std::string root, std::string filename)
{
	struct stat buffer;
/* 	std::string root = "www";
	filename = root + filename; */
	std::cout << "file name in IsARepertory > " << filename << std::endl;
	filename = root + filename;
	if (stat(filename.c_str(), &buffer) < 0) {
		std::cout << "404\n"; // 404 request not found;
		return (0);
	}
	if (S_ISREG(buffer.st_mode)) {
		std::cout << "is a fichier return 1\n";
		return (1);
	}
	if (S_ISDIR(buffer.st_mode)) {
		std::cout << "is a dossier return 3\n";
		return (3);
	}
	return(0);
}

std::string	trim(std::string & str) {
	size_t	start = str.find_first_not_of(" \t");
	size_t	end = str.find_last_not_of(" \t");
	return (str.substr(start, end - start + 1));
}

void	ft_print_coucou(int i) {
	std::cout << "COUCOU " << i << std::endl;
}

void	error_send(int fd, struct epoll_event & event, std::string message) {
	epoll_ctl(event.data.fd, EPOLL_CTL_DEL, fd, NULL);
	ft_perror(message.c_str());
	ft_close(fd);
}

bool	readContent(std::string root, std::string &uri, std::string &content)
{
	// std::cerr << "readContent 0\n";
	if (IsARepertory(root, uri) >= 3)
	{
		// std::cerr << "readContent 1\n";
		return false;
	}
	std::ifstream	file(uri.c_str());
	if (!file.good())
	{
		// std::cerr << "readContent 2\n";
		return false;
	}
	std::stringstream	buf;
	buf << file.rdbuf();
	content = buf.str();

	return true;
}
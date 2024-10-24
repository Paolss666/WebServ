/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:36 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/24 12:24:19 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
// #include "Location.hpp"

void	fillContentTypes() {
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
	std::cout << std::dec;
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

int	isRepertory(std::string root, std::string filename) {
	struct stat buffer;

	filename = root + filename;
	if (stat(filename.c_str(), &buffer) < 0)
		return (0); // 404 request not found;
	if (S_ISREG(buffer.st_mode))
		return (1); // is a file
	if (S_ISDIR(buffer.st_mode))
		return (3); // is a directory
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

const std::string & collect_lh_ip(void) {
	static std::string	ip_address;
	struct addrinfo		*res, *p, *hints = NULL;
	struct sockaddr_in	*ipv4;
	std::ostringstream	oss;
	int					status;

	if ((status = getaddrinfo("localhost", NULL, hints, &res)) != 0)
		return (ft_perror("Warning, no localhost found"), ip_address);

	for (p = res; p != NULL; p = p->ai_next) {
		if (p->ai_family == AF_INET) {
			ipv4 = (struct sockaddr_in *)p->ai_addr;
			uint32_t ip = ntohl(ipv4->sin_addr.s_addr);

			// Convert the IP to a string manually
			oss << ((ip >> 24) & 0xFF) << '.'
				<< ((ip >> 16) & 0xFF) << '.'
				<< ((ip >> 8) & 0xFF) << '.'
				<< (ip & 0xFF);
			ip_address = oss.str();
			break;
		}
	}

	freeaddrinfo(res);
	return ip_address;
}

char	**vectorStringToChar(std::vector<std::string> &vector) {
	char** tab = new char*[vector.size() + 1];
	for (size_t i = 0; i < vector.size(); i++)
	{
		tab[i] = new char[vector[i].size() + 1];
		std::copy(vector[i].begin(), vector[i].end(), tab[i]);
		tab[i][vector[i].size()] = '\0';
	}
	tab[vector.size()] = NULL;
	return (tab);
}

int hex_to_char(char hex) {
	if (hex >= '0' && hex <= '9') {
		return hex - '0';
	} else if (hex >= 'A' && hex <= 'F') {
		return hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'f') {
		return hex - 'a' + 10;
	}
	return -1; // Error
}

std::string replace_percentage(std::string str) {
	std::string decoded;
	char hex1, hex2;
	int byte;

	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '%' && i + 2 < str.length()) {
			hex1 = str[i + 1];
			hex2 = str[i + 2];
			
			byte = (hex_to_char(hex1) << 4) | hex_to_char(hex2);
			decoded += static_cast<char>(byte);
			
			i += 2;
		} else if (str[i] == '+')
			decoded += ' ';
		else 
			decoded += str[i];
	}
	return decoded;
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 18:24:47 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/17 12:07:55 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Response::Response(const Request & src, const Host &host): Request(src) {
	fillContentTypes();
	_response_ready = false;
	_request_line = src._request_line;
	_headers = src._headers;
	_body = src._body;
	_indexPages = host._IndexFile;
	_Location =  host._Location;
	_root = host._rootPath;
	_pagesError = host._PageError;
	_returnPages = host._CodeReturn;
	_err = 0; // 4 error 404 etc etc 
	_autoInxPrint = 0;
	_autoIndex = host._Autoindex;
	_found = 0;
	_serverName = host._name;
	_maxBodySize= host._maxBodySize;
	_Ip = host._ip;
	_Port = host._port;
	_statusCode = 200;
	_startUri = _request_line["uri"];

	if (_root[0] == '/')
		_root = _root.substr(1,_root.size() - 1);
	if (_root[_root.size() - 1] == '/')
		_root = _root.substr(0,_root.size() - 1);
}

void	Response::send_response(int fd, bool *done) {
	int	sent;

	
	if (_response_message.size() > BUFFER_SIZE)
		sent = send(fd, _response_message.c_str(), BUFFER_SIZE, MSG_MORE | MSG_NOSIGNAL);
	else
		sent = send(fd, _response_message.c_str(), _response_message.size(), MSG_NOSIGNAL);
	
	if (sent == -1)
		throw ErrorResponse("Error in the send of the response", ERR_CODE_INTERNAL_ERROR);
	else
		_response_message = _response_message.substr(sent);
	
	if (_response_message.empty())
		*done = true;
	else
		return ;
}

void	Response::buildAutoindex(void) {

	std::vector<std::string> filesList;
	_startUri  = _root + _startUri;
	DIR *dir = opendir(_startUri.c_str());
	if (!dir)
		throw ErrorResponse("Error in the opening of the directory pointer by the URI", ERR_CODE_INTERNAL_ERROR);

	struct dirent *fileRead;
	while ((fileRead = readdir(dir)) != NULL)
		if (strcmp(fileRead->d_name, ".") != 0 || (strcmp(fileRead->d_name, "..") != 0 && _startUri != "/"))
			filesList.push_back(fileRead->d_name);

	_headers["content-type"] = "text/html";
	this->_body = "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"<meta charset=\"UTF-8\">\n"
			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"<link href=\"style_autoindex.css\" rel=\"stylesheet\">\n"
			"<link href='../home/css/style.css' rel='stylesheet' type='text/css'>\n"
			"<link href=\"../../style_autoindex.css\" rel=\"stylesheet\" />\n"
			"<title>Auto index</title>\n"
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
			"\n"
			"    h1 {\n"
			"        font-size: 48px;\n"
			"        color: #333;\n"
			"        margin-bottom: 40px;\n"
			"    }\n"
			"\n"
			"    p {\n"
			"        font-size: 18px;\n"
			"        color: #777;\n"
			"        margin-bottom: 50px;\n"
			"    }\n"
			"\n"
			"    .button-container {\n"
			"        display: flex;\n"
			"        gap: 20px;\n"
			"    }\n"
			"\n"
			"    button, .link-button {\n"
			"        padding: 15px 30px;\n"
			"        font-size: 18px;\n"
			"        cursor: pointer;\n"
			"        border: none;\n"
			"        margin-bottom: 20px;\n"
			"        border-radius: 50px;\n"
			"        transition: background-color 0.3s, box-shadow 0.3s;\n"
			"    }\n"
			"\n"
			"    button {\n"
			"        background-color: #3498db;\n"
			"        color: white;\n"
			"    }\n"
			"\n"
			"    button:hover {\n"
			"        background-color: #2980b9;\n"
			"        box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);\n"
			"    }\n"
			"\n"
			"    .link-button {\n"
			"        background-color: #2ecc71;\n"
			"        color: white;\n"
			"        text-decoration: none;\n"
			"        display: inline-block;\n"
			"        line-height: 1.5;\n"
			"    }\n"
			"\n"
			"    .link-button:hover {\n"
			"        background-color: #03eb63;\n"
			"        box-shadow: 0px 4px 15px rgba(0, 0, 0, 0.1);\n"
			"    }\n"
			"</style>\n"
			"</head>\n"
			"<body>\n"
			"<h1 class=\"title1\"> Auto index </h1>\n"
			"<h2 class=\"autoindex\">\n" + _startUri + "\n"
			"</h2>\n"
			"</body>\n"
			"</html>";

	for (std::vector<std::string>::iterator it = filesList.begin(); it != filesList.end(); it++) {
		if (*it == ".")
			continue ;
		std::string hyperlink("");
		std::string filename("");
		if (*it == "..")
			filename = "<< COME BACK TO HOME ";
		else
			filename = (*it);
		_root = _root + "/";
		if (isRepertory(_root , filename) == 3)
			hyperlink = (*it) + "/";
		else
			hyperlink = (*it);
		this->_body += "<div class=\"button-container\"><a class=\"link-button\" href=" + hyperlink + ">" + filename + "</a></div>\n";
	}

	this->_body += "</body>\n";
	this->_body += "</html>";

	closedir(dir);
	// Add remainig headers
	std::ostringstream 	oss;
	oss << this->_body.size();
	_response_header.insert(std::make_pair("Content-Length",oss.str()));
	if (_response_header.find("Content-Type") == _response_header.end())
		_response_header.insert(std::make_pair("Content-Type", "text/html"));
	
	// Merge everything in the final message to send
	oss.str("");
	oss << "HTTP/1.1 " << this->_statusCode << " OK" << "\r\n";
	this->_response_message = oss.str();
	this->_response_message += "Server: " + this->_serverName + "\r\n";
	for (std::map<std::string, std::string>::iterator it = this->_response_header.begin(); it != _response_header.end(); it++)
		this->_response_message += it->first + ": " + it->second + "\r\n";
	this->_response_message += "\r\n";
	this->_response_message += this->_body;

	// Set the response ready
	_response_ready = true;
}

void	Response::buildPage(void) {
	std::stringstream	buffer;
	std::string			resourceName, fileExtension;
	size_t				pos;
	std::ostringstream 	oss;
	
	// Open the requested file
	_startUri = _root + _startUri;
	std::ifstream fileRequested(_startUri.c_str());
	if (fileRequested.good() == false)
		throw ErrorResponse("Error in the opening of the file requested", ERR_CODE_NOT_FOUND);
	
	buffer << fileRequested.rdbuf();
	
	_response_body = buffer.str();
	if (_response_body.size() > _maxBodySize) {
		_response_body = "";
		throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
	}

	pos = _startUri.find_last_of("/");
	if (pos != std::string::npos && (_startUri.begin() + pos + 1) != _startUri.end()) {
		resourceName = _startUri.substr(pos + 1);
		pos = resourceName.find_last_of(".");
		if (pos != std::string::npos && (resourceName.begin() + pos + 1) != resourceName.end()) {
			fileExtension = resourceName.substr(pos + 1);
			if (CONTENT_TYPES.find(fileExtension) != CONTENT_TYPES.end())		
				_response_header.insert(std::make_pair("Content-Type", CONTENT_TYPES[fileExtension]));
		}
	}

	// Add remainig headers
	oss << _response_body.size();
	_response_header.insert(std::make_pair("Content-Length",oss.str()));
	if (_response_header.find("Content-Type") == _response_header.end())
		_response_header.insert(std::make_pair("Content-Type", "text/html"));
	
	// Merge everything in the final message to send
	oss.str("");
	oss << "HTTP/1.1 " << _statusCode << " OK" << "\r\n";
	_response_message = oss.str();
	_response_message += "Server: " + _serverName + "\r\n";
	for (std::map<std::string, std::string>::iterator it = _response_header.begin(); it != _response_header.end(); it++)
		_response_message += it->first + ": " + it->second + "\r\n";
	_response_message += "\r\n";
	_response_message += _response_body;

	// Set the response ready
	_response_ready = true;
}

std::vector<std::string>	Response::MakeEnvCgi(std::string &cgi)
{
	std::vector<std::string> env;
	std::stringstream	buffer;

	// std::cout << "_cgi --- >>>>>>>>>>>>>>>>>>>>> " << _Cgi << std::endl;
	std::ifstream fileRequested(cgi.c_str());
	if (fileRequested.good() == false)
		throw ErrorResponse("Error in the opening of the file requested", ERR_CODE_NOT_FOUND);
	
	buffer  << fileRequested.rdbuf();

	// std::string			resourceName, fileExtension;
	// size_t				pos;



	std::ostringstream 	oss;
	_response_body = buffer.str();
	if (_response_body.size() > _maxBodySize) {
		_response_body = "";
		throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
	}
	oss << _response_body;
	exportENV(env, "CONTENT_LENGTH", oss.str());
	exportENV(env, "CONTENT_TYPE", "text/html");
	std::string pathInfo = "/home/npaolett/42/WebServerGet/" + cgi; // Assicurati che sia corretto
	exportENV(env, "GATEWAY_INTERFACE", "CGI/1.1");
	for (std::map<std::string, std::string>::iterator it = _response_header.begin(); it != _response_header.end(); it++)
	{
		std::string keyWord = it->first;
		int i = 0;
		while (keyWord[i])
		{
			keyWord[i] = toupper(keyWord[i]);
			if (keyWord[i] == '-')
				keyWord[i] = '_';
			i++;
		}
		keyWord.insert(0, "HTTP_");
		if (keyWord != "HTTP_CONNECTION")
			exportENV(env, keyWord, it->second);
	}
	size_t pos = _startUri.find_last_of('?');
	std::string query = _startUri.substr(pos, _startUri.size());
;	exportENV(env, "QUERY_STRING",query);
	exportENV(env, "REDIRECT_STATUS", "200");// 200 to indicate the requesst was hande correctly
	exportENV(env, "REQUEST_METHOD", _request_line["method"]);
	exportENV(env, "SCRIPT_NAME", _startUri);
	exportENV(env, "SCRIPT_FILENAME", pathInfo);
	exportENV(env, "SERVER_PROTOCOL", "HTTP/1.1");
	exportENV(env, "SERVER_SOFTWARE", "webserv");
	return (env);
}

void	Response::buildCgi()
{
	std::string root_Uri = _root + _startUri;

	std::cout << " ---> rootUri -> inside Cgi -> " << root_Uri << std::endl;
	size_t extention = _startUri.find_last_of('.');
	std::string format = _startUri.substr(extention, _startUri.size());
	size_t f_interr = format.find_last_of('?');
	std::string vrai_f = format.substr(0, f_interr);
	std::string _Cgi = _startUri.substr(0, extention);
	if (vrai_f == ".php")
		_Cgi += ".php";
	else if (vrai_f == ".py")
		_Cgi += ".py";
	_Cgi = _root + _Cgi;
	std::string uri_for_cgi = _startUri.substr(extention, _startUri.size());
	std::string cgi;
	if (access(_Cgi.c_str(), F_OK))
		throw ErrorResponse("Error in the request: access URI CGI", ERR_CODE_NOT_FOUND);

	std::cout << " <<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>." << std::endl;

	int writeStatus; 
	// generate a fd_out 4 the cgi 

	int	cgiFdOut = open(".cgi.txt", O_WRONLY | O_CREAT | O_TRUNC);
	if (cgiFdOut == -1)
	if (chmod(".cgi.txt", S_IRWXU | S_IRWXG | S_IRWXO) != 0)
	{
		close(cgiFdOut);
		throw ErrorResponse("Error in the request: FILE CGI", ERR_CODE_NOT_FOUND);
	}
	// generate a fd_in 4 the cgi
	int	cgiFdIn = open(".body_cgi.txt", O_WRONLY | O_CREAT | O_TRUNC);
	if (cgiFdIn == -1)
	{
		close(cgiFdOut);
		close(cgiFdOut);
		throw ErrorResponse("Error in the request: FILE CGI", ERR_CODE_NOT_FOUND);
	}
	if (chmod(".body_cgi.txt", S_IRWXU | S_IRWXG | S_IRWXO) != 0)
	{
		(close(cgiFdOut), close(cgiFdIn));
		throw ErrorResponse("Error in the request: FILE_body CGI", ERR_CODE_NOT_FOUND);

	}
	int	fd[2];
	if (pipe(fd) == - 1)
	{
		(close(cgiFdOut), close(cgiFdIn));
		throw ErrorResponse("Error in the request: PIPE CGI", ERR_CODE_NOT_FOUND);
	}



	// HERE I ll put or build the find version 4 Different CGI php or PY


	std::vector<std::string> vecEnv = MakeEnvCgi(_Cgi);

	char * finalUriChar = const_cast<char*>(_startUri.c_str());
	_cgi = const_cast<char*>(_Cgi.c_str());

	time_t	start = time(NULL);
	pid_t	pid = fork();
	if (pid == -1)
	{
		(close(cgiFdOut), close(cgiFdIn));
		throw ErrorResponse("Error in the request: FILE FORK CGI", 500);
	}
	if (pid == 0)//child
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);//stdin devient pipe[0]
		close(fd[0]);
		dup2(cgiFdOut, STDOUT_FILENO);//stdout devient pipe[1]
		close(cgiFdOut);
		char *av[] = {_cgi,finalUriChar,  NULL};
		char **env = vectorStringToChar(vecEnv);
		close(cgiFdIn);
		execve(_cgi, av, env);
		// execve failed
		perror("execve");

		sleep(3);
		exit(EXIT_FAILURE);
	}
	close(fd[0]);
	std::ofstream writeInPipe;
	writeInPipe.open(".body_cgi.txt");
	if (!writeInPipe.is_open())
	{
		close(fd[1]);
		throw ErrorResponse("Error in the request: FILE OPEN body CGI", 500);
	}
	writeInPipe << _response_body;
	writeInPipe.close();
	close(fd[1]);
	while (true)
	{
		pid_t	pid_result = waitpid(pid, &writeStatus, WNOHANG);
		if (pid_result > 0)// success
			break;
		if (pid_result == 0)// no state change detected
		{
			time_t	end = time(NULL);
			if (end - start >= 10)
			{
				close(cgiFdOut);
				close(cgiFdIn);
				if (kill(pid, SIGKILL) == -1)
					perror("kill");
				pid_result = waitpid(pid, &writeStatus, WNOHANG);
				_statusCode = 500;
				remove(".cgi.txt");
				remove(".body_cgi.txt");
				throw ErrorResponse("Error in the request: FILE_____ CGI", 500);
			}
		}
		else// waitpid failed
		{
			perror("waitpid");
			close(cgiFdOut);
			close(cgiFdIn);
			// free
			throw ErrorResponse("Error in the request: waitPID CGI", 500);
		}
	}
	close(cgiFdOut);
	close(cgiFdIn);

	std::ifstream ifs(".cgi.txt");
	if (!ifs.is_open()) {
	    throw ErrorResponse("Error in the request: Open FILE CGI", 404);
	}
	remove(".body_cgi.txt");
	remove(".cgi.txt");
	std::stringstream responseStream;
	responseStream << ifs.rdbuf();
	_response_body = responseStream.str(); 
	if (_response_body.size() > _maxBodySize) {
	    _response_body = "";
	    throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
	}

	std::ostringstream responseHeaders;
	responseHeaders << "HTTP/1.1 " << _statusCode << " OK\r\n";
	responseHeaders << "Content-Type: text/html; charset=UTF-8\r\n";
	responseHeaders << "Content-Length: " << _response_body.size() << "\r\n"; // Imposta Content-Length
	responseHeaders << "\r\n";

	_response_message = responseHeaders.str() + _response_body;
	_response_ready = true;
}





void	Response::exportENV(std::vector<std::string> &env, const std::string &key, const std::string &value)
{
	env.push_back(key + "=" + value);
}

void	Response::buildReturnPage()
{
	int	returnCode;
	std::string redirUri;
	for (std::map<int, std::string>::iterator i = _returnPages.begin(); i != _returnPages.end(); i++)
	{
		// std::cerr << i->first << " " << i->second << "\n";
		returnCode = i->first;
		redirUri = i->second;
	}
	if (returnCode == 300)
		_statusCode = 300;
	else if (returnCode == 301)
		_statusCode = 301 ;//STATUS_MOVED_PERMANENTLY;
	else if (returnCode == 302)
		_statusCode = 302; // STATUS_FOUND;
	else if (returnCode == 303)
		_statusCode = 303; //STATUS_SEE_OTHER;
	else if (returnCode == 304)
		_statusCode = 304; //STATUS_NOT_MODIFIED;
	else if (returnCode == 305)
		_statusCode = 305; //STATUS_USE_PROXY;
	else if (returnCode == 306)
		_statusCode = 306; //STATUS_SWITCH_PROXY;
	else if (returnCode == 307)
		_statusCode = 307; //STATUS_TEMPORARY_REDIRECT;
	else if (returnCode == 308)
		_statusCode = 308; //STATUS_PERMANENT_REDIRECT;
	if ((_startUri.size() -1 ) == '/' && redirUri[0] == '/')
	{
		redirUri  = redirUri.substr(1, redirUri.size());
		_startUri += redirUri;
	}
	else if ((_startUri.size() -1 ) == '/' && redirUri[0] != '/')
		_startUri +=  redirUri;
	else if ((_startUri.size() -1 ) != '/' && redirUri[0] == '/')
		_startUri +=  redirUri;
	else if ((_startUri.size() -1 ) != '/' && redirUri[0] != '/')
		_startUri = _startUri + "/" + redirUri;
	_headers["location"] = _startUri;
	// std::cout << "_startUri" << _startUri<< std::endl;
	buildPage();
}

void	Response::buildGet(void) {
	
	std::string	index, path, uri;
	if (_startUri.size() > 2 &&  _startUri[_startUri.size() - 1] == '/')
		uri = _startUri.substr(0, _startUri.size() - 1);
	else
		uri = _startUri;
	if (_Location[uri].getFlagIndex())
		_indexPages = _Location[uri].getIndexPages();
	if (_Location[uri].getFlagAutoInx())
		_autoIndex = _Location[uri].getAutoIndex();
	if (_Location[uri].getRootFlag())
		_root = _Location[uri].getRoot();
	if (_Location[uri].getReturnFlag())
		_returnPages = _Location[uri].getReturnPages();
	if (_Location[uri].getFlagErrorPages())
		_pagesError = _Location[uri].getPagesError();
	if (_Location[uri].getFlagCgi())
		_Cgi = _Location[uri].getCgiPath();
	if (!_returnPages.empty())
	{
		buildReturnPage();
		return ;
	}
	if (isRepertory(_root, _startUri) == 3)
	{
		if (_startUri[_startUri.size() -1] != '/')
		{
			this->_statusCode = 301;
			std::vector<std::string> filesList;
			std::string tmp  = _root + _startUri;
			DIR *dir = opendir(tmp.c_str());
			if (!dir)
				throw ErrorResponse("Error in the opening of the directory pointer by the URI", ERR_CODE_INTERNAL_ERROR);
			struct dirent *fileRead;
			while ((fileRead = readdir(dir)) != NULL)
				if (strcmp(fileRead->d_name, ".") != 0 || (strcmp(fileRead->d_name, "..") != 0 && _startUri != "/"))
					filesList.push_back(fileRead->d_name);
			for (std::vector<std::string>::iterator it = filesList.begin(); it != filesList.end(); it++) {
				if (*it == ".")
					continue ;
				if (*it == "..")
					continue ;
				std::string file = (*it);
				if (isRepertory(_root, _startUri + "/"+ file) == 1 && file[file.size() - 5] == '.')
				{
					_startUri = _startUri + "/"+ (*it);
					closedir(dir);
					buildPage();
					return;
				}
			}
			closedir(dir);
			throw ErrorResponse("Error in the request: URI is not a directory", ERR_CODE_NOT_FOUND);
		}
		else {
			if (!_indexPages.empty()) {
				for (std::vector<std::string>::iterator it = _indexPages.begin(); it != _indexPages.end(); it++) {
					index = (*it)[0] == '/' ? (*it).substr(1, std::string::npos) : (*it);
					path = _startUri + index;
					if (isRepertory(_root, path) == 1) {
						_startUri = path;
						buildPage();
					}
				}
			}
			else if (_autoIndex)
				buildAutoindex();
			else
				throw ErrorResponse("Error in the request: URI points nowhere", ERR_CODE_FORBIDDEN);
		}
	}
	else if (_startUri.find("/cgi/print_response.php") != std::string::npos)
		buildCgi();
	else if (_startUri.find("/cgi/print_res_py.py") != std::string::npos)
		buildCgi();
	else if (isRepertory(_root, _startUri) == 1)
		buildPage();
	else
		throw ErrorResponse("Error in the request: URI is not a directory", ERR_CODE_NOT_FOUND);
}

void	Response::buildPost(void) {

	std::istringstream	iss(_headers["Content-Type"]);
	std::string			boundary_start, boundary_end, line, filename, response_header;
	std::size_t 		pos;
	std::ostringstream	file_data;

	if (!std::getline(iss, boundary_start, '=') || !std::getline(iss, boundary_start) || boundary_start.empty())
		throw ErrorRequest("Error in the request: content-type not well formatted", ERR_CODE_INTERNAL_ERROR);
	boundary_start = "--" + boundary_start;
	boundary_end = "--" + boundary_start + "--";
	
	// Go to the start of the boundary
	pos = _body.find(boundary_start);
	iss.clear();
	iss.str(_body);
	iss.seekg(pos + boundary_start.size());
	
	// Parse the multipart form data
	while (std::getline(iss, line) && line != boundary_end) {
		if (line.find("Content-Disposition: form-data;") != std::string::npos) {
			
			// Extract filename
			pos = line.find("filename=\"");
			if (pos != std::string::npos) {
				pos += 10; // Move past 'filename="'
				filename = line.substr(pos, line.find("\"", pos) - pos);
			}
		}

		// Skip headers
		while (std::getline(iss, line) && !line.empty() && line != "\r");

		// Extract file data
		while (std::getline(iss, line) && line != boundary_start && line != boundary_end) {
			file_data << line << "\n";
		}

		// Save the file
		std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary);
		if (!outfile.is_open())
			throw ErrorRequest("Error in the request: cannot open the file", ERR_CODE_INTERNAL_ERROR);
		outfile << file_data.str();
		outfile.close();

		// // Send a response to the client
		// response_header = "HTTP/1.1 200 OK\r\n";
		// response_header += "Content-Length: 0\r\n";
		// response_header += "Content-Type: text/plain\r\n";
		// response_header += "\r\n";

		// int error = send(fd, response_header.c_str(), response_header.length(), MSG_NOSIGNAL);
		// if (error == -1)
		// 	throw ErrorRequest("Error in the request: cannot send the response", ERR_CODE_INTERNAL_ERROR);
	}
}

Response::~Response(void) {	return ; }
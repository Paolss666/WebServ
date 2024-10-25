/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 09:59:17 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/25 10:27:54 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

// besoim de response
std::vector<std::string>	Response::MakeEnvCgi(std::string &cgi) {
	std::vector<std::string>	env;
	std::stringstream			buffer;
	std::ostringstream			oss;
	std::string					pathInfo, keyWord, query;
	size_t						pos;

	std::ifstream fileRequested(cgi.c_str());
	if (fileRequested.good() == false)
		throw ErrorResponse("Error in the opening of the file requested", ERR_CODE_NOT_FOUND);
	
	buffer  << fileRequested.rdbuf();

	_response_body = buffer.str();
	if (_response_body.size() > _maxBodySize) {
		_response_body = "";
		throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
	}
	oss << _response_body;
	exportENV(env, "CONTENT_LENGTH", oss.str());
	exportENV(env, "CONTENT_TYPE", "text/html");
	pathInfo = "/home/npaolett/42/WebServerGet/" + cgi; // Assicurati che sia corretto
	exportENV(env, "GATEWAY_INTERFACE", "CGI/1.1");
	for (std::map<std::string, std::string>::iterator it = _response_header.begin(); it != _response_header.end(); it++) {
		keyWord = it->first;
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
	pos = _startUri.find_last_of('?');
	query = _startUri.substr(pos, _startUri.size());
	exportENV(env, "QUERY_STRING",query);
	exportENV(env, "REDIRECT_STATUS", "200"); // 200 to indicate the requesst was hande correctly
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
	std::string cgi, uri_for_cgi, format, vrai_f, _Cgi;
	std::vector<std::string> vecEnv = MakeEnvCgi(_Cgi);
	std::stringstream responseStream;
	std::ostringstream responseHeaders;
	std::ofstream writeInPipe;
	time_t	start;
	pid_t	pid;
	char * finalUriChar;
	int	fd[2], writeStatus, cgiFdOut, cgiFdIn;
	size_t extention, f_interr;
	
	extention = _startUri.find_last_of('.');
	format = _startUri.substr(extention, _startUri.size());
	f_interr = format.find_last_of('?');
	vrai_f = format.substr(0, f_interr);
	_Cgi = _startUri.substr(0, extention);
	
	if (vrai_f == ".php")
		_Cgi += ".php";
	else if (vrai_f == ".py")
		_Cgi += ".py";
	_Cgi = _root + _Cgi;
	uri_for_cgi = _startUri.substr(extention, _startUri.size());
	if (access(_Cgi.c_str(), F_OK))
		throw ErrorResponse("Error in the request: access URI CGI", ERR_CODE_NOT_FOUND);

	cgiFdOut = open(".cgi.txt", O_WRONLY | O_CREAT | O_TRUNC);
	if (cgiFdOut == -1 || chmod(".cgi.txt", S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		(ft_close(cgiFdOut), throw ErrorResponse("Error in the request: FILE CGI", ERR_CODE_NOT_FOUND));

	cgiFdIn = open(".body_cgi.txt", O_WRONLY | O_CREAT | O_TRUNC);
	if (cgiFdIn == -1)
		(ft_close(cgiFdOut), ft_close(cgiFdOut), throw ErrorResponse("Error in the request: FILE CGI", ERR_CODE_NOT_FOUND));
	if (chmod(".body_cgi.txt", S_IRWXU | S_IRWXG | S_IRWXO) != 0)
		(ft_close(cgiFdOut), ft_close(cgiFdIn), throw ErrorResponse("Error in the request: FILE_body CGI", ERR_CODE_NOT_FOUND));
	if (pipe(fd) == - 1)
		(ft_close(cgiFdOut), ft_close(cgiFdIn), throw ErrorResponse("Error in the request: PIPE CGI", ERR_CODE_NOT_FOUND));

	// HERE I ll put or build the find version 4 Different CGI php or PY
	finalUriChar = const_cast<char*>(_startUri.c_str());
	_cgi = const_cast<char*>(_Cgi.c_str());

	start = time(NULL);
	pid = fork();
	if (pid == -1)
		(ft_close(cgiFdOut), ft_close(cgiFdIn), throw ErrorResponse("Error in the request: FILE FORK CGI", 500));
	//child
	if (pid == 0) {
		ft_close(fd[1]);
		dup2(fd[0], STDIN_FILENO); //stdin devient pipe[0]
		ft_close(fd[0]);
		dup2(cgiFdOut, STDOUT_FILENO); //stdout devient pipe[1]
		ft_close(cgiFdOut);
		char *av[] = {_cgi,finalUriChar,  NULL};
		char **env = vectorStringToChar(vecEnv);
		ft_close(cgiFdIn);
		execve(_cgi, av, env);
		perror("execve");
		sleep(3);
		exit(EXIT_FAILURE);
	}
	ft_close(fd[0]);
	
	writeInPipe.open(".body_cgi.txt");
	if (!writeInPipe.is_open()) {
		(ft_close(fd[1]), throw ErrorResponse("Error in the request: FILE OPEN body CGI", 500));
	}
	writeInPipe << _response_body;
	writeInPipe.close();
	ft_close(fd[1]);
	
	while (true) {
		pid_t	pid_result = waitpid(pid, &writeStatus, WNOHANG);
		if (pid_result > 0)// success
			break;
		if (pid_result == 0) { // no state change detected
			time_t	end = time(NULL);
			if (end - start >= 10) {
				(ft_close(cgiFdOut), ft_close(cgiFdIn));
				if (kill(pid, SIGKILL) == -1)
					perror("kill");
				pid_result = waitpid(pid, &writeStatus, WNOHANG);
				_statusCode = 500;
				(remove(".cgi.txt"), remove(".body_cgi.txt"));
				throw ErrorResponse("Error in the request: FILE_____ CGI", 500);
			}
		}
		else { // waitpid failed
			(ft_close(cgiFdOut), ft_close(cgiFdIn), throw ErrorResponse("Error in the request: waitPID CGI", 500));
		}
	}
	(ft_close(cgiFdOut), ft_close(cgiFdIn));

	std::ifstream ifs(".cgi.txt");
	if (!ifs.is_open())
	    throw ErrorResponse("Error in the request: Open FILE CGI", 404);
	(remove(".cgi.txt"), remove(".body_cgi.txt"));
	responseStream << ifs.rdbuf();
	_response_body = responseStream.str(); 
	if (_response_body.size() > _maxBodySize) {
	    _response_body = "";
	    throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
	}

	responseHeaders << "HTTP/1.1 " << _statusCode << " OK\r\n"
					<< "Content-Type: text/html; charset=UTF-8\r\n"
					<< "Content-Length: " << _response_body.size() << "\r\n"
					<< "\r\n";

	_response_message = responseHeaders.str() + _response_body;
	_response_ready = true;
}

void	Response::exportENV(std::vector<std::string> &env, const std::string &key, const std::string &value) {
	env.push_back(key + "=" + value);
}

void	Response::buildReturnPage() {
	int	returnCode;
	std::string redirUri;
	
	for (std::map<int, std::string>::iterator i = _returnPages.begin(); i != _returnPages.end(); i++) {
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
	if ((_startUri.size() -1 ) == '/' && redirUri[0] == '/') {
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

	buildPage();
}

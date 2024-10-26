/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 14:12:48 by benoit            #+#    #+#             */
/*   Updated: 2024/10/26 16:23:02 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
	std::string pathInfo = "./" + cgi;
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
	std::cout << vrai_f << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ---- \n";
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
		remove(".cgi.txt");
		remove(".body_cgi.txt");
		(close(cgiFdOut), close(cgiFdIn));
		throw ErrorResponse("Error in the request: PIPE CGI", ERR_CODE_NOT_FOUND);
	}

	std::vector<std::string> vecEnv = MakeEnvCgi(_Cgi);

	char * finalUriChar = const_cast<char*>(_startUri.c_str());
	char * _cgi = const_cast<char*>(_Cgi.c_str());

	time_t	start = time(NULL);
	pid_t	pid = fork();
	if (pid == -1)
	{
		(close(cgiFdOut), close(cgiFdIn));
		throw ErrorResponse("Error in the request: FILE FORK CGI", 500);
	}
	char **env = vectorStringToChar(vecEnv);
	if (pid == 0)//child
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);//stdin devient pipe[0]
		close(fd[0]);
		dup2(cgiFdOut, STDOUT_FILENO);//stdout devient pipe[1]
		close(cgiFdOut);
		char *av[] = {_cgi,finalUriChar,  NULL};
		for (int i = 0; av[i]; i++)
			std::cerr << av[i] << std::endl;
		close(cgiFdIn);
		execve(_cgi, av, env);
		// execve failed
		// for(size_t i = 0; env[i] ; i++){
		// 	delete env[i];
		// }
		// delete [] env;
		perror("execve");

		sleep(2);
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
				_statusCode = 405;
				remove(".cgi.txt");
				remove(".body_cgi.txt");
				throw ErrorResponse("Error in the request: TIME OUT CGI", 405);
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

// void	add_to_env(std::vector<std::string> & storage, std::vector<char *> & env, std::string key, std::string value) {
// 	storage.push_back(key + "=" + value);
// 	env.push_back(const_cast<char*>(storage.back().c_str()));
// }

// std::vector<char *>	Response::MakeEnvCgi(std::string & cgi, std::vector<std::string> & storage) {
// 	std::vector<char *>			env;
// 	std::stringstream			buffer;
// 	std::ostringstream			oss;
// 	std::string					keyWord;
// 	std::ifstream				fileRequested(cgi.c_str());
// 	size_t						i;

// 	if (fileRequested.good() == false)
// 		throw ErrorResponse("Error in the opening of the file requested", ERR_CODE_NOT_FOUND);
	
// 	buffer << fileRequested.rdbuf();
// 	_response_body = buffer.str();
	
// 	if (_response_body.size() > _maxBodySize) {
// 		_response_body.clear();
// 		throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
// 	}

// 	oss << _response_body;
// 	add_to_env(storage, env, "CONTENT_LENGTH", oss.str());
// 	add_to_env(storage, env, "GATEWAY_INTERFACE", "CGI/1.1");

// 	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++) {
// 		keyWord = it->first;
// 		i = 0;
// 		while (keyWord[i]) {
// 			keyWord[i] = toupper(keyWord[i]);
// 			if (keyWord[i] == '-')
// 				keyWord[i] = '_';
// 			i++;
// 		}
// 		keyWord.insert(0, "HTTP_");
// 		if (keyWord != "HTTP_CONNECTION")
// 			add_to_env(storage, env, keyWord, it->second);
// 	}

// 	add_to_env(storage, env, "QUERY_STRING", _startUri.substr(_startUri.find_last_of('?'), _startUri.size()));
// 	add_to_env(storage, env, "REDIRECT_STATUS", "200");
// 	add_to_env(storage, env, "REQUEST_METHOD", _request_line["method"]);
// 	add_to_env(storage, env, "SCRIPT_NAME", _startUri);
// 	add_to_env(storage, env, "SCRIPT_FILENAME", "./" + cgi);
// 	add_to_env(storage, env, "SERVER_PROTOCOL", "HTTP/1.1");
// 	add_to_env(storage, env, "SERVER_SOFTWARE", "webserv");

// 	env.push_back(NULL);

// 	return env;
// }

// void	Response::buildCgi(void)
// {
// 	std::vector<std::string>	storage;
// 	size_t						extention = _startUri.find_last_of('.');
// 	std::string					root_Uri = _root + _startUri;
// 	std::string					format = _startUri.substr(extention, _startUri.size());
// 	std::string					lang = _startUri.substr(0, extention);
// 	std::string					uri_for_cgi = _startUri.substr(extention, _startUri.size());
// 	std::vector<char *>			env;
// 	int							writeStatus, cgiFdOut, cgiFdIn, cgiErr, tmp, fd[2];
// 	time_t						start, end;
// 	pid_t						pid, pid_result;
// 	std::ofstream				writeInPipe;
// 	std::stringstream			responseStream;

// 	// Prepare script to be executed, ppipes and env variables
// 	if (format.substr(0, format.find_last_of('?')) == ".php")
// 		lang += ".php";
// 	else if (format.substr(0, format.find_last_of('?')) == ".py")
// 		lang += ".py";
// 	lang = _root + lang;
// 	if (access(lang.c_str(), F_OK))
// 		throw ErrorResponse("Error in the request: access URI CGI", ERR_CODE_NOT_FOUND);
// 	storage.reserve(_headers.size() + 9);
// 	env = MakeEnvCgi(lang, storage);
	
// 	cgiFdOut = open(".cgi.txt", O_WRONLY | O_CREAT | O_TRUNC);
// 	if (cgiFdOut == -1 || chmod(".cgi.txt", S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
// 		ft_close(cgiFdOut);
// 		throw ErrorResponse("Error in the request: FILE CGI", ERR_CODE_NOT_FOUND);
// 	}

// 	cgiFdIn = open(".body_cgi.txt", O_WRONLY | O_CREAT | O_TRUNC);
// 	if (cgiFdIn == -1 || chmod(".body_cgi.txt", S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
// 		ft_close(cgiFdOut), ft_close(cgiFdOut);
// 		throw ErrorResponse("Error in the request: FILE CGI", ERR_CODE_NOT_FOUND);
// 	}
	
// 	if (pipe(fd) == - 1) {
// 		remove(".cgi.txt"), remove(".body_cgi.txt");
// 		ft_close(cgiFdOut), ft_close(cgiFdIn);
// 		throw ErrorResponse("Error in the request: PIPE CGI", ERR_CODE_NOT_FOUND);
// 	}

// 	start = time(NULL);
// 	pid = fork();
// 	if (pid == -1) {
// 		(ft_close(cgiFdOut), ft_close(cgiFdIn));
// 		throw ErrorResponse("Error in the request: FILE FORK CGI", 500);
// 	} else if (pid == 0) { // child
// 		dup2(fd[0], STDIN_FILENO);
// 		dup2(cgiFdOut, STDOUT_FILENO);
// 		char *args[] = {const_cast<char*>(lang.c_str()), const_cast<char*>(_startUri.c_str()), NULL};
		
// 		// Close all fds
// 		ft_close(fd[1]), ft_close(fd[0]), ft_close(cgiFdOut), ft_close(cgiFdIn);
// 		_host.close_everything();
// 		tmp = dup(STDERR_FILENO);
// 		cgiErr = open("/dev/null", O_WRONLY);
// 		dup2(cgiErr, STDERR_FILENO);
// 		ft_close(cgiErr);
// 		for (int i = 0; i < 100; i++)
// 			if (i != STDIN_FILENO && i != STDOUT_FILENO && i != STDERR_FILENO)
// 				ft_close(i);
// 		dup2(tmp, STDERR_FILENO);
// 		ft_close(tmp);

// 		execve(const_cast<char*>(lang.c_str()), args, env.data());
// 		perror("execve");
// 		sleep(2);
// 		return ;
// 	}
	
// 	ft_close(fd[0]);
// 	writeInPipe.open(".body_cgi.txt");
// 	if (!writeInPipe.is_open()) {
// 		ft_close(fd[1]);
// 		throw ErrorResponse("Error in the request: FILE OPEN body CGI", 500);
// 	}
// 	writeInPipe << _response_body;
// 	writeInPipe.close();
// 	ft_close(fd[1]);
	
// 	_statusCode = ERR_CODE_SUCCESS;
// 	while (true) {
// 		pid_result = waitpid(pid, &writeStatus, WNOHANG);
// 		if (pid_result > 0)// success
// 			break;
// 		if (pid_result == 0) { // no state change detected
// 			end = time(NULL);
// 			if (end - start >= 10) {
// 				ft_close(cgiFdOut), ft_close(cgiFdIn);
// 				if (kill(pid, SIGKILL) == -1)
// 					perror("kill");
// 				pid_result = waitpid(pid, &writeStatus, WNOHANG);
// 				_statusCode = ERR_CODE_MET_NOT_ALLOWED;
// 				remove(".cgi.txt"), remove(".body_cgi.txt");
// 				throw ErrorResponse("Error in the request: TIME OUT CGI", 405);
// 			}
// 		}
// 		else { // waitpid failed
// 			perror("waitpid");
// 			ft_close(cgiFdOut), ft_close(cgiFdIn);
// 			throw ErrorResponse("Error in the request: waitPID CGI", 500);
// 		}
// 	}
// 	ft_close(cgiFdOut), ft_close(cgiFdIn);

// 	std::ifstream ifs(".cgi.txt");
// 	if (!ifs.is_open())
// 		throw ErrorResponse("Error in the request: Open FILE CGI", 404);
// 	remove(".body_cgi.txt"), remove(".cgi.txt");

// 	responseStream << ifs.rdbuf();
// 	_response_body = responseStream.str(); 
// 	if (_response_body.size() > _maxBodySize) {
// 		_response_body = "";
// 		throw ErrorResponse("Error in the size of the file requested", ERR_CODE_NOT_FOUND);
// 	}

// 	_response_message = "HTTP/1.1 " + _statusCode;
// 	_response_message += " OK\r\n";
// 	_response_message += "Content-Type: text/html; charset=UTF-8\r\n";
// 	_response_message += "Content-Length: " + _response_body.size();
// 	_response_message += "\r\n\r\n";

// 	_response_message += _response_body;

// 	std::cout << "Response: " << _response_message << std::endl;

// 	_response_ready = true;
// }

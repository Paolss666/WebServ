/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benoit <benoit@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:30 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/26 12:58:46 by benoit           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Location::Location(void): _indexFlag(false), _CgiFlag(false), _ReturnFlag(false), _ErPages(false), _AutoFlag(false),
	_AutoIndex(false), _MetFlag(false), _rootflag(false), _flagPost(0), _flagGet(0), _flagDelete(0) { return ; }

void        Location::InLoc_Methos(std::istringstream& iss) {
    std::string line;

    while (iss >> line) {
	    if (line != "GET" && line != "POST" && line != "DELETE" )
            throw ErrorConfFile("In conf file: location: unkmown method");
		if (line == "GET")
			_flagGet = 1;
		if (line == "POST")
			_flagPost = 1;
		if (line == "DELETE")
			_flagDelete = 1;
        _Methods.push_back(line);
    }
    _MetFlag = true;
}

void	Location::InLoc_Index(std::istringstream& iss) {
    std::string     indx;

    if (!(iss >> indx) || indx.empty())
        throw ErrorConfFile("In conf file: location: index");
    else
	    _Indx.push_back(indx);
	
    _indexFlag = true;
	
    while (iss >> indx) {
		if (indx.empty() || indx.find(".") == std::string::npos)
			throw ErrorConfFile("In conf file: index ");
		else
			_Indx.push_back(indx);
	}
}


void	Location::InLoc_Cgi(std::istringstream& iss) {
    std::string	cgiPath;

	if (!(iss >> cgiPath))
		throw ErrorConfFile("In conf file: location: cgi;");

	if (cgiPath.compare(0, 4, "www/") != 0 && cgiPath.compare(0, 8, "www/cgi") != 0)
		throw ErrorConfFile("In conf file: location: cgi path;");

	_CgiPath = cgiPath;

	struct stat info;
	if (stat(_CgiPath.c_str(), &info) != 0)// cannot access path 
		throw ErrorConfFile("In conf file: location: cgi cannot access;");

	_CgiFlag = true;
}

int	Location::InLoc_p_Return(std::string &codeRetrn) {
	size_t	ix = codeRetrn.find_first_not_of("0123456789");
	std::string	path;
	
	if (ix == std::string::npos) {
		int errorCode = strtol(codeRetrn.c_str(), NULL, 10);
		if (errorCode < 300 || errorCode > 308)
			throw ErrorConfFile("In conf file: error_page  x < 300 || x > 308 ");
		return (errorCode);
	} else
		throw ErrorConfFile("In conf file: error_page parseCde");
}

void	Location::InLoc_Return(std::istringstream& iss) {
	std::string			rtrn;
	int					codeErr;
	std::vector<int>	V_Code;
	
	if (!(iss >> rtrn))
		throw ErrorConfFile("In conf file: location: return");
	codeErr = InLoc_p_Return(rtrn);
	V_Code.push_back(codeErr);
	while ((iss >> rtrn) && rtrn.find_first_not_of("0123456789") == std::string::npos) {
		codeErr = InLoc_p_Return(rtrn);
		V_Code.push_back(codeErr);
	}
	if (rtrn.empty())
		throw ErrorConfFile("In conf file: return empty");
	if (rtrn[0] != '/' && rtrn.find("..") != std::string::npos)
		throw ErrorConfFile("In conf file: return 2");
	if (iss >> rtrn)
		throw ErrorConfFile("In conf file: return 3");
	for (size_t i = 0; i < V_Code.size(); i++)
		_Retourn[V_Code[i]] = rtrn;
	_ReturnFlag = true;
}

int	Location::InLoc_p_errorCodes(std::string &pgError) {
	size_t		ix = pgError.find_first_not_of("0123456789");
	std::string	path;

	if (ix == std::string::npos) {
		int errorCode = strtol(pgError.c_str(), NULL, 10);
		if (errorCode < 100 || errorCode > 599)
			throw ErrorConfFile("In conf file: error_page  x < 100 || x > 599 ");
		return (errorCode);
	} else
		throw ErrorConfFile("In conf file: error_page parseCde");
}

void	Location::InLoc_ErPages(std::istringstream& iss) {
    std::string			pgError;
	std::vector<int>	erroCodeVector;
	int					eCode;

	if (!(iss >> pgError))
		throw ErrorConfFile("In conf file: error_pages");
	eCode = InLoc_p_errorCodes(pgError);
	erroCodeVector.push_back(eCode);
	while ((iss >> pgError) && pgError.find_first_not_of("0123456789") == std::string::npos) {
		eCode =  InLoc_p_errorCodes(pgError);
		erroCodeVector.push_back(eCode);
	}
	if (pgError.empty())
		throw ErrorConfFile("In conf file: error_pages 1");
	if (pgError[0] != '/' && pgError.find("..") != std::string::npos)
		throw ErrorConfFile("In conf file: error_pages 2");
	if (iss >> pgError)
		throw ErrorConfFile("In conf file: error_pages format /html");
	for (size_t i = 0; i < erroCodeVector.size(); i++)
		_PageError[erroCodeVector[i]] = pgError;

	_ErPages = true;
}

void	Location::InLoc_AutoIndex(std::istringstream& iss) {
    std::string autoIndex;

	if (iss >> autoIndex) {
		if (autoIndex == "on")
			_AutoIndex = true;
		else
			_AutoIndex = false;
	}
    _AutoFlag = true;
}

void	Location::InLoc_root(std::istringstream& iss) {
	std::string 	pathRoot;

	if (!(iss >> pathRoot))
		throw ErrorConfFile("In conf file: root path don't found;");

	if (pathRoot.compare(0, 3, "www") != 0 && pathRoot.compare(0, 4, "www/") != 0)
		throw ErrorConfFile("In conf file: root in location: wrong path;");

	_Root = pathRoot;

	struct stat info;
	if (stat(_Root.c_str(), &info) != 0)
		throw ErrorConfFile(("In conf file: root in location: cannot access path or file " + _Root).c_str());

	_rootflag = 1;
}	

void	Location::ParseLocation(std::istream &file) {
	std::string			line;
	std::string			keyword;
	std::istringstream	iss;
	
	std::cout << CYAN "URI: " RESET << this->getUri() << std::endl;
	while (std::getline(file, line)) {
		iss.clear();
		iss.str(line);
		
		if (line.empty() || line == "\t\t")
			continue ;
		if (!(iss >> keyword))
			throw ErrorConfFile("In conf file: Location");
	
		if (keyword == "methods" && !_MetFlag)
			InLoc_Methos(iss);
		else if (keyword == "index" && !_indexFlag)
			InLoc_Index(iss);
		else if (keyword == "cgi" && !_CgiFlag)
			InLoc_Cgi(iss);
		else if (keyword == "return" && !_ReturnFlag)
			InLoc_Return(iss);
		else if (keyword == "error_page" && !_ErPages)
			InLoc_ErPages(iss);
		else if (keyword == "autoindex" && !_AutoFlag)
			InLoc_AutoIndex(iss);
		else if (keyword == "root" && !_rootflag)
			InLoc_root(iss);
		else if (keyword == "}") {
			if (!_MetFlag) {
				_Methods.push_back("GET");
				_Methods.push_back("POST");
				_Methods.push_back("DELETE");
				_flagGet = 1;
				_flagPost = 1;
				_flagDelete = 1;
			}
			break;
		} else
			throw ErrorConfFile("In conf file: Location 2");
	}
	for (size_t i = 0; i < _Methods.size(); i++)
		std::cout << BLUE "Methods: " RESET << _Methods[i] << std::endl;
}

Location::~Location(void) { return; }

int	Location::getFlagIndex() { return (this->_indexFlag); }

std::vector<std::string>&	Location::getIndexPages() { return (this->_Indx); }

int	Location::getRootFlag() { return (this->_rootflag); }

std::string	Location::getRoot() { return (this->_Root); }

int	Location::getReturnFlag() { return (this->_ReturnFlag); }

std::map<int, std::string>& Location::getReturnPages() { return (this->_Retourn); }

int	Location::getFlagErrorPages() { return (this->_ErPages); }

std::map<int, std::string>& Location::getPagesError() { return (this->_PageError); }

int	Location::getFlagAutoInx() { return (this->_AutoFlag); }

bool	Location::getAutoIndex() { return(this->_AutoIndex); }

std::vector<std::string>	Location::getMtods(void) { return (this->_Methods); }

void	Location::setUri(std::string uri) { this->_Uri = uri; }

int	Location::getFlagCgi(void) { return (this->_CgiFlag); }

int	Location::getFlagPost(void) { return (this->_flagPost); }

int	Location::getFlagGet(void) { return (this->_flagGet); }

int	Location::getFlagDelete(void) { return (this->_flagDelete); }

std::string	Location::getCgiPath(void) { return (this->_CgiPath); }

std::string	Location::getUri(void) {    return(this->_Uri); }
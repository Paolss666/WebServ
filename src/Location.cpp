/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:30 by bdelamea          #+#    #+#             */
/*   Updated: 2024/10/08 17:33:18 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Location::Location(void)
{
    _MetFlag = false;
    _indexFlag = false;
    _CgiFlag = false;
    _ReturnFlag = false;
    _ErPages = false;
    _AutoFlag = false;
    _AutoIndex = false;
    _MetFlag   = false;
	_rootflag = false;
    return;
}

Location::~Location(void)
{return;}

void      Location::setUri(std::string uri)
{
    this->_Uri = uri;
}

std::string     Location::getUri(void)
{
    return(this->_Uri);
}

void        Location::InLoc_Methos(std::istringstream& iss)
{
    std::string line;
    if (!(iss >> line))
        throw ErrorConfFile("Error conf file: location : methods");
    std::cout << "methods -> " << line << std::endl;
    while (iss >> line)
    {
        if (line != "GET" && line != "POST" && line != "DELETE" )
            throw ErrorConfFile("Error conf file: location : wrong method");
        _Methods.push_back(line);
        std::cout << "methods -> " << line << std::endl;
    }
    _MetFlag = true;
}

void        Location::InLoc_Index(std::istringstream& iss)
{
    std::string     indx;

    if (!(iss >> indx) || indx.empty())
        throw ErrorConfFile("Error conf file : location : index");
    else
	    _Indx.push_back(indx);
	
    _indexFlag = true;
	
    while (iss >> indx)
	{
		// std::cout << std::getline(iss, index) << "--- "<< std::endl;
		if (indx.empty() || indx.find(".") == std::string::npos)
			throw ErrorConfFile("Error conf file: index ");
		else
			_Indx.push_back(indx);
	}
	printVector(_Indx);
    
}


void        Location::InLoc_Cgi(std::istringstream& iss)
{
    std::string 	cgiPath;

	if (!(iss >> cgiPath))
		throw ErrorConfFile("Error conf file: location: cgi;");
	
	// std::cout << " path-root found --->  " << pathRoot << std::endl; 

	if (cgiPath.compare(0, 4, "www/") != 0 && cgiPath.compare(0, 8, "www/cgi") != 0)
		throw ErrorConfFile("Error conf file: location: cgi path;");

	_CgiPath = cgiPath;

	struct stat info;
	if (stat(_CgiPath.c_str(), &info) != 0)// cannot access path 
		throw ErrorConfFile("Error conf file: location: cgi cannot access;");
	
	std::cout << " cgi path --->  " << _CgiPath << std::endl; 

	_CgiFlag = true;
}

int	Location::InLoc_p_Return(std::string &codeRetrn)
{
	size_t	ix = codeRetrn.find_first_not_of("0123456789");
	std::string	path;
	if (ix == std::string::npos)
	{
		int errorCode = strtol(codeRetrn.c_str(), NULL, 10);
		if (errorCode < 300 || errorCode > 308)
			throw ErrorConfFile("Error in the conf file : error_page  x < 100 || x > 599 ");
		return (errorCode);
	}
	else
		throw ErrorConfFile("Error in the conf file : error_page parseCde");
}

void        Location::InLoc_Return(std::istringstream& iss)
{
    std::string            rtrn;
    if (!(iss >> rtrn))
        throw ErrorConfFile("Error conf file: location: return");
    int                    codeErr;
    std::vector<int>       V_Code;
    codeErr = InLoc_p_Return(rtrn);
    V_Code.push_back(codeErr);
    while ((iss >> rtrn) && rtrn.find_first_not_of("0123456789") == std::string::npos)
	{
		codeErr = InLoc_p_Return(rtrn);
		V_Code.push_back(codeErr);
	}
	if (rtrn.empty())
		throw ErrorConfFile("Error conf file : return empty");
	if (rtrn[0] != '/' && rtrn.find("..") != std::string::npos)
		throw ErrorConfFile("Error conf file: return 2");
	if (iss >> rtrn)
		throw ErrorConfFile("Error conf file: return 3");
	for (size_t i = 0; i < V_Code.size(); i++)
		_Retourn[V_Code[i]] = rtrn;
	_ReturnFlag = true;
    Print_map_code_return(_Retourn);
}

int	Location::InLoc_p_errorCodes(std::string &pgError)
{
	size_t	ix = pgError.find_first_not_of("0123456789");
	std::string	path;
	if (ix == std::string::npos)
	{
		int errorCode = strtol(pgError.c_str(), NULL, 10);
		if (errorCode < 100 || errorCode > 599)
			throw ErrorConfFile("Error in the conf file : error_page  x < 100 || x > 599 ");
		return (errorCode);
	}
	else
		throw ErrorConfFile("Error in the conf file : error_page parseCde");
}

void        Location::InLoc_ErPages(std::istringstream& iss)
{
    std::string pgError;;
	std::vector<int> erroCodeVector;
	int				eCode;

	if (!(iss >> pgError))
		throw ErrorConfFile("Error conf file: error_pages");
	eCode = InLoc_p_errorCodes(pgError);
	erroCodeVector.push_back(eCode);
	while ((iss >> pgError) && pgError.find_first_not_of("0123456789") == std::string::npos)
	{
		eCode =  InLoc_p_errorCodes(pgError);
		erroCodeVector.push_back(eCode);
	}
	if (pgError.empty())
		throw ErrorConfFile("Error conf file: error_pages 1");
	if (pgError[0] != '/' && pgError.find("..") != std::string::npos)
		throw ErrorConfFile("Error conf file: error_pages 2");
	if (iss >> pgError)
		throw ErrorConfFile("Error conf file: error_pages format /html");
	for (size_t i = 0; i < erroCodeVector.size(); i++)
		_PageError[erroCodeVector[i]] = pgError;
	Print_map_code_errors(_PageError);
	_ErPages = true;
}

void        Location::InLoc_AutoIndex(std::istringstream& iss)
{
    std::string autoIndex;

	if (iss >> autoIndex)
	{
		if (autoIndex == "on")
			_AutoIndex = true;
		else
			_AutoIndex = false;
	}
	std::cout << "autoindex = " << _AutoIndex << std::endl;
    _AutoFlag = true;
}

void	Location::InLoc_root(std::istringstream& iss)
{
	std::string 	pathRoot;

	if (!(iss >> pathRoot))
		throw ErrorConfFile("Error conf file: root path don't found;");
	
	std::cout << " path-root found --->  " << pathRoot << std::endl; 

	if (pathRoot.compare(0, 3, "www") != 0 && pathRoot.compare(0, 4, "www/") != 0)
		throw ErrorConfFile("Error conf file: root wrong path;");

	_Root = pathRoot;


	struct stat info;
	if (stat(_Root.c_str(), &info) != 0)// cannot access path 
		throw ErrorConfFile("Error : root : cannot access path or file");
	
	std::cout << " root found --->  " << _Root << std::endl; 

	_rootflag = 1;
}	

void        Location::ParseLocation(std::istream &file)
{
    std::string line;
    while (std::getline(file, line))
    {
		std::istringstream iss(line);
		std::string	keyword;
        
        if (line.empty() || line == "\t\t")//
		    continue ;
        if (!(iss >> keyword))
            throw ErrorConfFile("Error in conf file: Location");
    
        std::cout << "-------------LOCATION-----------------\n";
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
        else if (keyword == "}")
            break;
        else
            throw ErrorConfFile("Error in conf file: Location 2");
    }
    return;
}


int		Location::getFlagIndex()
{
	return (this->_indexFlag);
}

std::vector<std::string> Location::getIndexPages()
{
	return (this->_Indx);
}

int		Location::getRootFlag()
{
	return (this->_rootflag);
}

std::string		Location::getRoot()
{
	return (this->_Root);
}

int		Location::getReturnFlag()
{
	return (this->_ReturnFlag);
}

std::map<int, std::string> Location::getReturnPages()
{
	return (this->_Retourn);
}

int		Location::getFlagErrorPages()
{
	return (this->_ErPages);
}

std::map<int, std::string> Location::getPagesError()
{
	return (this->_PageError);
}



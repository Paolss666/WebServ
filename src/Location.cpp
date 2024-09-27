
#include "../incl/Location.hpp"

Location::Location(void)
{
    _MetFlag = false;
    _indexFlag = false;
    _CgiFlag = false;
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

void        Location::p_Methos(std::istringstream& iss)
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

void        Location::p_Index(std::istringstream& iss)
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


void        Location::p_Cgi(std::istringstream& iss)
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

void        Location::ParseLocation(std::istream &file)
{
    std::string line;
    while (std::getline(file, line))
    {
		std::istringstream iss(line);
		std::string	keyword;
        if (iss >> keyword)
        {
            std::cout << "-------------LOCATION-----------------\n";
            if (keyword == "}")
                break;
            else if (keyword == "methods" && !_MetFlag)
                p_Methos(iss);
            else if (keyword == "index" && !_indexFlag)
                p_Index(iss);
            else if (keyword == "cgi" && !_CgiFlag)
                p_Cgi(iss);
            // std::cout << keyword << "<--- keyword\n";
        }
    }
    return;
}
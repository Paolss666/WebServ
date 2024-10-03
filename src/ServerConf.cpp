/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:28 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 18:57:52 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/webserv.hpp"

ServerConf::ServerConf() {
    memset(&_address, 0, sizeof _address);
	_address.sin_family = AF_INET; // IPv4
	_address.sin_port = htons(PORT);// port par defaut 8080
	_address.sin_addr.s_addr = htonl(INADDR_ANY);// address par default(00000)
    _port = 8080; // port par default;
    _ip = "0.0.0.0"; // address far default;
    _NotBind = 0; // check if is bind or not;
	_maxBodySize = 0;
	_FdSocket = 0;
	_IndexPages = 0;
	_nbServer = 0;
	_maxBodyState = false;
    _StateListen = true;
	_DefaultPort = true;
    _IpDefault = true;
	_isServerName = false;
	_rootFlag = 0;
	_Autoindex = false;
	_errorFlag = false;
	_Default_server = false;
	_CheckDefaultServer = false;
	_ReturnFlag = false;
	_PortString = "";
	_rootPath = "";
	_FdEpoll = 0;
}

void	ServerConf::setNbServer(int nb)
{
	_nbServer = ++nb;
}

void    ServerConf::p_IpAddrs(void)
{
    if (_ip.empty())
        throw ErrorConfFile("Erron in the conf file: ip not found");
    if (_ip == "locahost")
    {
        _address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        _IpDefault = false;
        return;
    }
    if (_ip.find_first_not_of("0123456789.") != std::string::npos)
        throw ErrorConfFile("Error in conf file: ip");
    int	replace = 0;
	for (int i = 0; _ip[i]; i++)
		if (_ip[i] == '.')
			_ip.replace(i, 1, 1, ' '), replace++;
	if (replace != 3)
		throw ErrorConfFile("Error in the conf file : listen : wrong host (ip)4");
    std::istringstream iss(_ip);
	std::string	more;
	int	ip[4];
    // std::cout << "error _--> la _p" << _ip << std::endl;
	if (!(iss >> ip[0]) || !(iss >> ip[1]) || !(iss >> ip[2]) || !(iss >> ip[3]))
		throw ErrorConfFile("Error in the conf file : listen : wrong (ip)1");
	int res = 0;
	for (int i = 0; i < 4; i++)
	{
		if (ip[i] < 0 || ip[i] > 255)
			throw ErrorConfFile("Error in the conf file : listen : wrong (ip)2");
		res = res << 8 | ip[i];
	}
	if (iss >> more)
		throw ErrorConfFile("Error in the conf file : listen : wrong (ip)3");
	// std::cerr << "res ipAddrs = " << res << "\n";
	_address.sin_addr.s_addr = htonl(res);
	_IpDefault = false;
    return ;
}

void    ServerConf::p_Listen(std::istringstream& iss)
{
    std::string    line;
    if (!(iss >> line))
        throw ErrorConfFile("Error in the conf file");

    size_t inx = line.find(":");
    size_t p = line.find(".");

    _ip = line.substr(0, inx);
    p_IpAddrs();
    std::cout << "_ip bien parse --> " << _ip << std::endl;
    // std::cout << "---> p = " << _ip << std::endl;
    _PortString = line.substr(inx + 1, line.size()); 
    // std::cout << "---> portString = " << _PortString << std::endl;
    
    if (_PortString.empty())
        return ;

    // find_first_not_f Searches the string for the first character that does not match any of the characters specified in its arguments.
    if (_PortString.find_first_not_of("0123456789") != std::string::npos || _PortString.size() > 5)
		throw ErrorConfFile("Error in the conf file : listen : wrong IP");

    std::stringstream ss(_PortString);
	ss >> _port;

	if (_port > 65535)// PORT MAX VALEUR
		throw ErrorConfFile("Error in the conf file : listen : wrong port");
	_address.sin_port = htons(_port);
    std::cout << "_port --> assigne _address.sin_port -> " << _port << std::endl;
	_DefaultPort = false;
    if (iss >> line)
        throw   ErrorConfFile("Error in the conf file");
    _StateListen = false;
}   

void ServerConf::printServerNames() const
{
	for (size_t i = 0; i < _name.size(); ++i)
	{
        std::cout << i + 1 << ". " << _name[i] << std::endl;  // Stampa con un indice
    }
}

void	ServerConf::p_name(std::istringstream &iss)
{
	std::string serverNames;
	

	if (!(iss >> serverNames))
		throw ErrorConfFile("Error conf file: serve_name not found 1;");



	if (iss >> serverNames)
		throw ErrorConfFile("Error conf file: serve_name not found;");

	_name = serverNames;

	std::cout << "server name -> " << _name << std::endl;
	_isServerName = true;
}

void	ServerConf::p_Root(std::istringstream &iss)
{
	std::string 	pathRoot;

	if (!(iss >> pathRoot))
		throw ErrorConfFile("Error conf file: root path don't found;");
	
	std::cout << " path-root found --->  " << pathRoot << std::endl; 

	if (pathRoot.compare(0, 3, "www") != 0 && pathRoot.compare(0, 4, "www/") != 0)
		throw ErrorConfFile("Error conf file: root wrong path;");

	_rootPath = pathRoot;


	struct stat info;
	if (stat(_rootPath.c_str(), &info) != 0)// cannot access path 
		throw ErrorConfFile("Error : root : cannot access path or file");
	
	std::cout << " root found --->  " << _rootPath << std::endl; 

	_rootFlag = 1;
}

/*
Il comando autoindex è una direttiva comunemente usata nei server come NGINX.
In pratica, controlla se il server web deve mostrare automaticamente una lista di directory
quando non è presente un file di indice (come index.html) nella directory richiesta.
Funzionamento di autoindex

autoindex on: Se abilitato (on), il server mostrerà una lista dei file e delle sottocartelle presenti nella directory richiesta, se non è disponibile un file di indice.
autoindex off: Se disabilitato (off), il server restituirà un errore (tipicamente un 403 Forbidden) se non trova un file di indice nella directory richiesta. */

void	ServerConf::p_AutoIndex(std::istringstream & iss)
{
	std::string autoIndex;

	if (iss >> autoIndex)
	{
		if (autoIndex == "on")
			_Autoindex = true;
		else
			_Autoindex = false;
	}
	std::cout << "autoindex = " << _Autoindex << std::endl;
}


/* client_max_body_size 1M;:
Imposta la dimensione massima del corpo della richiesta a 1 megabyte (MB). Se un client tenta di inviare una richiesta con un corpo che supera questa dimensione,
NGINX restituirà un errore HTTP 413 (Request Entity Too Large). */


void	ServerConf::p_MaxClientBodySize(std::istringstream &iss)
{
	const size_t KB_IN_BYTES = 1024;
	const size_t MB_IN_BYTES = 1024 * 1024;
	const size_t MAX_SIZE_LIMIT = 3 * MB_IN_BYTES; // 3MB
	std::string		maxClntBodySize;
	
	if (!(iss >> maxClntBodySize) || maxClntBodySize.empty())
		throw ErrorConfFile("Error in conf file: client_max_body_size");
	if (maxClntBodySize.find_first_not_of("0123456789") == 0)
		throw ErrorConfFile("Error in conf file: client_max_body_size");

	size_t sizeValue = 0;
    std::string::size_type index = 0;

    // Convert numeric part to value
    sizeValue = std::strtoul(maxClntBodySize.c_str(), NULL, 10);
	std::cout << maxClntBodySize << " " << sizeValue << "<------- maxClntBodySize , sizeValue" << std::endl;
    // Check the unit part
    if (index < maxClntBodySize.size()) {
		size_t size = maxClntBodySize.size();
        char unitChar = maxClntBodySize[size - 1];
		std::cout << unitChar << "<-----"  << "size = " << size << std::endl;
        switch (unitChar) {
            case 'k':
            case 'K':
                sizeValue *= KB_IN_BYTES;
                break;
            case 'm':
            case 'M':
                sizeValue *= MB_IN_BYTES;
                break;
            case '\0':
                // Default is bytes if no unit provided
                break;
            default:
                throw ErrorConfFile("Error in the conf file: max_client_body_size: wrong unit, must be k, K, m, M or empty");
        }
    }

    // Check if the size is within the valid range
    if (sizeValue > MAX_SIZE_LIMIT) {
        throw ErrorConfFile("Error in the conf file: max_client_body_size: size has to be between 0 and 3MB");
    }

    _maxBodySize = sizeValue;
    _maxBodyState = true;
	std::cout << _maxBodySize << "<------- MaxBodySize " << std::endl;
}

int	ServerConf::p_errorCodes(std::string &pgError)
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

void	ServerConf::p_ErrorPages(std::istringstream& iss)
{
	std::string pgError;;
	std::vector<int> erroCodeVector;
	int				eCode;

	if (!(iss >> pgError))
		throw ErrorConfFile("Error conf file: error_pages");
	eCode = p_errorCodes(pgError);
	erroCodeVector.push_back(eCode);
	while ((iss >> pgError) && pgError.find_first_not_of("0123456789") == std::string::npos)
	{
		eCode =  p_errorCodes(pgError);
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
	_errorFlag = true;
}


int	ServerConf::p_Return(std::string &codeRetrn)
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

void	ServerConf::p_CodeReturn(std::istringstream& iss)
{
	std::string 	codeRetrn;
	std::vector<int>	vectorCode;
	int					codeRe;
	if (!(iss >> codeRetrn))
		throw ErrorConfFile("Error conf file: return");
	codeRe = p_Return(codeRetrn);
	vectorCode.push_back(codeRe);
	while ((iss >> codeRetrn) && codeRetrn.find_first_not_of("0123456789") == std::string::npos)
	{
		codeRe = p_Return(codeRetrn);
		vectorCode.push_back(codeRe);
	}
	if (codeRetrn.empty())
		throw ErrorConfFile("Error conf file : return empty");
	if (codeRetrn[0] != '/' && codeRetrn.find("..") != std::string::npos)
		throw ErrorConfFile("Error conf file: return 2");
	if (iss >> codeRetrn)
		throw ErrorConfFile("Error conf file: return 3");
	for (size_t i = 0; i < vectorCode.size(); i++)
		_CodeReturn[vectorCode[i]] = codeRetrn;
	_ReturnFlag = true;
	Print_map_code_return(_CodeReturn);
}

void	ServerConf::p_Index(std::istringstream& iss)
{
	std::string		index;

	// std::cout << "before index --> " << index << std::endl;
	if (!(iss >> index) || index.empty())
		throw ErrorConfFile("Error conf file: index ");
	_IndexFile.push_back(index);
	_IndexPages = 1;
	while (iss >> index)
	{
		// std::cout << std::getline(iss, index) << "--- "<< std::endl;
		if (index.empty() || index.find(".") == std::string::npos)
			throw ErrorConfFile("Error conf file: index ");
		else
			_IndexFile.push_back(index);
	}
	printVector(_IndexFile);
}

void	ServerConf::p_DefaultServer(std::istringstream& iss)
{
	std::string line;
	_Default_server = true;
	if (iss >> line)
		throw ErrorConfFile("Error conf file: defualt_server");
	_CheckDefaultServer = true;
	std::cout << "default_server -> " << _Default_server << std::endl;
}

void	ServerConf::setDefaultErrorPages(void)
{
	_PageError[404]  = "www/error_pages/404_notfound.html";
	_PageError[403]  = "www/error_pages/403_forbidden.html";
	_PageError[400]  = "www/error_pages/400_badrequest.html";
	_PageError[496]  = "www/error_pages/496_ssl_required.html";
	_PageError[500]  = "www/error_pages/500_internal_server.html";
}

void    ServerConf::initWServer(std::istream &file)
{
    std::string	line, kw;
	bool	empty = true;
	int		i = 0;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		if (line.empty() || line == "\t\t")
			continue;
		if (!(iss >> kw))
			continue;
        else if (kw == "listen" && _StateListen )
			p_Listen(iss);
		else if (kw == "server_name" && !_isServerName)
			p_name(iss);
		else if (kw == "root" && !_rootFlag)
			p_Root(iss);
		else if (kw == "autoindex")
			p_AutoIndex(iss);
		else if (kw == "client_max_body_size" && !_maxBodyState)
			p_MaxClientBodySize(iss);
		else if (kw == "error_page" &&  !_errorFlag)
			p_ErrorPages(iss);
		else if (kw == "index" && !_IndexPages ) // is the file in the root path 
			p_Index(iss);
		else if (kw == "return" && !_ReturnFlag)
			p_CodeReturn(iss);
		else if (kw == "default_server" && !_CheckDefaultServer)
			p_DefaultServer(iss);
        else if (kw == "location")
		{
			Location	location;
			std::string	prefix;
			if (!(iss >> kw))
				throw ErrorConfFile("Error in the conf file : location : wrong content1");
			if (kw != "{")
			{
				prefix = kw;
				location.setUri(prefix);
				std::cerr << "prefix " << location.getUri() << "\n";
				if ((iss >> kw) && kw != "{")
					throw ErrorConfFile("Error in the conf file : location : wrong content3");
			}
			else
				throw ErrorConfFile("Error in the conf file : location : wrong content3");
			location.ParseLocation(file);
			_Location[location.getUri()] = location;
			i++;
			std::cout << "number of location for server -> " << i << std::endl;
		}
		else if (line == "}")
		{
			if (!_rootFlag || _StateListen || !_isServerName)
			{
				std::cout << "_rootFlag -> " << _rootFlag << "_StateListen -> " << _StateListen << std::endl;
				throw ErrorConfFile("Error in the config file : miss basic line");
			}
			if (!_errorFlag)
			{
				std::cout << "set default pages error" << std::endl;
 				setDefaultErrorPages();
				Print_map_code_errors(_PageError);
			}
			break;
		}
		else
			throw ErrorConfFile("Error in the config file : server section");
    }
	
}

void			ServerConf::setFdEpoll(int FdEpoll)
{
	this->_FdEpoll = FdEpoll;
	return;
}

int	ServerConf::getFdEpoll(void)
{
	return (this->_FdEpoll);
}

std::map<int, std::string> ServerConf::getPagesError()
{
	return (this->_PageError);
}

std::map<int, std::string> ServerConf::getCodesReturn()
{
	return (this->_CodeReturn);
}

std::string& 		ServerConf::getIp()
{
	return (this->_ip);
}

int					ServerConf::getPort()
{
	return (this->_port);
}

std::string&			ServerConf::getNameServer()
{
	return(this->_name);
}

int					ServerConf::getNumberServer()
{
	return (this->_nbServer);
}

std::vector<std::string> 	ServerConf::getIndexFiles(void)
{
	return (this->_IndexFile);
}


ServerConf::~ServerConf()
{
    return;
}
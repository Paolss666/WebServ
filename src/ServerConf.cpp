#include "ServerConf.hpp"

ServerConf::ServerConf()
{
    memset(&_address, 0, sizeof _address);
	_address.sin_family = AF_INET; // IPv4
	_address.sin_port = htons(PORT);// port par defaut 8080
	_address.sin_addr.s_addr = htonl(INADDR_ANY);// address par default(00000)
    _port = 8080; // port par default;
    _ip = "0.0.0.0"; // address far default;
    _NotBind = 0; // check if is bind or not;
    _StateListen = true;
	_isServerName = false;
	_DefaultPort = true;
    _IpDefault = true;
	_rootFlag = true;
	_Autoindex = false;
	_maxBodyState = false;
	_errorFlag = false;
	_IndexPages = 0;
	_nbServer = 0;
}

void	ServerConf::setNbServer(int nb)
{
	this->_nbServer = nb;
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
	for (size_t i = 0; i < _ServerNames.size(); ++i)
	{
        std::cout << i + 1 << ". " << _ServerNames[i] << std::endl;  // Stampa con un indice
    }
}

void	ServerConf::p_ServerNames(std::istringstream &iss)
{
	std::string serverNames;
	

	if (!(iss >> serverNames))
		throw ErrorConfFile("Error conf file: serve_name not found 1;");



	if (iss >> serverNames)
		throw ErrorConfFile("Error conf file: serve_name not found;");

	_ServerNames = serverNames;

	std::cout << "server name -> " << _ServerNames << std::endl;
	_isServerName = true;
}

void	ServerConf::p_Root(std::istringstream &iss)
{
	std::string 	pathRoot;

	if (!(iss >> pathRoot))
		throw ErrorConfFile("Error conf file: root path don't found;");
	
	// std::cout << " path-root found --->  " << pathRoot << std::endl; 

	if ( pathRoot.find("www") == std::string::npos ||pathRoot.find("www/") == std::string::npos)
		throw ErrorConfFile("Error conf file: root wrong path;");

	_rootPath = pathRoot;


	struct stat info;
	if (stat(_rootPath.c_str(), &info) != 0)// cannot access path 
		throw ErrorConfFile("Error : root : cannot access path or file");
	
	std::cout << " root found --->  " << _rootPath << std::endl; 

	_rootFlag = false;
}

/*
Il comando autoindex è una direttiva comunemente usata nei server come NGINX.
In pratica, controlla se il server web deve mostrare automaticamente una lista di directory
quando non è presente un file di indice (come index.html) nella directory richiesta.
Funzionamento di autoindex

autoindex on: Se abilitato (on), il server mostrerà una lista dei file e delle sottocartelle presenti nella directory richiesta, se non è disponibile un file di indice.
autoindex off: Se disabilitato (off), il server restituirà un errore (tipicamente un 403 Forbidden) se non trova un file di indice nella directory richiesta. */

void	ServerConf::p_AutoIndex(std::istringstream &iss)
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

void	ServerConf::p_Index(std::istringstream& iss)
{
	std::string		index;

	// std::cout << "before index --> " << index << std::endl;
	if (!(iss >> index) || index.empty())
		throw ErrorConfFile("Error conf file: index ");
	_IndexFile.push_back(index);
	// std::cout << "before index --> " << index << std::endl;
	// _IndexPages == true;
	// std::cout << "idexpages" << _IndexPages << std::endl;
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

void    ServerConf::initWServer(std::istream &file)
{
    std::string	line, kw;
	// bool	empty = true;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		if (!(iss >> kw))
			continue ;
        else if (kw == "listen" && _StateListen )
			p_Listen(iss);
		else if (kw == "server_name" && !_isServerName)
			p_ServerNames(iss);
		else if (kw == "root" && _rootFlag)
			p_Root(iss);
		else if (kw == "autoindex")
			p_AutoIndex(iss);
		else if (kw == "client_max_body_size" && !_maxBodyState)
			p_MaxClientBodySize(iss);
		else if (kw == "error_page" &&  !_errorFlag)
			p_ErrorPages(iss);
		else if (kw == "index" && !_IndexPages ) // is the file in the root path 
			p_Index(iss);
		// else if (kw == "default_server")
		// 	p_DefaultServer(iss);
		// std::cerr << "kw vs = " << kw << "\n";
		// if (kw == "}" && empty == true)
			// throw ErrorConfFile("Error in the config file : empty server section");
		// std::cout << kw << " " << std::endl;
        // if (kw == "location")
		if (line == "}")
			break;
        
    }
}

ServerConf::~ServerConf()
{
    return;
}
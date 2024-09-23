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
    _DefaultPort = true;
    _IpDefault = true;
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
    std::cout << "error _--> la _p" << _ip << std::endl;
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
    _StateListen = true; 
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
	
	std::getline(iss, serverNames);

	serverNames.erase(serverNames.find_last_not_of(" \n\r\t") + 1);
	serverNames.erase(0, serverNames.find_first_not_of(" \n\r\t"));
	 // Variabile temporanea per il nome del server
    std::string name;
    std::istringstream nameStream(serverNames);
    // Dividi la stringa in base agli spazi
	// PROBLEM IF SERVER_NAME N'EST PAS ECRIT !! 
    while (nameStream >> name)
	{
        _ServerNames.push_back(name);  // Aggiungi ogni nome di server al vettore
	}
	
	printServerNames();
}

void    ServerConf::initWServer(std::istream &file)
{
    std::string	line, keyword;
	bool	empty = true;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		if (!(iss >> keyword) /* || keyword[0] == '#' */)
			continue ;
        else if (keyword == "listen")
			p_Listen(iss);
		else if (keyword == "server_name")
			p_ServerNames(iss);
		// else if (keyword == "root")
		// 	p_Root(iss);
		// else if (keyword == "autoindex")
		// 	p_AutoIndex(iss);
		// // else if (keyword == "client_max_body_size")
		// // 	p_MaxClientBodySize(iss);
		// else if (keyword == "error_page")
		// 	p_ErrorPages(iss);
		// else if (keyword == "index")
		// 	p_Index(iss);
		// else if (keyword == "default_server")
		// 	p_DefaultServer(iss);
		// std::cerr << "keyword vs = " << keyword << "\n";
		// if (keyword == "}" && empty == true)
			// throw ErrorConfFile("Error in the config file : empty server section");
		// std::cout << keyword << " " << std::endl;
        // if (keyword == "location")
        
        // else if (keyword == "}" && empty == false)
		// {
		// 	checkNecessaryLine();
		// 	return ;
		// }
    }
}

ServerConf::~ServerConf()
{
    return;
}
#include "../incl/ServerV.hpp"
#include "../incl/ServerConf.hpp"

ServerV::ServerV()
{
    return ;
}

void ServerV::initServer(const char *fileName)
{
    std::string	line;
	bool	empty = true;
    Check_extension(fileName, ".conf");
	// isDirectory(filename);
    std::ifstream file(fileName);

    if (!file.is_open())
        throw ErrorConfFile("Not Open");
	
    while (std::getline(file, line))
	{
        if (line == "server {")
		{
            ServerConf virtualServer;	

			// vs.setIndex(i);
			// virtualServersTemp.push_back(vs);
			// i++;
             virtualServer.initWServer(file);
            std::cout << "FILE DONE\n";
		}
		else if (line.empty())
			continue ;
		else
			throw ErrorConfFile("Error in the config file : wrong content");
        // if (word == "}" && empty == true)
			// throw ErrorConfFile("Error in the config file");
    }
}

ServerV::~ServerV()
{
    // il faut close ici les fd;
    return ;
}
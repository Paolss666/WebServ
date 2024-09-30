// #include "../incl/Hosts.hpp"
// #include "../incl/ServerConf.hpp"

// Hosts::Hosts()
// {
//     return ;
// }

// void Hosts::initServer(const char *fileName)
// {
//     std::string	line;
// 	// bool	empty = true;
//     int     i = 0;
    
//     std::vector<ServerConf>    Servers;

//     Check_extension(fileName, ".conf");
// 	// isDirectory(filename);
//     std::ifstream file(fileName);
//     if (!file.is_open())
//         throw ErrorConfFile("Can't open the file");
	
//     while (std::getline(file, line))
// 	{
//         if (line == "server {")
// 		{
//             ServerConf virtualServer;	
//             std::cout << " ======> "<< line << "<============================== \n";
//              virtualServer.initWServer(file);
//             std::cout << "FILE DONE\n";
// 			Servers.push_back(virtualServer);
//             virtualServer.setNbServer(i);
//             std::cout << "nbserver " << i << "\n";
//             i++;
// 		}
//         else if (line.empty())
//             continue;
//         else
// 			throw ErrorConfFile("Error in the config file");
//     }
//     // std::cout << "valeur index ----> " << i << std::endl;
// }

// Hosts::~Hosts()
// {
//     // il faut close ici les fd;
//     return ;
// }
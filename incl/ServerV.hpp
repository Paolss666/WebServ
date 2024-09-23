#ifndef SERVERV_HPP
#define SERVERV_HPP

#include "webserve.hpp"

class ServerV
{
    private:
        // std::vector<VirtualS>					_virtualServers;
		// std::map<int, Client>						_clients;
		// std::map<int, std::vector<VirtualS*> >	_socketBoundServer;
		fd_set 										_all_sockets;
		fd_set										_read_fds;
		fd_set										_write_fds;
		int											_fd_max;
		std::vector<int>							_socketMax;
		std::map<int, int>							_maxConnections;

    public:
        ServerV(/* args */);
        ~ServerV();
        void    initServer(const char *fileName);
        void    loopServer();
};

// ServerV::ServerV(/* args */)
// {
// }

// ServerV::~ServerV()
// {
// }


#endif
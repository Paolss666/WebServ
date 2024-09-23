#include "webserve.hpp"

class ServerConf
{
private:
    /* data */
    struct sockaddr_in          _address;
    int                         _Fdsocket;
    int                         _NotBind;
    int                         _port;
    std::string                 _PortString;
    std::string                 _ip;
    std::vector<std::string>    _ServerNames;
    bool                        _DefaultPort;
    bool                        _StateListen;
    bool                        _IpDefault;
public:
    ServerConf(/* args */);
    ~ServerConf();
    // P means parsing;
    void    printServerNames(void)const;
    void    initWServer(std::istream &file);
    void    p_Listen(std::istringstream& iss);
    void    p_IpAddrs(void);
    void    p_ServerNames(std::istringstream& iss);
    void    p_Root(std::istringstream& iss);
    void    p_Autoindex(std::istringstream& iss);
    void    p_ErrorPages(std::istringstream& iss);
    void    p_Index(std::istringstream& iss);
    void    p_DefaultServer(std::istringstream& iss);
};


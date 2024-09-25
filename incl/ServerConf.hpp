#include "webserv.hpp"

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
    std::string                 _rootPath;
    std::string                 _ServerNames;
    size_t                      _maxBodySize;
    bool                        _Autoindex;
    std::map<int, std::string>  _PageError;
	std::vector<std::string>	_IndexFile;    
    bool                        _DefaultPort;
    bool                        _StateListen;
    bool                        _IpDefault;
    bool                        _isServerName;
    bool                        _rootFlag;
    bool                        _maxBodyState;
    bool                        _errorFlag;
    int                        _IndexPages;
    int                         _nbServer;

public:
    ServerConf(/* args */);
    ~ServerConf();
    void    printServerNames(void)const;
    void    initWServer(std::istream &file);
    void    p_Listen(std::istringstream& iss);
    void    p_IpAddrs(void);
    void    p_ServerNames(std::istringstream& iss);
    void    p_Root(std::istringstream& iss);
    void    p_AutoIndex(std::istringstream& iss);
    void    p_MaxClientBodySize(std::istringstream &iss);
    void    p_ErrorPages(std::istringstream& iss);
    int	    p_errorCodes(std::string &pgError);
    void    p_Index(std::istringstream& iss);
    void    p_DefaultServer(std::istringstream& iss);


    void    setNbServer(int nb);
};


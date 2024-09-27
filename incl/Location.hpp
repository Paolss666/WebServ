#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"
// #include "ServerConf.hpp"
/* 
    La directive location dans un fichier de configuration Nginx
    est utilisée pour définir comment Nginx doit traiter les requêtes HTTP pour des URL spécifiques.
    En fonction du chemin de la requête (le URI), Nginx peut décider de servir des fichiers, de rediriger,
    de proxyfier les requêtes vers un autre serveur, ou d'appliquer d'autres règles.
 */


class Location
{
    private:
      std::string                 _Uri;
      std::string                 _ServerName;
      std::string                 _Root;
      std::string                 _CgiPath;
      std::map<int, std::string>  _Retourn;
      std::map<int, std::string>  _PageError;
      std::vector<std::string>    _Methods;
	    std::vector<std::string>	    _Indx;
      bool                        _MetFlag;
      bool                        _indexFlag;
      bool                        _CgiFlag;

    public:
        Location(/* args */);
        ~Location();
        void            p_Methos(std::istringstream& iss);
        void            p_Index(std::istringstream& iss);
        void            p_Cgi(std::istringstream& iss);
        void            setUri(std::string uri);
        std::string     getUri(void);
        void            ParseLocation(std::istream &file);
        
};


#endif
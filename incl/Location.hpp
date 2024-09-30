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
      bool                        _AutoIndex;

      bool                        _MetFlag;
      bool                        _indexFlag;
      bool                        _CgiFlag;
      bool                        _ReturnFlag;
      bool                        _ErPages;
      bool                        _AutoFlag;

    public:
        Location(/* args */);
        ~Location();
        void            InLoc_Methos(std::istringstream& iss);
        void            InLoc_Index(std::istringstream& iss);
        void            InLoc_Cgi(std::istringstream& iss);
        void            InLoc_Return(std::istringstream& iss);
        int             InLoc_p_Return(std::string &codeRetrn);
        void            InLoc_ErPages(std::istringstream& iss);
        int             InLoc_p_errorCodes(std::string &pgError);
        void            InLoc_AutoIndex(std::istringstream& iss);

        void            setUri(std::string uri);
        std::string     getUri(void);
        void            ParseLocation(std::istream &file);
        
};


#endif
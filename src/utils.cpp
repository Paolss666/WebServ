#include "../incl/webserv.hpp"

ErrorConfFile::ErrorConfFile(std::string msg) throw() : _errorMsg(msg) {}
ErrorConfFile::~ErrorConfFile() throw() {}

const char *ErrorConfFile::what() const throw()
{
	return _errorMsg.c_str();
}

void	Check_extension(const std::string & str, const std::string & ext)
{
    if (str.size() >= ext.size() && !str.compare(str.size() - ext.size(), ext.size(), ext))
		return ;	
	throw ErrorConfFile("Error : wrong conf extension");
}

void	Print_map_code_errors(std::map<int, std::string> errorPages)
{
	for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		std::cout << "code -> " << it->first << " pages -> " << it ->second << "\n";
}

void printVector(const std::vector<std::string>& vec) {
    std::cout << "Vector content:" << std::endl;
    for (size_t i = 0; i!= vec.size(); ++i) {
        std::cout << " - " << vec[i] << std::endl;
    }
}
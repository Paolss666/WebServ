#include "../incl/webserve.hpp"

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

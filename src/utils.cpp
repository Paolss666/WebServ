/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:07:36 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 18:09:43 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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

void	Print_map_code_return(std::map<int, std::string> errorPages)
{
	for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		std::cout << "return -> " << it->first << " pages -> " << it ->second << "\n";
}

void printVector(const std::vector<std::string>& vec) {
	std::cout << "Vector content:" << std::endl;
	for (size_t i = 0; i!= vec.size(); ++i) {
		std::cout << " - " << vec[i] << std::endl;
	}
}
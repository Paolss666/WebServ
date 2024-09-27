/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bdelamea <bdelamea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 18:03:16 by bdelamea          #+#    #+#             */
/*   Updated: 2024/09/27 18:05:27 by bdelamea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "webserv.hpp"

void	Check_extension(const std::string & str, const std::string & ext);
void	Print_map_code_errors(std::map<int, std::string> PagesError);
void	printVector(const std::vector<std::string> & vec);
void	Print_map_code_return(std::map<int, std::string> errorPages);
void	ft_perror(const char * message);

// macros ------------------------------
# define RESET		"\033[0m"          // Reset to default color
# define BLACK		"\033[30m"         // Black
# define RED		"\033[31m"         // Red
# define GREEN		"\033[32m"         // Green
# define YELLOW		"\033[33m"         // Yellow
# define BLUE		"\033[34m"         // Blue
# define MAGENTA	"\033[35m"         // Magenta
# define CYAN		"\033[36m"         // Cyan
# define WHITE		"\033[37m"         // White
# define GRAY		"\033[90m"         // Bright Black (Gray)
# define BOLD		"\033[1m"          // Bold
# define ITALIC		"\033[3m"          // Italic
# define UNDERLINE	"\033[4m"          // Underline

#endif
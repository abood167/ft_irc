#ifndef UTILS_HPP
# define UTILS_HPP

#include "irc.hpp"

class User;

namespace utils
{
	std::vector<std::string>	split( std::string str, char c );
	std::string	joinSplit( std::vector<std::string>::iterator first, std::vector<std::string>::iterator last );
	std::string	joinSplit( std::vector<User *> users );
	std::string	toupper( std::string str );
	bool	invalidCharacter( std::string str );
	void	errorMessage( std::string function,	std::string error );
	void welcomemsg(void);
}

#endif


#include "irc.hpp" 


std::vector<std::string>	utils::split( std::string str, char c) {

	std::string	buff = "";
	std::vector<std::string>	split;

	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] != c)
			buff += str[i];
		else if (str[i] == c && buff != "") {
			split.push_back(buff);
			buff = "";
		}
	}
	if (buff != "")
		split.push_back(buff);
	return (split);

}

std::string	utils::joinSplit( std::vector<std::string>::iterator first,	std::vector<std::string>::iterator last ) {
	std::string	msg;
	for ( ; first != last; first++)
		msg += *first + " ";
	return (msg);
}

std::string	utils::joinSplit( std::vector<User *> users ) {
	std::string						msg;
	std::vector<User *>::iterator	it = users.begin();
	for ( ; it != users.end(); it++)
		msg += (*it)->getNick() + " ";
	return (msg);
}

std::string	utils::toupper( std::string str ) {
	for (int i = 0; str[i] != '\0'; i++)
		str[i] = std::toupper(str[i]);
	return (str);
}

bool	utils::invalidCharacter( std::string str ) {
	for (int i = 0; str[i] != '\0'; i++)
		if (!isalnum(str[i]) && !isalpha(str[i]))
			return (true);
	return (false);
}

void	utils::errorMessage( std::string function, std::string error ) {
	throw std::runtime_error(function + " " + error);
}

void utils::welcomemsg(void)
{
	std::string welcome;
	welcome.append("001 :Welcome\n");
	return ;
}

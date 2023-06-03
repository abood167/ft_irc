#include "irc.hpp"
 
//constractor of the channel 
Channel::Channel( std::string name, std::string password ): _channel_name(name), _password(password) {
	return ;
}

//~destroctor
Channel::~Channel( void ) {
	return ;
}

//returns the channel name.
std::string	Channel::getChannelName( void ) {
	return (this->_channel_name);
}

//returns a the password.
std::string	Channel::getPassword( void ) {
	return (this->_password);
}

//returns the users name.
std::vector<User *>	Channel::getUsersname( void ) {
	return (this->_users_name);
}

//returns a pointer to a User object representing by nickname in the channel.
//returns null if user not found.
User	*Channel::getUserByNick( std::string nick ) {
	std::vector<User *>::iterator	it = this->_users_name.begin();//iterates over the list of users.
	if (nick[0] == ':')
		nick.erase(0, 1);
	for ( ; it != this->_users_name.end(); it++) {
		if ((*it)->getNick() == nick)
			return (*it);
	}

	return (NULL);

}

User	*Channel::getOperByNick( std::string nick ) {
	std::vector<User *>::iterator	it = this->_operators.begin();//iterates over the list of operatores.
	if (nick[0] == ':')
		nick.erase(0, 1);
	for ( ; it != this->_operators.end(); it++) {
		if ((*it)->getNick() == nick)
			return (*it);
	}
	return (NULL);

}

//fuction to add new user.
void	Channel::addUser( User * user ) {
	if (getUserByNick(user->getNick()) == NULL)
		this->_users_name.push_back(user);
	return ;
}

//add new operator. 
void	Channel::addOper( User * user ) {
	if (getOperByNick(user->getNick()) == NULL)
		this->_operators.push_back(user);
	return ;
}

//funcation to remove a spicifc user.
void	Channel::removeUser( User * user ) {
	std::vector<User *>::iterator	it = this->_users_name.begin();
	for ( ; it != this->_users_name.end(); it ++) {
		if (*it == user) {
			this->_users_name.erase(it);
			return ;
		}
	}
	return ;
}

//funcation to remove an operator user.
void	Channel::removeOper( User * user ) {
	std::vector<User *>::iterator	it = this->_operators.begin();
	for ( ; it != this->_operators.end(); it ++) {
		if (*it == user) {
			this->_operators.erase(it);
			return ;
		}
	}
	return ;
}
//funcation to send a msg to all users in the channel.
//If send returns a value less than 0, this means there is an error 
//the function calls the ft::errorMessage function to print an error message to the standard error output stream. to the Fd(user)
void	Channel::messageFromChannel( std::string msg ) {
	std::vector<User *>::iterator	it = this->_users_name.begin();////iterate over the list of users 
	if (msg.find("\r\n") == std::string::npos)
		msg += "\r\n";
	for ( ; it != this->_users_name.end(); it++)
		if (send((*it)->getFd(), msg.c_str(), strlen(msg.c_str()), 0) < 0)
			utils::errorMessage("messageFromChannel: send:", strerror(errno));
	return ;
}

//funcation  sends a msg to all users in the channel except the user who sent the message.
void	Channel::messageToChannel( std::string msg, int senderFd ) {
	std::vector<User *>::iterator	it = this->_users_name.begin();
	if (msg.find("\r\n") == std::string::npos)// ensure that the message ends with a proper CRLF sequence.
		msg += "\r\n";
	for ( ; it != this->_users_name.end(); it++)
		if ((*it)->getFd() != senderFd)
			if (send((*it)->getFd(), msg.c_str(), strlen(msg.c_str()), 0) < 0)
				utils::errorMessage("messageToChannel: send:", strerror(errno));
	return ;
}
void 		Channel::setPassword(std::string pass){this->_password = pass;}

//set & get topic
void Channel::setTopic(std::string topic) { this->_topic = topic; }
std::string Channel::getTopic() { return _topic; }

//set & get mode
void Channel::setMode(std::string mode) { this->_mode = mode; }
std::string Channel::getMode() { return _mode; }

//set & get limit
void Channel::setLimit(unsigned int limit) { this->_limit = limit; }
unsigned int Channel::getLimit() { return _limit; }

bool Channel::check_mode(char c)
{
	for (size_t i = 0; i < _mode.size(); i++)
	{
		if (_mode[i] == c)
			return (true);
	}
	return (false);
}
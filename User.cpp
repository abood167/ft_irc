#include "irc.hpp" 

//constractor
User::User( int userFd )
	: _userFd(userFd), _nick(""), _username(""), _auth(false), _oper(false) {
	return ;
}
//destroctor 
User::~User( void ) {
	close(this->_userFd);
	return ;
}

// message to be sent to the user's socket.
void	User::receiveMessage( std::string msg ) {
	if (msg.find("\r\n") == std::string::npos)
		msg += "\r\n";
	if (send(getFd(), msg.c_str(), strlen(msg.c_str()), 0) < 0)
		utils::errorMessage("receiveMessage: send:", strerror(errno));
	return ;
}

int	User::getFd( void ) {
	return (this->_userFd);
}

void	User::auth( void ) {
	this->_auth = true;
}

bool	User::isAuth( void ) {
	return (this->_auth);
}

std::string	User::getNick( void ) {
	return (this->_nick);
}

void	User::setNick( std::string nick ) {
	this->_nick = nick;
}

std::string	User::getUsername( void ) {
	return (this->_username);
}

void	User::setUsername( std::string username ) {
	this->_username = username;
}

std::string	User::getRealname( void ) {
	return (this->_realname);
}

void	User::setRealname( std::string realname ) {
	this->_realname = realname;
}
std::string	User::getServername( void ) {
	return (this->_servername);
}

void	User::setServername( std::string servername ) {
	this->_servername = servername;
}
std::string	User::getHostname( void ) {
	return (this->_hostname);
}

void	User::setHostname( std::string hostname ) {
	this->_hostname = hostname;
}
std::vector<Channel *>	User::getChannels( void ) {
	return (this->_channelsVec);
}

//add user to channel
void					User::addChannel( Channel * channel ) {
	std::vector<Channel *>::iterator	it = this->_channelsVec.begin();
	for ( ; it != this->_channelsVec.end(); it++)
		if ((*it)->getChannelName() == channel->getChannelName())
			return ;
	this->_channelsVec.push_back(channel);
	channel->addUser(this);
	return ;
}

//  remove the current user from a given channel's.
// and remove the channel from the list of channels the user belongs to.
void	User::removeChannel( Channel * channel ) {
	std::vector<Channel *>::iterator	it = this->_channelsVec.begin();
	for ( ; it != this->_channelsVec.end(); it++) {
		if (*it == channel) {
			this->_channelsVec.erase(it);
			return ;
		}
	}
	return ;
}

//check if user is OPER.
bool	User::isOper( void ) {
	return (this->_oper);
}

//make a user an OPER/!OPER.
void	User::setOper( void ) {
	this->_oper = !this->_oper;
	
}
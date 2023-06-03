#include "irc.hpp" 

Server::Server(std::string host,std::string port, std::string pass):m_host(host),m_port(port), m_pass(pass){
    std::cout << "Server created." << std::endl;
}
Server::~Server(){
	std::cout << "desctuctor called!" << std::endl;

	for (size_t i = 0; i < this->m_usersVec.size(); i++)
	{
		delete this->m_usersVec[i];
	}
	for (size_t i = 0; i < this->m_channelsVec.size(); i++)
	{
		delete this->m_channelsVec[i];
	}
	for (size_t i = 0; i < this->m_fds.size(); i++)
	{
		close(this->m_fds[i].fd);
	}
}

void Server::start(){
    struct addrinfo hints, *result, *tmp;
    int yes = 0;
   
    memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
    hints.ai_family = AF_INET;     // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP stream socket type
    hints.ai_flags = AI_PASSIVE; //create a passive socket to listen

    if ((getaddrinfo(NULL, this->m_port.c_str(), &hints, &result)) != 0)
    {
        throw std::runtime_error("getaddrinfo error");
    }

    for (tmp = result; tmp; tmp = tmp->ai_next) //loop through all the results to find ip that we can bind to
    {
        if ((this->m_serverfd = socket(tmp->ai_family, tmp->ai_socktype,tmp->ai_protocol)) == -1) 
            continue ;
        if (setsockopt(this->m_serverfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) //SO_REUSEADDR -> Allows other sockets to bind() to this port to avoid "address already in use" error messag
        {
            close(this->m_serverfd);
            throw std::runtime_error("setsocket error");
        }
        if (bind(this->m_serverfd, tmp->ai_addr, tmp->ai_addrlen) < 0) {
            close(this->m_serverfd);
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (tmp == NULL)
        throw std::runtime_error("Server faild to bind.");

    if (listen(this->m_serverfd, 10) == -1)
    {
        close(this->m_serverfd);
        throw std::runtime_error("Server faild to listen.");
    }
}

void Server::receive(){
	struct pollfd					pollFd = { this->m_serverfd, POLLIN, 0 };
	std::vector<pollfd>::iterator	it;
	//Setting the socket fds to o_nonblock
	if (fcntl(this->m_serverfd, F_SETFL, O_NONBLOCK) == -1)
		utils::errorMessage("fcntl:", strerror(errno));

	this->m_fds.push_back(pollFd);
	// we are saving the socket fds into pointer to vector Vpollfds 
	std::cout << "Server listening on <host>:<port> = [ " << this->m_host  <<" : " <<this->m_port << " ]"<<std::endl;
	while (work) {
		it = this->m_fds.begin();
		if (poll(&(*it), this->m_fds.size(), 5000) == -1)
			utils::errorMessage("poll:", strerror(errno));
		this->_checkPoll();
	}
}

void	Server::_checkPoll( void ) {

	std::vector<pollfd>::iterator	it;

	for (it = this->m_fds.begin(); it != this->m_fds.end(); it++) {
		//checking the if poll fd vector and POLLIN is true
		if (it->revents && POLLIN) {
			if (it->fd == this->m_serverfd)
				this->_createUser(); // create new connection(User)
			else
				this->_messageReceived(it->fd); // get(command) from exiting user 
			break ;
		}
	}

}

void	Server::_createUser( void ) {

	int					userFd;
	User				*newUser;
	struct sockaddr_in	cli_addr;
	socklen_t			len;
	
	len = sizeof(cli_addr);
	// we create a new User(connection)passing fd by the accept function
	userFd = accept(this->m_serverfd, (struct sockaddr *)&cli_addr, &len);
	if (userFd < 0)
		utils::errorMessage("accept:", strerror(errno));
	// new user poll fd will be set to o_nonblock using fcntl.
	pollfd	userPollFd = { userFd, POLLIN, 0 };
	if (fcntl(userFd, F_SETFL, O_NONBLOCK) == -1)
		utils::errorMessage("createUser: fcntl:", strerror(errno));
	//The new User is created
	newUser = new User(userFd);
	// add new pollfd and new user fd to their respective vector.
	this->m_usersVec.push_back(newUser);
	//delete newUser;
	this->m_fds.push_back(userPollFd);
	std::cout <<  "new user added with fd: " << userFd << std::endl;
	return ;
}

bool	Server::checkOperators( void ) {
	std::vector<User *>::iterator	it = this->m_usersVec.begin();
	for(; it != this->m_usersVec.end(); it++)
		if ((*it)->isOper())
			return true;
	return false;
}

void	Server::_messageReceived( int fd ) {
	char		buff;
	std::string	str;
	int			a = 0;

	while (str.find("\n")) {
		// read the fd and get the buffer(message)
		int status = recv(fd, &buff, sizeof(buff), 0);
		if (status < 0)
			continue;
		else if (status == 0) //connection closed
		{
			std::cout << "user with fd [ "<< fd << " ] deleted" << std::endl;
			deleteUser(fd);
			return;
		}
		else {
			//std::cout << buff << "\n";
			// append buff to make str
			str += buff;
			if (a > 500)
				str = "/Shouldnt break!\r\n";
			// if command found, execute.
			if (str.find("\n") != std::string::npos) {
				if (str.size() == 1)
					str = "/Shouldnt break!\r\n";
				Command command(str, fd, *this);
				break ;
			}
		}
		a++;
	}
	str.clear();
}


std::string Server::getpass(){
    return(m_pass);
}
std::string Server::getport(){
    return (m_port);
}

std::vector<Channel *>	Server::getChannels( void ) {
	return (this->m_channelsVec);
}
// we create the name of the channels for easier lookup later 
Channel*	Server::getChannelByName( std::string name ) {

	std::vector<Channel *>::iterator	it = this->m_channelsVec.begin();

	if (name[0] != '#')
		name = "#" + name;

	for ( ; it != this->m_channelsVec.end(); it++)
		if ((*it)->getChannelName() == name)
			return (*it);

	return (NULL);

}

User	*Server::getUserByFd( int fd ) {

	std::vector<User *>::iterator	it = this->m_usersVec.begin();

	for ( ; it != this->m_usersVec.end(); it++)
		if ((*it)->getFd() == fd)
			return (*it);

	return (NULL);

}

User	*Server::getUserByNick( std::string nick ) {

	std::vector<User *>::iterator	it = this->m_usersVec.begin();

	for ( ; it != this->m_usersVec.end(); it++)
		if ((*it)->getNick() == nick)
			return (*it);

	return (NULL);

}

std::vector<User *>	Server::getUsers( void ) {
	return (this->m_usersVec);
}


void	Server::messageAllUsers( std::string msg ) {

	std::vector<User *>::iterator	it = this->m_usersVec.begin();

	if (msg.find("\r\n") == std::string::npos)
		msg +="\r\n";

	for( ; it != this->m_usersVec.end(); it++)
		if (send((*it)->getFd(), msg.c_str(), strlen(msg.c_str()), 0) < 0)
			utils::errorMessage("messageAllUsers: send:", strerror(errno));

	return ;

}

// to get messages accross to the server 
void	Server::messageToServer( std::string msg, int userFd ) {

	std::vector<User *>::iterator	it = this->m_usersVec.begin();

	if (msg.find("\r\n") == std::string::npos)
		msg +="\r\n";

	for( ; it != this->m_usersVec.end(); it++)
		if ((*it)->getFd() != userFd)
			if (send((*it)->getFd(), msg.c_str(), strlen(msg.c_str()), 0) < 0)
				utils::errorMessage("messageToServer: send:", strerror(errno));

	return ;

}

void	Server::deleteUser( int fd ) {

	std::vector<User *>::iterator	userIt = this->m_usersVec.begin();
	std::vector<Channel *>::iterator	channelIt = this->m_channelsVec.begin();
	std::vector<pollfd>::iterator	pollIt = this->m_fds.begin();

	for ( ; channelIt != this->m_channelsVec.end(); channelIt++)
		(*channelIt)->removeUser(getUserByFd(fd));

	for ( ; pollIt != this->m_fds.end(); pollIt++) {
		if ((*pollIt).fd == fd) {
			this->m_fds.erase(pollIt);
			close(fd);
			break ;
		}
	}

	for ( ; userIt != this->m_usersVec.end(); userIt++) {
		if ((*userIt)->getFd() == fd) {
			delete *userIt;
			this->m_usersVec.erase(userIt);
			break ;
		}
	}


}

void	Server::addChannel( Channel *channel ) {
	this->m_channelsVec.push_back(channel);
}

int	Server::getServerFd( void ) {
	return (this->m_serverfd);
}
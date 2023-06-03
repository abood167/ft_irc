#ifndef SERVER_H
#define SERVER_H

#include "irc.hpp" 

class Server {
private:
    std::string     m_host;
    std::string     m_port;
    std::string     m_pass; //password
    int             m_serverfd; //listening socket 
    std::vector<pollfd> m_fds;
    std::vector<User*>		m_usersVec; 
    std::vector<Channel*>	m_channelsVec; 
	void				_checkPoll( void );
	void				_createUser( void );
	void				_messageReceived( int fd );


public:
    Server(std::string m_host, std::string port, std::string pass);
    ~Server();
    void start(); 
    void receive();
    std::string getpass();
    std::string getport();
	void					messageAllUsers( std::string msg );
	void					messageToServer( std::string msg, int userFd );
	bool					checkOperators( void );
	void					deleteUser( int fd );
	void					addChannel( Channel *channel );
	int						getServerFd( void );
	std::vector<User *>		getUsers( void );
	User					*getUserByNick( std::string nick );
	User					*getUserByFd( int fd );
	std::vector<Channel *>	getChannels( void );
	Channel*				getChannelByName( std::string name );

};
#endif

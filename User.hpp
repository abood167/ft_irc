#ifndef USER_HPP
# define USER_HPP

#include "irc.hpp"

class Channel;

class User {

	private:
		int						_userFd;//fd of the user's socket
		std::string				_nick;//nickname
		std::string				_username;//user's username
		std::string				_realname;// real name
		std::string				_servername;//server name
		std::string				_hostname;//hostname
		std::vector<Channel *>	_channelsVec;//channels of the user 
		bool					_auth;//whether the user is authenticated
		bool					_oper;//whether the user is OPR

	public:
		User( int userFd );
		~User( void );
		void					receiveMessage( std::string msg );
		bool					isAuth( void );
		void					auth( void );
		int						getFd( void );
		std::string				getNick( void );
		void					setNick( std::string nick );
		std::string				getUsername( void );
		void					setUsername( std::string username );
		std::string				getRealname( void );
		void					setRealname( std::string realname );
		std::string				getServername( void );
		void					setServername( std::string servername );
		std::string				getHostname( void );
		void					setHostname( std::string hostname );
		std::vector<Channel *>	getChannels( void );
		void					addChannel( Channel * channel );
		void					removeChannel( Channel * channel );
		bool					isOper( void );
		void					setOper( void );

};

#endif

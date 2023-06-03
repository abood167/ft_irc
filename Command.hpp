
#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "irc.hpp"


class Server;

class Command {

	private:

		User						&_user;
		Server					    &_ircServer;
		std::vector<std::string>	_args;
		std::string					_command;
		
		bool is_valid_mode(char c);

	public:

		Command( std::string buffer, int userFd, Server & ircServer );
		~Command( void );

		void	parserBuffer( std::string buffer );
		void	checkCommand( void );
		void	replynError( std::string msg, std::string code, int fd = 0, std::string opt = "" );
		//commands
		void	Pass_cmd( void );
		void	Nick_cmd( void );
		void	User_cmd( void );
		void	Privmsg_cmd( void );
		void	Quit_cmd( void );
		void	Join_cmd( void );
		void	Oper_cmd( void );
		void	Part_cmd( void );
		void	Kick_cmd( void );
		void	Help_cmd( void );
		void	Kill_cmd( void );
		void	Topic_cmd ( void );
		void	Mode_cmd(void);
		void    Invite_cmd( void );
};

#endif

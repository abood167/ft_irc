#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "irc.hpp"

//"channel" chat rooms within an IRC server where users communicate with other users in the chat room. 

class User;

class Channel {

	private:

		std::string			_channel_name; 
		std::string 		_topic;
		std::string			_password; 
		std::vector<User *>	_users_name;
		std::string			_mode;
		std::vector<User *> _operators;
		unsigned int		_limit;  

	public:

		Channel( std::string name, std::string password );
		~Channel( void );

		std::string			getChannelName( void );
		std::string			getPassword( void );
		std::vector<User *>	getUsersname( void );
		User				*getUserByNick( std::string nick );
		void				addUser( User * user );
		void				removeUser( User * user );
		void				messageFromChannel( std::string msg );
		void				messageToChannel( std::string msg, int senderFd );
		void 				setTopic(std::string topic);
		std::string 		getTopic();
		void 				setMode(std::string mode);
		void 				setPassword(std::string pass);
		std::string 		getMode();
		User				*getOperByNick( std::string nick );
		void				addOper( User * user );
		void				removeOper( User * user );
		void 				setLimit(unsigned int limit);
		unsigned int		getLimit();
		bool				check_mode(char c);

};

#endif

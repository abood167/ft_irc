#include "irc.hpp"

//constructor (Buffer received of the user, /userFd Socket fd from user,ircServer Instance of server )
Command::Command( std::string buffer, int userFd, Server & ircServer ): _user(*ircServer.getUserByFd(userFd)), _ircServer(ircServer) {
	parserBuffer(buffer);
	checkCommand();
	return ;
}

Command::~Command( void ) {
	return ;
}

//parse the buffer received
//the first arg will be command, the rest will be attributes of the command
void	Command::parserBuffer( std::string buffer ) {
	if (buffer[0] == '/') //to remove / if found in the begining of the command
		buffer.erase(0, 1);
	buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
	buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());
	this->_args = utils::split(buffer, ' ');
	if (this->_args.size() == 0)
		return ;
	this->_command = utils::toupper(*this->_args.begin());// make command in CAPS letters
	this->_args.erase(this->_args.begin());
	if (this->_args[0][0] == ':')
		this->_args[0].erase(0, 1);
	return ;
}

//check if command is accepted
void	Command::checkCommand( void ) {
	if (this->_command == "PASS")
		Pass_cmd();
	else if (this->_command == "QUIT")
		Quit_cmd();
	else if (this->_command == "HELP")
		Help_cmd();
	else if (this->_command == "PONG")
		return ;
	else if (this->_user.isAuth()){
		if (this->_command == "NICK")
			Nick_cmd();
		else if (this->_user.getNick().empty() == false) {
			if (this->_command == "USER")
				User_cmd();
			else if (this->_user.getUsername().empty() == false) {
				if (this->_command == "PRIVMSG")
					Privmsg_cmd();
				else if (this->_command == "JOIN")
					Join_cmd();
				else if (this->_command == "OPER")
					Oper_cmd();
				else if (this->_command == "PART")
					Part_cmd();
				else if (this->_command == "KICK")
					Kick_cmd();
				else if (this->_command == "MODE")
					Mode_cmd();
				else if (this->_command == "KILL")
					Kill_cmd();
				else if (this->_command == "TOPIC")
					Topic_cmd();
				else if (this->_command == "INVITE")
					Invite_cmd();
			}
			else
				return (replynError("A user must be provide: usage: /USER <username> <hostname> <servername> <realname>", "431"));
		}
		else
			return (replynError("A nick must be provide: usage: /NICK <nick>", "431"));
	} else
		return (replynError("Password is required: usage: /PASS <password>", "464"));
	return ;
}

//function to send numeric response either a reply or error
void	Command::replynError( std::string msg, std::string code, int fd, std::string opt ) {
	std::string	response;
	std::string	nick = this->_user.getNick();

	if (nick.empty())
		nick = "Unknown";
	response = ":127.0.0.1 " + code + " " + nick + " ";
	if (opt != "")
		response += opt + " ";
	response += msg + "\r\n";
	if (fd == 0)
		fd = this->_user.getFd();
	if (send(fd, response.c_str(), strlen(response.c_str()), 0) < 0)
		utils::errorMessage("replynError: send:", strerror(errno));
	return ;
}

void	Command::Pass_cmd( void ) {
	if (this->_user.isAuth())
		return (replynError("User already registered!", "462"));
	if (this->_args.empty() || this->_args.size() != 1)
		return (replynError("usage: /PASS <password>", "461"));
	if (this->_args[0] == this->_ircServer.getpass()) {
		this->_user.auth();
		return (replynError("Password Correct!", "338"));
	} else
		return (replynError("Incorrect Password!", "339"));
}

void	Command::Nick_cmd( void ) {
	std::vector<User *>			usersVec;
	std::vector<User *>::iterator	it;

	if (this->_args.size() != 1)
		return (replynError("usage: /NICK <nick>", "431"));
	else if (this->_args[0].empty())
		return (replynError("Nick cannot be empty!", "432"));
	else if (this->_args[0] == "anonymous")
		return (replynError("This nick can't be used!", "432"));
	else if (utils::invalidCharacter(this->_args[0]))
		return (replynError("This nick contain invalids characters!", "432"));
	usersVec = this->_ircServer.getUsers();
	it = usersVec.begin();
	for( ; it != usersVec.end(); it++) {
		if (this->_args[0] == (*it)->getNick())
			return (replynError("This nick is already in use!", "432"));
	}
	this->_ircServer.messageAllUsers(this->_user.getNick() + ": change nick to " + this->_args[0]);
	this->_user.setNick(this->_args[0]);
	return (replynError("This nick changed", "001"));
}

void	Command::User_cmd( void ) {
	if (this->_args.size() != 4)
		return (replynError("usage: /USER <username> <hostname> <servername> <realname>", "461"));
	if (this->_user.getUsername().empty() == false)
		return (replynError("You are already register!", "462"));
	this->_user.setUsername(this->_args[0]);
	this->_user.setHostname(this->_args[1]);
	this->_user.setServername(this->_args[2]);
	this->_user.setRealname(this->_args[3]);
	replynError("Welcome to the ft_irc server " +
		this->_user.getNick() + "! " + this->_user.getUsername() + "@" + this->_user.getHostname(), "001");
	if (this->_ircServer.checkOperators() == false){
		replynError("You are now an IRC operator!", "381", this->_user.getFd());
			this->_ircServer.messageToServer(":127.0.0.1 001 all :" + this->_user.getNick() + " is an operator now!", this->_user.getFd());
		this->_user.setOper();
	}
		replynError("Avalible commands;-", "001", this->_user.getFd());
		replynError("/JOIN <channels> [<keys>]", "001", this->_user.getFd());
		replynError("/QUIT  <reason>", "001", this->_user.getFd());
		replynError("/USER <username> <hostname> <servername> <realname>", "001", this->_user.getFd());
		replynError("/OPER <user> <password>", "001", this->_user.getFd());
		replynError("/PART <channel>", "001", this->_user.getFd());
		replynError("/KICK <channel> <user> [<comment>]", "001", this->_user.getFd());
		replynError("/KILL <user> <comment>", "001", this->_user.getFd());
		replynError("/PRIVMSG <user> <msg!>", "001", this->_user.getFd());
		replynError("/TOPIC <channel> [<:new topic>]", "001", this->_user.getFd());
		replynError("/INVITE <NICK> <channel>", "001", this->_user.getFd());
		replynError("/MODE <channel>  [[+|-]modechars [parameters]]", "001", this->_user.getFd());
	return ;
}

void	Command::Privmsg_cmd( void ) {
	
	User		*receive;
	Channel		*channel;
	std::string	msg;
	std::string	response;

	if (this->_args.size() == 0)
		return (replynError("A nick must be provided!", "411"));
	if (this->_args.size() == 1)
		return (replynError("A message must be provided!", "412"));
	msg = utils::joinSplit(this->_args.begin() + 1, this->_args.end());
	std::vector<std::string>	pmu;
	pmu = utils::split(this->_args[0], ',');
	for (size_t i = 0; i < pmu.size(); i++)
	{
		if (pmu[i][0] != '#') 
		{
			receive = this->_ircServer.getUserByNick(pmu[i]);
			if (receive == NULL)
				{
					replynError("Nick not found!", "401");
					continue;
				}
			if (msg[0] == ':')
				msg.erase(0, 1);
			response = ":" + this->_user.getNick() + " PRIVMSG " + receive->getNick() + " :" + msg;
			receive->receiveMessage(response);
		}
		else 
		{
			channel = this->_ircServer.getChannelByName(pmu[i]);
			if (channel == NULL)
			{
				replynError("Channel Not Found!", "403");
				continue;
			}
			if (channel->getUserByNick(this->_user.getNick()) == NULL)
			{
				replynError("To send a message to a channel you need to join it!", "442");
				continue;
			}
			if (msg[0] == ':')
				msg.erase(0, 1);
			response = ":" + this->_user.getNick() + " PRIVMSG " + channel->getChannelName() + " :" + msg;
			channel->messageToChannel(response, this->_user.getFd());
		}
	}
	return ;
}


void	Command::Quit_cmd( void ) {
		User		*user;
	std::string	response;

	if (this->_args.size() < 1)
		return (replynError("usage: /QUIT <reason>", "461"));
	response = ":" + this->_user.getNick() + " QUIT :" + utils::joinSplit(this->_args.begin(), this->_args.end());
	this->_ircServer.messageAllUsers(response);
	std::cout <<  "User left: "  << this->_user.getFd() << std::endl;
	if(this->_user.isOper())
		this->_user.setOper();
	if (this->_ircServer.getUsers().size() != 1	&& this->_ircServer.checkOperators() == false) {
		user = *(this->_ircServer.getUsers().begin() + 1);
		if (user != NULL) {
			user->isOper();
			this->_ircServer.messageToServer(":127.0.0.1 001 all :" + user->getNick() + " is an operator now!", user->getFd());
			replynError("You are now an IRC operator!", "381", user->getFd());
		}
	}
	this->_ircServer.deleteUser(this->_user.getFd());
	return ;
}

void	Command::Join_cmd( void ) 
{
	Channel		*channel;
	std::string	users;
	std::string	channelName;
	std::string	password;

	if (this->_args.size() < 1 || this->_args.size() > 2)
		return (replynError("usage: /JOIN <channels> [<keys>]", "461"));
	password = this->_args.size() == 1 ? "" : this->_args[1];
	std::vector<std::string>	cs;
	cs = utils::split(this->_args[0], ',');

	for (size_t i = 0; i < cs.size(); i++)
	{
		if (cs[i][0] != '#')
			cs[i] = "#" + cs[i];
		channel = this->_ircServer.getChannelByName(cs[i]);
		if (channel == NULL) 
		{
			channel = new Channel(cs[i], password);
			this->_ircServer.addChannel(channel);
			channel->addOper(&this->_user);
		}
		else
		{
			if (channel->check_mode('i'))
				return (replynError("channel mode is invite only", "473"));
		}
		if (channel->getUserByNick(this->_user.getNick()) != NULL)
			return (replynError("You already are in this channel!", "443"));
		if (channel->check_mode('l'))
		{
			if (channel->getUsersname().size() >= channel->getLimit())
				return (replynError("channel is full!", "471"));
		}
		if (channel->check_mode('k')) //will check the password only if the mode was set to k
		{
			if (password == channel->getPassword())
				this->_user.addChannel(channel);
			else
				return (replynError("Password incorrect to channel!", "475"));
		}
		else
			this->_user.addChannel(channel);
		channel->messageFromChannel(":" + this->_user.getNick() + " JOIN " + channel->getChannelName());
		users = utils::joinSplit(channel->getUsersname());
		channelName = channel->getChannelName();
		if (channelName[0] == '#')
			channelName.erase(0, 1);
		replynError(users, "353", 0, "= " + channelName);
		replynError("End of /NAMES list", "366", 0, channelName);
	}
	return ;
}

void	Command::Oper_cmd( void ) {
	User	*user;

	if (this->_args.size() != 2)
		return (replynError("usage: /OPER <user> <password>", "461"));
	user = this->_ircServer.getUserByNick(this->_args[0]);
	if (user == NULL)
		return (replynError("The user passed doesn't exist!", "401"));
	if (user->isOper())
		return (replynError("User is an operator already!", "610"));
	if (this->_args[1] == OPERATOR_PASS)
		user->setOper();
	else
		return (replynError("Wrong password, You can't be operator!", "464"));
	this->_ircServer.messageToServer(":127.0.0.1 001 all :" + this->_args[0] + " is an operator now!", user->getFd());
	replynError("You are now an IRC operator!", "381", user->getFd());
	return ;
}

void	Command::Part_cmd( void ) {
	Channel								*channel;
	std::string							response;
	std::vector<std::string>::iterator	it;

	if (this->_args.size() < 1)
		return (replynError("usage: /PART <channel>", "461"));
	it = this->_args.begin();
	for ( ; it != this->_args.end(); it++) {
		if ((*it)[0] == ':')
			it->erase(0, 1);
		if ((*it)[0] != '#')
			*it = '#' + *it;
		channel = this->_ircServer.getChannelByName(*it);
		if (channel == NULL)
			replynError("There is no channel with this name!", "403", 0, *it);
		else if (channel->getUserByNick(this->_user.getNick()) == NULL)
			replynError("You aren't in this channel!", "442", 0, *it);
		else {
			response = ":" + this->_user.getNick() + " PART " + *it;
			channel->messageFromChannel(response);
			this->_user.removeChannel(channel);
			channel->removeUser(&this->_user);
		}
	}
	return ;
}

void	Command::Mode_cmd( void ) {
	Channel		*channel;
	User		*user;

	if (this->_args.size() < 1)
		return (replynError("usage: /MODE <channel>  [[+|-]modechars [parameters]]", "461"));
	if (this->_args[0][0] != '#')
		this->_args[0] = '#' + this->_args[0];
	channel = this->_ircServer.getChannelByName(this->_args[0]);
	if (channel == NULL)
		return (replynError("Channel Not Found!", "403", 0, this->_args[0]));
	// if only /MODE <channel> --> print the mode of the channel (all users can use this command)
	if (this->_args.size() == 1)
		return(replynError("The channel mode is +" + channel->getMode(), "1"));
	if (channel->getOperByNick(this->_user.getNick()) == false)
		return (replynError("You need be operator to use Mode cmd!", "482", 0, channel->getChannelName()));
	size_t arg = 1;
	while (arg < this->_args.size())
	{
		int next = 1;
		if (this->_args[arg][0] == '-') //remove mode
		{
			for (unsigned long i = 1; i < this->_args[arg].size(); i++)
			{
				if (this->_args[arg][i] == 'o')
				{
					while (arg + next < this->_args.size() && this->_args[arg + next][0] != '+' && this->_args[arg + next][0] != '-')
						{
							user = channel->getOperByNick(this->_args[arg + next]);
							if (user == NULL)
							{
								next++;
								continue;
							}
							channel->removeOper(user);
							next++;
						}
				}
				else
				{
					size_t found = channel->getMode().find(this->_args[arg][i]);
					if (found != std::string::npos)
					{
						std::string mode = channel->getMode().erase(found,1);
						channel->setMode(mode);
					}
				}
			}
		}
		else if (this->_args[arg][0] == '+')//add mode
		{
			for (size_t i = 1; i < this->_args[arg].size(); i++)
			{
				//CHECK if valid flag
				if (!(this->is_valid_mode(this->_args[arg][i])))
					return (replynError("NOT a valid mode", "472"));
				if (channel->getMode().find(this->_args[arg][i]) == std::string::npos && this->_args[arg][i] != 'o')
				{
					std::string mode = channel->getMode() + this->_args[arg][i];
					channel->setMode(mode);
				}
				switch (this->_args[arg][i])
				{
					case 'o':
					{
						while (arg + next < this->_args.size() && this->_args[arg + next][0] != '+' && this->_args[arg + next][0] != '-')
						{
							user = channel->getUserByNick(this->_args[arg + next]);
							if (user != NULL)
							{
								if (channel->getOperByNick(this->_args[arg + next]) == NULL)
								{
									channel->addOper(user);
								}
							}
							else
								replynError("User need to be a member of the channel to be an Operator!", "442");
							next++;
						}
						break;
					}
					case 'k':
						if(arg + next >= this->_args.size()){
							size_t found = channel->getMode().find('k');
							if (found != std::string::npos)
							{
								std::string mode = channel->getMode().erase(found,1);
								channel->setMode(mode);
							}
							return (replynError("missing parameter", "461"));
						}
						channel->setPassword(this->_args[arg + next]);
						next++;
						break;
					case 'l':
						{
							if(arg + next >= this->_args.size())
							{
								size_t found = channel->getMode().find('l');
								if (found != std::string::npos)
								{
									std::string mode = channel->getMode().erase(found,1);
									channel->setMode(mode);
								}
								return (replynError("missing parameter", "461"));
							}
						long long  limit = atoll(this->_args[arg + next].c_str());
						if (limit < 1){
							size_t found = channel->getMode().find('l');
								if (found != std::string::npos)
								{
									std::string mode = channel->getMode().erase(found,1);
									channel->setMode(mode);
								}
							return (replynError("invlid limit", "461"));

						}
						channel->setLimit(strtoul(this->_args[arg + next].c_str(), 0, 0));
						next++;
						break;
						}
					default:
						break;
				}
			}
		}
		else
			return (replynError("NOT a valid mode", "472"));
		arg += next;
	}
}

void	Command::Kick_cmd( void ) {
	Channel		*channel;
	User		*user;
	std::string	msg;
	std::string	response;
	if (this->_args[0] == this->_user.getNick())
		this->_args.erase(this->_args.begin());
	if (this->_args.size() < 2)
		return (replynError("usage: /KICK <channel> <user> [<comment>]", "461"));
	if (this->_args[0][0] != '#')
		this->_args[0] = '#' + this->_args[0];
	channel = this->_ircServer.getChannelByName(this->_args[0]);
	if (channel == NULL)
		return (replynError("Channel Not Found!", "403", 0, this->_args[0]));
	if (channel->getOperByNick(this->_user.getNick()) == false)
		return (replynError("You need be operator to kick a user!", "482", 0, channel->getChannelName()));
	if (channel->getUserByNick(this->_user.getNick()) == NULL)
		return (replynError("You need be a member of the channel to kick a user!", "442", 0, channel->getChannelName()));
	user = channel->getUserByNick(this->_args[1]);
	if (user == NULL)
		return (replynError("This user isn't in this channel!", "441", 0, channel->getChannelName() + " " + this->_args[1]));
	msg = utils::joinSplit(this->_args.begin() + 2, this->_args.end());
	if (msg[0] == ':')
		msg.erase(0, 1);
	response = ":" + this->_user.getNick() + " KICK " + channel->getChannelName() + " " + user->getNick() + " " + msg;
	user->removeChannel(channel);
	user->receiveMessage(response);
	channel->removeUser(user);
	channel->messageFromChannel(response);
	return ;
}

void	Command::Help_cmd( void ) {
	replynError("START of /HELP list", "704");
	replynError("Commands Avaliable:", "705", this->_user.getFd());
	replynError("PASS, QUIT, HELP, NICK, USER,PRIVMSG, JOIN, OPER, PART, KICK, KILL", "705", this->_user.getFd());
	replynError("To see how to use a command, send: /<command> ", "705", this->_user.getFd());
	replynError("END of /HELP list", "706");
	return ;
}

void	Command::Kill_cmd( void ) {
	User		*user;
	std::string	msg;
	std::string	response;

	if (this->_args.size() < 2)
	 	return (replynError("usage: /KILL <user> <comment>", "461"));
	if (this->_user.isOper() == false)
		return (replynError("You need be operator to kill a user!", "485", this->_user.getFd()));
	user = this->_ircServer.getUserByNick(this->_args[0]);
	if (user == NULL)
		return (replynError("The user passed doesn't exist!", "401"));
	msg = utils::joinSplit(this->_args.begin() + 1, this->_args.end());
	if (msg[0] == ':')
	 	msg.erase(0, 1);
	response = ":" + this->_user.getNick() + " KILL " + user->getNick() + " . " + msg;
	user->receiveMessage(response);
	response = user->getNick() + " have been killed by " + this->_user.getNick();
	this->_ircServer.messageToServer(response, user->getFd());
	std::cout << "User left: "<< user->getFd() << std::endl;
	this->_ircServer.deleteUser(user->getFd());
	return ;
}

void	Command::Topic_cmd( void ) {
	Channel		*cchannel;
	std::string channelName;
	std::string channelTopic;

	if (this->_args.size() < 1 || this->_args.size() > 2)
			return (replynError("usage: /TOPIC <channel> [<topicname>]", "461"));
	channelName = this->_args[0];
	if(!(cchannel = this->_ircServer.getChannelByName(channelName)))
		return (replynError("NOsuchCHANNEL ", "403"));
	if (this->_args.size() == 1) //print the topic
	{
		replynError("The topic of " + channelName + " is " + cchannel->getTopic() , "4");
	}
	else
	{
		if (cchannel->check_mode('t') && cchannel->getOperByNick(this->_user.getNick()) == false)
			return (replynError("You need to be channel operator to set the topic!", "481"));
		else
		{
			channelTopic = this->_args[1];
			cchannel->setTopic(channelTopic);
		}
	}
}

bool Command::is_valid_mode(char c)
{
	std::string mode = "itkol";

	for (int i = 0; i < (int)mode.size(); i++)
	{
		if (c == mode[i])
			return (true);
	}
	return (false);
}

void	Command::Invite_cmd( void ) {
	Channel *channel;
	std::string	users;

	if (this->_args.size() !=2){
	return (replynError("usage: /INVITE <NICK> <channel>", "461"));
	}
	std::string Username = this->_args[0];
	std::string Channelname = this->_args[1];
	User *invited=this->_ircServer.getUserByNick(Username);
	if(!invited)
	{
		return (replynError("NOsuchUSER ", "444"));
	}
	if(!this->_ircServer.getChannelByName(Channelname))
	{
		return (replynError("NOsuchChannel ", "403"));
	}
	else 
		channel= this->_ircServer.getChannelByName(Channelname);
	if (channel->getOperByNick(this->_user.getNick()) == false)
	{
		return (replynError("You need be channel operator to invite a user!", "482", 0, Channelname));
	}
	//std::cout <<"in" <<std::endl;
	//std::cout <<"userbynick: "<< Username <<std::endl;
	if ((this->_ircServer.getChannelByName(Channelname))->getUserByNick(Username) != NULL)
		return (replynError("User Already are in this channel!", "443")); //y
	else 
	{
		//std::cout <<"the user is : "<< invited->getNick() <<std::endl;
		invited->addChannel(this->_ircServer.getChannelByName(Channelname));
		(this->_ircServer.getChannelByName(Channelname))->messageFromChannel(":" + invited->getNick() + " JOIN " + channel->getChannelName());
		users = utils::joinSplit((this->_ircServer.getChannelByName(Channelname))->getUsersname());
		replynError(users, "353", 0, "= " + Channelname);
		replynError("End of /NAMES list", "366", 0, Channelname);
	}
}

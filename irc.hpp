#ifndef IRC_HPP
# define IRC_HPP

# define HOST "127.0.0.1"
# define MODE "itkol"
extern bool	work;

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cerrno>
#include <vector>
#include <algorithm>
#include <cctype>
#include <signal.h>
#include <stdexcept>


#include "Channel.hpp"

#include "User.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "Command.hpp"


// info about replynError:
// https://www.alien.net.au/irc/irc2numerics.html

# define OPERATOR_PASS "oprpassword"


#endif
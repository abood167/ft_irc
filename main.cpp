
#include "irc.hpp" 

bool	work;

int check_port_num(std::string port)
{
    for (int i = 0; port[i]; i++)
    {
        if (!isdigit(port[i]))
            return(1);
    }
    
    if (atoi(port.c_str()) < 1024 || atoi(port.c_str()) > 65535)
        return (1);
    return (0);
}

void	handle_ctrl_c( int sig ) {

	(void)sig;

	std::cout << std::endl;
	std::cout << "control + c recieved !" << std::endl;

	work=false;

}

int	main( int argc, char **argv ) {

	if (argc != 3) {
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	if (check_port_num(argv[1]))
     {
        std::cerr << "Port number is not valid." << std::endl;
        exit (1);
    }
	work = true;
	signal(SIGINT, handle_ctrl_c);
	Server	server(HOST, argv[1], argv[2]);
	try {


		server.start();
		server.receive();
	
	} catch ( std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}


	return (0);

}

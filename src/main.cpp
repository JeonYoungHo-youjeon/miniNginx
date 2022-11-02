#include "../inc/event/Event.hpp"
#include "../inc/parse/Config.hpp"

#include <iostream>
#include <stdexcept>
#include <ctime>

using namespace std;
Config	g_conf("config/default.conf");

int main(int argc, char** argv)
{
	if (argc == 2)
		try
		{
			g_conf = Config(argv[1]);
		}
	catch(std::exception& e)
	{
		cerr << "Error" << e.what() << endl;
		return 1;
	}
	try
	{
		Event event;
		
		event.event_loop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	// system("leaks miniNginx");
    return 0;
}

#include "../inc/event/Event.hpp"
#include "../inc/parse/Config.hpp"

#include <iostream>
#include <stdexcept>
#include <ctime>

Config	g_conf("config/default.conf");

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		try
		{
			g_conf = Config(argv[1]);
		}
		catch (std::exception& e)
		{
			std::cerr << "Error: Config" << std::endl;
			return 1;
		}
	}
	try
	{
		Event event;

		event.event_loop();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: Event" << std::endl;
		return 1;
	}
	
    return 0;
}

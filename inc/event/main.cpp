#include "Event.hpp"
#include "../parse/Config.hpp"

#include <ctime>
#include <iostream>
#include <stdexcept>

using namespace std;
Config	g_config("../../config/default2.conf");

int main()
{
	try
	{
		Event event;
		
		event.event_loop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

    return 0;
}
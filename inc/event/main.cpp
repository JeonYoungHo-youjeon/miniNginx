#include "Event.hpp"
#include "../parse/Config.hpp"

#include <iostream>
#include <stdexcept>
#include <ctime>

using namespace std;
Config	g_conf("../../config/default2.conf");

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
	// system("leaks miniNginx");
    return 0;
}
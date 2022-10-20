#include "Event.hpp"
#include "../parse/Config.hpp"

#include <ctime>
#include <iostream>
#include <stdexcept>

using namespace std;

int main()
{
	try
	{
		Config config("../../config/default2.conf");
		Event event(config);
		
		event.event_loop();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	// ParsingException
	// EventInitException
	// EventLoopException

    return 0;
}
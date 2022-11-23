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
	Event event;
	
	srand((unsigned)time(NULL) * getpid());
	event.event_loop();
	// system("leaks miniNginx");
    return 0;
}

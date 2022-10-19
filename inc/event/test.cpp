#include <iostream>
#include <map>
#include "Event.hpp"
#include "../parse/Config.hpp"

int main()
{
	Config parse("../../config/default.conf");
	Event event(parse);

	event.event_loop();
}
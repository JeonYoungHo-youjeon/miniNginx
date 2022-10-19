#include "Event.hpp"
#include "../parse/Config.hpp"

#include <ctime>
#include <iostream>

using namespace std;

int main()
{
	Config parse("../../config/default2.conf");
	Event event(parse);

	event.event_loop();

    // time_t tmNow = time(0);
    // char *dt = ctime(&tmNow);
	// std::string s = dt;
	// s.pop_back();
    // cout<<"\033[1;34m Current Date/Time: "<< s << "]";
    // cout<<endl;

    return 0;

}
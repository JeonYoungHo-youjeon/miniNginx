#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Config.hpp"

Config	g_config;
using namespace std;

int main() {
	std::cout << "[ TEST 1 ]===================" << std::endl;
	//Config parse("../../default.conf");
	g_config = Config("../../config/default.conf");
	std::cout << g_config.str(0) << std::endl;

	std::cout << ::g_config["0.0.0.0:8000"]["/hello"]["root"].front() << std::endl;
	std::cout << g_config["0.0.0.0:8000"]["/limit"]["client_max_body_size"].front() << std::endl;
	std::cout << g_config["0.0.0.0:8002"]["/"]["index"].front() << std::endl;

	return 0;
}

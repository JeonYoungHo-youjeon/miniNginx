#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Config.hpp"

using namespace std;

int main() {
	std::cout << "[ TEST 1 ]===================" << std::endl;
	Config parse("../../default.conf");
	
	std::cout << parse.str(0) << std::endl;

	std::cout << parse["*:8000"]["/hello"]["root"].front() << std::endl;
	std::cout << parse["*:8000"]["/limit"]["client_max_body_size"].front() << std::endl;
	std::cout << parse["*:8002"]["/"]["index"].front() << std::endl;

	return 0;
}

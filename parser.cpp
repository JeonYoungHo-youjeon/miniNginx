#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "inc/parse/Config.hpp"

using namespace std;

int main() {
	Config parse("default.conf");
	std::cout << parse.str(0) << std::endl;
	return 0;
}

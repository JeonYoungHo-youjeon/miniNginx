#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "inc/parse/Parser.hpp"

using namespace std;

int main() {
	Parser parse("default.conf");
	std::cout << parse.getStringBuf() << std::endl;
	return 0;
}
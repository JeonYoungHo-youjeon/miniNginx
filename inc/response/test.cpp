#include "Response.hpp"
#include <iostream>
#include "../parse/Util.hpp"
# include "../http.hpp"
# include "../parse/Config.hpp"

using namespace std;

std::string example =
    "GET /cgi-bin/test_cgi.sh"	//METHOD URL
	"?name=youngpar&id=qwe&arg=TEST"	//PARAM
	" "
	"HTTP/1.1\r\n"				//PROTOCOL
    "Host: localhost:8000\r\n"	//HEADER
    "\r\n"
    "Hello world\r\n";

Request req(example);
Config conf("../../config/default.conf");
int main(int argc, char** argv, char** envp) {
    //cout << "Request Test" << endl;
    //req.print_request();
    //cout << "Test end" << endl;

	cout << conf.str(0) << endl;
    ResponseImpl resp(req, conf);
    //cout << "result : " << resp.str() << endl;
    return 0;
}
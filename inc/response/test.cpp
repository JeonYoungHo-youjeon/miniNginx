#include "Response.hpp"
#include <iostream>
#include "../parse/Util.hpp"
# include "../http.hpp"
# include "../parse/Config.hpp"

using namespace std;

std::string example =
	//"GET /filetest.txt"
	//"GET /cgi-bin/pycgi.py"		//METHOD URL
	//"GET /cgi-bin/test_cgi.sh"		//METHOD URL
	"POST /cgi-bin/pycgi.py"		//METHOD URL
	//"DELETE /filetest.txt"		//METHOD URL
	"?name=youngpar&id=qwe&arg=<ARGTEST>"	//PARAM
	" "
	"HTTP/1.1\r\n"				//PROTOCOL
    "Host: localhost:8000\r\n"	//HEADER
    "\r\n"
    "테스트1\r\n"
	"테스트2\r\n";

Config g_conf("../../config/default.conf");
Request req(example);

int main(int argc, char** argv, char** envp) {
	//cout << g_conf.str(0) << endl;
    cout << "Request Test" << endl;
	//req.print_request();
    cout << "Test end" << endl;

    try
	{
		Response resp(req);
		cout << "---------------------" << endl;
		try
		{
			resp.out();
		}
		catch (Exception & e)
		{
			cout << e.what() << endl;
			cout << "QWEQWE" << endl;
		}
		cout << "---------------------" << endl;
	}
	catch (Exception& e)
	{
		cout << e.what() << endl;
	}
    return 0;
}
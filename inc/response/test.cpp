#include "Response.hpp"
#include <iostream>
#include "../parse/Util.hpp"
# include "../http.hpp"
# include "../parse/Config.hpp"

using namespace std;

std::string example =
	"GET /cgi-bin/test_cgi.sh"		//METHOD URL
	//"POST /filetest.txt"		//METHOD URL
	//"DELETE /filetest.txt"		//METHOD URL
	"?name=youngpar&id=qwe&arg=TEST"	//PARAM
	" "
	"HTTP/1.1\r\n"				//PROTOCOL
    "Host: localhost:8000\r\n"	//HEADER
    "\r\n"
    "포스트 잘 가나용?\r\n"
	"총 3줄이 입력될 예정인데\r\n"
	"잘 되겠지?\r\n";

Config g_conf("../../config/default.conf");
Request req(example);

int main(int argc, char** argv, char** envp) {
    cout << "Request Test" << endl;
	req.print_request();
    cout << "Test end" << endl;

	cout << g_conf.str(0) << endl;
    try
	{
		ResponseImpl resp(req);

		cout << "---------------------" << endl;
		try
		{
			cout << "response body : " << resp.str() << endl;
		}
		catch (Exception & e)
		{
			cout << e.what() << endl;
		}
		cout << "---------------------" << endl;
	}
	catch (Exception& e)
	{
		cout << e.what() << endl;
	}
    return 0;
}
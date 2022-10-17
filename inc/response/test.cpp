#include "Response.hpp"
#include <iostream>
# include "../http.hpp"

using namespace std;

std::string example =
    "GET examfile.txt HTTP/1.1\r\n"
    "Host: localhost:8085\r\n"
    "\r\n"
    "Hello world\r\n";

Request req(example);

int main() {
    cout << "Request Test" << endl;
    req.print_request();
    cout << "Test end" << endl;
    
    ResponseImpl resp(req);
    cout << "result : " << resp.str() << endl;
    return 0;
}
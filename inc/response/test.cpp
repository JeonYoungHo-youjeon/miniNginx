#include <string>
# include <iostream>

# include <dirent.h>
# include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using std::string;
using std::cout;
using std::endl;

	string get_dirlist_page(string path, string head)
	{
		string ret;
		string page;

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(path.c_str()))) 
		{
			while ((ent = readdir(dir))) 
			{
				struct stat statbuf;
				std::string tmp = ent->d_name;
				std::string checker = path + tmp;
				stat(checker.c_str(), &statbuf);
				if (S_ISDIR(statbuf.st_mode))
					tmp += "/" ;

				page += "<a href=\"";
				page += tmp;
				page += "\">";
				page += tmp;
				page += "</a>\n" ;
			}
			closedir (dir);
		} 
	
		ret =
			"<html>\n"
			"<head><title>Index of " + head + " </title></head>\n"
			"<body>\n"
			"<h1>Index of " + head + " </h1><hr><pre>\n" +
			page +
			"</pre><hr></body>\n"
			"</html>\n";
		cout << "TEST\n" << ret << endl;
		return ret;
	}

    int main()
    {
        string path = "./";
        string head = "test";

		time_t now = time(NULL) + 120;
		struct tm *tm = gmtime(&now);
		char expires[80];
		strftime(expires, 80, "%a, %d %b %Y %H:%M:%S GMT", tm);
		printf("%s", expires);

        int fd = open("./test.html", O_RDWR);
        string tmp = get_dirlist_page(path, head); 
        write(fd, tmp.c_str(), tmp.size());
    }
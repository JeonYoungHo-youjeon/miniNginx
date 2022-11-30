#ifndef FILE_HPP
# define FILE_HPP

# include "Contents.hpp"
# include "../exception/Exception.hpp"
# include <sys/stat.h>

struct File : public Contents
{
	File(const std::string& url, int flag) : Contents(url), flag(flag) {};
	File(const std::string& path, const std::vector<std::string>& param)
	: Contents(path) {};
	~File()
	{
		close();
	};

	int	set();
	int	close();
	void kill() {};
	bool checkNull();

	int	flag;
};

int	File::set()
{
	inFd = ::open(url.c_str(), flag, 777);
	outFd = ::open(url.c_str(), O_RDONLY, 777);

	if (outFd < 0 || inFd < 0)
		throw 404;
	pid = 0;
	return BODY;
}
int	File::close()
{
	::close(outFd);
	::close(inFd);
	return true;
}

bool File::checkNull()
{
	std::ifstream is(url.c_str());
	if (!is.seekg(0, std::ios::end).tellg())
		throw 204;
	return true;
}

#endif
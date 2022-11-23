#ifndef FILE_HPP
# define FILE_HPP

# include "Contents.hpp"
# include "../exception/Exception.hpp"

struct File : public Contents
{
	File(const std::string& url) : Contents(url) {};
	File(const std::string& path, const std::vector<std::string>& param)
	: Contents(path) {};
	~File()
	{
		close();
	};

	int	set();
	int	close();
	void kill() {};

};

int	File::set()
{
	outFd = ::open(url.c_str(), O_RDONLY);
	inFd = ::open(url.c_str(), O_WRONLY);

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


#endif
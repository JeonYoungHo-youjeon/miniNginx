#ifndef PARSE_UTIL_HPP
# define PARSE_UTIL_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <string>
# include <map>
# include <dirent.h>
# include <sys/stat.h>

struct Util
{
    static std::string remover(const std::string& input, const char rmchar);
    static std::vector<std::string> split(const std::string& input, const char delimeter);
	static std::string& strip(std::string& input, const char c);
	static std::pair<std::string, std::string>
	divider(const std::string& ps, const char delimeter);
	static std::pair<std::string, std::string>
	divider(const std::pair<std::string, std::string>& pss, const char delimeter);
	static int stoi(const std::string& str);
	static std::string& remove_crlf(std::string& str);
	static std::string join(const std::string& str1, const std::string& str2, const char c);
	static int to_hex(const std::string& str);
	static std::string get_date();
	static std::string to_string(int num);
	static void getline(std::string& buffer, std::string& to, const char c);
	static int is_dir(const std::string& path);
};

std::string Util::remover(const std::string& input, const char rmchar)
{
    std::string                 ret;
    std::vector<std::string>    buf = Util::split(input, rmchar);

	for (std::vector<std::string>::iterator it = buf.begin(); it != buf.end(); ++it)
        ret += *it;
    return ret;
}

std::vector<std::string> Util::split(const std::string& input, const char delimeter)
{
    std::vector<std::string>	ret;
    std::stringstream			ss(input);
    std::string					tmp;

    while (std::getline(ss, tmp, delimeter))
        ret.push_back(tmp);

    return ret;
}

std::string& Util::strip(std::string& input, const char c)
{
	std::size_t pos;

	pos = input.find_first_not_of(c);
	if (pos != std::string::npos)
		input.erase(0, pos);

	pos = input.find_last_not_of(c);
	if (pos != std::string::npos)
		input.erase(pos + 1, input.length());

	return input;
}

std::pair<std::string, std::string>
Util::divider(const std::string& ps, const char delimeter)
{
	size_t idx = ps.rfind(delimeter);
	std::string first(ps.begin(), ps.begin() + idx);
	std::string second(ps.begin() + idx + 1, ps.end());
	std::cout << "divider" << std::endl;
	std::cout << "[" << first << "]" << std::endl;
	std::cout << "[" << second << "]" << std::endl;
	return std::make_pair(first, second);
}

std::pair<std::string, std::string>
Util::divider(const std::pair<std::string, std::string>& pss, const char delimeter)
{
	std::pair<std::string, std::string>	newpss = divider(pss.first, delimeter);
	newpss.second += pss.second;
	return newpss;
}

int Util::stoi(const std::string& str)
{
	std::stringstream ss(str);
	int ret;
	ss >> ret;
	return ret;
}

std::string& Util::remove_crlf(std::string& str)
{
	Util::strip(str, '\n');
	Util::strip(str, '\r');
	return str;
}

std::string Util::join(const std::string& str1, const std::string& str2, const char c)
{
	return str1 + c + str2;
}

int Util::to_hex(const std::string& str)
{
	int idx = 0;

	while (idx < str.size())
	{
		if (!(('0' <= str[idx] && str[idx] <= '9') || ('a' <= str[idx] && str[idx] <= 'f')))
			return -1;
		++idx;
	}
	
	int ret;
	std::stringstream ss(str);

	ss >> std::hex >> ret;
	return ret;
}

std::string Util::get_date()
{
	time_t rawtime;
	struct tm *timeinfo;
	char buf[31];

	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

	return buf;
}

std::string Util::to_string(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

void Util::getline(std::string& buffer, std::string& to, const char c = '\n')
{
	std::stringstream ss(buffer);
	std::getline(ss, to, c);
	buffer.erase(0, to.size() + 1);
	size_t	pos = buffer.find_first_not_of(c);
	buffer.erase(0, pos);
	std::cout << "ORIGIN BUF : " << buffer << std::endl;
	std::cout << "TO BUF : " << to << std::endl;
}

int Util::is_dir(const std::string& path)
{
	DIR *dir;
	if ((dir = opendir(path.c_str())))
	{
		closedir(dir);
		return 1;
	}

	switch (errno)
	{
	case ENOTDIR:
		return 0;
	case ENOENT:
		throw 404;
	case EACCES:
		throw 403;
	default:
		throw 500;
	}
}

#endif
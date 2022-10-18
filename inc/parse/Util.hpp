#ifndef PARSE_UTIL_HPP
# define PARSE_UTIL_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <string>
# include <map>

struct Util
{
    static std::string remover(const std::string& input, const char rmchar);
    static std::vector<std::string> split(const std::string& input, const char delimeter);
	static std::string strip(const std::string& input);
};

std::string Util::remover(const std::string& input, const char rmchar)
{
    std::string                 ret;
    std::vector<std::string>    buf = Util::split(input, rmchar);
    for (std::vector<std::string>::iterator it = buf.begin(); it != buf.end(); ++it)
    {
        ret += *it;
    }
    return ret;
}

std::vector<std::string> Util::split(const std::string& input, const char delimeter)
{
    std::vector<std::string>	ret;
    std::stringstream			ss(input);
    std::string					tmp;

    while (getline(ss, tmp, delimeter))
    {
        ret.push_back(tmp);
	}

    return ret;
}

std::string Util::strip(const std::string& input)
{
	std::string::const_iterator	begin = input.begin();
	std::string::const_iterator	end = input.end();

	for (; begin != end && *begin == ' '; ++begin);
	for (; end != begin && *(end - 1) == ' '; --end);

	if (begin == input.begin() && end == input.end())
	{
		return input;
	}
	return std::string(begin, end);
}

#endif
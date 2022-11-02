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
	static std::string& strip(std::string& input, const char c);
	static std::pair<std::string, std::string>
	divider(const std::string& ps, const char delimeter);
	static std::pair<std::string, std::string>
	divider(const std::pair<std::string, std::string>& pss, const char delimeter);
	static int stoi(const std::string& str);
	static std::string& remove_crlf(std::string& str);
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

    while (getline(ss, tmp, delimeter))
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
	std::string second(ps.begin() + idx, ps.end());
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

#endif
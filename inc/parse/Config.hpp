#ifndef PARSE_CONFIG_HPP
# define PARSE_CONFIG_HPP

# include <fstream>
# include <algorithm>
# include <stdexcept>

# include "Util.hpp"
# include "Server.hpp"
# include "Mime.hpp"
# include "Status.hpp"

class Config
{
public:
	Config(){};
	Config(const char* filepath) ;
	const std::map<std::string, Server>& getServers() const ;
	string	str(size_t tab_size);
	const bool is_exist(const string& key) const;
	const Server& operator[](const string& key) const;
	const string& getContentType(const string& key) const;
	const string& getStatusMsg(int key) const;

protected:
private:
	void	preprocess(std::ifstream& ifs);

public:
	class ServerNotExist : public std::exception
	{
	public:
		const char* what() const throw();
	};
protected:
private:
	std::map<std::string, Server>	mServers;
	std::string						mStringBuf;
	static const Mime				mime;
	static const StatusMsg			statusMsg;
};	//	PARSER

/*
 *		Config Impliment
 */

Config::Config(const char* filepath)
{
	std::ifstream	ifs(filepath);
	if (!ifs.is_open())
		throw std::runtime_error(filepath);
	preprocess(ifs);

	std::stringstream			ss(mStringBuf);
	std::string					key = "";
	std::string					value = "";
	char						buf;
	int							cnt = 0;

	while ((buf = ss.get()) != -1)
	{
		if (cnt == 0 && value.empty())
		{
			key += buf;
			while ((buf = ss.get()) != -1 && buf != '{')
				key += buf;
		}
		if (buf == '}')
			cnt--;
		if (cnt)
			value += buf;
		if (buf == '{') cnt++;
		if (!key.empty() && !cnt)
		{
			key = Util::strip(key, ' ');
			value = Util::strip(value, ' ');

			if (key.empty() || value.empty())
			{
				std::cerr << "Key-Value Exception" << std::endl;
				throw std::bad_exception();
			}
			if (key != "server")
				std::cerr << "'server' Directive Error" << std::endl;

			Server	tmp(value);

			if (mServers.find(tmp.id()) != mServers.end())
				std::cerr << "Parse Map Collision" << std::endl;
			else
				mServers[tmp.id()] = tmp;
			key.clear();
			value.clear();
		}
	}
	if (cnt)
		std::cerr << "{ } Error" << std::endl;
}

void	Config::preprocess(std::ifstream& ifs)
{
	std::string tmp;

	while (std::getline(ifs, tmp, '\n'))
		mStringBuf += Util::strip(tmp, ' ');
    mStringBuf = Util::remover(mStringBuf, '\t');
}

const bool Config::is_exist(const string& key) const
{
	return mServers.find(key) != mServers.end();
}

const Server& Config::operator[](const string& key) const
{
	return mServers.find(key)->second;
}

string	Config::str(size_t tab_size)
{
	string	tmp;
	for (map<string, Server>::iterator it = mServers.begin(); it != mServers.end(); ++it)
	{
		for (size_t i = 0; i < tab_size; ++i)
			tmp += '\t';
		tmp += "[ " + it->first + " ]\n";
		tmp += it->second.str(tab_size + 1);
	}
	return tmp;
}
const std::map<std::string, Server>& Config::getServers() const
{
	return mServers;
}

const char* Config::ServerNotExist::what() const throw()
{
	return "Server is Not Exist";
}

const string& Config::getContentType(const string& key) const
{
	std::map<string, string>::const_iterator it = mime.find(key);
	if (it == mime.end())
		return "application/octet-stream";
	return it->second;
}

const string& Config::getStatusMsg(int key) const
{
	std::map<int, string>::const_iterator	it = statusMsg.find(key);
}


#endif  // PARSE_CONFIG_HPP
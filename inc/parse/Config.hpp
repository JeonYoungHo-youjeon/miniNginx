#ifndef PARSE_CONFIG_HPP
# define PARSE_CONFIG_HPP

# include <fstream>
# include <algorithm>
# include <stdexcept>

# include "Util.hpp"
# include "Server.hpp"

class Config
{
public:
	Config() {};
  Config(const char* filepath);
	const std::map<std::string, Server>& getServers() const ;
	string	str(size_t tab_size);
	Server& operator[](const string& key);

protected:
private:
	void	preprocess();

public:
protected:
private:
	std::ifstream					mIfs;
	std::map<std::string, Server>	mServers;
	std::string						mStringBuf;
};	//	PARSER

/*
 *		Config Impliment
 */

Config::Config(const char* filepath) : mIfs(filepath)
{
	if (!mIfs.is_open())
		throw std::runtime_error(filepath);
	preprocess();

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
			key = Util::strip(key);
			value = Util::strip(value);

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

void	Config::preprocess()
{
	std::string tmp;		
	while (std::getline(mIfs, tmp, '\n'))
		mStringBuf += Util::strip(tmp);
    mStringBuf = Util::remover(mStringBuf, '\t');
}

Server& Config::operator[](const string& key)
{
	return mServers[key];
}

string	Config::str(size_t tab_size)
{
	string	tmp;
	for (map<string, Server>::iterator it = mServers.begin(); it != mServers.end(); ++it)
	{
		for (size_t i = 0; i < tab_size; ++i)
			tmp += '\t';
		tmp += "[ ";
		tmp += it->first;
		tmp += " ]\n";
		tmp += it->second.str(tab_size + 1);
	}
	return tmp;
}
const std::map<std::string, Server>& Config::getServers() const
{
	return mServers;
}

#endif  // PARSE_CONFIG_HPP
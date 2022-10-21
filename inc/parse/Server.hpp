#ifndef PARSE_SERVER_HPP
# define PARSE_SERVER_HPP

# include "Util.hpp"
# include "Location.hpp"

using std::map;
using std::string;
using std::vector;

struct Server
{
public:

    Server(){};
	Server(const string& input);
    ~Server() {};
    
    //  Access Location
	const bool is_exist(const string& key) const;
	const Location& operator[](const string& key) const;
	const map<string, Location>& getLocations() const;

    //  Access Attribution
	const vector<string>& getAttr(const string& key) const;

	//	data to string
	string id();
	string str(size_t tab_size);

protected:
private:
public:
protected:
private:
    map<string, vector<string> >        mAttr;
    map<string, Location>               mLocation;
};

Server::Server(const string& input)
{
	int     cnt = 0;
	string  stack;

	for (string::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if (*it == '{')
			cnt++;
		if (*it == '}')
			cnt--;
		if (!cnt && *it == ';')
		{
			vector<string> tmp = Util::split(stack, ' ');
			if (tmp.size() < 2)
				std::cerr << "Parsing Error" << std::endl;
			else
			{
				if (!(mAttr.insert(make_pair(tmp[0], vector<string>(tmp.begin() + 1, tmp.end()))).second))
					std::cerr << "Map Collision" << std::endl;
				else
					mAttr[tmp[0]] = vector<string>(tmp.begin() + 1, tmp.end());
			}
			stack.clear();
		}
		else if (*it == '}')
		{
			vector<string> tmp = Util::split(stack, ' ');

			if (tmp[0] == "location")
			{
				if (mLocation.find(tmp[1]) != mLocation.end())
					std::cerr << "Map Collision" << std::endl;
				else
					mLocation[tmp[1]] = Location(tmp.begin() + 2, tmp.end());
			}
			stack.clear();
		}
		else
			stack += *it;
	}
	if (mAttr.find("listen") == mAttr.end())
		mAttr["listen"] = vector<string>(1, "80");
}

const map<string, Location>& Server::getLocations() const
{
	return mLocation;
}

const bool Server::is_exist(const string& key) const
{
	return mLocation.find(key) != mLocation.end();
}

const Location& Server::operator[](const string& key) const
{
	return mLocation.find(key)->second;
}

const vector<string>& Server::getAttr(const string& key) const
{
	return mAttr.find(key)->second;
}

string Server::id()
{
	vector<string> tmp = Util::split(mAttr["listen"][0], ':');
	if (tmp.size() > 2)
		throw std::out_of_range("listen range Error");
	string  addr = tmp.size() == 1 ? "0.0.0.0" : tmp[0];
	string  port = tmp.size() == 1 ? tmp[0] : tmp[1];
	return addr + ":" + port;
}

string Server::str(size_t tab_size)
{
	string	tmp;

	for (map<string, vector<string> >::iterator it = mAttr.begin(); it != mAttr.end(); ++it)
	{
		for (size_t i = 0; i < tab_size; ++i)
			tmp += '\t';
		tmp += it->first + " : ";
		for (vector<string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
			tmp += *vit + ' ';
		tmp += '\n';
	}
	for (map<string, Location>::iterator it = mLocation.begin(); it != mLocation.end(); ++it)
	{
		for (size_t i = 0; i < tab_size; ++i)
			tmp += '\t';
		tmp += "location '" + it->first + "'" + "\n";
		tmp += it->second.str(tab_size + 1);
	}
	return tmp;
}

#endif  //  PARSE_SERVER_HPP

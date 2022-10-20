#ifndef PARSE_LOCATION_HPP
# define PARSE_LOCATION_HPP

# include "Util.hpp"

using std::map;
using std::string;
using std::vector;

struct Location
{
public:
    Location() {}
    Location(vector<string>::iterator begin, vector<string>::iterator end);

	const bool is_exist(const string& key) const;
	const vector<string>& operator[](const string& key) const;
    string  str(size_t tab_size);

public:
    map<string, vector<string> > mAttr;
};

//	Location Implementation
Location::Location(vector<string>::iterator begin, vector<string>::iterator end)
{
	string  buffer;
	for (; begin != end; ++begin)
	{
		for (string::iterator it = begin->begin(); it != begin->end(); ++it)
		{
			if (*it == '{' || *it == '}')
				continue ;
			if (*it == ';')
			{
				vector<string>  tmp = Util::split(buffer, ' ');
				if (mAttr.find(tmp[0]) != mAttr.end())
					std::cerr << "Map Colision [" << tmp[0] << "]"<< std::endl;
				else
					mAttr[tmp[0]] = vector<string>(tmp.begin() + 1, tmp.end());
				buffer.clear();
			}
			else
				buffer += *it;
		}
		buffer += ' ';
	}
}

const bool Location::is_exist(const string& key) const
{
	return mAttr.find(key) != mAttr.end();
}

const vector<string>& Location::operator[](const string& key) const
{
	return mAttr.find(key)->second;
}

string  Location::str(size_t tab_size)
{
	string  tmp;

	for (map<string, vector<string> >::iterator it = mAttr.begin(); it != mAttr.end(); ++it)
	{
		for (size_t i = 0; i < tab_size; ++i)
			tmp += '\t';
		tmp += it->first;
		tmp += " : ";
		for (size_t i = 0; i < it->second.size(); ++i)
			tmp += it->second[i] + ' ';
		tmp += '\n';
	}
	return tmp;
}

#endif  //  PARSE_LOCATION_HPP
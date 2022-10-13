#ifndef PARSE_LOCATION_HPP
# define PARSE_LOCATION_HPP

# include <map>
# include <vector>
# include <string>

using std::map;
using std::string;

struct Location
{
    map<string, string> mAttr;

    string operator[](const string& key)
    {
        return mAttr[key];
    }
};

#endif  //  PARSE_LOCATION_HPP
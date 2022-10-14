#include <utility>

#include "../inc/Config.hpp"

void Config::add_server(const key_type& k, const val_type& v)
{
	// TODO : duplicated server error handling
	// if (config.count(k) != 0)
	// 	error
	config.insert(std::pair<key_type, val_type>(k, v));
}

const Config::val_type& Config::get_server(const key_type& k)
{
	// TODO : server doesn't exist error handling
	// if (config.count(k) == 0)
	// 	error
	return config[k];
}

const std::map<Config::key_type, Config::val_type>& Config::get_map() const
{
	return config;
}

Config::Config()
{}

Config::Config(const Config& c)
: config(c.config)
{}

Config::~Config()
{}

Config& Config::operator=(const Config& c)
{
	if (this != &c)
	{
		config.clear();
		config = c.config;
	}
	return *this;
}

Config::map_type::iterator Config::begin()
{
	return config.begin();
}

Config::map_type::const_iterator Config::begin() const
{
	return config.begin();
}

Config::map_type::iterator Config::end()
{
	return config.end();
}

Config::map_type::const_iterator Config::end() const
{
	return config.end();
}

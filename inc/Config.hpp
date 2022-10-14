#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "ServerBlock.hpp"

/**
 * @class Config
 * @brief server key(ip + port)와 server value(ServerBlock)을 갖는 class
 * 
 * @key_type ip + port - std::string
 * @val_type ServerBlock
 */
class Config
{
public:
	typedef ServerBlock::key_type				key_type;
	typedef ServerBlock::val_type				val_type;
	typedef std::map<key_type, val_type>		map_type;
	typedef std::size_t							size_type;

public:
	void add_server(const key_type& k, const val_type& v);
	const val_type& get_server(const key_type& k);
	const map_type& get_map() const;

	Config();
	Config(const Config& c);
	~Config();

	Config& operator=(const Config& c);

private:
	map_type config;
};

#endif 
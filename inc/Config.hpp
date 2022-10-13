#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "ServerBlock.hpp"

/**
 * @class Config
 * @brief server key(ip + port)와 server value(ServerBlock)을 갖는 map
 * 
 * @key_type ip + port - std::string
 * @val_type ServerBlock
 */
class Config
{
	typedef std::string							server_key;
	typedef ServerBlock							server_val;
	typedef server_key							key_type;
	typedef std::map<server_key, server_val> 	val_type;

public:
	void push_server(const server_key& key, const server_val& val);
	const val_type& get_val(const key_type& key) const;

	Config();
	~Config();

private:
	Config(const Config& c);
	Config& operator=(const Config& c);
	
private:
	std::map<key_type, val_type> config;	

};

#endif 
#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <map>
#include <utility>

#include "LocationBlock.hpp"
#include "Directive.hpp"

/**
 * @class ServerBlock
 * @brief ip + port를 key로 갖고, LocationBlock을 value로 갖는 pair
 * 
 * @key_type ip + port - std::string
 * @val_type directive map과 location map의 pair - std::pair<directive_map_type, location_map_type>
 */ 
class ServerBlock
{
	typedef std::string 										directive_key;
	typedef Directive 											directive_val;
	typedef std::map<directive_key, directive_val> 				directive_map_type;
	typedef std::string 										location_key;
	typedef LocationBlock										location_val;
	typedef std::map<location_key, location_val> 				location_map_type;
	typedef std::string 										key_type;
	typedef std::pair<directive_map_type, location_map_type>	val_type;

public:
	void push_directive(const directive_key& key, const directive_val& val);
	void push_location(const location_key& key, const location_val& val);
	void set_key(const key_type& key);
	const directive_map_type& get_directive_map() const;
	const location_map_type& get_location_map() const;
	const key_type& get_key();

	ServerBlock();
	ServerBlock(std::string& key_type);
	~ServerBlock();
	
private:
	ServerBlock(const ServerBlock& server);
	ServerBlock& operator=(const ServerBlock& server);

private:
	std::pair<key_type, val_type> server;
};
#endif 
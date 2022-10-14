#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <map>
#include <utility>

#include "LocationBlock.hpp"
#include "Directive.hpp"

/**
 * @class ServerBlock
 * @brief ip + port를 key로 갖고, LocationBlock을 value로 갖는 class
 * 
 * @key_type ip + port - std::string
 * @val_type directive map과 location map의 pair - std::pair<directive_map_type, location_map_type>
 */ 
class ServerBlock
{
public:
	typedef Directive::key_type									directive_key;
	typedef Directive::val_type									directive_val;
	typedef std::map<directive_key, directive_val> 				directive_map_type;
	typedef LocationBlock::key_type 							location_key;
	typedef LocationBlock::val_type								location_val;
	typedef std::map<location_key, location_val> 				location_map_type;
	typedef std::string 										key_type;
	typedef std::pair<directive_map_type, location_map_type>	val_type;
	typedef std::size_t											size_type;

public:
	void set_key(const key_type& k);
	void set_value(const val_type& v);
	void set_server_block(const key_type& k, const val_type& v);
	void add_directive(const directive_key& k, const directive_val& v);
	void add_location(const location_key& k, const location_val& v);
	const directive_map_type& get_directive_map() const;
	const location_map_type& get_location_map() const;
	const key_type& get_key() const;
	const val_type& get_value() const;

	ServerBlock();
	ServerBlock(const ServerBlock& server);
	ServerBlock(const key_type& k, const val_type& v);
	~ServerBlock();
	
	ServerBlock& operator=(const ServerBlock& server);
	size_type directive_size() const;
	size_type location_size() const;

private:
	key_type key;
	val_type val;

};
#endif 
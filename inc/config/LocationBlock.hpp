#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

#include <map>
#include <string>
#include <utility>

#include "Directive.hpp"

/**
 * @class LocationBlock
 * @brief pattern을 key로 갖고, directive 클래스를 value로 갖는 class
 * 
 * @key_type pattern - std::string
 * @val_type directive map - std::map<directive_key, directive_val>
 */
class LocationBlock
{
public:
	typedef Directive::key_type									directive_key;
	typedef Directive::val_type									directive_val;
	typedef std::string 										key_type;
	typedef std::map<directive_key, directive_val>				val_type;
	typedef std::size_t											size_type;

public:
	void set_key(const key_type& k);
	void set_value(const val_type& v);
	void set_location_block(const key_type& k, const val_type& v);
	void add_value_elem(const directive_key& k, const directive_val& v);
	const key_type& get_key() const;
	const val_type& get_value() const;

	LocationBlock();
	LocationBlock(const LocationBlock& loc);
	LocationBlock(const key_type& k, const val_type& v);
	~LocationBlock();

	LocationBlock& operator=(const LocationBlock& loc);
	size_type value_size() const;

private:
	key_type key;
	val_type val;
};

#endif 
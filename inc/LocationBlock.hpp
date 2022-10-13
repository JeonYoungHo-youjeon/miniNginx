#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

#include <map>
#include <string>
#include <utility>

#include "Directive.hpp"

/**
 * @class LocationBlock
 * @brief pattern을 key로 갖고, directive 클래스를 value로 갖는 pair
 * 
 * @key_type pattern - std::string
 * @val_type directive map - std::map<directive_key, directive_val>
 */
class LocationBlock
{
	typedef std::string											directive_key;
	typedef Directive											directive_val;
	typedef std::string 										key_type;
	typedef std::map<directive_key, directive_val>				val_type;

public:
	void set_key(const key_type& pattern);
	void push_directive(key_type& k, val_type& v);
	const key_type& get_key() const;
	const val_type& get_val() const;

	LocationBlock();
	LocationBlock(const key_type& pattern);
	~LocationBlock();

private:
	LocationBlock(const LocationBlock& l);
	LocationBlock& operator=(const LocationBlock& d);

private:
	std::pair<key_type, val_type> location;
};

#endif 
#include "../../inc/config/LocationBlock.hpp"

void LocationBlock::set_key(const key_type& k)
{
	key = k;
}

void LocationBlock::set_value(const val_type& v)
{
	val = v;
}

void LocationBlock::set_location_block(const key_type& k, const val_type& v)
{
	set_key(k);
	set_value(v);
}

void LocationBlock::add_value_elem(const directive_key& k, const directive_val& v)
{
	// TODO : duplicated directive error handling
	// if (val.count(k) != 0)
	// 	error
	val[k] = v;
}

const LocationBlock::key_type& LocationBlock::get_key() const
{
	return key;
}

const LocationBlock::val_type& LocationBlock::get_value() const
{
	return val;
}

LocationBlock::LocationBlock()
{}

LocationBlock::LocationBlock(const LocationBlock& loc)
: key(loc.get_key()), val(loc.get_value())
{}

LocationBlock::LocationBlock(const key_type& k, const val_type& v)
: key(k), val(v)
{}

LocationBlock::~LocationBlock()
{}


LocationBlock& LocationBlock::operator=(const LocationBlock& loc)
{
	if (this != &loc)
	{
		key.clear();
		val.clear();
		set_location_block(loc.get_key(), loc.get_value());
	}
	return *this;
}

LocationBlock::size_type LocationBlock::value_size() const
{
	return val.size();
}
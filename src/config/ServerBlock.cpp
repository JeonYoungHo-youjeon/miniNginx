#include "../../inc/config/ServerBlock.hpp"

void ServerBlock::set_key(const key_type& k)
{
	key = k;
}

void ServerBlock::set_value(const val_type& v)
{
	val = v;
}

void ServerBlock::set_server_block(const key_type& k, const val_type& v)
{
	set_key(k);
	set_value(v);
}

void ServerBlock::add_directive(const directive_key& k, const directive_val& v)
{
	// TODO : duplicated directive error handling
	// if (val.first.count(k) != 0)
	// 	error
	val.first[k] = v;
}

void ServerBlock::add_location(const location_key& k, const location_val& v)
{
	// TODO : duplicated location error handling
	// if (val.second.count(k) != 0)
	// 	error
	val.second[k] = v;
}

const ServerBlock::directive_map_type& ServerBlock::get_directive_map() const
{
	return val.first;
}

const ServerBlock::location_map_type& ServerBlock::get_location_map() const
{
	return val.second;
}

const ServerBlock::key_type& ServerBlock::get_key() const
{
	return key;
}

const ServerBlock::val_type& ServerBlock::get_value() const
{
	return val;
}

ServerBlock::ServerBlock()
{}

ServerBlock::ServerBlock(const ServerBlock& server)
: key(server.get_key()), val(server.get_value())
{}

ServerBlock::ServerBlock(const key_type& k, const val_type& v)
: key(k), val(v)
{}

ServerBlock::~ServerBlock()
{}

ServerBlock& ServerBlock::operator=(const ServerBlock& server)
{
	if (this != &server)
	{
		key.clear();
		val.first.clear();
		val.second.clear();
		set_server_block(server.get_key(), server.get_value());
	}
	return *this;
}

ServerBlock::size_type ServerBlock::directive_size() const
{
	return val.second.size();
}

ServerBlock::size_type ServerBlock::location_size() const
{
	return val.first.size();
}

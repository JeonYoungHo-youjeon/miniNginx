#include "../../inc/config/Directive.hpp"

void Directive::set_key(const key_type& k)
{
	key = k;
}

void Directive::set_value(const val_type& v)
{
	val = v;
}

void Directive::set_directive(const key_type& k, const val_type& v)
{
	set_key(k);
	set_value(v);
}

void Directive::add_value_elem(const val_elem_type& vElem)
{
	val.push_back(vElem);
}

const Directive::key_type& Directive::get_key() const
{
	return key;
}

const Directive::val_type& Directive::get_value() const
{
	return val;
}


Directive::Directive()
{}

Directive::Directive(const Directive& d)
: key(d.get_key()), val(d.get_value())
{}

Directive::Directive(key_type& k, val_type& v)
: key(k), val(v)
{}

Directive::~Directive()
{}

Directive& Directive::operator=(const Directive& d)
{
	if (this != &d)
	{
		key.clear();
		val.clear();
		set_directive(d.get_key(), d.get_value());
	}
	return *this;
}

Directive::size_type Directive::value_size() const
{
	return val.size();
}

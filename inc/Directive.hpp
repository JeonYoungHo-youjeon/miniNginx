#ifndef DIRECTIVE_HPP
# define DIRECTIVE_HPP

#include <utility>
#include <vector>
#include <string>

/**
 * @class Directive
 * @brief directive를 key로 갖고, value vector를 value로 갖는 class
 * 
 * @key_type directive - std::string
 * @val_type directive's values - std::vector<std::string>
 */
class Directive
{
public:
	typedef std::string					key_type;
	typedef std::string					val_elem_type;
	typedef std::vector<val_elem_type>	val_type;
	typedef std::size_t					size_type;

public:
	void set_key(const key_type& k);
	void set_value(const val_type& v);
	void set_directive(const key_type& k, const val_type& v);
	void add_value_elem(const val_elem_type& vElem);
	const key_type& get_key() const;
	const val_type& get_value() const;

	Directive();
	Directive(const Directive& d);
	Directive(key_type& k, val_type& v);
	~Directive();

	Directive& operator=(const Directive& d);
	size_type value_size() const;
	
private:
	key_type key;
	val_type val;
};

#endif 
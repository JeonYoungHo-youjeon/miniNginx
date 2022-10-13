#ifndef DIRECTIVE_HPP
# define DIRECTIVE_HPP

#include <utility>
#include <vector>
#include <string>

/**
 * @class Directive
 * @brief directive를 key로 갖고, value vector를 value로 갖는 pair
 * 
 * @key_type directive - std::string
 * @val_type directive's values - std::vector<std::string>
 */
class Directive
{
	typedef std::string					key_type;
	typedef std::vector<std::string>	val_type;

public:
	void set_directive(key_type& s, val_type& v);
	const key_type& get_key() const;
	const val_type& get_value() const;

	Directive();
	Directive(key_type& s, val_type& v);
	~Directive();
	
private:
	Directive(const Directive& d);
	Directive& operator=(const Directive& d);

private:
	std::pair<key_type, val_type> directive; 
};

#endif 
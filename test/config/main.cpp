#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <utility>
#include "../../inc/Config.hpp"

typedef const std::vector<std::pair<std::string, std::vector<std::string> > > directive_type;


static const directive_type locationDirective {
	{"limit_except", {"method1", "method2", "method3"}},
	{"error_page", {"code1", "code2", "code3"}},
	{"client_max_body_size", {"size"}},
	{"root", {"path"}},
	{"autoindex", {"on"}},
	{"index", {"file"}},
	{"upload_path", {"location"}},
	{"return", {"code", "URL"}}
};

void print_config(Config& config)
{
	std::map<int, int> m;

	for (auto& pr : config)
	{
		std::cout << "[ server block ]" << std::endl;
		std::cout << "server key : " << pr.first << std::endl;
		std::cout << "{" << std::endl;
		for (auto& directive : pr.second.first)
		{
			std::cout << "\t" << directive.first << " ";
			for (auto& value : directive.second)
				std::cout << value << " ";
			std::cout << std::endl;
		}
		std::cout << std::endl;
		for (auto& location : pr.second.second)
		{
			std::cout << "\tlocation " << location.first << " {" << std::endl;
			for (auto& directive : location.second)
			{
				std::cout << "\t\t" << directive.first << " ";
				for (auto& value : directive.second)
					std::cout << value << " ";
				std::cout << std::endl;
			}
			std::cout << "\t}" << std::endl;
			std::cout << std::endl;
		}
		std::cout << "}" << std::endl;
		std::cout << std::endl;
	}
}

LocationBlock create_location_block(const LocationBlock::key_type& key)
{
	LocationBlock location;

	location.set_key(key);
	for (auto& pr : locationDirective)
		location.add_value_elem(pr.first, pr.second);
	return location;
}

ServerBlock create_server_block(const directive_type& serverDirective)
{
	ServerBlock server;
	ServerBlock::directive_map_type directive_map;
	LocationBlock location;
	ServerBlock::key_type key = "";

	for (auto& pr : serverDirective)
		server.add_directive(pr.first, pr.second);

	ServerBlock::directive_map_type d = server.get_directive_map();
	for (ServerBlock::size_type i = 0; i < d["server_name"].size(); ++i)
		key += d["server_name"][i] + ":";

	for (ServerBlock::size_type i = 0; i < d["listen"].size(); ++i)
	{
		key += d["listen"][i];
		if (i < d["listen"].size() - 1)
			key += ":";
	}
	server.set_key(key);

	location = create_location_block("/");
	server.add_location(location.get_key(), location.get_value());
	location = create_location_block("/home");
	server.add_location(location.get_key(), location.get_value());

	return server;
}

Config create_config()
{
	const directive_type serverDirective {
	{"server_name", {"hostname", "hostname2"},},
	{"listen", {"127.0.0.1:80"}},
	{"error_page", {"code1", "code2"}},
	{"client_max_body_size", {"size"}},
	{"root", {"path"}},
	{"autoindex", {"on"}},
	{"index", {"file"}},
	{"upload_path", {"location"}},
	{"return", {"code", "URL"}}
	};
	const directive_type serverDirective2 {
		{"server_name", {"hostname3", "hostname4"},},
		{"listen", {"0.0.0.0:80"}},
		{"error_page", {"code3", "code4"}},
		{"client_max_body_size", {"size"}},
		{"root", {"path"}},
		{"autoindex", {"on"}},
		{"index", {"file"}},
		{"upload_path", {"location"}},
		{"return", {"code", "URL"}}
	};
	Config config;
	ServerBlock server = create_server_block(serverDirective);
	config.add_server(server.get_key(), server.get_value());
	ServerBlock server2 = create_server_block(serverDirective2);
	config.add_server(server2.get_key(), server2.get_value());

	return config;
}

int main()
{
	Config config = create_config();

	print_config(config);

}

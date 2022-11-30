#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <sys/socket.h>

# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../response/Cgi.hpp"

extern Config g_conf;

struct RequestStartLine
{
	string method;			// GET, POST, DELETE 등의 메소드	*
	string url;				// 요청 URL (./ , /www/...)		*
	string protocol;		// HTTP 버전 (HTTP/1.1)

	void out()
	{
		std::cout << "[    Start Line    ]" << std::endl;
		std::cout << "[" << method << "] " << "[" << url << "] " << "[" << protocol << "]" << std::endl;
	}
};

struct Request
{
	RequestStartLine			StartLine;
	std::map<string, string>	Header;
	std::stringstream 			bodySS;

	string						configName;
	string						locationName;
	string 						fileName;
	string 						ext;
	std::stringstream			buffer;
	int 						progress;
	std::string					tmp;
	string						ip;

	string virtualPath;

	int maxBodySize;
	int readSize;
	int contentLength;
	int remainReadLength;
	bool chunkFlag;
	int statusCode;
	int clientFd;
	char charBuffer[BUFFER_SIZE];

	std::vector<string>	params;

	Request() {}
	Request(int fd, const string& configName)
	: statusCode(200), configName(configName), clientFd(fd)
	{
		this->configName = configName;
		progress = START_LINE;
	};

	Request& operator=(const Request& req)
	{
		progress = req.progress;
		configName = req.configName;
		maxBodySize = req.maxBodySize;
		clientFd = req.clientFd;

		return *this;
	}

	int clear_read()
	{
		progress = START_LINE;
		Header.clear();
		bodySS.str("");

		return read();
	}

	int read()
	{
		if (is_empty_buffer())
		{
			char rcvData[BUFFER_SIZE];
			memset(rcvData, 0, BUFFER_SIZE);

			int byte = recv(clientFd, &rcvData[0], BUFFER_SIZE, 0);

			if (byte < 0)
			{
				clear_buffer();
				throw statusCode = 400;
			}
			buffer.write(rcvData, byte);
		}
		return parse();
	}
	void set_header()
	{
		parse_url();

		locationName = findLocation(virtualPath);
		fileName = virtualPath;
		fileName.erase(0, locationName.size());
		if (g_conf[configName][locationName].is_exist("client_max_body_size"))
			maxBodySize = Util::stoi(g_conf[configName][locationName]["client_max_body_size"][0]);
		else
			maxBodySize = DEFAULT_MAX_BODY_SIZE;

		if (Header.count(HEAD[CONTENT_LENGTH]))
		{
			contentLength = Util::stoi(Header[HEAD[CONTENT_LENGTH]]);

			if (contentLength > maxBodySize)
			{
				clear_buffer();
				throw statusCode = 413;
			}
			remainReadLength = contentLength;
		}
		if (Header.count(HEAD[TRANSFER_ENCODING]))
			chunkFlag = true;
	}

	int parse()
	{
		while (true)
		{
			switch (progress)
			{
			case START_LINE:
				if (parse_startline() == false)
				{
					clear_buffer();
					throw statusCode = 400;
				}
				progress = HEADER;
				break;
			case HEADER:
				if (is_empty_buffer() == true)
					return READ_REQUEST;
				else if (is_crlf_line() == true)
				{
					set_header();
					progress = CRLF;
				}
				else
				{
					std::getline(buffer, tmp);
					make_header(tmp);
				}

				break;
			case CRLF:
				skip_crlf();

				// if (StartLine.method == "POST" && is_empty_buffer() == true)
				// 	return READ_REQUEST;
				// else if (StartLine.method != "POST" \
				// 		&& !Header.count(HEAD[CONTENT_LENGTH]) && !Header.count(HEAD[TRANSFER_ENCODING]))
				// 	return END_REQUEST;
				// else if (StartLine.method != "POST" \
				// 		&& (Header.count(HEAD[CONTENT_LENGTH]) || Header.count(HEAD[TRANSFER_ENCODING])))
				// {
				// 	while (is_empty_buffer() == false)
				// 	{
				// 		std::getline(buffer, tmp);
				// 		if (tmp == "\r")
				// 			break;
				// 	}
				// 	return END_REQUEST;
				// }
				if (StartLine.method == "GET" || StartLine.method == "DELETE")
					return END_REQUEST;
				else if (StartLine.method != "POST" \
						&& !Header.count(HEAD[CONTENT_LENGTH]) && !Header.count(HEAD[TRANSFER_ENCODING]))
					return END_REQUEST;
				else if (chunkFlag)
					progress = CHUNK_SIZE;
				else
					progress = LENGTH_BODY;
				
				break;
			case LENGTH_BODY:
				if (contentLength == 0)
					return END_REQUEST;
				if (remainReadLength <= 0)
					return END_REQUEST;

				if (is_empty_buffer() == true)
					return READ_REQUEST;

				if (remainReadLength > BUFFER_SIZE)
					readSize = BUFFER_SIZE;
				else
					readSize = remainReadLength;

				memset(charBuffer, 0, readSize);
				buffer.read(charBuffer, readSize);
				bodySS.write(charBuffer, readSize);
				remainReadLength = contentLength - bodySS.str().size();

				break;
			case CHUNK_SIZE:
				if (is_empty_buffer() == true)
					return READ_REQUEST;

				std::getline(buffer, tmp);
				Util::remove_crlf(tmp);

				readSize = Util::to_hex(tmp);
				if (readSize == -1)
				{
					std::cout << "FOUR" << std::endl;
					clear_buffer();
					throw statusCode = 400;
				}
				else if (readSize == 0)
					return END_REQUEST;

				progress = CHUNK_DATA;
				
				break;
			case CHUNK_DATA:
				if (is_empty_buffer() == true)
					return READ_REQUEST;

				std::getline(buffer, tmp);
				tmp += "\n";
				bodySS << tmp.substr(0, readSize);

				int read_len = tmp.size();

				if (read_len < readSize)
				{
					readSize -= read_len;
					if (is_empty_buffer())
						return READ_REQUEST;
				}
				else
					progress = CHUNK_SIZE;
				break;
			}
		}
		return END_REQUEST;
	}

	void skip_line()
	{
		std::getline(buffer, tmp);
	}

	void make_header(const std::string& buf)
	{
		std::stringstream ss(buf);
		std::string key;
		std::string val;

		std::getline(ss, key, ':');
		if (ss.peek() == ' ')
			ss.get();
		std::getline(ss, val);
		Header[string_to_metavar(key)] = val;
	}

	std::string findLocation(const std::string& path)
	{
		std::string ret;
		std::string tmp;
		if (g_conf[configName].is_exist(path))
			return path;
		vector<std::string> pathTree = Util::split(path, '/');

		for (std::vector<std::string>::iterator it = pathTree.begin(); it != pathTree.end(); ++it)
			{
				tmp = '/' + *it;
				if (g_conf[configName].is_exist(tmp))
					ret = tmp;
			}
		return ret;
	};

	void print_request()
	{
		StartLine.out();
		for (std::map<string, string>::iterator it = Header.begin(); it != Header.end(); ++it)
			std::cout << it->first << ": " << it->second << std::endl;
		std::cout << "[Body]" << std::endl;
		std::cout << bodySS.str() << std::endl;
		std::cout << "[Body byte]" << std::endl;
		for (int i = 0; i < bodySS.str().size(); ++i)
			std::cout << (int)bodySS.str()[i] << " ";
		std::cout << std::endl;
	}

	bool parse_startline() {
		// std::cout << "[BUFFER]" << std::endl;
		// std::cout << buffer.str() << std::endl;
		// std::cout << "[ENDBUFFER]" << std::endl;
		buffer >> StartLine.method >> StartLine.url >> StartLine.protocol;

		if (StartLine.method == "" || StartLine.url == "" || StartLine.protocol == "")
			return false;
		if (is_crlf_line() == false)
			return false;
		if (StartLine.url[0] != '/')
			return false;

		skip_crlf();
		return true;
	}

	void clear_buffer()
	{
		buffer.str("");
		buffer.clear();
	}

	bool is_empty_buffer()
	{
		if (buffer.peek() == -1)
		{
			clear_buffer();
			return true;
		}
		return false;
	}

	void skip_crlf()
	{
		if (buffer.peek() == '\r')
			buffer.get();
		if (buffer.peek() == '\n')
			buffer.get();
	}

	bool is_crlf_line()
	{
		if (buffer.peek() == '\r')
		{
			buffer.get();
			if (buffer.peek() == '\n')
			{
				buffer.unget();
				return true;
			}
			buffer.unget();
		}
		else if (buffer.peek() == '\n')
			return true;

		return false;
	}

	void parse_url()
	{
		std::stringstream ss(StartLine.url);
		
		std::getline(ss, virtualPath, '?');
		std::getline(ss, Header["QUERY_STRING"]);
	}

	std::string string_to_lower(std::string s)
	{
		for (int i = 0; i < s.size(); ++i)
			s[i] = std::tolower(s[i]);
		return s;
	}

	std::string string_to_metavar(std::string s)
	{
		for (int i = 0; i < s.size(); ++i)
		{
			if (s[i] == '-')
				s[i] = '_';
			s[i] = std::toupper(s[i]);
		}
		return s;
	}
};

#endif //REQUEST_H

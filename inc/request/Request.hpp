#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <sys/socket.h>

# include "../parse/Config.hpp"
# include "../parse/Util.hpp"
# include "../response/Cgi.hpp"

using std::string;
using std::cout;
using std::endl;

extern Config g_conf;

struct RequestStartLine
{
	string method;			// GET, POST, DELETE 등의 메소드	*
	string url;				// 요청 URL (./ , /www/...)		*
	string protocol;		// HTTP 버전 (HTTP/1.1)

	void out()
	{
		cout << "[    Start Line    ]" << endl;
		cout << "[" << method << "] " << "[" << url << "] " << "[" << protocol << "]" << endl;
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

	string virtualPath;

	int maxBodySize;
	int chunkSize;
	int contentLength;
	bool chunkFlag;
	int statusCode;
	int clientFd;

	std::vector<string>	params;

	/**
	* @brief : 생성자 초기화, set()을 통한 초기화 등 임의로 다양하게 구현
	*/
	Request() : statusCode(200) {};
	Request(int fd, const string& configName)
	: statusCode(200), configName(configName)
	{
		this->configName = configName;
		this->clientFd = fd;
		progress = START_LINE;
	};

	Request& operator=(const Request& req)
	{
		return *this;
	}

	void set(int fd, const string& configName)
	{
		this->configName = configName;
		this->clientFd = fd;
		maxBodySize = MAX_BODY_SIZE;
		progress = START_LINE;
	}

	int read()
	{
		char rcvData[BUFFER_SIZE] = {0};
		int byte = recv(clientFd, &rcvData[0], BUFFER_SIZE, 0);

		if (byte <= 0)
		{
			strerror(errno);
			throw statusCode = 400;	//	catch (int statusCode);
		}
		buffer << rcvData;

		if (buffer.str().find('\n') == std::string::npos)
			return READ_REQUEST;
		return parse();
	}

	int parse()
	{
		if (progress == START_LINE)
		{
			if (parse_startline() == false)
				throw statusCode = 400;

			StartLine.out();
			progress = HEADER;
			return READ_REQUEST;
		}

		while (progress == HEADER)
		{
			if (is_done_buffer() == true)
				return READ_REQUEST;

			if (is_crlf_line() == true) {
				progress = HEADER_SET;
				break;
			}

			std::getline(buffer, tmp);
			make_header(tmp);
		}

		if (progress == HEADER_SET)
		{
			parse_url();

			locationName = findLocation(virtualPath);
			fileName = virtualPath.erase(0, locationName.size());
			ext = findExtension(virtualPath);

			if (Header.count(HEAD[CONTENT_LENGTH]))
			{
				contentLength = Util::stoi(Header[HEAD[CONTENT_LENGTH]]);
				// TODO : get max body size
				// if (g_conf[configName][locationName].is_exist("client_max_body_size"))
				// 	maxBodySize = Util::stoi(g_conf[configName][locationName]["client_max_body_size"][0]);
			}
			
			if (Header.count(HEAD[TRANSFER_ENCODING]))
				chunkFlag = true;

			if (StartLine.method != "POST")
				return DONE_REQUEST;

			progress = CRLF;
			if (is_done_buffer() == true)
				return READ_REQUEST;
		}

		if (progress == CRLF)
		{
			skip_crlf();
			if (chunkFlag)
				progress = CHUNK_SIZE;
			else
				progress = BODY;

			if (is_done_buffer() == true)
				return READ_REQUEST;
		}

		char charBuffer[BUFFER_SIZE] = {0};
		int readSize;

		if (progress == CHUNK_SIZE || progress == CHUNK_DATA)
		{
			while (true)
			{
				if (progress == CHUNK_SIZE)
				{
					std::getline(buffer, tmp);
					Util::remove_crlf(tmp);

					chunkSize = Util::to_hex(tmp);

					std::cout << "[CHUNK SIZE]" << std::endl;
					std::cout << chunkSize << std::endl;

					if (chunkSize == -1)
						throw statusCode = 400;

					if (chunkSize == 0)
						return DONE_REQUEST;

					progress = CHUNK_DATA;
					if (is_done_buffer() == true)
						return READ_REQUEST;
				}

				if (progress == CHUNK_DATA)
				{
					// TODO : if occur erorr, maybe chunkSize and BUFFER_SIZE
					buffer.read(charBuffer, chunkSize);
					bodySS << charBuffer;

					std::cout << "[CHUNK DATA]" << std::endl;
					for (char c : bodySS.str())
						std::cout << (int)c << " ";
					std::cout << std::endl;

					int buffer_len = strlen(charBuffer);
					memset(charBuffer, 0, BUFFER_SIZE);

					if (buffer_len < chunkSize)
					{
						chunkSize -= buffer_len;
						is_done_buffer();
						return READ_REQUEST;
					}
					
					if (is_crlf_line() == false)
						std::getline(buffer, tmp);

					progress = CHUNK_SIZE;
					if (is_done_buffer())
						return READ_REQUEST;
				}
			}
		}
		else
		{
			// TODO : remaining data is another request. but now read until contentLength
			if (contentLength > MAX_BODY_SIZE)
				contentLength = MAX_BODY_SIZE;

			readSize = contentLength - bodySS.str().size();

			while (true)
			{
				buffer.read(charBuffer, readSize);
				bodySS << charBuffer;
				memset(charBuffer, 0, readSize);

				if (bodySS.str().size() == contentLength)
					break;

				if (is_done_buffer())
					return READ_REQUEST;
			}	
		}
		skip_crlf();
		return DONE_REQUEST;
	}

	void make_header(const std::string& buf)
	{
		std::stringstream ss(buf);
		std::string key;
		std::string val;

		std::getline(ss, key, ':');
		ss >> val;

		Header[string_to_lower(key)] = val;
	}

	//	TODO : 맘에 안드는 함수
	std::string findLocation(const std::string& path)
	{
		std::string ret;
		std::string tmp;
		vector<std::string> pathTree = Util::split(path, '/');

		for (std::vector<std::string>::iterator it = pathTree.begin(); it != pathTree.end(); ++it)
			if (g_conf[configName].is_exist(tmp = Util::join(tmp, *it, '/')))
				ret = tmp;
		return ret;
	};

	std::string findExtension(const std::string& url)
	{
		std::string::size_type pos = url.rfind('.');
		if (pos == std::string::npos)
			return "";
		return std::string(url.begin() + pos, url.end());
	}

	/**
	*  현재 리퀘스트 구조체의 내용 전체를 출력. 빈 변수는 출력하지 않음
	*
	*/
	void print_request()
	{
		StartLine.out();
		for (std::map<string, string>::iterator it = Header.begin(); it != Header.end(); ++it)
			cout << it->first << ": " << it->second << endl;
		cout << "[Body]" << endl;
		std::cout << bodySS.str() << std::endl;
		std::cout << "[Body byte]" << std::endl;
		for (int i = 0; i < bodySS.str().size(); ++i)
			std::cout << (int)bodySS.str()[i] << " ";
		std::cout << std::endl;
	}

	bool parse_startline() {
		buffer >> StartLine.method >> StartLine.url >> StartLine.protocol;
		if (StartLine.method == "" || StartLine.url == "" || StartLine.protocol == "")
			return false;
		if (buffer.peek() != '\r' && buffer.peek() != '\n')
			return false;
		if (StartLine.url[0] != '/')
			return false;

		skip_crlf();
		return true;
	}

	bool is_done_buffer()
	{
		if (buffer.peek() == -1)
		{
			buffer.str("");
			buffer.clear();
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
				skip_crlf();
				return true;
			}
			buffer.unget();
		}
		return false;
	}

	void parse_url()
	{
		std::stringstream ss(StartLine.url);
		std::string param;
		
		std::getline(ss, virtualPath, '?');

		while (!ss.eof())
		{
			std::getline(ss, param, '&');
			params.push_back(param);
		}
	}
	std::string string_to_lower(std::string s)
	{
		for (int i = 0; i < s.size(); ++i)
			s[i] = std::tolower(s[i]);
		return s;
	}

};

#endif //REQUEST_H

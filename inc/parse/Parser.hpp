#ifndef PARSE_PARSER_HPP
# define PARSE_PARSER_HPP

# include <fstream>
# include <sstream>
# include <algorithm>
# include <stdexcept>

# include "Server.hpp"

class Parser
{
public:
    static std::string remover(const std::string& input, const char rmchar)
    {
        std::string                 ret;
        std::vector<std::string>    buf = split(input, rmchar);
        for (std::vector<std::string>::iterator it = buf.begin(); it != buf.end(); ++it)
        {
            ret += *it;
        }
        return ret;
    }

    static vector<std::string> split(const std::string& input, const char delimeter)
    {
        std::vector<std::string>	ret;
        std::stringstream			ss(input);
        std::string					tmp;

        while (getline(ss, tmp, delimeter))
        {
            ret.push_back(tmp);
    	}

        return ret;
    }

	const std::string getStringBuf() const
	{
		return mStringBuf;
	}

    Parser(const char* filepath)
    {
		std::ifstream ifs(filepath);

		if (!ifs.is_open())
		{
			throw std::runtime_error(filepath);
		}

		std::string tmp;		
		while (std::getline(ifs, tmp, '\n'))
		{
			mStringBuf += tmp;
		}
        mStringBuf = remover(mStringBuf, '\t');

		std::stringstream	ss(mStringBuf);
		char				buf;
		std::vector<std::string>	stk;
		std::string bufferString = "";
		int					cnt = 0;

		while ((buf = ss.get()) != -1)
		{
			if (buf == '{') ++cnt;
			if (buf == '}')
			{
				stk.push_back("");
				while (!bufferString.empty() && bufferString.back() != '{')
				{
					stk.back() += bufferString.back();
					bufferString.pop_back();
				}
				std::reverse(stk.back().begin(), stk.back().end());

				if (bufferString.back() == '{')
				{
					--cnt;
					bufferString.pop_back();
					stk.push_back(bufferString);
					bufferString = "";
				}
				if (!cnt)
				{
					stk.push_back(bufferString);
					bufferString = "";
				}
				/*
				{
					//std::cout << bufferString << std::endl;
					//bufferString = "";
					//std::cout << "========================" << std::endl;
					while (!stk.empty())
					{
						std::cout << stk.back() << std::endl;
						stk.pop_back();
					}
					std::cout << "========================\n" << std::endl;
					stk.clear();
				}
				*/
			}
			else
			{
				bufferString += buf;
			}
		}
		while (!stk.empty())
		{
			std::cout << stk.back() << std::endl;
			stk.pop_back();
		}
		//stk.push_back(bufferString);
		//for (int i = 0; i < stk.size(); ++i) std::cout << stk[i] << std::endl;
		std::cout << std::endl;
    }
protected:
private:

public:
protected:
private:
    std::vector<Server>         mServers;
	std::string					mStringBuf;
};

#endif //   PARSER_HPP
#ifndef SESSION_HPP
# define SESSION_HPP

#include <map>
#include <string>
#include <vector>
#include <time.h>

struct Session
{
	// 리퀘스트 헤더에서 쿠키를 저장한다
    // 해당 쿠키를 키로 해서 값을 찾고, 헤더에 없거나 해당 키로 세션을 찾지못하면 랜덤값 생성해서 set-cookie 로 담기
    // cgi 실행시 세션의 값을 환경변수에 담아서 전달
    // TODO:클라에서 넘어온 값을 해당 세션에 저장
    // TODO:cookie.php을 이용해서 로그인 구현

	std::map<std::string, std::string> Session;
	std::map<std::string, time_t> expireInfo;

	std::string get(std::string key)
	{
		if (Session.count(key) > 0 && expireInfo[key] > time(NULL))
		{
			expireInfo[key] = time(NULL) + SESSION_TIMEOUT;
			return Session[key];
		}
		return "";
		
	}

	std::string set(std::string val)
	{
		std::string key;

		key = gen_random(12);
		Session[key] = val;
		expireInfo[key] = time(NULL) + SESSION_TIMEOUT;

		return key;
	}
 
	std::string gen_random(const int len) const
	{
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		std::string tmp;
		tmp.reserve(len);

		for (int i = 0; i < len; ++i) 
		{
			tmp += alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		return tmp;
	}
};



#endif
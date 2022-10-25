#ifndef FILE_HPP
# define FILE_HPP

# include "Contents.hpp"
# include "../exception/Exception.hpp"


struct File : public Contents
{
	File(const std::string& url, const std::string& body)
	: Contents(url, body) {};
	~File(){};

	std::string	_get();
	void		_post();
	void		_put();
	void		_delete();
};

std::string File::_get()
{
	std::ifstream ifs(mUrl.c_str());
	mContents = "";		//	GET Method일 때 body 무시 -> 컨텐츠 fill

	if (!ifs.is_open())
		throw Code404Exception();
	std::string buf;
	while (getline(ifs, buf, '\n'))
		mContents += buf + "\r\n";
	return mContents;
}

void 		File::_post()
{
	//	TODO : 디렉토리가 없을 시 생성 201?
	std::ofstream ofs(mUrl.c_str());

	if (!ofs.is_open())
		throw Code404Exception();

	ofs.write(mContents.c_str(), mContents.length() - 1);
}
void 		File::_put() {}
void 		File::_delete()
{
	mContents = "";
	if (remove(mUrl.c_str()))
		throw Code404Exception();
}

#endif
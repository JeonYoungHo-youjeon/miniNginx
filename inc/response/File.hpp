#ifndef FILE_HPP
# define FILE_HPP

# include "Contents.hpp"
# include "../exception/Exception.hpp"

struct File : public Contents
{
	//File(Response& res) : Contents(res.path + res.filename, res.body) {
	//	cout << res.path + res.filename << ' ' << endl;
	//};
	File(const std::string& path, const std::string& filename, const std::string& body, const std::vector<std::string>& param)
	: Contents(path + filename, body) {};
	~File(){};

	void		_get();
	void		_post();
	void		_put();
	void		_delete();
};

void File::_get()
{
	std::ifstream ifs(url.c_str());
	contentsBuf = "";		//	GET Method일 때 body 무시 -> 컨텐츠 fill

	code = 200;
	std::cout << url << std::endl;
	if (!ifs.is_open())
	{
		code = 404;
		return;
	}
	std::string buf;
	while (getline(ifs, buf, '\n'))
		contentsBuf += buf + "\r\n";
}

void 		File::_post()
{
	//	TODO : 디렉토리가 없을 시 생성 201?
	std::ofstream ofs(url.c_str());

	code = 200;
	if (!ofs.is_open())
		code = 404;
		//throw Code404Exception();

	ofs.write(contentsBuf.c_str(), contentsBuf.length() - 1);
	if (ofs.fail())
		code = 500;
}
void 		File::_put() {}
void 		File::_delete()
{
	contentsBuf = "";
	code = 200;
	if (remove(url.c_str()))
		code = 404;
}

#endif
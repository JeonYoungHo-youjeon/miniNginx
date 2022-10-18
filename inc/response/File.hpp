#ifndef FILE_HPP
# define FILE_HPP

# include "Contents.hpp"


struct File : public Contents
{
    File(const std::string& url)
    {
        std::ifstream ifs(url);

        if (!ifs.is_open())
        {
            std::cerr << "File Open Error" << std::endl;
            throw std::bad_exception();
        }
        std::string buf;
        
        while (getline(ifs, buf, '\n'))
        {
            mContents += buf;
        }
        std::cout << "content : " << mContents << std::endl;
    };
    ~File(){};
};

#endif
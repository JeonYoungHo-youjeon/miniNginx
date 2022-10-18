#ifndef FILE_HPP
# define FILE_HPP

# include "Contents.hpp"
# include <iostream>
# include <fstream>

struct File : public Contents
{
    File(const std::string& url) {
        std::ifstream ifs(url);

        if (!ifs.is_open())
        {
            std::cerr << "File Open Error" << std::endl;
            exit(1);
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
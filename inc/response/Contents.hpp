#ifndef CONTENTS_HPP
# define CONTENTS_HPP

# include <iostream>
# include <fstream>

struct Contents
{
    Contents(){};
	//Contents(const Contents& contents) : mContents("")
	//{
	//	mContents = contents.mContents;
	//}
    virtual ~Contents(){};
    const std::string	getContents() const;
	std::string			mContents;
};      //  Contents

const std::string	Contents::getContents() const
{
	return mContents;
};

#endif  //  CONTENTS_HPP
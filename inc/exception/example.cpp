#include "Exception.hpp"
#include <iostream>

int main()
{
	try
	{
		throw EventInitException("Occur event init exception, because of me");
	}
	catch(const EventLoopException& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(const EventInitException& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(const Exception& e)
	{
		std::cerr << e.what() << std::endl;
	}


	try
	{
		throw std::runtime_error("Occur runtime error, because of me");
	}
	catch(const EventLoopException& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(const EventInitException& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(const Exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}
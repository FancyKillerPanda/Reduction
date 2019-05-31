#pragma once

#ifdef _DEBUG

#include <iostream>

template<typename... Args>
void info(Args&& ... args)
{
	std::cout << "[INFO] - ";
	(std::cout << ... << args) << std::endl;
}

template<typename... Args>
void warn(Args && ... args)
{
	std::cout << "[WARNING] - ";
	(std::cout << ... << args) << std::endl;
}

template<typename... Args>
void error(Args && ... args)
{
	std::cout << "[ERROR] - ";
	(std::cout << ... << args) << std::endl;
}

#endif  // _DEBUG

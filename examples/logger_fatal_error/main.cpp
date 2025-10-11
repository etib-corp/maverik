/*
** ETIB PROJECT, 2025
** maverik
** File description:
** main
*/

#include "Logger.hpp"

std::shared_ptr<maverik::ALogger> logger = std::make_shared<maverik::vk::Logger>(std::cout, "Maverik", maverik::ALogger::DEV);
const char * BINARY_NAME = nullptr;

void failure()
{
	LOG(maverik::ALogger::FATAL, "This is a fatal error message");
	// LOG(maverik::ALogger::ERROR, "This is an error message");
	// LOG(maverik::ALogger::WARNING, "This is a warning message");
	// LOG(maverik::ALogger::INFO, "This is an info message");
	// LOG(maverik::ALogger::DEBUG, "This is a debug message");
}

class Example {
public:
	void test(int a, int b, Example *example)
	{
		failure();
	}
};

int main()
{
	Example example;
	example.test(1, 2, &example);
	return 0;
}

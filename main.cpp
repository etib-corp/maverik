/*
** ETIB PROJECT, 2025
** maverik
** File description:
** main
*/

#include "Logger.hpp"

std::shared_ptr<maverik::ALogger> logger = std::make_shared<maverik::vk::Logger>(std::cout, "Maverik", maverik::ALogger::DEV);
const char * BINARY_NAME = nullptr;

void totalFailure()
{
	LOG(maverik::ALogger::FATAL, "This is a fatal error message");
}

class Toto {
public:
	void test(int a, int b, Toto *toto)
	{
		totalFailure();
	}
};

int main()
{
	// LOG(maverik::ALogger::FATAL, "This is a fatal message");
	// LOG(maverik::ALogger::ERROR, "This is an error message");
	// LOG(maverik::ALogger::WARNING, "This is a warning message");
	// LOG(maverik::ALogger::INFO, "This is an info message");
	// LOG(maverik::ALogger::DEBUG, "This is a debug message");
	Toto toto;
	toto.test(1, 2, &toto);
	return 0;
}

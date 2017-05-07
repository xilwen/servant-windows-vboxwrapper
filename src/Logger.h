#pragma once
#include <string>
#include <sstream>

enum InfoLevel //Message Output Level
{
    INFO, WARNING, ERR, DEBUG
};

class Logger
{
public:
    Logger();
    static void log(std::string className, std::string func, InfoLevel level, std::string string);
    ~Logger();

private:
    static bool locked;
	static std::stringstream stringstream;
};



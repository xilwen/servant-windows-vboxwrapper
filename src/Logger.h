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
	static void log(std::string className, std::string func, InfoLevel level, std::wstring wstring);
    ~Logger();

private:
    static bool locked;
	static std::stringstream stringstream;
	static std::string fileName;
	static void findAndDelete(std::string& timeString, const std::string& toReplace);
	static std::string getLogFileName();
	static std::string toString(std::wstring in);
};



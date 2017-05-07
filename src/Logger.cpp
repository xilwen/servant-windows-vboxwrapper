#include "Logger.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

bool Logger::locked(false);
std::stringstream Logger::stringstream;

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::log(std::string className, std::string func, InfoLevel level, std::string message)
{
	while (locked)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	locked = true;
	stringstream.str("");
	stringstream << "[";
	switch (level)
	{
	case InfoLevel::INFO:
		stringstream << "Info";
		break;
	case InfoLevel::WARNING:
		stringstream << "Warn";
		break;
	case InfoLevel::ERR:
		stringstream << "Eror";
		break;
	case InfoLevel::DEBUG:
		stringstream << "Dbug";
		break;
	default:
		stringstream << "Unkn";
	}
	stringstream << "] ";

	stringstream << "[" << className << ":" << func << "] " << message << std::endl;
	std::ofstream ofstream;
	ofstream.open("C:\\SERVANT\\1.txt", std::ios::out | std::ios::app);
	ofstream << stringstream.str();
	ofstream.close();
	locked = false;
}
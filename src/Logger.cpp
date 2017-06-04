#include "Logger.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "WindowsUtilities.h"
#include <filesystem>

bool Logger::locked(false);
std::stringstream Logger::stringstream;
std::string Logger::fileName;

Logger::Logger()
{
}

void Logger::findAndDelete(std::string& timeString, const std::string& toReplace)
{
	auto position = timeString.find(toReplace);
	while (position != std::wstring::npos)
	{
		timeString.replace(position, std::string(toReplace).length(), "");
		position = timeString.find(toReplace);
	}
}

Logger::~Logger()
{
}

std::string Logger::getLogFileName()
{
	if(fileName.empty())
	{
		auto timePoint = std::chrono::system_clock::now();
		auto timeT = std::chrono::system_clock::to_time_t(timePoint);
		std::string timeString(ctime(&timeT));

		findAndDelete(timeString, " ");
		findAndDelete(timeString, ":");
		findAndDelete(timeString, "\n");

		if(!std::experimental::filesystem::exists("C:\\SERVANT"))
		{
			std::experimental::filesystem::create_directory("C:\\SERVANT");
		}

		fileName = "C:\\SERVANT\\Wrapper" + timeString + ".log";
	}
	return fileName;
}

std::string Logger::toString(std::wstring in)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	auto string(converter.to_bytes(in));
	return string;
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
	ofstream.open(getLogFileName().c_str(), std::ios::out | std::ios::app);
	if (ofstream.is_open())
	{		
		ofstream << stringstream.str();
		ofstream.close();
	}
	locked = false;
}

void Logger::log(std::string className, std::string func, InfoLevel level, std::wstring wstring)
{
	log(className, func, level, toString(wstring));
}

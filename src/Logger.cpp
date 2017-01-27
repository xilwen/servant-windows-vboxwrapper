#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

bool Logger::locked(false);

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
    std::cout << "[";
    switch (level)
    {
    case InfoLevel::INFO:
        std::cout << "Info";
        break;
    case InfoLevel::WARNING:
        std::cout << "Warn";
        break;
    case InfoLevel::ERR:
        std::cout << "Eror";
        break;
    case InfoLevel::DEBUG:
        std::cout << "Dbug";
        break;
    default:
        std::cout << "Unkn";
    }
    std::cout << "] ";

    std::cout << "[" << className << ":" << func << "] " << message << std::endl;
    locked = false;
}
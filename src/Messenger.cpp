#include "Messenger.h"
#include <sstream>
#include "Logger.h"

Messenger::Messenger(tcp::socket* socket) :socket(socket), server(server)
{
	std::thread(&Messenger::timerRunner, this).detach();
}

Messenger::~Messenger()
{
	doTimeRunner = false;
}

void Messenger::send(std::wstring wstring) const
{
	if (!socket)
	{
		throw std::runtime_error("Send Response failed because there is no socket.");
	}
#ifdef _DEBUG
	if (wstring != L"keepAlive")
	{
		Logger::log("Messenger", __func__, InfoLevel::DEBUG, std::string(wstring.begin(), wstring.end()));
	}
#endif //ifdef _DEBUG
	asio::write(*socket, asio::buffer(wstring));
}

std::wstring Messenger::receive()
{
	if (!socket)
	{
		throw std::runtime_error("Receive command failed because there is no socket.");
	}
	resetIdleTimer();

	std::vector<wchar_t> buf;
	std::wstringstream wstringstream;
	buf.resize(2048);

	auto len = socket->read_some(asio::buffer(buf), lastError);
	for (auto it = buf.begin(); it != (buf.begin() + len); ++it)
	{
		wstringstream << *it;
	}
	auto received(wstringstream.str());
	auto returningWstring(received.substr(0, received.find(L'\0')));
#ifdef _DEBUG
	if (returningWstring != L"keepAlive")
	{
		Logger::log("Messenger", __func__, InfoLevel::DEBUG, std::string(returningWstring.begin(), returningWstring.end()));
	}
#endif //ifdef _DEBUG
	return returningWstring;
}

int Messenger::getIdleTime()
{
	return idleTime;
}

void Messenger::resetIdleTimer()
{
	idleTime = 0;
}

void Messenger::timerRunner()
{
	while (doTimeRunner)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		++idleTime;
	}
}

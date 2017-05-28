#include "Server.h"
#include <string>
#include "Logger.h"
#include "VBoxWrapperService.h"

void Server::init()
{
    Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, "SERVANT VBoxWrapper Server");
    try
    {
        io_service = new asio::io_service;
        acceptor = new tcp::acceptor(*io_service, tcp::endpoint(tcp::v4(), SERVER_TCPV4_PORT));
        socket = nullptr;
    }
    catch (std::exception& e)
    {
        Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, e.what());
        throw;
    }
    Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, "Server Instance Created Successfully.");
}

Server::Server()
{
    init();
}

Server::Server(Interpreter* interpreter)
{
    setInterpreter(interpreter);
    init();
}

void Server::waitForConnect()
{
    std::string output("Waiting for connection on Port ");
    output += std::to_string(SERVER_TCPV4_PORT);
    Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, output);
    try
    {
        if (!socket)
        {
            socket = new tcp::socket(*io_service);
        }
        else
        {
            Logger::log(typeid(*this).name(), __func__, InfoLevel::WARNING, "Using exist socket. Why does it exist?");
        }
        acceptor->accept(*socket);
    }
    catch (std::exception& e)
    {
        Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, e.what());
    }
	catch (...)
	{
		Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, "something went wrong");
	}
    messenger = new Messenger(socket);
    Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, "Client connected.");
}

void Server::handShakeWithClient() const
{
    auto msg(message()->receive());
    std::wstring wstring(L"ready");
    if (msg.compare(wstring) == 0)
    {
        Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, "Handshake received and responsed.");
        message()->send(wstring);
    }
}

void Server::runInterpreterDaemon()
{
    if (!interpreter)
        throw std::runtime_error("Set up interpreter first to run InterpreterDaemon.");
    std::wstring result;
    if (!socket)
    {
        throw std::runtime_error("Interpreter can not start because no socket created.");
    }
    do
    {
        try
        {
            result = interpreter->run(messenger->receive());
            messenger->send(result);
        }
        catch (...)
        {
            Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, "Exception catched. Killing server...");
            restart();
        }
    } while (result != L"exit" && !VBoxWrapperService::isStopping());
}

void Server::setInterpreter(Interpreter* interpreter)
{
    if (interpreter)
        this->interpreter = interpreter;
    else
        throw std::runtime_error("Interpreter has not been created!");
}

Messenger* Server::message() const
{
    return messenger;
}

void Server::restart()
{
    Logger::log(typeid(*this).name(), __func__, InfoLevel::WARNING, "Server is now restarting!");
    uninit();
    init();
    waitForConnect();
    handShakeWithClient();
}


void Server::uninit()
{
	if (messenger)
	{
		delete messenger;
		messenger = nullptr;
	}
	if (acceptor)
	{
		Logger::log(typeid(*this).name(), __func__, InfoLevel::WARNING, "acceptor closing");
		try
		{
			acceptor->close();
		}
		catch(std::exception &e)
		{
			Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, std::string("acceptor closing failed with ") + e.what());
		}
		catch(...)
		{
			Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, "acceptor closing failed, unknown reason");
		}
		Logger::log(typeid(*this).name(), __func__, InfoLevel::WARNING, "acceptor closed");
		delete acceptor;
		acceptor = nullptr;
	}
    if (socket)
    {
		Logger::log(typeid(*this).name(), __func__, InfoLevel::WARNING, "socket shutting down");
		try
		{
			socket->shutdown(asio::ip::tcp::socket::shutdown_both, lastError);
			socket->close();
		}
		catch (std::exception &e)
		{
			Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, std::string("socket closing failed with ") + e.what());
		}
		catch (...)
		{
			Logger::log(typeid(*this).name(), __func__, InfoLevel::ERR, "socket closing failed, unknown reason");
		}
		Logger::log(typeid(*this).name(), __func__, InfoLevel::WARNING, "socket closed");
        delete socket;
		socket = nullptr;
    }
    
	if (io_service)
	{
		io_service->stop();
		delete io_service;
		io_service = nullptr;
	}
    Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, "Server has been turned off.");
}

Server::~Server()
{
    uninit();
}


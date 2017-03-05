#include "Server.h"
#include <string>
#include "Logger.h"

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
    } while (result != L"exit" && messenger->getIdleTime() < 10);
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
    if (io_service)
        delete io_service;
    if (acceptor)
        delete acceptor;
    if (socket)
    {
        socket->shutdown(asio::ip::tcp::socket::shutdown_both, lastError);
        socket->close();
        delete socket;
    }
    if (messenger)
        delete messenger;
    Logger::log(typeid(*this).name(), __func__, InfoLevel::INFO, "Server has been turned off.");
}

Server::~Server()
{
    uninit();
}


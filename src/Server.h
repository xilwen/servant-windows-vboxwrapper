#pragma once

#define SERVER_TCPV4_PORT 9870
#ifndef ASIO_STANDALONE
#pragma warning ("If errors occured in asio, you should define ASIO_STANDALONE and _WIN32_WINNT=0x0501 in compiler's preprocessor settings.")
#endif

#include "asio.hpp"
#include "Messenger.h"
#include "Interpreter.h"

using asio::ip::tcp;

class Messenger;

class Server
{
public:
    Server();
    explicit Server(Interpreter *interpreter);
    ~Server();
    void waitForConnect();
    void handShakeWithClient() const;
    void runInterpreterDaemon();
    void setInterpreter(Interpreter *interpreter);
    Messenger* message() const;
    void restart();
    void uninit();
private:
    asio::io_service *io_service;
    tcp::acceptor *acceptor;
    tcp::socket *socket;
    asio::error_code lastError;
    Messenger *messenger = nullptr;
    Interpreter *interpreter = nullptr;

    void init();
};


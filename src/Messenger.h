#pragma once
#include <string>
#include "Server.h"
#include "asio.hpp"
using asio::ip::tcp;
class Server;

class Messenger
{
public:
    Messenger() = delete;
    explicit Messenger(tcp::socket *socket);
    ~Messenger();
    void send(std::wstring wstring) const;
    std::wstring receive();
private:
    asio::error_code lastError;
    tcp::socket* socket;
    Server* server;
};


#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <iostream>

class Client {
public:
    Client();
    ~Client();
    static std::string sendMessage(std::string message);
    static std::string sendMessageTo(const std::string& IPDirection, int port, std::vector<unsigned char> message);
};

#endif  // CLIENT_H

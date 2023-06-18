#ifndef CHANNEL_H
#define CHANNEL_H

#include <vector>
#include <string>
#include <iostream>
#include "Client.h"
#include "FileManager.cpp"

class Channel {
private:
    int beginListen(std::string IP_Listen,int Port);
public:
    Channel();
    ~Channel();
    int start(std::string IP_Listen,int Port);
};

#endif

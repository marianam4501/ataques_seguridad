#ifndef SENDER_H
#define SENDER_H

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <syslog.h>
#include <iomanip>
#include <sstream>
#include <filesystem> 
#include "Encrypt.h"
#include "FileManager.cpp"
#include "Client.h"

class Sender
{
public:
    Sender();
    ~Sender();

    int start();

private:
    const std::string path;
    const std::string publicKeyPath;
    const std::string privateKeyPath;
    Encrypt encryptObj;
    FileManager FileMng;
};

#endif

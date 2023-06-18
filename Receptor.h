#ifndef RECEPTOR_H
#define RECEPTOR_H

#include <string>
#include <vector>
#include <iostream>
#include "Encrypt.h"
#include "FileManager.cpp"

class Receptor
{
public:
    Receptor();
    ~Receptor();
    int start();


private:
    const std::string publicKeyPath;
    const std::string privateKeyPath;
    Encrypt* encryptObj;
};

#endif  // RECEPTOR_H

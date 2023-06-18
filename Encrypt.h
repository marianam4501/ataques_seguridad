#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <vector>
#include <string>
#include <openssl/rsa.h>

class Encrypt {
private:
    RSA* publicKey;
    RSA* privateKey;

public:
    Encrypt(const std::string& publicKeyPath, const std::string& privateKeyPath);
    ~Encrypt();

    std::vector<unsigned char> encryptText(const std::string& plaintext);
    std::string decryptText(const std::vector<unsigned char>& encryptedText);
};

#endif

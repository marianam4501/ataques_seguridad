#include "Encrypt.h"
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>


Encrypt::Encrypt(const std::string& publicKeyPath, const std::string& privateKeyPath) {
    FILE* publicKeyFile = fopen(publicKeyPath.c_str(), "r");
    FILE* privateKeyFile = fopen(privateKeyPath.c_str(), "r");

    if (!publicKeyFile || !privateKeyFile) {
        std::cerr << "Error al abrir los archivos de clave pública o privada." << std::endl;
        publicKey = nullptr;
        privateKey = nullptr;
    } else {
        publicKey = PEM_read_RSA_PUBKEY(publicKeyFile, nullptr, nullptr, nullptr);
        privateKey = PEM_read_RSAPrivateKey(privateKeyFile, nullptr, nullptr, nullptr);
        fclose(publicKeyFile);
        fclose(privateKeyFile);
    }
}

Encrypt::~Encrypt() {
    if (publicKey) {
        RSA_free(publicKey);
    }
    if (privateKey) {
        RSA_free(privateKey);
    }
}

std::vector<unsigned char> Encrypt::encryptText(const std::string& plaintext) {
    if (!publicKey) {
        std::cerr << "No se ha cargado la clave pública." << std::endl;
        return {};
    }

    std::vector<unsigned char> encryptedText(RSA_size(publicKey));

    int encryptedSize = RSA_public_encrypt(plaintext.size(), reinterpret_cast<const unsigned char*>(plaintext.c_str()),
                                           encryptedText.data(), publicKey, RSA_PKCS1_PADDING);
    if (encryptedSize == -1) {
        std::cerr << "Error encriptando el texto." << std::endl;
        ERR_print_errors_fp(stderr);
        return {};
    }

    encryptedText.resize(encryptedSize);
    return encryptedText;
}

std::string Encrypt::decryptText(const std::vector<unsigned char>& encryptedText) {
    if (!privateKey) {
        std::cerr << "No se ha cargado la clave privada." << std::endl;
        return "";
    }

    std::vector<unsigned char> decryptedText(RSA_size(privateKey));

    int decryptedSize = RSA_private_decrypt(encryptedText.size(), encryptedText.data(),
                                            decryptedText.data(), privateKey, RSA_PKCS1_PADDING);
    if (decryptedSize == -1) {
        std::cerr << "Error desencriptando el texto." << std::endl;
        ERR_print_errors_fp(stderr);
        return "";
    }

    return std::string(decryptedText.begin(), decryptedText.begin() + decryptedSize);
}

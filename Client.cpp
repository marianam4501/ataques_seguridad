#include "Client.h"
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#define PORT 52685

Client::Client()
{
}

Client::~Client()
{
}

std::string Client::sendMessage(std::string message)
{
    int status, valread, client_fd;
    std::string error = "SUCCES";
    struct sockaddr_in serv_addr;
    const char *messageP = message.c_str();
    char buffer[1024] = {0};
    
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error = "\n Socket creation error \n";
        return error;
    }
    
    struct sockaddr_in client;        
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(0);
    if (inet_pton(AF_INET, "192.168.5.33", &client.sin_addr) <= 0) {
      error = "\nInvalid address/ Address not supported \n";
      return error;
    }
    // Bind the socket to the client address
    if (bind(client_fd, (struct sockaddr *)&client, sizeof(client)) < 0) {
      error = "Bind failed\n";
      return error;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "192.168.5.34", &serv_addr.sin_addr) <= 0)
    {
        error = "\nInvalid address/ Address not supported \n";
        return error;
    }
    
    status = connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    

    if ( status < 0)
    {
        error = "\nConnection Failed \n";
        return error;
    }
    
    size_t totalSent = 0;
    size_t messageSize = message.length();

    int maxAttempts = 3;
    int currentAttempt = 0;
    int timeoutSeconds = 5;

    while (currentAttempt < maxAttempts && totalSent < messageSize)
    {
        ssize_t sent = send(client_fd, messageP + totalSent, messageSize - totalSent, 0);
        if (sent == -1){
            error= "\nError sending message\n";
            return error;
        }
        totalSent += sent;
        
        currentAttempt++;
    }

    // closing the connected socket
    close(client_fd);
    return error;
}

std::string Client::sendMessageTo(const std::string& IPDirection, int port, std::vector<unsigned char> message)
{   
    int status, valread, client_fd;
    std::string error = "SUCCES";
    struct sockaddr_in serv_addr;
    const unsigned char* messageP = message.data();
    char buffer[1024] = {0};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error = "\n Socket creation error \n";
        return error;
    }

    struct sockaddr_in client;        
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(0);
    if (inet_pton(AF_INET, "192.168.5.34", &client.sin_addr) <= 0) {
      error =  "\nInvalid address/ Address not supported \n";
      return error;
    }
    // Bind the socket to the client address
    if (bind(client_fd, (struct sockaddr *)&client, sizeof(client)) < 0) {
      error =  "Bind failed\n";
      return error;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "192.168.5.35", &serv_addr.sin_addr) <= 0)
    {
        error = "\nInvalid address/ Address not supported \n";
        return error;
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        error =  "\nConnection Failed \n";
        return error;
    }

    size_t totalSent = 0;
    size_t messageSize = message.size();

    int maxAttempts = 3;
    int currentAttempt = 0;
    int timeoutSeconds = 5;

    while (currentAttempt < maxAttempts && totalSent < messageSize)
    {
        ssize_t sent = send(client_fd, messageP + totalSent, messageSize - totalSent, 0);
        if (sent == -1)
        {
            error =  "\nError sending message\n";
            return error;
        }
        totalSent += sent;

        currentAttempt++;
    }

    // closing the connected socket
    close(client_fd);
    return error;
}

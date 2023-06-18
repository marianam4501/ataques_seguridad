#include "Channel.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <syslog.h>
#include <arpa/inet.h>

#define PORT 52685
using namespace std;

Channel::Channel()
{
}
Channel::~Channel()
{
}

int Channel::start(string IP_Listen,int Port)
{
    int error = 0;
    while (true) {
        error = beginListen(IP_Listen,Port);
        if (error != 0) {
            std::cerr << "Error occurred, restarting the server..." << std::endl;
        } else {
            break;
        }
    }
    return error;
}

int Channel::beginListen(string IP_Listen,int Port)
{
  int server_fd, new_socket, valread;
  string fileLog;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  const char *hello = "Hello from server";
  FileManager FM;
  std::string lastFolderPath = "/home/cesar.lopezurena/CDCD/last/";
  std::string lastFilePath = lastFolderPath + "0000.txt";

  openlog("server", LOG_CONS | LOG_NDELAY, LOG_LOCAL3);
  syslog(LOG_NOTICE, "Server started by User %d", getuid());

  // Crear el socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    std::cerr << "socket failed" << std::endl;
    return -1;
  }

  // Configurar opciones del socket
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    std::cerr << "setsockopt" << std::endl;
    return -1;
  }

  address.sin_family = AF_INET;
  if (inet_pton(AF_INET, IP_Listen.c_str(), &address.sin_addr) <= 0)
  {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return -1;
  }
  address.sin_port = htons(52685);

  // Asociar el socket al puerto
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    std::cerr << "bind failed" << std::endl;
    string fileLog = "bind failed by %s ";
    FM.writeTextFile(lastFilePath,fileLog);
    return -1;
  }

  // Escuchar conexiones entrantes
  if (listen(server_fd, 3) < 0)
  {
    std::cerr << "listen" << std::endl;
    return -1;
  }

  for (;;)
  {
    
    // Registrar inicio en SUCCES
    syslog(LOG_NOTICE, "Channel is listenig!! %s", buffer);
    
    fileLog = "Channel is listening succesfully!!";
    FM.writeTextFile(lastFilePath,fileLog);

    // Aceptar una nueva conexión
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
      std::cerr << "accept" << std::endl;
      return -1;
    }

    // Leer datos recibidos
    valread = read(new_socket, buffer, 1024);

    // Registrar mensaje recibido en el syslog
    syslog(LOG_NOTICE, "Message received by channel: %s", buffer);

    const std::vector<unsigned char> message(buffer, buffer + valread);
    
    fileLog = "Message Receiced by channel %s ";
    FM.writeTextFile(lastFilePath,fileLog);

    std::string errorClient = Client::sendMessageTo(IP_Listen, Port, message);
    
    // Registrar mensaje enviado en el syslog
    syslog(LOG_NOTICE, "Message send it by channel: %s", buffer);

    fileLog = "Message send it by channel %s " + errorClient;
    FM.writeTextFile(lastFilePath,fileLog);

    closelog();

  }
  // Cerrar el socket conectado
  close(new_socket);
  // Cerrar el socket de escucha
  shutdown(server_fd, SHUT_RDWR);

  return 0;
}

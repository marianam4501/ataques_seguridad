#include "Receptor.h"
#include <experimental/filesystem>
#include <filesystem>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <syslog.h>
#include <arpa/inet.h>

#define PORT 52685


Receptor::Receptor() : publicKeyPath("/home/cesar.lopezurena/CDCD/keys/public_key.pem"),
                       privateKeyPath("/home/cesar.lopezurena/CDCD/keys/private_key.pem")
{
  encryptObj = new Encrypt(publicKeyPath, privateKeyPath);
}

Receptor::~Receptor()
{
  delete encryptObj;
}

int Receptor::start()
{
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  const char *hello = "Hello from server";
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

  if (inet_pton(AF_INET, "192.168.5.32", &address.sin_addr) <= 0)
  {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return -1;
  }

  address.sin_port = htons(PORT);

  // Asociar el socket al puerto
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    std::cerr << "bind failed" << std::endl;
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
    // Aceptar una nueva conexión
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
      std::cerr << "accept" << std::endl;
      return -1;
    }
    
    // Leer datos recibidos
    valread = read(new_socket, buffer, 1024);

    // Desencriptar Datos
    const std::vector<unsigned char> encryptedText(buffer, buffer + valread);

    std::string message = encryptObj->decryptText(encryptedText);

    FileManager fs;

    fs.writeTextFile(lastFilePath, message);

    // Registrar mensaje recibido en el syslog
    syslog(LOG_NOTICE, "Message received by server: %s", buffer);

    closelog();

    // Enviar respuesta al cliente
    send(new_socket, hello, strlen(hello), 0);
    
    std::cout << "Responded to client" << std::endl;
  }
  
  // Cerrar el socket conectado
  close(new_socket);
  // Cerrar el socket de escucha
  shutdown(server_fd, SHUT_RDWR);

  return 0;
}

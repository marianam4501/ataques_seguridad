#include "Sender.h"
#include <iomanip>
#include <sstream>
#include <experimental/filesystem>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_set>

namespace fs = std::experimental::filesystem;

Sender::Sender() : path("/home/cesar.lopezurena/CDCD/messages/"),
                   publicKeyPath("/home/cesar.lopezurena/CDCD/keys/public_key.pem"),
                   privateKeyPath("/home/cesar.lopezurena/CDCD/keys/private_key.pem"),
                   encryptObj(publicKeyPath, privateKeyPath)
{
}

Sender::~Sender()
{
}

int Sender::start()
{
    int error = 0;
    std::string lastFolderPath = "/home/cesar.lopezurena/CDCD/last/";
    std::string lastFilePath = lastFolderPath + "0000.txt";
    std::unordered_set<std::string> sentMessages;

    openlog("sender", LOG_CONS | LOG_NDELAY, LOG_LOCAL3);
    syslog(LOG_NOTICE, "Sender started by user %d", getuid());

    std::vector<std::string> scheduledHours = {
        "01:30", "03:30", "05:30", "07:30", "09:30", "11:30", "13:30",
        "15:30", "17:30", "19:30", "21:30", "23:30"};

    try
    {
        while (true)
        {
            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm *currentTime = std::localtime(&now);

            int currentHour = currentTime->tm_hour;
            int currentMinute = currentTime->tm_min;

            bool shouldSend = false;

            for (const auto &scheduledHour : scheduledHours)
            {
                int scheduledHourHour = std::stoi(scheduledHour.substr(0, 2));
                int scheduledHourMinute = std::stoi(scheduledHour.substr(3, 2));

                if (currentHour == scheduledHourHour)
                {
                    int minuteDifference = currentMinute - scheduledHourMinute;
                    if (minuteDifference >= 0 && minuteDifference < 8)
                    {
                        shouldSend = true;
                        break;
                    }
                }
            }

            if (shouldSend)
            {
                bool sentMessage = false;
                bool messageSent = false;

                for (const auto &entry : fs::directory_iterator(path))
                {

                    std::string completePath = path + entry.path().filename().string();
                    std::string message = FileManager::readTextFile(completePath);

                    if (message.size() > 4096)
                    {
                        std::cout << "El mensaje en el archivo '" << completePath << "' excede el tamaño máximo permitido y no puede ser enviado." << std::endl;
                        continue;
                    }

                    if (sentMessages.count(completePath) > 0)
                    {
                        std::cout << "El mensaje en el archivo '" << completePath << "' ya ha sido enviado." << std::endl;
                        continue;
                    }

                    std::vector<unsigned char> encryptedText = encryptObj.encryptText(message);
                    std::string messageToString(encryptedText.begin(), encryptedText.end());

                    if (message.size() != 0)
                    {

                        if (Client::sendMessage(messageToString) == "SUCCES")
                        {
                            syslog(LOG_NOTICE, "Message sent by : %s", messageToString.c_str());
                            sentMessages.insert(completePath);
                            sentMessage = true;
                            messageSent = true;

                            std::string fileName = entry.path().filename().string();
                            std::string newLastFilePath = lastFolderPath + fileName;
                            std::error_code ec;
                            fs::rename(lastFilePath, newLastFilePath, ec);
                            
                            if (!ec)
                            {
                                lastFilePath = newLastFilePath;
                            }
                            else
                            {
                                std::cout << "Error al actualizar el nombre del archivo en la carpeta 'last': " << ec.message() << std::endl;
                            }
                            FileMng.writeTextFile(newLastFilePath, "Este es el ultimo mensaje enviado" + entry.path().filename().string());
                            std::cout << "Esta en horario "<<lastFilePath<< newLastFilePath<< std::endl;

                            break;
                        }
                    }
                }

                if (sentMessage)
                {
                    std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(now) + std::chrono::hours(1));
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(240));
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        error = 1;
    }
    return error;
}

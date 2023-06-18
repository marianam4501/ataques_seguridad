#include <fstream>
#include <string>

using namespace std;

class FileManager
{
public:
    static string readTextFile(const string fileName)
    {
        ifstream inputFile(fileName);
        if (!inputFile.is_open())
        {
            // Handle error: unable to open file
            return "";
        }

        string fileContents;
        string line;
        while (getline(inputFile, line))
        {
            fileContents += line;
            fileContents += "\n";
        }

        inputFile.close();

        return fileContents;
    }

static void writeTextFile(const std::string& filePath, const std::string& message)
{
    std::ofstream outputFile(filePath, std::ios::app); // Abrir el archivo en modo de apertura en el final

    if (outputFile.is_open())
    {
        outputFile << message<< std::endl;
        outputFile.close();
    }
    else
    {
        std::cout << "No se pudo abrir el archivo para escribir: " << filePath << std::endl;
    }
}
};

#include <sys/wait.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;


struct Location
{
    double latitude;
    double longitude;
};



void printChildProcInfo()
{
    std::cerr << "\nFORK SUCCESSFUL..." << std::endl;
    std::cerr << "Child PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent PID: " << getppid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Child Proc User ID (UID): " << getuid() << std::endl;
    std::cerr << "Child Proc Group User ID (GID): " << getgid() << std::endl;
}

std::string Get_Request_URL_String(Location location)
{
    // std::cerr << "\nWeatherAPI generated url: "
    //           << "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True" << std::endl
    //           << std::endl;
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}



int main()
{
    int file_counter = 0;

    std::string filename{"input.txt"};
    std::ifstream inputFile;

    inputFile.open(filename);

    if (!inputFile.is_open())
    {
        throw std::runtime_error("Failed to open the file.");
    }

    std::string line;

    // Loop over each location previously loaded from input.txt
    while (std::getline(inputFile, line))
    {
        Location loc;
        std::istringstream iss(line);
        iss >> loc.latitude >> loc.longitude;

        ++file_counter;

        // fork a child process
        pid_t pid = fork();

        // Error check to make sure the child was successfully created
        if (pid < 0)
        {
            std::cerr << "fork failed\n"; // previously perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            //std::cerr << "Child process, " << getpid() << " is set to work on location: " << iter->latitude << ", " << iter->longitude << std::endl;
            printChildProcInfo();

            std::string url = Get_Request_URL_String(loc);

            std::string filename("file" + std::to_string(file_counter) + ".json");
            std::string curlQuery("-o " + filename);

            // call curl here
            execlp(
                "/usr/bin/curl",
                "curl",
                "-s",
                curlQuery.c_str(),
                url.c_str(),
                NULL);

            exit(0);
        }
        else
        {
            // parent process
            // wait for the child process to exit
            int status;
            if (waitpid(pid, &status, 0) < 0)
            {
                std::cerr << "waitpid failed\n";
                exit(EXIT_FAILURE);
            }
            std::cerr << "\n\nchild process " << pid << " exited with status " << status << "\n\n";
        }
    }
    inputFile.close();

    return 0;
}

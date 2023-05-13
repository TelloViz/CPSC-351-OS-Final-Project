#include "locationreader.h"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include "json.hpp"

using json = nlohmann::json;

void printJSON(std::string filename)
{
    // Open the file
    std::ifstream jsonFile(filename);

    // Could not open the file
    if (!jsonFile.is_open())
    {
        std::cerr << "Could not open file " << filename << std::endl;
        exit(1);
    }

    // Parse the JSON file
    json data = json::parse(jsonFile);

    // Print the information
    std::cerr << "This is the weather for latitude = " << data["latitude"];

    std::cerr << " and longitude = " << data["longitude"]
              << std::endl;

    std::cerr << "----------------------------------------------------------------" << std::endl;

    std::cerr << "Currrent temperature: " << data["current_weather"]["temperature"]
              << std::endl;

    std::cerr << "Current windspeed: " << data["current_weather"]["windspeed"]
              << std::endl;

    std::cerr << "Current Wind direction: " << data["current_weather"]["winddirection"]
              << std::endl;
}

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
    std::cerr << "\nWeatherAPI generated url: "
                << "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True" << std::endl
                << std::endl;
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}

int main()
{
    int file_counter = 0;
    LocationReader reader("input.txt");

    std::vector<Location> locations = reader.readLocations();

    std::cerr << "LocationReader has read " << locations.size() << " locations..." << std::endl;

    for (const auto &loc : locations)
    {
        std::cerr << "Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
    }

    std::cerr << "\nParent PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent User ID (UID): " << getuid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent Group ID (GID): " << getgid() << " reported by: " << getpid() << std::endl;

    auto iter = locations.begin();

    while (iter < locations.end())
    {
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
            std::cerr << "Child process, " << getpid() << " is set to work on location: " << iter->latitude << ", " << iter->longitude << std::endl;
            printChildProcInfo();

            std::string url = Get_Request_URL_String(*iter);

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
            iter++;
        }
    }

    return 0;
}

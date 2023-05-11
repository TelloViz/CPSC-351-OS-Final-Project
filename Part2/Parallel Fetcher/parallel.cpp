#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>

#include "json.hpp"
#include "locationreader.h"

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
    std::cout << "\nFORK SUCCESSFUL..." << std::endl;
    std::cout << "Child PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent PID: " << getppid() << " reported by: " << getpid() << std::endl;
    std::cout << "Child Proc User ID (UID): " << getuid() << std::endl;
    std::cout << "Child Proc Group User ID (GID): " << getgid() << std::endl;
}

void printParentProcInfo()
{
    std::cout << "\nParent PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent User ID (UID): " << getuid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent Group ID (GID): " << getgid() << " reported by: " << getpid() << std::endl;
}

std::string Get_Request_URL_String(Location location)
{
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}

int main()
{
    // Incremental value for file name generation
    int file_counter = 0;

    // Init the locatoin reader
    LocationReader reader("input.txt");

    // Read locations from file and store in vector for iteration
    std::vector<Location> locations = reader.readLocations();

    // Init an iterator to the first location
    auto iter = locations.begin();

    // Stores information about events in the child (e.g., child termination/exit status)
    int childEventInfo;

    // The return value of execlp
    int execlpRetVal;

    // The return value of wait
    int waitReturn;

    // Loop over each location previously loaded from input.txt
    while (iter < locations.end())
    {
        // Increment counter for unique file naming generation.
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
            // Build the query string
            std::string url = Get_Request_URL_String(*iter);
            std::string filename("file" + std::to_string(file_counter) + ".json");
            std::string curlQuery("-o " + filename);

            // Child calls curl with query string
            execlpRetVal = execlp(
                "/usr/bin/curl",
                "curl",
                "-s",
                curlQuery.c_str(),
                url.c_str(),
                NULL);

            // execlp error handling
            if (execlpRetVal < 0)
            {
                perror("execlp");
                exit(1);
            }
            else
            {
                // Child success
                exit(0);
            }
        }
        else
        {
            // Next location, next fork
            iter++; 
        }
    }

    // After iterating through N locations in the previous loop, where it forked N children,
    // the parent loops over wait() until each of the N children has finished its work.
    while (waitReturn = wait(&childEventInfo))
    {
                // Handle wait() errors
        if (waitReturn < 0)
        {
            perror("wait");
            exit(1);
        }
        // If wait() was terminated by a child,
        // check exit code for reason.
        if (WIFEXITED(childEventInfo))
        {
            // Print the terminated child's process id
            fprintf(stderr, "Parent: The child with pid=%d has terminated\n", waitReturn);

            // WEXITSTATUS extracts the child's exit code from the childEventInfo
            fprintf(stderr, "Parent: The child's exit code is %d\n", WEXITSTATUS(childEventInfo));
        }


    }

    return 0;
}

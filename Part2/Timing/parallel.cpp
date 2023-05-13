#include <sys/wait.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

struct Location
{
    double latitude;
    double longitude;
};

std::string Get_Request_URL_String(Location location)
{
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}

int main()
{
    // Incremental value for file name generation
    int file_counter = 0;

    // Stores information about events in the child (e.g., child termination/exit status)
    int childEventInfo;

    // The return value of execlp
    int execlpRetVal;

    // The return value of wait
    int waitReturn;

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
            std::string url = Get_Request_URL_String(loc);
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
    }

    inputFile.close();

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

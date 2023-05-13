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
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}



int main()
{
    // Parent prints its own various ID values
    std::cerr << "\nParent PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent User ID (UID): " << getuid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent Group ID (GID): " << getgid() << " reported by: " << getpid() << std::endl;

    // init file counter for file naming
    int file_counter = 0;

    // init string with filename
    std::string filename{"input.txt"};

    // declare input file stream for reading file
    std::ifstream inputFile;

    // open file @filename w/ input stream
    inputFile.open(filename);

    // catch file open failures
    if (!inputFile.is_open())
    {
        throw std::runtime_error("Failed to open the file.");
    }

    // declare string for holding each line
    std::string line;

    // Loop over each location previously loaded from input.txt
    while (std::getline(inputFile, line))
    {
        // temp location object for each loop
        Location loc;

        // string stream for parsing each line
        std::istringstream iss(line);

        // parsing the line into its component parts
        iss >> loc.latitude >> loc.longitude;

        // increment location counter read from file
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
            // child reports its ID values
            printChildProcInfo();

            // build URL for query
            std::string url = Get_Request_URL_String(loc);

            // build filename using incremented counter
            std::string filename("file" + std::to_string(file_counter) + ".json");

            // prep our curl arg
            std::string curlQuery("-o " + filename);
            
            execlp(                     // call curl with url, filename, and curlQuery
                "/usr/bin/curl",        // curl system directory location
                "curl",                 // curl command
                "-s",                   // silence extra output from curl
                curlQuery.c_str(),      // prebuilt query string
                url.c_str(),            // prebuilt url string
                NULL                    // end of args
                );

            exit(0);                    // child exit success
        }
        else // parent process
        {
            // var to hold status of child when finished
            int status;
            
            // wait for the child process to exit
            // catch any potential error
            if (waitpid(pid, &status, 0) < 0)
            {
                std::cerr << "waitpid failed\n";
                exit(EXIT_FAILURE);
            }

            // report child exit status
            std::cerr << "\n\nchild process " << pid << " exited with status " << status << "\n\n";
        }
    }

    // close input file
    inputFile.close();

    return 0;
}

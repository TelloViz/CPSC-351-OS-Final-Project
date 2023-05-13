#include <sys/wait.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

// convenience type for dealing with geocoordinate locations
struct Location
{
    double latitude;
    double longitude;
};

// convenience func for child to print its ID values
void printChildProcInfo()
{
    std::cerr << "\nFORK SUCCESSFUL..." << std::endl;
    std::cerr << "Child PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent PID: " << getppid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Child Proc User ID (UID): " << getuid() << std::endl;
    std::cerr << "Child Proc Group User ID (GID): " << getgid() << std::endl;
}

// convenience func for Parent to print its ID values
void printParentProcInfo()
{
    std::cerr << "\nParent PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent User ID (UID): " << getuid() << " reported by: " << getpid() << std::endl;
    std::cerr << "Parent Group ID (GID): " << getgid() << " reported by: " << getpid() << std::endl;
}

// Convenience function for building the query URL from given location.
std::string Get_Request_URL_String(Location location)
{
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}

int main()
{
    printParentProcInfo();                                // Parent prints its own various ID values
    
    int numLocations = 0;                                 // counter for file naming
    
    std::string filename{"input.txt"};                    // string for filename

    std::ifstream inputFile;                              // input file stream for reading file

    inputFile.open(filename);                             // open file @filename w/ input stream

    if (!inputFile.is_open())                             // catch file open failures
    {
        throw std::runtime_error(
            "Failed to open the file."
            );
    }
    
    std::string line;                                     // string to hold read lines
    
    while (std::getline(inputFile, line))                 // Loop over each location in file
    {
        Location loc;                                     // location read from file
        
        std::istringstream iss(line);                     // string stream to parse lines
        
        iss >> loc.latitude >> loc.longitude;             // parse the line 
        
        ++numLocations;                                   // increase location counter

        pid_t pid = fork();                               // fork a child process
                                            
        if (pid < 0)                                      // Error check to make sure the child was successfully created
        {
            std::cerr << "fork failed\n"; 
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)                                // Enter if child
        {
            printChildProcInfo();                         // child reports its ID values

            std::string url =                              // build URL for query
                Get_Request_URL_String(loc);
            
            std::string filename(                          // build filename using incremented counter     
                "file" + 
                std::to_string(numLocations) + 
                ".json"
                );

            std::string curlQuery("-o " + filename);       // prep our curl arg            
            
            int execlpRetVal =                             // returned value of execlp
                                execlp(                    // call curl with url, filename, and curlQuery
                                "/usr/bin/curl",           // curl system directory location
                                "curl",                    // curl command
                                "-s",                      // silence extra output from curl
                                curlQuery.c_str(),         // prebuilt query string
                                url.c_str(),               // prebuilt url string
                                NULL                       // end of args
                                    );


            if (execlpRetVal < 0)                          // execlp error handling
            {
                perror("execlp");
                exit(1);
            }
            else                                           // Child success
            {
                exit(0);
            }
        }
        else                                              // parent process
        {
            int status;                                   // child exit status
            
            if (waitpid(pid, &status, 0) < 0)             // catch wait errors
            {
                std::cerr << "waitpid failed\n";
                exit(EXIT_FAILURE);
            }

            std::cerr << "\n\nchild process "             // report child exit status
                << pid << " exited with status " 
                << status << "\n\n";
        }
    }

    inputFile.close();                                   // close input file

    return 0;
}

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

// Convenience function for building the query URL from given location.
std::string Get_Request_URL_String(Location location)
{
    return "https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(location.latitude) + "&longitude=" + std::to_string(location.longitude) + "&current_weather=True";
}

int main()
{
    
    int childEventInfo;                                   // child termination/exit status)
    
    int execlpRetVal;                                     // The return value of execlp
    
    int waitReturn;                                       // The return value of wait

    int numLocations = 0;                                 // counter for file naming
    
    std::string filename{"input.txt"};                    // string for filename

    std::ifstream inputFile;                              // input file stream for reading file

    inputFile.open(filename);                             // open file @filename w/ input stream

    if (!inputFile.is_open())                             // catch file open failures
    {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return 1;
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
        }
    }

    inputFile.close();                                    // close input file
   
    do                                                    // parent loops on wait
    {   
        waitReturn = wait(&childEventInfo);
        if (waitReturn < 0                               // if there's a wait error
            && errno != ECHILD)                          // and its not a "No More Children" error...
        {
            perror("Waiting error!");                    // signal the error
            exit(1);                                     // terminate with error code
        }
        else if(waitReturn < 0 && errno == ECHILD) std::cerr << "All children have been waited for...\n";
        
        if (errno != ECHILD && WIFEXITED(childEventInfo))                    // If child terminated normally
        {
            // Print the terminated child's process id
            fprintf(stderr, "Parent: The child with pid=%d has terminated\n", waitReturn);

            // WEXITSTATUS extracts the child's exit code from the childEventInfo
            fprintf(stderr, "Parent: The child's exit code is %d\n", WEXITSTATUS(childEventInfo));
        }

    }while (waitReturn > 0);

    return 0;
}

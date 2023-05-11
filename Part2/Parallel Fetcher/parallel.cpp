#include "locationreader.h"
#include "weatherapi.h"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include "../../samples/json.hpp"

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

int main()
{
    int file_counter = 0;
    LocationReader reader("input.txt");

    std::vector<Location> locations = reader.readLocations();

    std::cout << "LocationReader has read " << locations.size() << " locations..." << std::endl;

    for (const auto &loc : locations)
    {
        std::cout << "Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
    }


// // Forking N children in a row as per assignment specification.
//     int numForks = locations.size();
//     int currentForkCount = 0;
//     std::vector<pid_t> pidVec;

//     while( currentForkCount < numForks  )
//     {        
//         pid_t curr = fork();
//         if(curr < 0) 
//         {
//             std::cerr << "fork failed\n";
//             exit(EXIT_FAILURE);
//         }
//         else if(getpid() == 0) break;
//         else 
//         {
//             pidVec.push_back(curr);
//             ++currentForkCount;
//         }
//     }
    

    auto iter = locations.begin();
    // Stores information about events in the child (e.g., child termination/exit status)
    int childEventInfo;	
    
    // The return value of execlp
    int execlpRetVal;

    // The return value of wait
    int waitReturn; 
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
            //std::cout << "Child process, " << getpid() << " is set to work on location: " << iter->latitude << ", " << iter->longitude << std::endl;

            std::string url = WeatherAPI::Get_Request_URL_String(*iter);
            std::string filename("file" + std::to_string(file_counter) + ".json");
            std::string curlQuery("-o " + filename);

            //std::cout << "Resulting in curl query: " << curlQuery << std::endl;

            // call curl here
            execlpRetVal = execlp(
                "/usr/bin/curl",
                "curl",
                curlQuery.c_str(),
                url.c_str(),
                NULL);

            // This code is unreachable unless execlp has failed 
            if(execlpRetVal < 0)
            {
                perror("execlp");
                exit(1);
            }
            else exit(0);
        }
        else
        {
            // // parent process
            // // wait for the child process to exit
            // int status;
            // if (waitpid(pid, &status, 0) < 0)
            // {
            //     std::cerr << "waitpid failed\n";
            //     exit(EXIT_FAILURE);
            // }
            // std::cout << "\n\nchild process " << pid << " exited with status " << status << std::endl;




            iter++;
        }
    }

    while(waitReturn = wait(&childEventInfo))
    {
        		// Sanity checks -- wait failed?
		if(waitReturn < 0)
		{
			perror("wait");
			exit(1);
		}

        // If the reason why wait has stopped waiting
		// was child termination (could be other reasns
		// in practice!), let's print the child's exit status
		// WIFEXITED macro checks the bits in the childEventInfo
		// to see if the reason why the wait has stopped waiting
		// was child termination
		if(WIFEXITED(childEventInfo))
		{
			// Print the terminated child's process id
			fprintf(stderr, "Parent: The child with pid=%d has terminated\n", waitReturn);
			
			// WEXITSTATUS extracts the child's exit code from the childEventInfo
			fprintf(stderr, "Parent: The child's exit code is %d\n", WEXITSTATUS(childEventInfo));	
		}
    }

    return 0;
}

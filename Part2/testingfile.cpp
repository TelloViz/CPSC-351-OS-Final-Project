#include "locationreader.h"
#include "weatherapi.h"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include "../samples/json.hpp"

using json = nlohmann::json;

void printJSON(std::string filename)
{
    	// Open the file
	std::ifstream jsonFile(filename);
			
	// Could not open the file
	if(!jsonFile.is_open())
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


int main() {
    int file_counter = 0;
     LocationReader reader("input.txt");

     std::vector<Location> locations = reader.readLocations();

     std::cout << "LocationReader has read " << locations.size() << " locations..." << std::endl;

     for (const auto& loc : locations) {
         std::cout << "Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
     }

    std::cout << "\nParent PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent User ID (UID): " << getuid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent Group ID (GID): " << getgid() << " reported by: " << getpid() << std::endl;

auto iter = locations.begin();



while(iter < locations.end()) {
    ++file_counter;
    // fork a child process
    pid_t pid = fork();			
    
    //Error check to make sure the child was successfully created 
    if (pid < 0) 
    {
        std::cerr << "fork failed\n"; // previously perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) 
    {
        /*** TODO: If I am child, I will do this: ****/
    /* Call execlp() to replace my program with that specified at the command line.
        * PLEASE NOTE: YOU CANNOT PASS cmdBuff DIRECTLY to execlp(). It is because 
        * cmdBuff is an object of type string (i.e., a class) and execlp() expects
        * an array of characters.  However, you can pass cmdBuff.c_str(), which will
        * return an array of characters representation of the string object.
        * 
        * Also, please do not forget to error check your exelp() system calls.
        */

        std::cout << "\nFORK SUCCESSFUL..." << std::endl;
        std::cout << "Child PID: " << getpid() << " reported by: " << getpid() << std::endl;
        std::cout << "Parent PID: " << getppid()<< " reported by: " << getpid() << std::endl;
        std::cout << "Child Proc User ID (UID): " << getuid() << std::endl;
        std::cout << "Child Proc Group User ID (GID): " << getgid() << std::endl;
        std::cout << "Child process, " << getpid() << " is set to work on location: " << iter->latitude << ", " << iter->longitude << std::endl;

        std::string url = WeatherAPI::Get_Request_URL_String(*iter);

        std::string filename("file" + std::to_string(file_counter) + ".json"); 
        std::string curlQuery("-o " + filename);

        
        std::cout << "Resulting in curl query: " << curlQuery << std::endl;

        // call curl here
        execlp(
            "/usr/bin/curl",
            "curl",
            curlQuery.c_str(),
            url.c_str(),
            NULL);
        
        exit(0);


    } 
    
    /*** TODO: If I am a parent, I will do the following ***?
    /* Wait for the child process to terminate */
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
        std::cout << "\n\nchild process exited with status " << status << std::endl;
        iter++;

    }
}

    return 0;
}

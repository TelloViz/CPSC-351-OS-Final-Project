#include "locationreader.h"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    LocationReader reader("input.txt");
    std::vector<Location> locations = reader.readLocations();
    std::cout << "LocationReader has read " << locations.size() << " locations..." << std::endl;

    for (const auto& loc : locations) {
        std::cout << "Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
    }

/* TODO: Create a child */
    // fork a child process
    pid_t pid = fork();			
    
    /* TODO: Error check to make sure the child was successfully created */
    if (pid < 0) 
    {
        std::cerr << "fork failed\n"; // previously perror("fork");
        exit(EXIT_FAILURE);
    }

    /*** TODO: If I am child, I will do this: ****/
    /* Call execlp() to replace my program with that specified at the command line.
        * PLEASE NOTE: YOU CANNOT PASS cmdBuff DIRECTLY to execlp(). It is because 
        * cmdBuff is an object of type string (i.e., a class) and execlp() expects
        * an array of characters.  However, you can pass cmdBuff.c_str(), which will
        * return an array of characters representation of the string object.
        * 
        * Also, please do not forget to error check your exelp() system calls.
        */
    else if (pid == 0) 
    {        
        for (const auto& coord : locations) 
        {
            // cycles through pairs of lat/long previously read from input.txt
        }
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
        std::cout << "child process exited with status " << status << std::endl;
    }

    return 0;
}

#include "locationreader.h"
#include "weatherapi.h"
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{

    LocationReader reader("input.txt");

    std::vector<Location> locations = reader.readLocations();

    std::cout << "LocationReader has read " << locations.size() << " locations..." << std::endl;

    for (const auto &loc : locations)
    {
        std::cout << "Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
    }

    std::cout << "\nParent PID: " << getpid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent User ID (UID): " << getuid() << " reported by: " << getpid() << std::endl;
    std::cout << "Parent Group ID (GID): " << getgid() << " reported by: " << getpid() << std::endl;

    auto iter = locations.begin();

    while (iter < locations.end())
    {

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
            std::cout << "Parent PID: " << getppid() << " reported by: " << getpid() << std::endl;
            std::cout << "Child Proc User ID (UID): " << getuid() << std::endl;
            std::cout << "Child Proc Group User ID (GID): " << getgid() << std::endl;
            std::cout << "Child process, " << getpid() << " is set to work on location: " << iter->latitude << ", " << iter->longitude << std::endl;

            auto url = WeatherAPI::Get_Request_URL_String(*iter);

            // call curl here
            execlp(
                "/usr/bin/curl",
                "curl",
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
            std::cout << "child process exited with status " << status << std::endl;
            iter++;
        }
    }

    return 0;
}

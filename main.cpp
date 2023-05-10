#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>   // for fork(), execvp()
#include <sys/wait.h> // for waitpid()

int main()
{
    while (true)
    {
        std::string command;
        std::cout << "cmd>";
        std::getline(std::cin, command);

        // check if the user wants to exit the shell
        if (command == "exit")
        {
            break;
        }

        // fork a child process
        pid_t pid = fork();

        if (pid < 0)
        {
            std::cerr << "fork failed\n";
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        { // child process
            // tokenize the command
            std::vector<std::string> tokens;
            std::istringstream iss(command);
            std::string token;
            while (iss >> token)
            {
                tokens.push_back(token);
            }
            // convert the vector of strings to an array of char* (required by execvp)
            std::vector<char *> args;
            for (const auto &token : tokens)
            {
                args.push_back(const_cast<char *>(token.c_str()));
            }
            args.push_back(nullptr); // add a null terminator

            // execute the command using execvp
            if (execvp(args[0], args.data()) < 0)
            {
                std::cerr << "execvp failed\n";
                exit(EXIT_FAILURE);
            }
        }
        else
        { // parent process
            // wait for the child process to exit
            int status;
            if (waitpid(pid, &status, 0) < 0)
            {
                std::cerr << "waitpid failed\n";
                exit(EXIT_FAILURE);
            }
            std::cout << "child process exited with status " << status << std::endl;
        }
    }
    return 0;
}

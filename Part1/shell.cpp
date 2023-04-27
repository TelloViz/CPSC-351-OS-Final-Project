#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;


int main()
{
	/* The command buffer */
	string cmdBuff;
	
	/* The ID of the child process */
	pid_t pid;
	
	/* Keep running until the user has typed "exit" */
	do 
	{
		/* Prompt the user to enter the command */
		std::cout << "cmd>";
		if (!std::getline(std::cin, cmdBuff)) 
		{
			// Pressing ctrl-d while program waiting for input
			// is an easy way to test this control path.
			std::cerr << "Error reading input from user\n";
			exit(EXIT_FAILURE);
		}

		
		/* If the user wants to exit */
		if(cmdBuff != "exit")
		{
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
			{ // child process
				// tokenize the command
				std::vector<std::string> tokens;
				std::istringstream iss(cmdBuff);
				std::string token;
				while (iss >> token) 
				{
					tokens.push_back(token);
            	}
				// convert the vector of strings to an array of char* (required by execvp)
            	std::vector<char*> args;
            	
				for (const auto& token : tokens) 
				{
                	args.push_back(const_cast<char*>(token.c_str()));
            	}
            	args.push_back(nullptr); // add a null terminator
            
				// execute the command using execvp
				if (execvp(args[0], args.data()) < 0) 
				{
					std::cerr << "execvp failed\n";
					exit(EXIT_FAILURE);
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
		}
	}
	while(cmdBuff != "exit");
	
	return 0;	
}

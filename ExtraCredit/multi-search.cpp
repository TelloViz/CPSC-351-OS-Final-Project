// CPSC 351 : OS Extra Credit Project
// Written by Joshua Lollis
// Last updated 5/14/23

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>


//***************************************************************************
// bool readString(const std::string &filename, std::string &fillString)
//***************************************************************************
// Reads text from a file, fills the provided fillString with
// said text, and then returns a success/failure boolean flag.
//------------------------------------------------------------------
// param: filename : std::string 
//      The filename of the file containing the text to be searched.
// param: fillString : std::string&
//      The container in which to fill the loaded text string.
// return: bool stating success/failure to load text from file. 
bool readString(const std::string &filename, std::string &fillString)
{
    // Open the file
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to open the file." << std::endl;
        return false;
    }

    // Read the file contents into a string
    fillString = std::string((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    // Close the file
    file.close();
    return true;
}


//***************************************************************************
// std::vector<std::string> splitString(const std::string &str, int N)
//***************************************************************************
// Splits a std::string into N substrings, then returns a vector
// containing the substrings.
//------------------------------------------------------------------
// param: str : const std::string & 
//      The target string to split into N substrings.
// param: N : int
//      The amount of substrings to subdivide the string into.
// return: std::vector<std::string> containing the substrings. 
std::vector<std::string> splitString(const std::string &str, int N)
{
    std::vector<std::string> result;
    int length = str.length();
    int substringLength = length / N;
    int remainingCharacters = length % N;
    int startIndex = 0;

    for (int i = 0; i < N; i++)
    {
        int currentSubstringLength = substringLength + (i < remainingCharacters ? 1 : 0);
        result.push_back(str.substr(startIndex, currentSubstringLength));
        startIndex += currentSubstringLength;
    }

    return result;
}



//***************************************************************************
// int searchSubstring(const std::string &substring, const std::string &key)
//***************************************************************************
// Searches substring for key, returning a status code integer.
//---------------------------------------------------------------------------
// param: substring : const std::string & 
//      The substring to search for key
// param: key : const std::string&
//      The string we are looking for in the substring.
// return: integer meaning: (0) not found, (1) found
// Note: Integer is returned as a convenience to the way the return value is intended to be used.
//  example: exit(<returnValue>);
int searchSubstring(const std::string &substring, const std::string &key)
{
    if (substring.find(key) != std::string::npos) // check for substring "key". npos is a type safe std::string const like size_t or pid_t
    {
        return 0; // Found the string
    }
    else
    {
        return 1; // String not found
    }
}


//***************************************************************************
// void ParentAnnounce(pid_t childPid, const std::vector<std::string>& strings, const std::string& key)
//***************************************************************************
// Convenience function for code modularization that allows parent to announce
// who is being forked, what they are searching for, and what they are searching through.
//---------------------------------------------------------------------------
// param: childPid : pid_t
//      PID of the child that announcement pertains to.
// param: str : const std::string&
//      The substring that the child is searching through.
// param: key : const std::string&
//      The string that the child is searching for.
// param: i : int
//      Index of the child that is doing the searching (Which child in our N children is this?)
void ParentAnnounce(pid_t childPid, const std::string& str, const std::string& key, int i)
{
    std::cout << "Parent: Forked process " << i + 1                 // Announce which child was forked. (DEBUG)
              << "(pid:" << childPid << ")"
              << std::endl;

    std::cout << "Parent: " << childPid << " searches \"";          // Announce the substring being searched. (DEBUG)

    std::string convertedStr;                                       // Temp string var for pretty print-ability. (DEBUG)
    for (auto ch : str)                                      // Loop over chars in current substring. (DEBUG)
    {
        if (ch == '\n')                                             // If '\n' 
        {
            convertedStr.push_back('\\');                           // Then replace with a visible equivalent "\n".
            convertedStr.push_back('n');
        }
        else if (ch == ' ')                                         // If ' '
        {
            convertedStr.push_back('_');                            // Then replace with a visible equivalent "_".
        }
        else
            convertedStr.push_back(ch);                             // Otherwise remain unchanged.
    }

    std::cout << convertedStr                                       // Finish reporting child source and target strings.
                << "\" for " << key << "\n\n";
}



int main(int argc, char *argv[])
{
    if (argc != 4)                                                          // Force proper CL Arg count
    {
        std::cerr << "Usage: "                                              // Display proper usage.
                  << argv[0]
                  << " <FILE NAME> <KEY> <NUMBER OF PROCESSES>\n";
        return 1;                                                           // End program so user can try again
    }

    std::string fileName = argv[1];                                         // Init fileName from filename argv[1]
    
    std::string key = argv[2];                                              // Init search keyword from argv[2]

    int numProcesses;                                                       // Declare int for holding CLI argv[3]

    try
    {
        numProcesses = std::stoi(argv[3]);                                  // Init num forked procs from argv[3]
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() 
        << " Make sure argument is convertable to an integer type.\n ";     // if std.stoi fails, maybe argv[3] not numeric.
        
        return 1;
    }

    std::string fillString;                                                 // Prep string for reading call.
    readString(fileName, fillString);                                       // Read the string from file.

    std::vector<std::string>
        strings(splitString(fillString, numProcesses));                     // Instnatiate vec with parsed string.

    std::vector<pid_t> childPids(numProcesses);                             // Vec for caching child PID values.

    std::cout << "\n";                                                      // Formatting newline

    for (int i = 0; i < numProcesses; ++i)                                  // Loop numProcesses times.
    {
        pid_t childPid = fork();                                            // Fork a child, cache its pid.

        if (childPid == -1)                                                 // Catch forking errors.
        {
            perror("fork");
            return 1;                                                       // Abort on first failed fork.
        }
        else if (childPid == 0)                                             // Child process
        {
            exit(searchSubstring(strings[i], key));                         // Exit with our found status.
        }
        else                                                                // Parent process
        {
            ParentAnnounce(childPid,strings[i], key, i);                    // Make some announcements

            childPids[i] = childPid;                                        // Parent cache current child pid
        }
    }

    bool foundString = false;                                               // Flag to hold our found/not found status.
    pid_t childFound_pid;                                                   // PID of the successful child. (if any)
    int waitReturn;                                                         // integer to hold wait status code value.
    int childEventInfo;                                                     // Integer to hold child exit status value.
    std::cout << "\n";                                                      // Formatting output.

    do                                                                      // Parent loops on wait.
    {
        waitReturn = wait(&childEventInfo);                                 // Wait and cache wait status.
        // Handle wait() errors
        if (waitReturn < 0 && errno != ECHILD)                              // If there's a wait error (not last child)
        {
            perror("wait");
            exit(1);
        }
        else if (WIFEXITED(childEventInfo)                                  // If child exits properly:
                && errno != ECHILD)                                         // And it's not simply "Last Child"
        {                                                                   // Then:
            fprintf(                                                        // Print the terminated child's process id
                stderr, 
                "Parent: Child w/ pid=%d has terminated\n",                 // Report which child exited.
                waitReturn
                );

            // WEXITSTATUS 
            fprintf(
                stderr, 
                "Parent: The child's exit code is %d\n\n",                  // Report which child has status.
                WEXITSTATUS(childEventInfo)                                 // Extract the child's exit code from childEventInfo.
                );

            if (WEXITSTATUS(childEventInfo) == 0)                           // If child exited with a (0) status:
            {
                foundString = true;                                         // Then set foundString flag to true; 
                childFound_pid = waitReturn;                                // Cache id of child who found the string.
            }
        }
    } while (waitReturn > 0);                                               // Keep looping as long as we have more children to wait for.

    if (foundString)                                                        // If key was found in string data:
    {                                                                       // Then:
        std::cout << "\nParent: String found by PID: "                      // Report which child found it.
                  << childFound_pid << "!\n";
    }
    else
    {
        std::cout << "\nParent: No string found\n";                         // Otherwise report no successes.
    }

    std::cout << "\n";                                                      // Output formatting.
    
    for (pid_t pid : childPids)                                             // Loop over all previously forked children.
    {
        kill(pid, SIGKILL);                                                 // Terminate all children
        std::cout << "Parent: "                                             // Report which children are being killed. 
                  << pid 
                  << " killed...\n";
    }

    return 0;
}

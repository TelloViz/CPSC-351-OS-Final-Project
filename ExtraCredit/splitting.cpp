#include <iostream>
#include <fstream>
#include <string>
#include <vector>


bool readString(const std::string& filename, std::string& fillString)
    {
        // Open the file
        std::ifstream file(filename);
        if (!file) {
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

int main()
{
    std::string filename{"input.txt"};
    int N = 7;

    std::string inputString;
    readString(filename, inputString);

    std::vector<std::string> substrings = splitString(inputString, N);

    for (const auto &substring : substrings)
    {
        std::cout << substring << std::endl;
    }

    if (N > inputString.length())
        std::cout << "N > input.length()..." << std::endl;
    return 0;
}




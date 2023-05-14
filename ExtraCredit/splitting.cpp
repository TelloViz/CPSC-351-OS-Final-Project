#include <iostream>
#include <string>
#include <vector>

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
    std::string input = "Hello, world!";
    int N = 7;

    std::vector<std::string> substrings = splitString(input, N);

    for (const auto &substring : substrings)
    {
        std::cout << substring << std::endl;
    }

    if (N > input.length())
        std::cout << "N > input.length()..." << std::endl;
    return 0;
}

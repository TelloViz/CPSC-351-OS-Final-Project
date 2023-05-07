#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


struct Location {
    double latitude;
    double longitude;
};

class LocationReader {
public:
    LocationReader(const std::string& filename) : filename_(filename) {}

    std::vector<Location> readLocations() {
        std::vector<Location> locations;
        std::ifstream input(filename_);

        if (!input) {
            std::cerr << "Error: Unable to open file " << filename_ << std::endl;
            return locations;
        }

        std::string line;
        while (std::getline(input, line)) {
            Location loc;
            std::istringstream iss(line);
            iss >> loc.latitude >> loc.longitude;
            locations.push_back(loc);
        }

        input.close();
        return locations;
    }

private:
    std::string filename_;
};

#include "locationreader.h"

int main() {
    LocationReader reader("input.txt");
    std::vector<Location> locations = reader.readLocations();

    for (const auto& loc : locations) {
        std::cout << "Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
    }

    return 0;
}

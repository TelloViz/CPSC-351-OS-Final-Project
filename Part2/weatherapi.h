#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include "location.h"

#define MAX_BUF_SIZE 200

class WeatherAPI 
{

public:

    static std::string Get_Request_URL_String(Location location) 
        {
            char finalURLBuff[MAX_BUF_SIZE];
            snprintf(
                finalURLBuff, 
                MAX_BUF_SIZE, 
                "https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current_weather=True", 
                location.latitude, 
                location.longitude
                );
            return finalURLBuff;
        }

    using LocationIterator = std::vector<Location>::const_iterator;
    
    WeatherAPI(const std::vector<Location>& locations) : m_locations(locations) 
    {
        current_loc = m_locations.begin();
    }

    LocationIterator NextLocation()
    {
        if(current_loc == m_locations.end()) return m_locations.end();
        else return ++current_loc;
    }

private:
    std::string BuildRequestURL(Location location) const
    {
        return Get_Request_URL_String(location);
    }

    std::vector<Location> m_locations;
    LocationIterator current_loc;

};

/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
For a 2D grid of N x M point charges all with the same
charge in the x-y plane centered around the origin,
this code asks the user for the point at which the electric field due 
to the grid points should be calculated and interrogates
the input string for valid input.
*/


#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>
#include "Point.h"


//Takes in string input passed by reference and checks if it's a valid x,y,z location and returns true if it is
bool isValidLocation(const std::string& str)
{
    bool decimalFound = false;
    bool isNegative = false;

    // Allow for negative values
    size_t startPlace = 0;
    if (str[0] == '-')
    {
        isNegative = true;
        startPlace = 1;
    }

    //Loop through characters to see if they are digits and there can only be one decimal
    for (size_t i = startPlace; i < str.size(); ++i)
    {
        char c = str[i];
        if (std::isdigit(c))
        {
            continue;
        }
        else if (c == '.' && !decimalFound)
        {
            decimalFound = true;
        }
        else {
            return false;  // Invalid
        }
    }

    //A single negative sign is not valid
    if (isNegative && startPlace == 1 && str.size() == 1)
    {

        return false;
    }

    return true;  // Valid 
}

//Inputs are x,y,z location of point passed in by reference and a 2D grid containing all the point charges
//Output is true if the location is valid numerical value and is not at the same location as an existing point charge
bool getLocationInSpace(double& xLocationInput, double& yLocationInput, double& zLocationInput, const std::vector<std::vector<Point>>& result) {
    while (true)
    {
        std::string input;
        std::cout << std::endl << "Please enter the location in space to determine the electric field (x y z) in meters: ";
        std::getline(std::cin, input);

        std::istringstream iss(input);
        if (iss >> xLocationInput >> yLocationInput >> zLocationInput)
        {
            std::string xStr = std::to_string(xLocationInput);
            std::string yStr = std::to_string(yLocationInput);
            std::string zStr = std::to_string(zLocationInput);

            if (isValidLocation(xStr) && isValidLocation(yStr) && isValidLocation(zStr))
            {
                // Check for non numeric characters
                std::string remainder;
                if (!(iss >> remainder))
                {
                    // Check if the point matches an existing point charge
                    bool invalidLocation = false;
                    for (const auto& row : result)
                    {
                        for (const Point& point : row)
                        {
                            if (point.xCreate == xLocationInput && point.yCreate == yLocationInput && point.zCreate == zLocationInput)
                            {
                                std::cout << "Invalid location! Location matches a point charge position!" << std::endl << "Invalid input! Please enter three numerical values separated by spaces." << std::endl;
                                invalidLocation = true;
                                break;
                            }
                        }
                        if (invalidLocation)
                        {
                            break;
                        }
                    }
                    if (invalidLocation)
                    {
                        continue;  // If invalid, user has to enter point again
                    }
                    return true;  // Valid 
                }
            }
        }

        std::cout << "Invalid input! Please enter three numerical values separated by spaces." << std::endl;
    }

    return false;
}

/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
For a 2D grid of N x M point charges all with the same
charge in the x-y plane centered around the origin,
this code takes a user string input for the x and y 
separation values of the 2D grid and interrogates the 
input for valid values. 
*/


#include <iostream>
#include <string>
#include <cctype>

// Function to check if a string contains only numerical values. Takes string passed by reference
// Returns true if valid 
bool isValidNumericValue(const std::string& str)
{
    bool decimalFound = false;

    for (char c : str)
    {
        if (std::isdigit(c))
        {
            continue; //Check to see if everything is a digit
        }
        else if (c == '.' && !decimalFound)
        {
            decimalFound = true;  // Check to see that there is only up to one decimal
        }
        else {
            return false;  // Invalid 
        }
    }

    return true;  // Valid
}

// Function to get "x" and "y" separation distances from user input. Takes x and y separation variables passed by reference
// Returns true for successfull entry 
bool getSeparationValues(float& xSeparation, float& ySeparation)
{
    std::string input;

    while (true)
    {
        std::cout << "Please enter the x and y separation distances in meters: ";
        std::getline(std::cin, input);

        // Find the space delimiter
        size_t spacePos = input.find(' ');

        //Make sure space isn't first or last

        if (spacePos != std::string::npos && spacePos > 0 && spacePos < input.length() - 1)
        {
            // Split the input into two parts using space
            std::string firstPart = input.substr(0, spacePos);
            std::string secondPart = input.substr(spacePos + 1);

            // Check if both parts are valid numbers
            if (isValidNumericValue(firstPart) && isValidNumericValue(secondPart))
            {
                // Convert the numbers to double
                xSeparation = std::stod(firstPart);
                ySeparation = std::stod(secondPart);

                // Valid input 
                return true;
            }
        }

        std::cout << "Invalid input! Please enter two numerical values separated by a space." << std::endl;
    }

    return false;
}
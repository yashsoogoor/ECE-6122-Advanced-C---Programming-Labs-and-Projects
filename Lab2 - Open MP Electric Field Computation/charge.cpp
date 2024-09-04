/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
For a 2D grid of N x M point charges all with the same
charge in the x-y plane centered around the origin,
this code asks for a user input string for a charge for all
the grid points and interrogates the string to make sure
it's a valid charge.
*/


#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

//Checks to see if input string is a valid charge by seeing if it can be made a double and is not zero
//Takes string and charge passed in by reference and returns true for valid charge
bool isValidCharge(const std::string& str, double& charge)
{
    if (str.empty())
    {
        return false;
    }

    // Attempt to convert the string to a double
    std::istringstream iss(str);
    iss >> std::noskipws >> charge;

    if (!iss.fail() && iss.eof())
    {
        // Makes sure value is no 0 
        if (charge != 0.0)
        {
            return true; // Valid 
        }
    }

    return false; // Invalid
}

//Obtains input for charge and takes input passed in by reference and checks if valid

void getCharge(double& charge)
{
    while (true)
    {
        std::string input;
        std::cout << "Please enter the common charge on the points in micro C: ";
        std::getline(std::cin, input);

        if (isValidCharge(input, charge))
        {
            break; // Valid input, exit the loop
        }
        else
        {
            std::cout << "Invalid input! Enter a valid non-zero numerical charge value." << std::endl;
        }

        std::cin.clear();
    }
}
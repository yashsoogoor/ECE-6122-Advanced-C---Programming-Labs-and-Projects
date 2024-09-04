/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
This file contains code to take a user string input
and interrogate whether it is a valid input for
the number of rows and columns for a 2D grid.
*/


#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

// Checks to see if string input is a natural number and returns true if it is 
bool isNaturalNumber(const std::string& str)
{
    for (char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}

//Gets N and M inputs from user and takes N,M passed by reference and returns true if valid input
bool getArray(int& N, int& M)
{
    std::string input;

    while (true)
    {
        std::cout << "Please enter the number of rows and columns in the N x M array: ";
        std::getline(std::cin, input);

        //Look thru the input to find N,M inputs
        std::istringstream iss(input);
        std::string rowN, colM;

        // Try to extract N and M from the input using space delimiter and see if they are natural numbers 
        if (iss >> rowN >> colM && isNaturalNumber(rowN) && isNaturalNumber(colM) && iss.eof())
        {
            N = std::stoi(rowN);
            M = std::stoi(colM);

            if (N > 0 && M > 0)
            {
                return true;
            }
        }

        std::cout << "Invalid input! Try again." << std::endl;

        // Clear input
        std::cin.clear();
    }
    return false;
}

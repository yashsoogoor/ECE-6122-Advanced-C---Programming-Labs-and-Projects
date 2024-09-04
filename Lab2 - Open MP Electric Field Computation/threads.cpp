/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 10/03/2023
Description :
This code takes in a user input for the number of threads
to be used in multithreading for calculating electric field
at a user specified point. The user input string is 
interrogated for validity. 
*/


#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

//Checks to see if input string is a valid number for threads greater than zero
//Takes string and number passed in by reference and returns true for valid number of threads
bool isValidNumber(const std::string& str, int& number)
{
    if (str.empty())
    {
        return false;
    }

    // See if you can convert the string to an integer
    std::istringstream iss(str);
    iss >> std::noskipws >> number;

    if (!iss.fail() && iss.eof() && number > 0)
    {
        return true; // Valid number
    }

    return false; // Invalid
}


//Obtains input for number of threads and takes input passed in by reference and checks if valid

void getThreads(int& threads)
{
    while (true)
    {
        std::string input;
        std::cout << "Please enter the number of concurrent threads to use: ";
        std::getline(std::cin, input);

        if (isValidNumber(input, threads))
        {
            break; // Valid input, exit the loop
        }
        else
        {
            std::cout << "Invalid input! Enter a valid number of threads to use." << std::endl;
        }

        std::cin.clear();
    }
}
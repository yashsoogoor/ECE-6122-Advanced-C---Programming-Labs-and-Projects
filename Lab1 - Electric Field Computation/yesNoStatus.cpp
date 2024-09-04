/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
For a 2D grid of N x M point charges all with the same
charge in the x-y plane centered around the origin,
this code computes the magnitude and direction of the
electric field at a specified point (x,y,z) due to all
the charges.
*/


#include <iostream>

#include <iostream>
#include <limits>


//Function to see if the input is a valid Y or N, returns 1 if user input is Y, 0 if input is N
//Prompts again if invalid 
bool getStatus() {
    char user_input;
    bool invalid_input = false;  // initialize invalid input as false

    while (true) {
        if (invalid_input) 
        {
            std::cout << "Invalid input. Please enter 'Y' or 'N'." << std::endl;
            invalid_input = false;  // Reset the flag
        }

        std::cout << "Do you want to enter a new location (Y/N)? ";
        std::cin >> user_input;

        // Clear input incase of multiple character inputs 
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        //Check to see if there is one Y or one N input, not case sensitive

        if ((user_input == 'Y' || user_input == 'y') && std::cin.gcount() == 1) 
        {
            return 1;  // valid 
        }
        else if ((user_input == 'N' || user_input == 'n') && std::cin.gcount() == 1) 
        {
            return 0;  // valid 
        }
        else 
        {
            invalid_input = true;  // Set the flag for invalid input
        }
    }
}


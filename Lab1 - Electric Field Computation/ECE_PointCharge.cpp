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
#include "ECE_PointCharge.h"
using namespace std;

//Constructor 
ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q)
    : x(x), y(y), z(z), q(q) {}


// Function to set the location of the point charge taking in x, y, z values
void ECE_PointCharge::setLocation(double x_new, double y_new, double z_new) 
{
    x = x_new;
    y = y_new;
    z = z_new;
}

// Function to set the charge of the point charge taking in charge value
void ECE_PointCharge::setCharge(double q_new) 
{
    q = q_new;
}




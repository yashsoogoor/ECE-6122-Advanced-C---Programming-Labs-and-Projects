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


#pragma once

//Create class to hold x,y,z values as well as charge at a point in a grid
//and member functions to set location and charge

class ECE_PointCharge
{
protected:
	double x;
	double y;
	double z;
	double q;
public:
	ECE_PointCharge(double x, double y, double z, double q);
	void setLocation(double x_new, double y_new, double z_new);
	void setCharge(double q_new);
};
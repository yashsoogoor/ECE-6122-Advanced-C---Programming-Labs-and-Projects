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
#include "ECE_PointCharge.h"

//Create class to hold electric field component values and member functions to compute field at a given point
class ECE_ElectricField: public ECE_PointCharge
{
protected:
	double Ex;
	double Ey;
	double Ez;
public:
	ECE_ElectricField(double x, double y, double z, double q);
	void computeFieldAt(double xLocation, double yLocation, double zLocation);
	void getElectricField(double& Ex_field, double& Ey_field, double& Ez_field);
};
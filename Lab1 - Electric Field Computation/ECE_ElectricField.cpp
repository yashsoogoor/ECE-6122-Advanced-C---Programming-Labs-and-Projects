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
#include "ECE_ElectricField.h"
#include "ECE_PointCharge.h"
#include <cmath>
#include <mutex>
using namespace std;

std::mutex mutex_;

//inherits from ECE_PointCharge in this constructor 

ECE_ElectricField::ECE_ElectricField(double x, double y, double z, double q)
	: ECE_PointCharge(x, y, z, q) {}

//Takes in x,y,z location of point for which electric field should be calculated
//Computes each component of electric field held in atomic 

void ECE_ElectricField::computeFieldAt(double xLocation, double yLocation, double zLocation)
{
	double k = 9000000000;

	double dx = xLocation - x;
	double dy = yLocation - y;
	double dz = zLocation - z;
	double dxSquared = pow(dx, 2.0);
	double dySquared = pow(dy, 2.0);
	double dzSquared = pow(dz, 2.0);
	double sumSquares = dxSquared + dySquared + dzSquared;
	double denominator = pow(sumSquares, 1.5);
	double q_units = q * pow(10, -6);

	// Protect access to Ex, Ey, and Ez with a mutex
	std::lock_guard<std::mutex> lock(mutex_);

	Ex = k * q_units * dx / denominator;
	Ey = k * q_units * dy / denominator;
	Ez = k * q_units * dz / denominator;
}

//Takes in x,y,z components of electric field passed in by reference and updates them, using lock to do so
void ECE_ElectricField::getElectricField(double& Ex_field, double& Ey_field, double& Ez_field)
{
	// Protect access to Ex, Ey, and Ez using mutex
	std::lock_guard<std::mutex> lock(mutex_);

	Ex_field = Ex;
	Ey_field = Ey;
	Ez_field = Ez;
}






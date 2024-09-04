/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
For a 2D grid of N x M point charges all with the same
charge in the x-y plane centered around the origin,
this code contains helper functions for 
taking in user input parameters to
compute the magnitude and direction of the
electric field at a specified point (x,y,z) due to all
the charges
*/


#pragma once
#include <string>
#include <vector>
#include "Point.h"

//Stores helper functions for obtaining user inputs and checking for valid inputs

using namespace std;

bool getArray(int& N, int& M);

bool isNaturalNumber(const std::string& str);

bool isValidFloat(const std::string& str);

bool getSeparationValues(float& xSeparation, float& ySeparation);

void getCharge(double& charge);

bool isValidCharge(const std::string& str, double& charge);

bool isValidLocation(const std::string& str);

bool getLocationInSpace(double& xLocation, double& yLocation, double& zLocation, const std::vector<std::vector<Point>>& result);

bool getStatus();

void getThreads(int& threads); 










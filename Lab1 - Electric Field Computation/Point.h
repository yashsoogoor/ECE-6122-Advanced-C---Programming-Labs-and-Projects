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

#include <vector>

//Create class to hold values in a 2D grid

class Point {
public:
    double xCreate;
    double yCreate;
    double zCreate;
    double qCreate;

    Point(double xCreate, double yCreate, double zCreate, double qCreate)
        : xCreate(xCreate), yCreate(yCreate), zCreate(zCreate), qCreate(qCreate) {}
};

std::vector<std::vector<Point>> generatePoints(int N, int M, double x, double y, double q);


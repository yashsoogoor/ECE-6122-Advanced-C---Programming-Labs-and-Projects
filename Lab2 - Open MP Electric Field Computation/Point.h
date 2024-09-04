/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 09/25/2023
Description :
This code creates a Point class for a point
object to hold x,y,z, and charge values
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


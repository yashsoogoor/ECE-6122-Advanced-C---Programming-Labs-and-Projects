/*
Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 10/03/2023
Description :
For a 2D grid of N x M point charges all with the same
charge in the x-y plane centered around the origin,
this code computes the magnitude and direction of the
electric field at a specified point (x,y,z) due to all
the charges by using Open MP multithreading.
*/



#include "helpers.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cmath>
#include <stdio.h>
#include <omp.h>
#include "Point.h"

using namespace std;


std::atomic<double> xLocation, yLocation, zLocation;


// Function generates an N X M array of points by taking in inputs for N rows, M columns,
// x-separation distance, y-separation distance, and a charge for every point.
// A 2D grid is made to be centered at the origin by using nested for loops and the points vector
// is initialized and returned by using the Point class which holds x,y,z,q values for each point. 

std::vector<std::vector<Point>> generatePoints(int N, int M, double x, double y, double q) {
    // The points vector is created for each Point object in the 2D grid
    std::vector<std::vector<Point>> points(N, std::vector<Point>(M, Point(0.0, 0.0, 0.0, 0.0)));

    // Calculate the starting point
    double start_x = -(M - 1) * x / 2.0;
    double start_y = -(N - 1) * y / 2.0;

    // Generate points
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            points[i][j].xCreate = start_x + j * x;
            points[i][j].yCreate = start_y + i * y;
            points[i][j].zCreate = 0;
            points[i][j].qCreate = q;
        }
    }

    return points;
}

//The main function continuously prompts the user for a point at which to calculate the electric field
//as long as they wish to continue and calculates the electric field at that point due to all the points
//in a 2D grid created based on parameters specified by the user. 

int main() {
    int N, M;
    float xSeparation, ySeparation;
    double charge;
    int threads;

    //Call these functions to get user inputs for number of threads to use, N, M, x separation distance, y separation distance, and charge

    getThreads(threads);

    getArray(N, M);

    getSeparationValues(xSeparation, ySeparation);

    getCharge(charge);

    // Generate the point grid based on the user inputs

    std::vector<std::vector<Point>> result = generatePoints(N, M, xSeparation, ySeparation, charge);



    double xLocationInput;
    double yLocationInput;
    double zLocationInput;


    std::ostringstream ss;

    //Keep track of whether user has entered yes or no to continue; do the loop while status is true 
    bool status = 1;

    do
    {
        //Call function to have user input the point at which to calculate electric field

        getLocationInSpace(xLocationInput, yLocationInput, zLocationInput, result);


        //Store user input for point 

        xLocation = xLocationInput;  //.store(xLocationInput, std::memory_order::memory_order_seq_cst);
        yLocation = yLocationInput; //.store(yLocationInput, std::memory_order::memory_order_seq_cst);
        zLocation = zLocationInput; // .store(zLocationInput, std::memory_order::memory_order_seq_cst);

        //Initialize the electric field totals in each direction and the magnitude
        //Use an open MP for loop to iterate through the points in the 2D array and add up the electric field contributions 
        double Ex_total(0.0), Ey_total(0.0), Ez_total(0.0), magnitudeField(0.0);
        //Initialize start and stop times for clock
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point stop_time;

        //Create omp parallelization threads by using the user input for number of threads
#pragma omp parallel num_threads(threads) 
        {

            //Use omp master directive to start clock timer
#pragma omp barrier
            {

                start_time = std::chrono::high_resolution_clock::now();
            }


            //Use omp for directive to go thru the 2d grid using the created threads

#pragma omp for reduction(+:Ex_total,Ey_total,Ez_total) collapse(2) nowait 


// For loop to iterate through each point object in the result 2d array created earlier and calculate and sum
// up the individual contributions to the total electric field

            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < M; j++)
                {
                    const Point& point = result[i][j]; // Get the point ojbect from result

                    double k = 9000000000;

                    double dx = xLocation - point.xCreate;
                    double dy = yLocation - point.yCreate;
                    double dz = zLocation - point.zCreate;
                    double dxSquared = pow(dx, 2.0);
                    double dySquared = pow(dy, 2.0);
                    double dzSquared = pow(dz, 2.0);
                    double sumSquares = dxSquared + dySquared + dzSquared;
                    double denominator = pow(sumSquares, 1.5);
                    double q_units = charge * pow(10, -6);


                    double Ex = k * q_units * dx / denominator;
                    double Ey = k * q_units * dy / denominator;
                    double Ez = k * q_units * dz / denominator;

                    // Update the Ex_total, Ey_total, and Ez_total within this loop



                    Ex_total += Ex;

                    Ey_total += Ey;

                    Ez_total += Ez;


                }
            }

#pragma omp barrier
            {

                // Record stop using master directive
                stop_time = std::chrono::high_resolution_clock::now();
            }


        }



        // Calculate how long it took
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);



        std::cout << "The electric field at (" << xLocation << ", " << yLocation << ", " << zLocation << ") in V/m is" << std::endl;
        std::cout << "Ex = " << std::scientific << Ex_total << std::endl;
        std::cout << "Ey = " << std::scientific << Ey_total << std::endl;
        std::cout << "Ez = " << std::scientific << Ez_total << std::endl;
        magnitudeField = pow(Ex_total * Ex_total + Ey_total * Ey_total + Ez_total * Ez_total, 0.5);
        std::cout << "|E| = " << std::scientific << magnitudeField << std::endl;
        std::cout << std::defaultfloat;
        std::cout << "The calculation took " << duration.count() << " microsec!" << std::endl;


        //Check to see if user wants to enter another point
        //If so, tell threads to wait, otherwise tell them to exit
        status = getStatus();
        if (!status)
        {
            break;
        }

    } while (status);


    std::cout << "Bye!" << std::endl;



    return 0;
}

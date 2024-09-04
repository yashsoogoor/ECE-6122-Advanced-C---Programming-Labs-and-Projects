/*
Author: Yashwanth Soogoor
Class: ECE6122 Section A
Last Date Modified: 11/21/2023
Description:
This file uses Open MPI and the Monte Carlo method to estimate one of two integrals based on the user input commands to choose an integral and enter
the number of random samples to use in the Monte Carlo method. 
*/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <cstring>

#include "mpi.h"

using namespace std;

//This is the function for the first integral which takes in a double value of x and outputs a double value. The function is x^2
double function1(double x) 
{
    return x * x;
}

//This is the function for the second integral which takes in a double value of x and outputs a double value. The function is exp(-x^2)
double function2(double x) 
{
    return exp(-x * x);
}

//This main function has a return type of int. It takes in the command line number of arguments and an array representing the arguments.
//It uses Open MPI and the MonteCarlo method to calculate one of two integers based on the user input and number of samples chosen by the user. 

int main(int argc, char** argv) 
{
    int numtasks, rank, rc;

    rc = MPI_Init(&argc, &argv);

    //Initialize MPI
    if (rc != MPI_SUCCESS) 
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    //Calculate the number of processors and assign rank
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Initialize integral choice and number of samples
    int integralChoice = -1;
    int totalSamples = 0;

    //In the root process, the command line user entry can be parsed to get the user input for integral to calculate and number ofo samples

    if (rank == 0) 
    {
        for (int i = 1; i < argc; i += 2) 
        {
            if (strcmp(argv[i], "-P") == 0) 
            {
                integralChoice = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-N") == 0) 
            {
                totalSamples = atoi(argv[i + 1]);
            }
        }
    }

    int samplesPerProcessor = totalSamples / numtasks;
    //Commented out some debug output statements
    //std::cout << "total samples: " << totalSamples << std::endl; 
    //std::cout << "numtasks: " << numtasks << std::endl;
    int remainingSamples = totalSamples % numtasks;

    //Broadcast the choice of integral and samples per processor to all processors 
    MPI_Bcast(&integralChoice, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&samplesPerProcessor, 1, MPI_INT, 0, MPI_COMM_WORLD);

 
    //This makes sure all the processors use different seeds
    srand48(time(NULL) + rank * 100 + 10);

    //Initialize the integral summation 
    double integralSum = 0.0;

    //Each processor computes the portion of the integral sum for its number of samples and keeps a running sum 
    for (int i = 0; i < samplesPerProcessor; ++i) 
    {
        double randomSample = drand48();
        double integralValue = 0.0;

        if (integralChoice == 1) 
        {
            integralValue = function1(randomSample);
        }
        else if (integralChoice == 2) 
        {
            integralValue = function2(randomSample);
        }

        integralSum += integralValue;
    }

    double localResult = integralSum;
    //Commented out debug statement
    //std::cout << "local result: " << localResult << "and rank number: " << rank << std::endl;

    
    double globalResult = 0.0;
    //Send the results back to the root and sum them up in the globalResult variable
    MPI_Reduce(&localResult, &globalResult, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    //Commented out debug statement
    //std::cout << "global result: " << globalResult << std::endl;

    //Do the summation for the remaining samples in the root processor
    if (rank == 0) 
    {
        //limits of integration
        double a = 0.0; 
        double b = 1.0; 

        double integralSumCore = 0.0; 
        for (int i = 0; i < remainingSamples; ++i) 
        {
            double randomSample = drand48();
            double integralValueCore = 0.0;

            if (integralChoice == 1) 
            {
                integralValueCore = function1(randomSample);
            }
            else if (integralChoice == 2) 
            {
                integralValueCore = function2(randomSample);
            }

            integralSumCore += integralValueCore;
        }

        double localResultCore = integralSumCore;

        //Combine all the integral sums, multiply by the range of integration, and divide by the total number of samples
        double finalResult = (b - a) * (globalResult + localResultCore) / (totalSamples);


        //Output the result based on the integral chosen
        if (integralChoice == 1) 
        {
            cout << "The estimate for integral 1 is " << finalResult << endl;
        }
        else if (integralChoice == 2) 
        {
            cout << "The estimate for integral 2 is " << finalResult << endl;
        }
        cout << "Bye!" << endl;
    }

    //end MPI
    MPI_Finalize();
}

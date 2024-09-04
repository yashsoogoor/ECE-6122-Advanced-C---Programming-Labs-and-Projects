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



#include "helpers.h"
#include "ECE_ElectricField.h"
#include "ECE_PointCharge.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <cmath>
//#include < iomanip >
#include "Point.h"

using namespace std;


//initialize necessary mutexes and atomics and status variables to keep track of multithreading
enum enStatus { eWaiting, eRun, eFinished, eExit };

std::atomic<int> threadsStatus[100];
std::mutex mtxCout;
std::mutex totalMutex;
std::mutex cvMutex; 
std::mutex mut;
const int numThreads = 5;
std::atomic<bool> shouldExit[numThreads];
std::atomic<int> test; 
std::atomic<double> xLocation, yLocation, zLocation;
std::atomic<double> Ex_total, Ey_total, Ez_total;
std::atomic<double> magnitudeField;




//std::mutex mtx; 
/*
void writeToConsole(const std::string& strOut)
{
    std::unique_lock<std::mutex> lck1(mtxCout);

    std::cout << strOut;

}
*/



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

//This thread function creates all the worker threads by taking in the thread id and a vector 
//of the points that the particular thread is responsible for in the electric field calculations. 
//The function constantly checks the status of the threads and when active the thread performs its
//portion of the calculations by calling the functions from the ECE_ElectricField class. 

void threadFunction(const std::vector<Point> pointsToProcess, int id) 
{   

    std::ostringstream ss;
    while (true)
    {
        // Wait until signaled to start
        while (threadsStatus[id] == eWaiting)
        {
            std::this_thread::yield();
        }  

        /*
        // Perform some calculation
        ss << "Thread " << id << " is working...\n";
        writeToConsole(ss.str());
        ss.str(""); ss.clear();
       // std::cout << std::endl << Ex_total << std::endl;
       */
    
        //This loop does the calculations by extracting the point values and calling 
        //the member functions of ECE_ElectricField. A mutex is used while the electric
        //field totals  are updated constantly by each thread. 

        for (const Point& point : pointsToProcess)
        {
           
            std::unique_lock<std::mutex> lck2(mtxCout);
            double Ex_field;
            double Ey_field;
            double Ez_field;
            double currentX = xLocation.load(std::memory_order::memory_order_seq_cst); 
            double currentY = yLocation.load(std::memory_order::memory_order_seq_cst);
            double currentZ = zLocation.load(std::memory_order::memory_order_seq_cst);
            ECE_ElectricField ECE_ElectricFieldInstance(point.xCreate, point.yCreate, point.zCreate, point.qCreate);
            ECE_ElectricFieldInstance.computeFieldAt(currentX, currentY, currentZ);
            ECE_ElectricFieldInstance.getElectricField(Ex_field, Ey_field, Ez_field);
         
            

            {
                std::lock_guard<std::mutex> lock(totalMutex);
                Ex_total = Ex_total + Ex_field;
                Ey_total = Ey_total + Ey_field;
                Ez_total = Ez_total + Ez_field; 
            }
           
        }

        /*
        ss << "Thread " << id << " finished work.\n";
        writeToConsole(ss.str());
        ss.str(""); ss.clear();
        */
         

        threadsStatus[id] = eFinished;

       

        // Wait until signaled what to do next
        while (threadsStatus[id] == eFinished)
        {
            std::this_thread::yield();
        }

        if (threadsStatus[id] == eExit)
        {
            break;
        }
    }
    /*
    ss << "Thread " << id << " terminating.\n";
    writeToConsole(ss.str());
    ss.str(""); ss.clear();
    */
}

int main() {
    int N, M;
    float xSeparation, ySeparation;
    double charge;

    //Check the number of allowed threads

    unsigned int numThreads = std::thread::hardware_concurrency();
    std::cout << "Your computer supports " << numThreads << " concurrent threads." << std::endl;

    //Call these functions to get user inputs for N, M, x separation distance, y separation distance, and charge

    getArray(N, M);

    getSeparationValues(xSeparation, ySeparation);

    getCharge(charge);

    // Generate the point grid based on the user inputs

    std::vector<std::vector<Point>> result = generatePoints(N, M, xSeparation, ySeparation, charge);


    int totalPoints = N * M;
    int pointsPerThread = totalPoints / numThreads;
    int remainingPoints = totalPoints % numThreads;
    int startIndex = 0;
    bool AllFinished;
    double xLocationInput;
    double yLocationInput;
    double zLocationInput;

    //Create the threads to do the calculations based on the number of points and threads 

    std::vector<std::thread> threads;

    //Set the threads to wait at the beginning

    for (int i = 0; i < numThreads; ++i) 
    {
        threadsStatus[i] = eWaiting;
    }

    //Split the point grid among the threads by calculating points per thread and the remainder and indexing

    for (int i = 0; i < numThreads; i++) 
    {
        int endIndex = startIndex + pointsPerThread + (i < remainingPoints ? 1 : 0);
        std::vector<Point> pointsToProcess;

        for (int j = startIndex; j < endIndex; j++) 
        {
            int row = j / M;
            int col = j % M;
            pointsToProcess.push_back(result[row][col]);
        }

        //Send each thread a vector of the points that it is responsible for

        threads.push_back(std::thread(threadFunction, pointsToProcess, i));

        startIndex = endIndex;
    }

    std::ostringstream ss;

    //Keep track of whether user has entered yes or no to continue; do the loop while status is true 
    bool status = 1;

    do
    {   
        //writeToConsole(ss.str());
       // ss.str(""); ss.clear();

        //Call function to have user input the point at which to calculate electric field

        getLocationInSpace(xLocationInput, yLocationInput, zLocationInput, result);

        // Record start time

        auto start_time = std::chrono::high_resolution_clock::now();

        //Store user input for point 

        xLocation.store(xLocationInput, std::memory_order::memory_order_seq_cst);
        yLocation.store(yLocationInput, std::memory_order::memory_order_seq_cst);
        zLocation.store(zLocationInput, std::memory_order::memory_order_seq_cst);

        //Tell threads to run

        for (int j = 0; j < numThreads; ++j)
        {
            threadsStatus[j] = eRun;
        }


        // Wait for the calculations to finish
        do
        {
            AllFinished = true;
            for (int j = 0; j < numThreads; ++j)
            {
                if (threadsStatus[j] != eFinished)
                {
                    AllFinished = false;
                    break;
                }
            }


        } while (!AllFinished); 


        // Record stop
        auto stop_time = std::chrono::high_resolution_clock::now();

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
        if (status)
        {
            for (int j = 0; j < numThreads; ++j)
            {
                threadsStatus[j] = eWaiting;
            }
        }
        else
        {
            // Tell threads to exit
            for (int j = 0; j < numThreads; ++j)
            {
                threadsStatus[j] = eExit;
            }
        }
    } while (status);

    //Join threads
    for (auto& thread : threads) 
    {
        thread.join();
    }

    std::cout << "Bye!" << std::endl; 

    
    
    return 0;
}
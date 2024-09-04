/*
Author: Yashwanth Soogoor
Class: ECE6122 Section A
Last Date Modified: 11/7/2023
Description:
This file implements a CUDA random walk by talking in the number of walkers and number of steps using three different memory models: pageable, pinned, and managed.
For each model, it calculates the average distance of the walkers from the starting position and the time to calculate in microseconds. 
*/




#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <curand_kernel.h>
#include <string>
#include <cuda.h>
#include <cuda_runtime.h>

// Function to interrogate command line arguments
bool interrogateCommandLine(int commandLineLength, char* commandLineArguments[], uint64_t& walkers, uint64_t& steps);

// Function to validate numerical input
bool validInput(const std::string& input, uint64_t& value);

// CUDA kernel to calculate random walk using curand for random direction generation
// Updates the positions of the walkers and takes in the number of walkers and number of steps and seed for random direction generation
__global__ void randomWalk(uint64_t* position, uint64_t walkers, uint64_t steps, unsigned long long seed) 
{
    int walkerId = blockIdx.x * blockDim.x + threadIdx.x;
    if (walkerId < walkers) 
    {
        int x = 0, y = 0;

        // Start curand for random direction generation
        curandState state;
        curand_init(seed, walkerId, 0, &state);

        for (uint64_t step = 0; step < steps; step++) 
        {
            // Generate a random float between 0 and 1
            float randomValue = curand_uniform(&state);

            // Determine the direction that the walker goes next according to the random float
            if (randomValue < 0.25f) x--; // Left
            else if (randomValue < 0.5f) x++; // Right
            else if (randomValue < 0.75f) y++; // Up
            else y--; // Down
        }

        //update x and y values of position
        position[walkerId * 2] = x;
        position[walkerId * 2 + 1] = y;
    }
}


//The main function returns 0 on finishing and exits on 1 if there is error or invalid entries by taking in the command line length and arguments
int main(int commandLineLength, char* commandLineArguments[]) 
{

    uint64_t walkers;
    uint64_t steps;   

    // Interrogate command-line for valid arguments and update the number of walkers/steps if entered, otherwise use defaults 
    if (!interrogateCommandLine(commandLineLength, commandLineArguments, walkers, steps)) 
    {
        return 1; // Exit if not valid command line entries 
    }



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //Pageable Warm-Up; Used to warm-up 

    // Memory allocation on host

    uint64_t* positionHostPageablewarmup;
    positionHostPageablewarmup = (uint64_t*)malloc(2 * walkers * sizeof(uint64_t));

    // Initialize host position array
    memset(positionHostPageablewarmup, 0, 2 * walkers * sizeof(uint64_t));

    // Memory allocation on device
    uint64_t* positionPageablewarmup = nullptr;
    cudaMalloc((void**)&positionPageablewarmup, 2 * walkers * sizeof(uint64_t));


    // Tell kernel to do random walk

    int numThreadswarmup = 256;
    int numBlockswarmup = (walkers + numThreadswarmup - 1) / numThreadswarmup;
    randomWalk << <numBlockswarmup, numThreadswarmup >> > (positionPageablewarmup, walkers, steps, time(NULL));

    // Transfer position to the host
    cudaMemcpy(positionHostPageablewarmup, positionPageablewarmup, 2 * walkers * sizeof(uint64_t), cudaMemcpyDeviceToHost);

    // Calculate average distance for Pageable CUDA memory allocation
    double totalDistancePageablewarmup = 0.0;
    for (int i = 0; i < walkers; i++) 
    {
        int x = positionHostPageablewarmup[i * 2];
        int y = positionHostPageablewarmup[i * 2 + 1];
        totalDistancePageablewarmup += sqrt(x * x + y * y);
    }
    double averageDistancePageablewarmup = totalDistancePageablewarmup / walkers;

    // Free memory for Pageable CUDA memory allocation
    cudaFree(positionPageablewarmup);
    free(positionHostPageablewarmup);






    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //Pageable

    // Start measuring time for Pageable CUDA memory allocation
    auto startPageable = std::chrono::high_resolution_clock::now();

    // Memory allocation on host

    uint64_t* positionHostPageable;
    positionHostPageable = (uint64_t*)malloc(2 * walkers * sizeof(uint64_t));

    // Initialize host position array
    memset(positionHostPageable, 0, 2 * walkers * sizeof(uint64_t));

    // Memory allocation on device
    uint64_t* positionPageable = nullptr;
    cudaMalloc((void**)&positionPageable, 2 * walkers * sizeof(uint64_t));


    // Kernel for random walk 

    int numThreads = 256;
    int numBlocks = (walkers + numThreads - 1) / numThreads;
    randomWalk << <numBlocks, numThreads >> > (positionPageable, walkers, steps, time(NULL));

    // Transfer position to host
    cudaMemcpy(positionHostPageable, positionPageable, 2 * walkers * sizeof(uint64_t), cudaMemcpyDeviceToHost);

    // Calculate average distance for Pageable CUDA memory allocation
    double totalDistancePageable = 0.0;
    for (int i = 0; i < walkers; i++) 
    {
        int x = positionHostPageable[i * 2];
        int y = positionHostPageable[i * 2 + 1];
        totalDistancePageable += sqrt(x * x + y * y);
    }
    double averageDistancePageable = totalDistancePageable / walkers;

    // Free memory for Pageable CUDA memory allocation
    cudaFree(positionPageable);
    free(positionHostPageable);

    // End time for Pageable CUDA memory allocation
    auto endPageable = std::chrono::high_resolution_clock::now();


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Pinned

    // Start time for pinned CUDA memory allocation
    auto startPinned = std::chrono::high_resolution_clock::now();

    // Memory allocation on host using cudaMallocHost
    uint64_t* positionHostPinned = nullptr;
    cudaMallocHost((void**)&positionHostPinned, 2 * walkers * sizeof(uint64_t));

    // Initialize host position array
    memset(positionHostPinned, 0, 2 * walkers * sizeof(uint64_t));

    // Memory allocation on device
    uint64_t* positionPinned = nullptr;
    cudaMalloc((void**)&positionPinned, 2 * walkers * sizeof(uint64_t));

    // Transfer position from host to device
    cudaMemcpy(positionPinned, positionHostPinned, 2 * walkers * sizeof(uint64_t), cudaMemcpyHostToDevice);

    // Kernel for random walk for pinned CUDA memory allocation
    randomWalk << <numBlocks, numThreads >> > (positionPinned, walkers, steps, time(NULL));

    // Transfer position from device to host
    cudaMemcpy(positionHostPinned, positionPinned, 2 * walkers * sizeof(uint64_t), cudaMemcpyDeviceToHost);

    // Calculate average distance for pinned CUDA memory allocation
    double totalDistancePinned = 0.0;
    for (int i = 0; i < walkers; i++) 
    {
        int x = positionHostPinned[i * 2];
        int y = positionHostPinned[i * 2 + 1];
        totalDistancePinned += sqrt(x * x + y * y);
    }
    double averageDistancePinned = totalDistancePinned / walkers;

    // Free memory for pinned CUDA memory allocation
    cudaFree(positionPinned);
    cudaFreeHost(positionHostPinned);

    // End time for pinned CUDA memory allocation
    auto endPinned = std::chrono::high_resolution_clock::now();


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //Managed

    // Start time for managed CUDA memory allocation
    auto startManaged = std::chrono::high_resolution_clock::now();

    // Memory allocation using cudaMallocManaged
    uint64_t* positionManaged = nullptr;
    cudaMallocManaged((void**)&positionManaged, 2 * walkers * sizeof(uint64_t));

    // Initialize host position array
    memset(positionManaged, 0, 2 * walkers * sizeof(uint64_t));

    // Launch kernel for managed CUDA memory allocation
    randomWalk << <numBlocks, numThreads >> > (positionManaged, walkers, steps, time(NULL));

    // Synchronize
    cudaDeviceSynchronize();

    // Calculate average distance for managed CUDA memory allocation
    double totalDistanceManaged = 0.0;
    for (int i = 0; i < walkers; i++) 
    {
        int x = positionManaged[i * 2];
        int y = positionManaged[i * 2 + 1];
        totalDistanceManaged += sqrt(x * x + y * y);
    }
    double averageDistanceManaged = totalDistanceManaged / walkers;

    // Free memory for managed CUDA memory allocation
    cudaFree(positionManaged);

    // End time for managed CUDA memory allocation
    auto endManaged = std::chrono::high_resolution_clock::now();

    // Calculate the time for Pageable CUDA memory allocation
    std::chrono::duration<double, std::micro> totalTimePageable = endPageable - startPageable;

    // Calculate the time for pinned CUDA memory allocation
    std::chrono::duration<double, std::micro> totalTimePinned = endPinned - startPinned;

    // Calculate the time for managed CUDA memory allocation
    std::chrono::duration<double, std::micro> totalTimeManaged = endManaged - startManaged;

    // Output the results
    std::cout << "Lab4 -W " << walkers << " -I " << steps << std::endl;
    std::cout << " Normal CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << totalTimePageable.count() << std::endl;
    std::cout << "    Average distance from origin: " << averageDistancePageable << std::endl;
    std::cout << " Pinned CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << totalTimePinned.count() << std::endl;
    std::cout << "    Average distance from origin: " << averageDistancePinned << std::endl;
    std::cout << " Managed CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << totalTimeManaged.count() << std::endl;
    std::cout << "    Average distance from origin: " << averageDistanceManaged << std::endl;
    std::cout << "Bye" << std::endl;

    return 0;
}

//This function interrogrates command line entries; Inputs are the number of entries and character pointers of the entries and the number of walkers and steps are passed in by reference
//True is returned for valid entries and false is returned if there are invalid entries
bool interrogateCommandLine(int commandLineLength, char* commandLineArguments[], uint64_t& walkers, uint64_t& steps) 
{
    bool walkersEntered = false;
    bool stepsEntered = false;

    for (int i = 1; i < commandLineLength; i++) 
    {
        std::string arg = commandLineArguments[i];
        if (arg == "-W" && i + 1 < commandLineLength) 
        {
            if (validInput(commandLineArguments[i + 1], walkers)) 
            {
                walkersEntered = true;
                continue;
            }
            else 
            {
                std::cerr << "Invalid input for the number of walkers." << std::endl;
                return false;
            }
        }
        else if (arg == "-I" && i + 1 < commandLineLength) 
        {
            if (validInput(commandLineArguments[i + 1], steps)) 
            {
                stepsEntered = true;
                continue;
            }
            else 
            {
                std::cerr << "Invalid input for the number of steps." << std::endl;
                return false;
            }
        }
    }

    // Set defaults if not set in command line
    if (!walkersEntered) 
    {
        walkers = 1000; 
    }
    if (!stepsEntered) 
    {
        steps = 10000; 
    }

    return true;
}

//This checks if the inputs for the number of walkers and steps are valid numbers and takes them by reference and outputs true for valid input and false for invalid input
bool validInput(const std::string& input, uint64_t& value) 
{
    size_t loc; // To check where conversion stops

    // Try to convert the input string to an unsigned 64-bit integer
    value = std::stoull(input, &loc, 10);

    // Check if string was successfully converted
    if (loc == input.length() && value > 0) 
    {
        return true; // Input is valid
    }

    return false; // Input is invalid
}

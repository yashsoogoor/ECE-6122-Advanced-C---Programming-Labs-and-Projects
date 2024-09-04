/*
Author: Yashwanth Soogoor
Class: ECE6122 Section A
Last Date Modified: 11/18/2023
Description:
This file sets up clients which prompt users for inputs and display messages recieved from the server.
The user can input a version number, type number followed by message, or close the client. 
*/



#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <atomic>

//Structure to carry data for transfer across connections 
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

unsigned char exitNotification = 50;

// Operator to convert packet information to a format for sending 
sf::Packet& operator <<(sf::Packet& packet, const tcpMessage& msg)
{
    return packet << msg.nVersion << msg.nType << msg.nMsgLen << std::string(msg.chMsg, msg.nMsgLen);
}

// Operator to extract packet information 
sf::Packet& operator >>(sf::Packet& packet, tcpMessage& msg)
{
    std::string str;
    packet >> msg.nVersion >> msg.nType >> msg.nMsgLen >> str;

    std::memset(msg.chMsg, 0, sizeof(msg.chMsg));
    std::memcpy(msg.chMsg, str.c_str(), std::min(sizeof(msg.chMsg), static_cast<std::size_t>(msg.nMsgLen)));

    return packet;
}

//Function to process the received message which takes in the socket by reference, mutex, cv, and exit flag
void receiveMessages(sf::TcpSocket& socket, std::mutex& mutex, std::condition_variable& cv, bool& exitFlag)
{
    //Continually receives messages to client until exit flag is set to 1
    while (!exitFlag)
    {
        tcpMessage msg;
        sf::Packet receivedPacket;

        if (socket.receive(receivedPacket) == sf::Socket::Done)

        {
            receivedPacket >> msg;
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::cout << std::endl << "Received Msg Type: " << static_cast<int>(msg.nType) << "; Msg: " << msg.chMsg << std::endl;
              
                if (static_cast<int>(msg.nType) == 77)
                {
                    std::cout << "Please enter command: ";
                }

                // Check for when the server has been closed
                if (msg.nType == 50) 
                {
                    std::cout << "Error!! Server connection closed. Terminating program!" << std::endl;
                    exitFlag = true;
                }

            }
            cv.notify_one();

        }
    }
}

int main(int argc, char* argv[])
{
    // Take in command line arguments
    if (argc != 3)
    {
        std::cerr << "Invalid Input. Try Again" << std::endl;
        return 1;
    }

    // Extract server address and port from command-line arguments
    sf::IpAddress serverAddress = argv[1];
    unsigned short serverPort = std::stoi(argv[2]);

    // Set the server address and port
    //sf::IpAddress serverAddress = "localhost";
    //unsigned short serverPort = 51717;

    // Create a socket
    sf::TcpSocket socket;

    // Connect to the server
    if (socket.connect(serverAddress, serverPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to connect!" << std::endl;
        return 1;
    }

    // Display connection success message
    std::cout << "Connected to server " << serverAddress << " and port " << serverPort << std::endl;

    // Version number
    unsigned char version = 1;

    std::mutex mutex;
    std::condition_variable cv;
    bool exitFlag = false;
 

    // Thread to get messages started
    std::thread receiveThread(receiveMessages, std::ref(socket), std::ref(mutex), std::ref(cv), std::ref(exitFlag));

    //Continually prompt users for commands until exit flag is set to 1
    while (!exitFlag)
    {

            // Continually prompt the user for commands
            std::string command;
            std::cout << "Please enter command: ";
            std::getline(std::cin, command);
    


            tcpMessage msg;
            msg.nVersion = version;

            if (command == "q")
            {
                std::cout << "Disconnected from the server." << std::endl;

                // Create message to send when disconnected
                tcpMessage disconnectMessage;
                disconnectMessage.nVersion = version;
                disconnectMessage.nType = exitNotification;
                disconnectMessage.nMsgLen = 0;

                // Send exit message
                sf::Packet disconnectPacket;
                disconnectPacket << disconnectMessage;

                if (socket.send(disconnectPacket) == sf::Socket::Done)
                {
                    std::cout << "Disconnect message sent to the server." << std::endl;
                }
                else
                {
                    std::cerr << "Failed to send disconnect message to the server!" << std::endl;
                }

                break;
            }
            else if (command.substr(0, 2) == "v ")
            {
                // Get version number
                version = static_cast<unsigned char>(std::stoi(command.substr(2)));
                std::cout  << "This version number is now used in all new messages." << std::endl; 
                continue;
            }
            else if (command.substr(0, 2) == "t ")
            {
                // Get the type number and message 
                std::size_t spacePos = command.find(' ', 2);
                if (spacePos != std::string::npos)
                {
                    std::string typeStr = command.substr(2, spacePos - 2);
                    std::string message = command.substr(spacePos + 1);

                    // Set nType
                    msg.nType = static_cast<unsigned char>(std::stoi(typeStr));

                    // Create message
                    msg.nMsgLen = static_cast<unsigned short>(message.length());
                    std::memset(msg.chMsg, 0, sizeof(msg.chMsg)); // Clear the buffer
                    std::memcpy(msg.chMsg, message.c_str(), std::min(sizeof(msg.chMsg), static_cast<std::size_t>(msg.nMsgLen)));

                    // Send the message to the server
                    sf::Packet sendPacket;
                    sendPacket << msg;

                    if (socket.send(sendPacket) == sf::Socket::Done)
                    {

                        // Prepare packet to be sent from message structure
                        sendPacket >> msg;

                    }
                    else
                    {
                        std::cerr << "Failed to send message!" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Invalid input. Use 't <type number> <message>'." << std::endl;
                    continue;
                }
            }
            else
            {
                std::cout << "Invalid ipnut. Enter 'q' to quit, 'v' to enter the version number, or 't' to enter a type number and message." << std::endl;
                continue;
            }
       

    }


    // Set the exit flag to signal receive thread
    {
        std::lock_guard<std::mutex> lock(mutex);
        exitFlag = true;
    }

    // Tell receive thread
    cv.notify_one();

    // Join thread
    receiveThread.join();

    // Closesocket
    socket.disconnect();

    return 0;
}

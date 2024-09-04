/*
Author: Yashwanth Soogoor
Class: ECE6122 Section A
Last Date Modified: 11/18/2023
Description:
This file sets up a server which listens for connection requests from clients. A thread is used to
handle data from each client and can send and recieve data until either the client or server is closed.
It handles user inputs to display the last message received, clients, and an exit command. 
*/


#include <SFML/Network.hpp>
#include <iostream>
#include <atomic>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <cstring>

//Packet structure used to send data back and forth 
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
};

//Initialize the clients list, mutex, last received message, and nType for exit notification
std::list<sf::TcpSocket*> clients;
std::mutex mutex;
std::string lastReceivedMessage;
unsigned char exitNotification = 50;



// Operator to convert packet information to a format for sending 
sf::Packet& operator <<(sf::Packet& packet, const tcpMessage& message)
{
    return packet << message.nVersion << message.nType << message.nMsgLen << std::string(message.chMsg, message.nMsgLen);
}



// Operator to extract packet information 
sf::Packet& operator >>(sf::Packet& packet, tcpMessage& message)
{
    std::string messageContent;
    packet >> message.nVersion >> message.nType >> message.nMsgLen >> messageContent;

    
    memset(message.chMsg, 0, sizeof(message.chMsg)); 
    memcpy(message.chMsg, messageContent.c_str(), std::min(sizeof(message.chMsg), static_cast<std::size_t>(message.nMsgLen)));

    return packet;
}


//Function to process the received message which takes in pointer to the sending client and the received message structure by reference
//Takes appropriate action based on nVersion and nType
void processReceivedMessage(sf::TcpSocket* sender, const tcpMessage& receivedMessage)

{

    if (receivedMessage.nVersion == 102)
    {
        
        if (receivedMessage.nType == 201)
        {
            lastReceivedMessage = receivedMessage.chMsg;
            //Create reversed message
            std::string reversedMessage(receivedMessage.chMsg);
            std::reverse(reversedMessage.begin(), reversedMessage.end());

            // Create another message struct to send reverse message
            tcpMessage tempMessage = receivedMessage;

            memcpy(tempMessage.chMsg, reversedMessage.c_str(), reversedMessage.size());
            tempMessage.nMsgLen = static_cast<unsigned short>(reversedMessage.size());
            tempMessage.nType = receivedMessage.nType; 

            // Make packet with reversed message and send to client
            sf::Packet reversedPacket;
            reversedPacket << tempMessage;
            sender->send(reversedPacket);

        }
        if (receivedMessage.nType == 77)
        {
            lastReceivedMessage = receivedMessage.chMsg;

            //For loop goes through the clients to send the message with nType 77 to all the other clients except sender
            for (const auto& client : clients)
            {

                if (client != sender)
                {
                    //Creates temporary struct for sending message
                    tcpMessage tempMessage = receivedMessage;

                    // Makes new packet 
                    sf::Packet sendingPacket;
                    sendingPacket << tempMessage;

  
                    // Send the message to all clients but sender
                    client->send(sendingPacket);

                }
                
                
            }

        }

        else
        {
            lastReceivedMessage = receivedMessage.chMsg;
        }

            
    }
}



std::atomic<bool> exitServerThread(false);


int main(int argc, char* argv[])
{
    // Check for port
    if (argc != 2)
    {
        std::cerr << "Invalid Input. Try Again" << std::endl;
        return 1;
    }

    // Convert the port to number
    unsigned short port = std::stoi(argv[1]);

    // Create a socket to listen to new connections   Code referenced from: https://www.sfml-dev.org/documentation/2.6.1/classsf_1_1SocketSelector.php
    sf::TcpListener listener;
    if (listener.listen(port) != sf::Socket::Done)
    {
        std::cerr << "Not able to listen on port " << port << std::endl;
        return 1;
    }

    // Create a selector
    sf::SocketSelector selector;

    // Add the listener to the selector
    selector.add(listener);

    // Start the server thread to handle client connections
    std::thread serverThread([&]() {
        // Continually runs through until exited
        while (!exitServerThread)
        {
            // Make the selector wait for data on any socket
            if (selector.wait())
            {
                // Test the listener
                if (selector.isReady(listener))
                {
                    // The listener is ready: there is a pending connection
                    sf::TcpSocket* client = new sf::TcpSocket;
                    if (listener.accept(*client) == sf::Socket::Done)
                    {
                        // Add the new client to the clients list
                        //std::lock_guard<std::mutex> lock(mutex);
                        clients.push_back(client);

                        // Add the new client to the selector so that we will
                        // be notified when he sends something
                        selector.add(*client);
                    }
                    else
                    {
                        // Error, we won't get a new connection, delete the socket
                        delete client;
                    }
                }
                else
                {
                    // The listener socket is not ready, test all other sockets (the clients)
                    //std::lock_guard<std::mutex> lock(mutex);

                    for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end();)
                    {
                        sf::TcpSocket& client = **it;
                        if (selector.isReady(client))
                        {
                            // The client has sent some data, we can receive it
                            sf::Packet packet;
                            if (client.receive(packet) == sf::Socket::Done)                              
                            {
                                // Process the received packet
                                tcpMessage receivedMessage;


                                if (packet >> receivedMessage)
                                {

                                    // Process the received message
                                    processReceivedMessage(&client, receivedMessage);

                                    // Remove client from list if the client disconnects and sends exit notification
                                    if (receivedMessage.nType == exitNotification)
                                    {
                                        std::cout << std::endl << "Disconnect notification received. Removing client connected to port: " << client.getRemotePort() << std::endl << "Please enter command: ";
                                        selector.remove(client);
                                        client.disconnect();
                                        delete *it;
                                        it = clients.erase(it);
                                        continue;
   
 
                                    }
                                }
                                else
                                {

                                }
                            }
                          
                        }
                        ++it;
                        
                    }
                }
            }
        }
    });

    // Main thread for user input that runs continually 
    while (true)
    {
        // Continuously prompt user
        std::string command;
        std::cout << "Please enter command: " << std::flush;
        std::getline(std::cin, command);

        if (command == "msg")
        {
            // Print the last received message
            std::cout << "Last message: " << lastReceivedMessage << "\n";
        }
        else if (command == "clients")
        {
            // Print a list of all connected clients
            std::lock_guard<std::mutex> lock(mutex);
            std::cout << "Number of Clients: " << clients.size() << "\n";
            for (const auto& client : clients)
            {
                // Get remote address and port 
                const auto& socket = *client;
                sf::IpAddress remoteAddress = socket.getRemoteAddress();
                unsigned short remotePort = socket.getRemotePort();
                std::cout << "IP Address: " << remoteAddress << " | Port: " << remotePort << "\n";
            }
        }
        else if (command == "exit")
        {
            exitServerThread = true;
            listener.close();
            // Close all sockets and terminate program
            std::lock_guard<std::mutex> lock(mutex);
            // Mark clients for deletion
            std::vector<sf::TcpSocket*> clientsToDelete;
            for (auto& client : clients)
            {
                tcpMessage exitMessage;
                exitMessage.nVersion = 102;
                exitMessage.nType = exitNotification;
                exitMessage.nMsgLen = 0;
                sf::Packet exitPacket;
                exitPacket << exitMessage;
                client->send(exitPacket);
                client->disconnect();
                clientsToDelete.push_back(client);
            }
            clients.clear();

            // Delete marked clients outside the loop
            for (auto& client : clientsToDelete)
            {
                delete client;
            }
            break;
           
        }
        else
        {
            std::cout << "Invalid command. Please reenter.\n";
        }
    }

    
    serverThread.join();


    return 0;
}

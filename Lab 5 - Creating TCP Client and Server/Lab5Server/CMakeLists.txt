# Set the root directory for sources
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/sockets)

# Server source files
set(SERVER_SRC
    #${SRCROOT}/Sockets.cpp
    #${SRCROOT}/TCP.cpp
    ${SRCROOT}/server1.cpp
)

# Client source files
set(CLIENT_SRC
    #${SRCROOT}/Sockets.cpp
    #${SRCROOT}/TCP.cpp
    ${SRCROOT}/client1.cpp
)

# Define the server executable
sfml_add_example(server
                 SOURCES ${SERVER_SRC}
                 DEPENDS sfml-network)

# Define the client executable
sfml_add_example(client
                 SOURCES ${CLIENT_SRC}
                 DEPENDS sfml-network)


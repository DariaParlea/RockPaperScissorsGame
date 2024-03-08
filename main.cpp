#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    const char* serverIP = "127.0.0.1";
    const int serverPort = 12345;

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Step 1: Create a Socket
    // The 'socket' function creates a socket for network communication.
    // It specifies the address family (IPv4), socket type (TCP), and protocol (0).

    if (clientSocket == -1) {
        std::cerr << "socket() failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    // Step 2: Set Up Server Address
    // Here, we configure the server's address information, including the address family and port.

    if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0) {
        std::cerr << "inet_pton() failed" << std::endl;
        return 1;
    }

    // Step 3: Connect to the Server
    // The 'connect' function establishes a connection to the server using the provided address.

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "connect() failed" << std::endl;
        return 1;
    }

    char word[1024];
    ssize_t bytesRead = recv(clientSocket, word, sizeof(word), 0);

    // Step 4: Receive and Display Initial Word
    // The client receives an initial word from the server and displays it.
    // Any errors in the 'recv' operation are handled.

    if (bytesRead == -1) {
        std::cerr << "recv() failed" << std::endl;
        close(clientSocket);
        return 1;
    }

    word[bytesRead] = '\0';
    std::cout << word << std::endl;

    while (true) {
        std::cout << "Guess a letter: ";
        std::string guess;
        std::cin >> guess;

        // Step 5: Send User's Guess
        // The client sends the user's guess to the server using the 'send' function.

        ssize_t bytesSent = send(clientSocket, guess.c_str(), guess.size(), 0);

        if (bytesSent == -1) {
            std::cerr << "send() failed" << std::endl;
            close(clientSocket);
            return 1;
        }

        char gameData[1024];
        bytesRead = recv(clientSocket, gameData, sizeof(gameData), 0);

        // Step 6: Receive and Process Game Data
        // The client receives game data from the server, which may include game updates.
        // Errors in the 'recv' operation are handled.

        if (bytesRead == -1) {
            std::cerr << "recv() failed" << std::endl;
            close(clientSocket);
            return 1;
        }

        gameData[bytesRead] = '\0';
        std::cout << gameData << std::endl;

        // Step 7: Check Game Status
        // The client checks if the game has ended by searching for specific strings in the received data.

        if (strstr(gameData, "You win!") || strstr(gameData, "You lose!")) {
            break;
        }
    }

    // Step 8: Close the Socket and Exit
    // After the game loop ends, the client closes the socket and exits the program.

    close(clientSocket);
    return 0;
}
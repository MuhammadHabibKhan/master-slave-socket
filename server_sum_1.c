#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/sock1"

int main() {

    // variables to hold descriptors and length of client address' struct
    int sockfd, new_sockfd, client_len;

    // struct to hold server and client address | sockaddr_un used for AF_LOCAL 
    struct sockaddr_un server_addr, client_addr; 

    // Create a socket for server listening | AF_LOCAL => local host | SOCK_STREAM => TCP | Protocol => 0 for TCP/UDP
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    // Check if failed to create socket 
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a local address
    server_addr.sun_family = AF_LOCAL;

    // copy socket path to struct | socket path => File that acts as the endpoint for communication on the same machine
    // It's similar to an IP address and port combination for network sockets, but for local communication within your system.
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Remove existing socket file (if any)
    unlink(SOCKET_PATH); 
    
    // Bind socket | socket descriptor, address of struct holding server address (casted to sockaddr for generic type and following function parameters), size of struct
    // Throw error if failed
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    listen(sockfd, 1);

    while (1) {
        // Accept a connection
        client_len = sizeof(client_addr);
        
        // Creating a new socket descriptor for each client that attaches to the server to manage send and receiving of data for that particular client
        new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);

        // check for failure
        if (new_sockfd < 0) {
            perror("accept failed");
            continue;
        }

        // setting a buffer of max amount that can be received | size of original array
        int *dataElements = malloc(100 * sizeof(int));

        // Receive data from the client | socket descriptor of client, where data is to be stored, size of the container, flag => 0 )
        // Returns the number of reads
        int bytes_received = recv(new_sockfd, dataElements, 101 * sizeof(int), 0);

        int totalElements = dataElements[0];
        int *local_sum = malloc(sizeof(int));
        
        printf("Total Elements: %d \n", totalElements);

        for (int i = 1; i < totalElements + 1; i++)
        {
            *local_sum += dataElements[i];
            printf("DEI: %d \n", dataElements[i]);
        }

        // check for failure
        if (bytes_received < 0) 
        {
            perror("recv failed");
            continue;
        }

        printf("Local Sum : %d\n", *local_sum);

        // Send the processed data back to the client
        int bytes_sent = send(new_sockfd, local_sum, sizeof(local_sum), 0);

        // check for failure
        if (bytes_sent < 0) 
        {
            perror("send failed");
            // close(new_sockfd);
            continue;
        }

        close(new_sockfd);
    }

    close(sockfd);
    return 0;
}

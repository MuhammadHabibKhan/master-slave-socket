#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

int data[100];
int dataLength = sizeof(data)/sizeof(data[0]);

int sum = 0;

char socketPaths[3][50] = {"/tmp/sock1", "/tmp/sock2", "/tmp/sock3"};

void generateData()
{
    for (int i = 0; i < dataLength; i++) { data[i] = i+1; }
}

int sendData(int socketIndex, int dataElements[], int dataSize)
{    
    int sockfd; // socket descriptor
    struct sockaddr_un server_addr; // struct to hold server address

    // Create a socket | AF_LOCAL => local host | SOCK_STREAM => TCP | Protocol => 0 for TCP/UDP
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    // check for failure
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    server_addr.sun_family = AF_LOCAL;

    // Copy socket path to server address struct
    strcpy(server_addr.sun_path, socketPaths[socketIndex]);
    printf("Socket Address: %s \n", socketPaths[socketIndex]);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // Send data to the server
    int bytes_sent = send(sockfd, dataElements, dataSize * sizeof(int), 0);

    if (bytes_sent < 0) 
    {
        perror("send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int *local_sum = malloc(sizeof(int));

    // Receive the processed data from the server
    int bytes_received = recv(sockfd, local_sum, sizeof(local_sum), 0);

    if (bytes_received < 0) 
    {
        // printf("Local Sum: %d \n", *local_sum);
        perror("recv failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Received Local Sum: %d\n", *local_sum);    
    return *local_sum;
}


void assignWorkload(int servers)
{
    int extraWorkLoad = 0;
    int workLoad = dataLength / servers;

    int *serverEWL = malloc(servers * sizeof(int));

    if ( dataLength % servers != 0)
    {
        extraWorkLoad = dataLength % servers;
        printf("EWL: %d \n", extraWorkLoad);
    }

    int index = 0;

    while (extraWorkLoad != 0)
    {
        serverEWL[index % servers] += 1;
        extraWorkLoad -= 1;
        index += 1;
    }

    for (int i = 0; i < servers; i++)
    {
        int startIndex = i * workLoad;
        int endIndex = startIndex + workLoad + serverEWL[i];

        printf("Start Index: %d | EndIndex: %d \n", startIndex, endIndex);

        int totalElements = endIndex - startIndex + 1;
        int *dataElements = malloc(totalElements * sizeof(int));

        // setting first element as size of array as arrays are not first class citizens
        // hence cannot pass by values and need to pass array as a pointer which gives size of pointer instead of size of array 
        
        dataElements[0] = totalElements - 1;

        for (int h = 1; h < totalElements; h++)
        {
            dataElements[h] = data[startIndex++];
            printf("%d | ", dataElements[h]);
        }
        printf("\n");

        int local_sum = sendData(i, dataElements, totalElements);
        sum += local_sum;

        free(dataElements);
    }
    printf("Total Sum: %d \n", sum);
}

int main (int argc, char *argv[])
{
    generateData();
    assignWorkload(2);

    return 0;
}

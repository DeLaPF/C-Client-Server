#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    // Read in command line arguments
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    int portno = atoi(argv[2]);

    // Create the TCP/IP socket using IPV4
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    // The below code creates a sockaddr_in struct that
    // references the remote hose we’re trying to connect to

 
    // Step 1: Create the sockaddr_in struct
    struct sockaddr_in serv_addr;
    // Step 2: Zero out the struct. This is necessary because in C 
    // and C++, it’s not guaranteed that new, unassigned variables 
    // are set to their default values 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // Step 3: Set the protocol to IPV4
    serv_addr.sin_family = AF_INET;
    // Step 4: Set the remote server’s ip address
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    // Step 5: Set the remote server’s port.
    // Note that we must use the htons method here on the portno.
    // This is because different computers have different ways of 
    // formatting integers. In order to standardize this for 
    // networking, all networking libraries use an integer 
    // format called network ordering, and ship a method called
    // htons (which stands for host to network short) which
    // converts a number from the native host format into the 
    // standard network format.
    serv_addr.sin_port = htons(portno);

    // Try connecting to the remote server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    // Read in a message from the command line up to 256 
    // characters in length
    printf("Please enter the message: ");
    char buffer[256];
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    // Send the message to the server
    int n = send(sockfd, buffer, strlen(buffer), 0);
    if (n < 0) 
         error("ERROR writing to socket");

    // Receive the response from the server
    bzero(buffer,256);
    n = recv(sockfd, buffer, 255, 0);
    if (n < 0) 
         error("ERROR reading from socket");

    // Print the response to the command line
    printf("Response: %s\n", buffer);

    // Close the connection
    printf("Closing the connection");
    close(sockfd);
    return 0;
}

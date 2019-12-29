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

int main(int argc, char const *argv[])
{
	// Step 1: Set up socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    int portno = 8080;

	// Step 2: Bind
	struct sockaddr_in host;
	host.sin_family = AF_INET;
	host.sin_port = htons(portno);
	host.sin_addr.s_addr = INADDR_ANY;
	const struct sockaddr *host_ptr = (struct sockaddr*) &host;
	int bound = bind(sockfd, host_ptr, sizeof(host));
	if (bound < 0)
		error("ERROR binding to socket");

	// Step 3: Listen
	int listening = listen(sockfd, 2);
	if (listening < 0)
		error("ERROR listener failed");

	// Step 4: Accecpt
	struct sockaddr_in remote;
	struct sockaddr *remote_ptr = (struct sockaddr*) &remote;
	socklen_t remote_len = (socklen_t) sizeof(remote);
	int remotefd = accept(sockfd, remote_ptr, &remote_len);
	if (remotefd < 0)
		error("ERROR receiving request");

	// Step 5: Echo
	char buffer[256];
	bzero(buffer, 256);
	int recvd = recv(remotefd, buffer, 255, 0);
	if (recvd < 0)
		error("ERROR data not received");
	int sent = send(remotefd, buffer, strlen(buffer), 0);
	if (recvd < 0)
		error("ERROR data not sent");

	// Close the connection
	printf("Closing the connection");
    close(sockfd);
	return 0;
}
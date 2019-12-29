#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ThreadPool.h"
#include "fdlist.h"

const int PORT_NO = 8080;
const int BACKLOG_SIZE = 2;

// Prototypes
void select(int sockfd, fd_set *read_fds_ref, struct node *head);
bool wasEchoed(fd_set *read_fds_ref, struct node *head);
void echo(int fd);
int accept(int sockfd);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char const *argv[])
{
	// Step 1: Set up socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create socket file descriptor for server
    if (sockfd < 0) 
        error("ERROR opening socket");

	// Step 2: Bind
	struct sockaddr_in host;
	host.sin_family = AF_INET; // IPv4 connection
	host.sin_port = htons(PORT_NO); // bound to port 8080
	host.sin_addr.s_addr = INADDR_ANY; // connected to localhost
	const struct sockaddr *host_ptr = (struct sockaddr*) &host;
	int bound = bind(sockfd, host_ptr, sizeof(host)); // bind sockfd to port
	if (bound < 0)
		error("ERROR binding to socket");

	// Step 3: Listen
	int listening = listen(sockfd, BACKLOG_SIZE); // The socket will actively listen for incoming connections and queue them up
	if (listening < 0)
		error("ERROR listener failed");

	fd_set read_fds;
	struct node *head = createFDList(0); // Create empty fdlist (file descriptor list)
	while (1) {
		select(sockfd, &read_fds, head); // select incoming connections and handle them as necessary
	}

	// Close the connection
	printf("Closing the connection");
    close(sockfd);
	return 0;
}

//Pre: sockfd is the socket bound to this server and head is a pointer to the beginning of an fdlist
//Post: sleeps until there is an available connection and then determines if it must be accepted and read from
void select(int sockfd, fd_set *read_fds_ref, struct node *head) {
	printf("%s\n", "Attempting to Select . . .");
	makeFDS(head, read_fds_ref); // make fd_set from fdlist
	FD_SET(sockfd, read_fds_ref); // (re)add sockfd to allow for new connections
	int maxsockfd = maxfd(sockfd, head); // determine the maximum value of file descriptor for select
	int selected = select(maxsockfd, read_fds_ref, NULL, NULL, NULL); // sleep until there has been in a change in one of the file descriptors in the fd_set
	if (selected >= 0) {
		printf("%s\n", "Connection Selected");

		if (!wasEchoed(read_fds_ref, head)) { // determine if the connection should be echoed
			int remotefd = accept(sockfd); // accept connection to remote client through remotefd
			pushToList(head, remotefd); // add remotefd to list
		}
	}
	else {error("ERROR Selection failed");}
}

//Pre: sockfd is the socket bound to this server
//Post: accept connection and return the file descriptor of connected client
int accept(int sockfd) {
	// Step 4: Accecpt
	struct sockaddr *remote_host; // a pointer to a sockaddr struct that will be filled in with the address information of the remote host
	socklen_t *remote_len; // the size of the sockaddr struct in bytes
	printf("%s\n", "Attempting to Accept . . .");
	int remotefd = accept(sockfd, remote_host, remote_len); // dequeue from sockfd and accept connection through remotefd
	if (remotefd < 0)
		error("ERROR receiving request");
	printf("%s\n", "Connection Accecpted");
	return remotefd;
}

//Pre: head is a pointer to the beginning of an fdlist
//Post: return 1 if data was echoed and echoes it or return 0 if data does not need to be echoed
bool wasEchoed(fd_set *read_fds_ref, struct node *head) {
	struct node *cur_node = head;
	while (cur_node != NULL) { // iterate through fdlist
		int fd = cur_node->data;
		if (FD_ISSET(fd, read_fds_ref)) { // check if file descriptor is ready to be read from
			printf("%s\n", "Echoing Data");
			removeFromList(head, fd); // close connection to client part 1
			FD_CLR(fd, read_fds_ref); // close connection to client part 2

			ThreadPool thread_pool(4); // create threadpool of size 4
			thread_pool.enqueue([fd]{ // run "echo" asynchronously to improve not force clients to wait for another clients poosibly slow processes 
				echo(fd); // echo  data from fd
			});
			return 1; // true
		}
		cur_node = cur_node->next;
	}
	return 0; // false
}

//Pre: fd is a valid file descritor of a client that has sent data to the server
//Post: return exact data back to client
void echo(int fd) {
	// Step 5: Echo
	char buffer[256]; // create buffer of 256 chars
	bzero(buffer, 256); // zero out all values in buffer
	int recvd = recv(fd, buffer, 255, 0); // receive data from fd and put it in buffer
	if (recvd < 0)
		error("ERROR data not received");
	int sent = send(fd, buffer, strlen(buffer), 0); // send buffer back to echo data
	if (sent < 0)
		error("ERROR data not sent");
	close(fd); // close connection to client part 3
}
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

void handleConnnection(int sockfd, fd_set *read_fds_ref, struct node *head);
bool wasEchoed(fd_set *read_fds_ref, struct node *head, bool *wasechoed);
void accept(struct node *head, int sockfd);

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

	fd_set read_fds;
	FD_SET(sockfd, &read_fds);
	struct node *head = createFDList(0);

	while (1) {
		handleConnnection(sockfd, &read_fds, head);
	}

	// Close the connection
	printf("Closing the connection");
    close(sockfd);
	return 0;
}

void handleConnnection(int sockfd, fd_set *read_fds_ref, struct node *head) {
	printf("%s\n", "Attempting to Select . . .");
	makeFDS(head, read_fds_ref);
	FD_SET(sockfd, read_fds_ref);
	int maxsockfd = maxfd(sockfd, head);
	int selected = select(maxsockfd, read_fds_ref, NULL, NULL, NULL);
	if (selected >= 0) {
		printf("%s\n", "Connection Selected");
		bool wasechoed = 1;

		ThreadPool thread_pool(4);
		std::vector<std::future<void>> handles;
		handles.emplace_back(thread_pool.enqueue([read_fds_ref, head, &wasechoed]{
				wasEchoed(read_fds_ref, head, &wasechoed);
    	}));

		handles.emplace_back(thread_pool.enqueue([head, sockfd, &wasechoed]{
			if (!wasechoed)
				accept(head, sockfd);
    	}));
	}
	else {error("ERROR Selection failed");}
}

void accept(struct node *head, int sockfd) {
	// Step 4: Accecpt
	struct sockaddr_in remote;
	struct sockaddr *remote_ptr = (struct sockaddr*) &remote;
	socklen_t remote_len = (socklen_t) sizeof(remote);
	printf("%s\n", "Attempting to Accept . . .");
	int remotefd = accept(sockfd, remote_ptr, &remote_len);
	if (remotefd < 0)
		error("ERROR receiving request");
	if (remotefd != sockfd)
		pushToList(head, remotefd);
	printf("%s\n", "Connection Accecpted");
}

bool wasEchoed(fd_set *read_fds_ref, struct node *head, bool *wasechoed) {
	struct node *cur_node = head;
	while (cur_node != NULL) {
		int fd = cur_node->data;
		if (FD_ISSET(fd, read_fds_ref)) {
			printf("%s\n", "Echoing Data");
			// Step 5: Echo
			char buffer[256];
			bzero(buffer, 256);
			int recvd = recv(fd, buffer, 255, 0);
			if (recvd < 0)
				error("ERROR data not received");
			int sent = send(fd, buffer, strlen(buffer), 0);
			if (sent < 0)
				error("ERROR data not sent");
			close(fd);
			removeFromList(head, fd);
			FD_CLR(fd, read_fds_ref);
			*wasechoed = 1;
			return 1;
		}
		cur_node = cur_node->next;
	}
	*wasechoed = 0;
	return 0;
}
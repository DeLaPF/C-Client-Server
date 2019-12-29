#ifndef FD_LIST_H
#define FD_LIST_H

#include <stdio.h>
#include <stdlib.h>

struct node {
	int data;
	struct node *next;
};

struct node* createFDList(int data) {
	int size_of_node = sizeof(struct node);
	struct node *head = (struct node*) malloc(size_of_node);
	head->data = data;
	head->next = NULL;
	return head;
}

void pushToList(struct node *head_ref, int data) {
	int size_of_node = sizeof(struct node);
	struct node *new_node = (struct node*) malloc(size_of_node);
	new_node->data = data;
	struct node *temp_node = head_ref->next;
	head_ref->next = new_node;
	new_node->next = temp_node;
	//printf("%i %s\n", data, "added");
}

int removeFromList(struct node *head_ref, int data) {
	struct node *pre_node = head_ref;
	struct node *cur_node = head_ref->next;
	while(cur_node != NULL && cur_node->data != data) {
		pre_node = cur_node;
		cur_node = cur_node->next;
	}
	if (cur_node == NULL) {
		return -1;
	}

	pre_node->next = cur_node->next;
	cur_node->next = NULL;
	free(cur_node);
	//printf("%i %s\n", data, "removed");
	return data;
}

void makeFDS(struct node *head_ref, fd_set *fds_ref) {
	fd_set new_fds;
	*fds_ref = new_fds;
	struct node *cur_node = head_ref->next;
	while(cur_node != NULL) {
		int data = cur_node->data;
		FD_SET(data, fds_ref);
		cur_node = cur_node->next;
	}
}

int maxfd(int sockfd, struct node *head_ref) {
	int max = sockfd;
	struct node *cur_node = head_ref->next;
	while(cur_node != NULL) {
		int data = cur_node->data;
		if (data > max)
			max = data;
		cur_node = cur_node->next;
	}

	return max + 1;
}

void printList(struct node *head_ref) {
	struct node *cur_node = head_ref;
	while(cur_node != NULL) {
		int data = cur_node->data;
		printf("%i\n", data);
		cur_node = cur_node->next;
	}
}

#endif
#include <stdio.h>
#include <stdlib.h>

struct node {
	int data;
	struct node *next;
};

struct node* createList(int data) {
	int size_of_data = sizeof(data);
	struct node *head = (struct node*) malloc(size_of_data);
	head->data = data;
	head->next = NULL;
	return head;
}

void pushToList(struct node *head_ref, int data) {
	int size_of_data = sizeof(data);
	struct node *new_node = (struct node*) malloc(size_of_data);
	new_node->data = data;
	struct node *temp_node = head_ref->next;
	head_ref->next = new_node;
	new_node->next = temp_node;
}

//This implementaion is dangerous because it does not allow for a linked list with duplicate values
//Made with the intention of beign used on file descriptors which should all have unique vals
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

	return data;
}

void printList(struct node *head_ref) {
	struct node *cur_node = head_ref;
	while(cur_node != NULL) {
		int data = cur_node->data;
		printf("%i\n", data);
		cur_node = cur_node->next;
	}
}

int main(void) {
	int a = 5;
	struct node *head = createList(a);
	for (int i = 1; i < 5; i++)
		pushToList(head, i);

	printList(head);
	removeFromList(head, 4);
	printList(head);

	
	return 0;
}
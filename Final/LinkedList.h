#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node {
    int data;
    struct node *next;
} node;

class LinkedList {
public:
    // Constructor
    LinkedList();
    // Destructor
    ~LinkedList();

    // Pushes node to front of list
    void push(int data);
    // Removes the first instance of a node with the given data
    void remove(int data);

    // Exposed to allow for easy iteration through the linked list:
    // for (node *cur = list.begin(); cur; cur = cur->next) {
    //     // Do something
    // }
    node *begin();
    node *end();
private:
    node *head;
    node *sentinel;
};

#endif
#ifndef SYNC_LINKED_LIST_H
#define SYNC_LINKED_LIST_H

#include <shared_mutex>
#include <mutex>
#include <functional>
#include "LinkedList.h"

class LinkedList;

class SyncLinkedList {
public:
    // Constructor
    SyncLinkedList();
    // Destructor
    ~SyncLinkedList();

    // Pushes node to front of list
    void push(int data);
    // Removes the first instance of a node with the given data
    void remove(int data);

    // Exposed to allow for easy iteration through the linked list
    // If the iter handler returns false, the iteration will stop.
    void for_each(std::function<bool (node *)> iter);
private:
    LinkedList *linked_list;
    std::shared_mutex rwm; 
};

#endif
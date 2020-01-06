#include "SyncLinkedList.h"

// Constructor
SyncLinkedList::SyncLinkedList() {
    linked_list = new LinkedList();
}
// Destructor
SyncLinkedList::~SyncLinkedList() {
    linked_list->~LinkedList();
}

// Pushes node to front of list
void SyncLinkedList::push(int data) {
    std::unique_lock lock(rwm);
    linked_list->push(data);
}

// Removes the first instance of a node with the given data
void SyncLinkedList::remove(int data) {
    std::unique_lock lock(rwm);
    linked_list->remove(data);
}

// Exposed to allow for easy iteration through the linked list
// If the iter handler returns false, the iteration will stop.
void SyncLinkedList::for_each(std::function<bool (node *)> iter) {
    std::shared_lock lock(rwm);
    for(node *cur = linked_list->begin(); cur != linked_list->end(); cur = cur->next) {
        if(!iter(cur))
            break;
    }
}
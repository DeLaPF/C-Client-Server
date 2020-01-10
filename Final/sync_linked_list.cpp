#include "sync_linked_list.h"

// Destructor
dlpf::sync::sync_linked_list::~sync_linked_list() {
}

// Pushes node to front of list
void dlpf::sync::sync_linked_list::push(int data) {
    std::unique_lock lock(rwm);
    linked_list.push(data);
}

// Removes the first instance of a node with the given data
void dlpf::sync::sync_linked_list::remove(int data) {
    std::unique_lock lock(rwm);
    linked_list.remove(data);
}

// Exposed to allow for easy iteration through the linked list
// If the iter handler returns false, the iteration will stop.
void dlpf::sync::sync_linked_list::for_each(std::function<bool (dlpf::sync::node *)> iter) {
    std::shared_lock lock(rwm);
    for(dlpf::sync::node *cur = linked_list.begin(); cur != linked_list.end(); cur = cur->next) {
        if(!iter(cur))
            break;
    }
}
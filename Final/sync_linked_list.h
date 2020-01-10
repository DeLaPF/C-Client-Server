#ifndef SYNC_LINKED_LIST_H
#define SYNC_LINKED_LIST_H

#include <shared_mutex>
#include <mutex>
#include <functional>
#include "linked_list.h"

class linked_list;

namespace dlpf::sync {
    class sync_linked_list {
        public:
            // Constructor
            sync_linked_list():linked_list(){};
            // Destructor
            ~sync_linked_list();

            // Pushes node to front of list
            void push(int data);
            // Removes the first instance of a node with the given data
            void remove(int data);

            // Exposed to allow for easy iteration through the linked list
            // If the iter handler returns false, the iteration will stop.
            void for_each(std::function<bool (dlpf::sync::node *)> iter);
        private:
            dlpf::sync::linked_list linked_list;
            std::shared_mutex rwm; 
    };
}

#endif
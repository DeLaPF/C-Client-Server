#include "linked_list.h"

#include <iostream>
#include <stdlib.h>

// Create linked list
dlpf::sync::linked_list::linked_list()
{
    head = sentinel = (dlpf::sync::node *)calloc(1, sizeof(dlpf::sync::node));
}

// Clean up nodes
dlpf::sync::linked_list::~linked_list()
{
    dlpf::sync::node *cur = head;
    while (cur)
    {
        dlpf::sync::node *temp = cur->next;
        free(cur);
        cur = temp;
    }
}

// Adds node to list
void dlpf::sync::linked_list::push(int data)
{
    dlpf::sync::node *new_node = (dlpf::sync::node *) malloc(sizeof(dlpf::sync::node));
    new_node->data = data;
    new_node->next = head;
    head = new_node;
}

// Removes node from list
void dlpf::sync::linked_list::remove(int data)
{
    dlpf::sync::node *pre;
    dlpf::sync::node *cur = head;
    while (cur->data != data && cur)
    {
        pre = cur;
        cur = cur->next;
    }
    if (!cur || cur == sentinel)
        return;
    if (cur == head)
        head = cur->next;
    else
        pre->next = cur->next;
    free(cur);
}

dlpf::sync::node *dlpf::sync::linked_list::begin()
{
    return head;
}

dlpf::sync::node *dlpf::sync::linked_list::end()
{
    return sentinel;
}
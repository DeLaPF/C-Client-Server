#include "LinkedList.h"

#include <iostream>
#include <stdlib.h>

// Create linked list
LinkedList::LinkedList()
{
    head = sentinel = (node *)calloc(1, sizeof(node));
}

// Clean up nodes
LinkedList::~LinkedList()
{
    node *cur = head;
    while (cur)
    {
        node *temp = cur->next;
        free(cur);
        cur = temp;
    }
}

// Adds node to list
void LinkedList::push(int data)
{
    node *new_node = (node *) malloc(sizeof(node));
    new_node->data = data;
    new_node->next = head;
    head = new_node;
}

// Removes node from list
void LinkedList::remove(int data)
{
    node *pre;
    node *cur = head;
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

node *LinkedList::begin()
{
    return head;
}

node *LinkedList::end()
{
    return sentinel;
}
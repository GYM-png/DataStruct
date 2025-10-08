//
// Created by GYM on 2025/8/14.
//

#ifndef DATASTRUCTS_NODE_H
#define DATASTRUCTS_NODE_H
#include <stdint.h>
#include <stdbool.h>

typedef struct Node{
    void *args;
    struct Node *next;
}Node;

void Node_Add(Node *head, Node *node);
bool Node_Inster(Node *head, Node *node, uint8_t index);
bool Node_Delete(Node *head, uint8_t index);
bool Node_isCircle(Node *head);
#endif //DATASTRUCTS_NODE_H

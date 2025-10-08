//
// Created by GYM on 2025/8/14.
//

#include "Node.h"



/**
 * @brief 在链表末尾添加节点
 * @param head 头结点
 * @param node 添加节点
 */
void Node_Add(Node *head, Node *node)
{
    Node *p = head;
    while (p->next != NULL)
    {
        p = p->next;
    }
    p->next = node;
    node->next = NULL;
}

/**
 * 在链表指定位置插入节点
 * @param head 头结点
 * @param node 插入的节点
 * @param index 插入位置
 * @return true 插入成功 false 插入失败
 */
bool Node_Inster(Node *head, Node *node, uint8_t index)
{
    Node *p = head;
    for (uint8_t i = 0; i < index; ++i)
    {
        p = p->next;
        if (p->next == NULL)
        {
            return false;
        }
    }
    node->next = p->next;
    p->next = node;
    return true;
}

/**
 * 删除链表指定位置的节点
 * @param head 头结点
 * @param index 删除节点的位置
 * @return true 删除成功 false 删除失败
 */
bool Node_Delete(Node *head, uint8_t index)
{
    Node *p = head;
    for (uint8_t i = 0; i < index - 1; ++i)
    {
        p = p->next;
        if (p->next == NULL)
        {
            return false;
        }
    }
    p->next = p->next->next;
    return true;
}

/**
 * 判断链表是否成环
 * @param head 头结点
 * @return true 成环 false 未成环
 */
bool Node_isCircle(Node *head)
{
    Node *fast = head;
    Node *slow = head;

    while (slow->next != NULL && fast->next != NULL)
    {
        fast = fast->next->next;
        slow = slow->next;
        if (fast == slow)
        {
            return true;
        }
    }
    return false;
}
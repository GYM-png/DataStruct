//
// Created by GYM on 2025/8/14.
//

#include "Queue.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#define QUEUE_UPDATE_READ_INDEX(q)      do {q->read_index = (q->read_index + 1) % q->queue_size; if (!q->read_index) q->read_mirror ^= 1;} while(0)
#define QUEUE_UPDATE_WRITE_INDEX(q)     do {q->write_index = (q->write_index + 1) % q->queue_size; if (!q->write_index) q->write_mirror ^= 1;} while(0)

#define FDQUEUE_UPDATE_RECORD_READ_INDEX(q)  do {q->size_record[queue->record_read_index] = 0; q->record_read_index = (q->record_read_index + 1) % q->queue_size; if (!q->record_read_index) q->record_read_mirror ^= 1;} while(0)
#define FDQUEUE_UPDATE_RECORD_WRITE_INDEX(q, size) do {q->size_record[queue->record_write_index] = size; q->record_write_index = (q->record_write_index + 1) % q->queue_size; if (!q->record_write_index) q->record_write_mirror ^= 1;} while(0)

#define FDQUEUE_UPDATE_RECORD_WRITE_INDEX_BEGIN(q, size) do {q->size_record[queue->record_write_index] += size; } while(0)
#define FDQUEUE_UPDATE_RECORD_WRITE_INDEX_END(q, size) do {q->size_record[queue->record_write_index] += size; q->record_write_index = (q->record_write_index + 1) % q->queue_size; if (!q->record_write_index) q->record_write_mirror ^= 1;} while(0)

#define QUEUE_ASSERT(x)  do{ if(!(x)){printf("assert faild %s:%d", __FILE_NAME__, __LINE__);}}while(0)
#define QUEUE_ASSERT_RETURN(x, ret)  do{ if(!(x)){printf("assert faild %s:%d", __FILE_NAME__, __LINE__); return ret;} }while(0)

/**
 * @brief 初始化队列
 * @param queue 队列指针
 * @param buffer 数据buffer
 * @param message_size 消息大小
 * @param queue_size 队列大小
 */
void queueInit(Queue *queue, uint8_t *buffer, uint16_t message_size, uint16_t queue_size)
{
    QUEUE_ASSERT(queue != NULL);
    QUEUE_ASSERT(buffer != NULL);
    QUEUE_ASSERT(message_size != 0);
    QUEUE_ASSERT(queue_size != 0);

    queue->buffer = buffer;
    queue->message_size = message_size;
    queue->queue_size = queue_size;
    queue->write_mirror = 0;
    queue->read_mirror = 0;
    queue->write_index = 0;
    queue->read_index = 0;
}

/**
 * @brief 创建一个队列
 * @param message_size 队列中消息的大小
 * @param queue_size 队列的大小
 * @return 队列指针
 */
Queue* queueCreate(uint16_t message_size, uint16_t queue_size)
{
    QUEUE_ASSERT_RETURN(message_size != 0, NULL);
    QUEUE_ASSERT_RETURN(queue_size != 0, NULL);
    Queue *queue = (Queue *)malloc(sizeof (Queue));
    if (queue == NULL)
    {
        free(queue);
        return NULL;
    }
    uint8_t *data = (uint8_t *)malloc(message_size * queue_size);
    if (data == NULL)
    {
        free(queue);
        return NULL;
    }
    queue->buffer = data;
    queue->message_size = message_size;
    queue->queue_size = queue_size;
    queue->write_mirror = 0;
    queue->read_mirror = 0;
    queue->write_index = 0;
    queue->read_index = 0;
    return queue;
}

/**
 * @brief 队列是否为空
 * @param queue 队列指针
 * @return true:队列为空 false:队列不为空
 */
static bool queueIsEmpty(Queue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    return (queue->write_index == queue->read_index ) && (queue->write_mirror == queue->read_mirror);
}

/**
 * @brief 队列是否已满
 * @param queue 队列指针
 * @return true:队列已满 false:队列未满
 */
static bool queueIsFull(Queue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, true);
    return (queue->write_index == queue->read_index) && (queue->write_mirror != queue->read_mirror);
}

/**
 * @brief 获取队列剩余空间
 * @param queue 队列指针
 * @return 剩余空间
 */
uint16_t queueGetFreeSpace(Queue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, 0);
    uint16_t free_space = 0;
    if (queueIsEmpty(queue))
    {
        free_space = queue->queue_size;
    }
    else if (queueIsFull(queue))
    {
        free_space = 0;
    }
    else
    {
        if (queue->write_index > queue->read_index)
        {
            free_space = queue->queue_size - queue->write_index + queue->read_index;
        }
        else
        {
            free_space = queue->read_index  - queue->write_index;
        }
    }
    return free_space;
}

/**
 * @brief 发送消息
 * @param queue 队列指针
 * @param message 消息指针
 * @return true:发送成功 false:发送失败
 */
bool queueSend(Queue *queue, const uint8_t *message)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(message != NULL, false);
    if (queueIsFull(queue))
    {
        return false;
    }
    memcpy(queue->buffer + (queue->write_index * queue->message_size), message, queue->message_size);
    QUEUE_UPDATE_WRITE_INDEX(queue);
    return true;
}

/**
 * @brief 强制发送消息
 * @param queue 队列指针
 * @param message 待发送的消息指针
 */
void queueSendForce(Queue *queue, const uint8_t *message)
{
    QUEUE_ASSERT(queue != NULL);
    QUEUE_ASSERT(queue != NULL);
    if (queueIsFull(queue))
    {
        QUEUE_UPDATE_READ_INDEX(queue);
    }
    memcpy(queue->buffer + (queue->write_index * queue->message_size), message, queue->message_size);
    QUEUE_UPDATE_WRITE_INDEX(queue);
}

/**
 * @brief 接收消息
 * @param queue 队列指针
 * @param message 接收到的消息指针
 * @return true:接收成功 false:接收失败
 */
bool queueReceive(Queue *queue, uint8_t *message)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(message != NULL, false);
    if (queueIsEmpty(queue))
    {
        return false;
    }
    memcpy(message, queue->buffer + (queue->read_index * queue->message_size), queue->message_size);
    QUEUE_UPDATE_READ_INDEX(queue);
    return true;
}

/**
 * @brief 可变长队列初始化
 * @param queue 队列指针
 * @param buffer 缓冲区
 * @param buffer_size 缓冲区大小
 * @param queue_size 队列大小
 * @return true:初始化成功 false:初始化失败
 */
bool fdQueueInit(FDQueue *queue, uint8_t *buffer, uint16_t buffer_size, uint8_t queue_size)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(buffer != NULL, false);
    QUEUE_ASSERT_RETURN(buffer_size != 0, false);
    QUEUE_ASSERT_RETURN(queue_size != 0, false);
    queue->size_record = (uint16_t *)malloc(sizeof (uint16_t) * queue_size);
    if (queue->size_record == NULL)
    {
        free(queue->size_record);
        return false;
    }
    memset(queue->size_record, 0, queue_size + 1);
    queue->buffer = buffer;
    queue->buffer_size = buffer_size;
    queue->queue_size = queue_size;
    queue->read_index = 0;
    queue->write_index = 0;
    queue->record_read_index = 0;
    queue->record_read_mirror = 0;
    queue->record_write_index = 0;
    queue->record_write_mirror = 0;
    return true;
}

/**
 * @brief 创建一个可变长度队列
 * @param buffer_size 队列缓冲区大小
 * @param queue_size 队列大小
 * @return 队列指针
 */
FDQueue* fdQueueCreate(uint16_t buffer_size, uint8_t queue_size)
{
    QUEUE_ASSERT_RETURN(buffer_size != 0, NULL);
    QUEUE_ASSERT_RETURN(queue_size != 0, NULL);
    FDQueue *queue = (FDQueue *)malloc(sizeof (FDQueue));
    if (queue == NULL)
    {
        free(queue);
        return NULL;
    }
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
    if (buffer == NULL)
    {
        free(queue);
        return NULL;
    }
    queue->buffer = buffer;
    queue->buffer_size = buffer_size;
    queue->queue_size = queue_size;
    queue->size_record = (uint16_t *)malloc(sizeof (uint16_t) * queue_size);
    if (queue->size_record == NULL)
    {
        free(queue->size_record);
        free(queue);
        return NULL;
    }
    memset(queue->size_record, 0, sizeof (uint16_t) * queue_size);
    memset(queue->buffer, 0, buffer_size);
    queue->read_index = 0;
    queue->write_index = 0;
    queue->record_read_index = 0;
    queue->record_read_mirror = 0;
    queue->record_write_index = 0;
    queue->record_write_mirror = 0;
    return queue;
}

/**
 * @brief 获取可变长度队列剩余buffer空间
 * @param queue 队列指针
 * @return 剩余空间大小
 */
uint16_t fdQueueGetFreeSpace(FDQueue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, 0);
    uint16_t free_space = 0;

    if (queue->write_index > queue->read_index)
    {
        free_space = queue->buffer_size - queue->write_index + queue->read_index;
    }
    else if (queue->write_index < queue->read_index)
    {
        free_space = queue->read_index  - queue->write_index;
    }
    else if (queue->write_index == queue->read_index)
    {
        free_space = fdQueueIsEmpty(queue) ? queue->buffer_size : 0; // 队列消息个数为0时候，buffer为空, 否则为buffer已满
    }
    return free_space;
}

/**
 * @brief 获取可变长度队列中待处理的消息数量
 * @param queue 队列指针
 * @return 待处理的消息数量
 */
uint16_t fdQueueMessageWatting(FDQueue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, 0);
    uint16_t message_count = 0;
    for (int i = 0; i < queue->queue_size; ++i)
    {
        if (queue->size_record[i] != 0)
        {
            message_count++;
        }
    }
    return message_count;
}

/**
 * @brief 可变长度队列是否为空
 * @param queue 队列指针
 * @return true: 队列为空 false: 队列非空
 */
bool fdQueueIsEmpty(FDQueue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    return (queue->record_read_index == queue->record_write_index) && (queue->record_read_mirror == queue->record_write_mirror);
}

/**
 * @brief 可变长度队列是否已满
 * @note 这里针对的是队列消息个数，而不是buffer的状态
 * @param queue 队列指针
 * @return true: 队列已满 false: 队列未满
 */
bool fdQueueIsFull(FDQueue *queue)
{
    QUEUE_ASSERT_RETURN(queue != NULL, true);
    return (queue->record_read_index == queue->record_write_index) && (queue->record_read_mirror != queue->record_write_mirror);
}

/**
 * @brief 向可变长度队列发送一条消息
 * @param queue 队列指针
 * @param message 消息
 * @param message_size 消息长度
 * @return true: 发送成功 false: 队列已满
 */
bool fdQueueSend(FDQueue *queue, const uint8_t *message, uint16_t message_size)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(message != NULL, false);
    QUEUE_ASSERT_RETURN(message_size != 0, false);
    if (fdQueueGetFreeSpace(queue) < message_size)
    {
        return false;
    }

    if (fdQueueIsFull(queue))
    {
        return false;
    }

    if (queue->buffer_size - queue->write_index >= message_size)    //顺序写入
    {
        memcpy(&queue->buffer[queue->write_index], message, message_size);
        queue->write_index = (queue->write_index + message_size) % queue->buffer_size;
    }
    else    //回卷写入，分两次
    {
        memcpy(&queue->buffer[queue->write_index], message, queue->buffer_size - queue->write_index);
        memcpy(&queue->buffer[0], message + queue->buffer_size - queue->write_index, message_size - (queue->buffer_size - queue->write_index));
        queue->write_index = message_size - (queue->buffer_size - queue->write_index);
    }


    FDQUEUE_UPDATE_RECORD_WRITE_INDEX(queue, message_size);
    return true;
}

/**
 * @brief 向可变长度队列发送一条消息, 强制写入
 * @note 此函数会强制写入，如果队列已满，会先删除队列中的消息，直到有空间为止
 * @param queue 队列指针
 * @param message 消息
 * @param message_size 消息长度
 * @return true: 发送成功 false: 队列已满
 */
bool fdQueueSendForce(FDQueue *queue, const uint8_t *message, uint16_t message_size)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(message != NULL, false);
    QUEUE_ASSERT_RETURN(message_size != 0, false);
    uint32_t free_space = fdQueueGetFreeSpace(queue);
    bool is_full = fdQueueIsFull(queue);
    if (free_space < message_size || is_full)
    {
        while (free_space < message_size || is_full)
        {
            uint16_t recoed_size = queue->size_record[queue->record_read_index];
            for (uint16_t i = 0; i < recoed_size; i++)
            {
                queue->read_index = (queue->read_index + 1) % queue->buffer_size;
            }
            FDQUEUE_UPDATE_RECORD_READ_INDEX(queue);
            free_space = fdQueueGetFreeSpace(queue);
            is_full = fdQueueIsFull(queue);
        }
    }
    return fdQueueSend(queue, message, message_size);
}

/**
 * @brief 向可变长度队列发送一条消息
 * @note 和 fdQueueSendEnd 互为一对，用于分包发送同一条消息的情况
 * @note 此函数可调用多次，直到 fdQueueSendEnd 成功结束发送
 * @note 此函数和 fdQueueSendEnd 函数无法处理队列buffer满的异常情况，需谨慎使用
 * @param queue 队列指针
 * @param message 消息
 * @param message_size 消息长度
 * @return true: 发送成功 false: 队列已满
 */
bool fdQueueSendBegin(FDQueue *queue, const uint8_t *message, uint16_t message_size)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(message != NULL, false);
    QUEUE_ASSERT_RETURN(message_size != 0, false);
    if (fdQueueGetFreeSpace(queue) < message_size)
    {
        return false;
    }

    if (fdQueueIsFull(queue))
    {
        return false;
    }

    if (queue->buffer_size - queue->write_index >= message_size)    //顺序写入
    {
        memcpy(&queue->buffer[queue->write_index], message, message_size);
        queue->write_index = (queue->write_index + message_size) % queue->buffer_size;
    }
    else    //回卷写入，分两次
    {
        memcpy(&queue->buffer[queue->write_index], message, queue->buffer_size - queue->write_index);
        memcpy(&queue->buffer[0], message + queue->buffer_size - queue->write_index, message_size - (queue->buffer_size - queue->write_index));
        queue->write_index = message_size - (queue->buffer_size - queue->write_index);
    }

    FDQUEUE_UPDATE_RECORD_WRITE_INDEX_BEGIN(queue, message_size);
    return true;
}

/**
 * @brief 向可变长度队列发送一条消息
 * @note 和 fdQueueSendBegin 互为一对，用于分包发送同一条消息的情况
 * @note 此函数和 fdQueueSendBegin 函数无法处理队列buffer满的异常情况，需谨慎使用
 * @param queue 队列指针
 * @param message 消息
 * @param message_size 消息长度
 * @return true: 发送成功 false: 队列已满
 */
bool fdQueueSendEnd(FDQueue *queue, const uint8_t *message, uint16_t message_size)
{
    QUEUE_ASSERT_RETURN(queue != NULL, false);
    QUEUE_ASSERT_RETURN(message != NULL, false);
    QUEUE_ASSERT_RETURN(message_size != 0, false);
    if (fdQueueGetFreeSpace(queue) < message_size)
    {
        return false;
    }

    if (fdQueueIsFull(queue))
    {
        return false;
    }

    uint16_t begin_pos = 0; //Begin函数调用前的写入位置
    for (int i = 0; i < queue->record_write_index; ++i)
    {
        begin_pos += queue->size_record[i];
    }

    if (queue->buffer_size - queue->write_index >= message_size)    //顺序写入
    {
        memcpy(&queue->buffer[queue->write_index], message, message_size);
        queue->write_index = (queue->write_index + message_size) % queue->buffer_size;
    }
    else    //回卷写入，分两次
    {
        memcpy(&queue->buffer[queue->write_index], message, queue->buffer_size - queue->write_index);
        memcpy(&queue->buffer[0], message + queue->buffer_size - queue->write_index, message_size - (queue->buffer_size - queue->write_index));
        queue->write_index = message_size - (queue->buffer_size - queue->write_index);
    }

    FDQUEUE_UPDATE_RECORD_WRITE_INDEX_END(queue, message_size);
    return true;
}

/**
 * @brief 从可变长队列中接收数据
 * @param queue 队列指针
 * @param message 接收缓存
 * @param message_size_max 接收缓存大小
 * @return 0: 队列为空 其他值: 接收到的数据大小
 */
uint16_t fdQueueReceive(FDQueue *queue, uint8_t *message, uint16_t message_size_max)
{
    QUEUE_ASSERT_RETURN(queue != NULL, 0);
    QUEUE_ASSERT_RETURN(message != NULL, 0);
    QUEUE_ASSERT_RETURN(message_size_max != 0, 0);
    if (fdQueueIsEmpty(queue))
    {
        return 0;
    }
    uint16_t message_size = queue->size_record[queue->record_read_index];
    if (message_size > message_size_max)
    {
        return 0;
    }

    for (uint16_t i = 0; i < message_size; i++)
    {
        message[i] = queue->buffer[queue->read_index];
        queue->read_index = (queue->read_index + 1) % queue->buffer_size;
    }

    FDQUEUE_UPDATE_RECORD_READ_INDEX(queue);
    return message_size;
}

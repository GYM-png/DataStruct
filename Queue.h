//
// Created by GYM on 2025/8/14.
//

#ifndef DATASTRUCTS_QUEUE_H
#define DATASTRUCTS_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint8_t *buffer;
    uint16_t message_size;
    uint16_t queue_size;
    uint16_t write_mirror : 1;
    uint16_t write_index : 15;
    uint16_t read_mirror : 1;
    uint16_t read_index : 15;
}Queue;

typedef struct{
    uint8_t *buffer;
    uint16_t buffer_size;
    uint8_t queue_size;
    uint16_t write_index;
    uint16_t read_index;
    /* record the size of each message */
    uint16_t *size_record;
    uint8_t record_write_mirror : 1;
    uint8_t record_write_index : 7;
    uint8_t record_read_mirror : 1;
    uint8_t record_read_index : 7;
}FDQueue;


typedef Queue* QueueHandle_t;
typedef FDQueue* FDQueueHandle_t;


void queueInit(QueueHandle_t queue, uint8_t *buffer, uint16_t message_size, uint16_t queue_size);
QueueHandle_t queueCreate(uint16_t message_size, uint16_t queue_size);
static bool queueIsEmpty(QueueHandle_t queue);
static bool queueIsFull(QueueHandle_t queue);
uint16_t queueGetFreeSpace(QueueHandle_t queue);
bool queueSend(QueueHandle_t queue, const uint8_t *message);
void queueSendForce(QueueHandle_t queue,const uint8_t *message);
bool queueReceive(QueueHandle_t queue, uint8_t *message);

bool fdQueueInit(FDQueueHandle_t queue, uint8_t *buffer, uint16_t buffer_size, uint8_t queue_size);
FDQueueHandle_t fdQueueCreate(uint16_t buffer_size, uint8_t queue_size);
uint16_t fdQueueGetFreeSpace(FDQueueHandle_t queue);
uint16_t fdQueueMessageWatting(FDQueueHandle_t queue);
bool fdQueueIsEmpty(FDQueueHandle_t queue);
bool fdQueueIsFull(FDQueueHandle_t queue);
bool fdQueueSend(FDQueueHandle_t queue, const uint8_t *message, uint16_t message_size);
bool fdQueueSendForce(FDQueueHandle_t queue, const uint8_t *message, uint16_t message_size);
bool fdQueueSendBegin(FDQueueHandle_t queue, const uint8_t *message, uint16_t message_size);
bool fdQueueSendEnd(FDQueueHandle_t queue, const uint8_t *message, uint16_t message_size);
uint16_t fdQueueReceive(FDQueueHandle_t queue, uint8_t *message, uint16_t message_size_max);
#endif //DATASTRUCTS_QUEUE_H

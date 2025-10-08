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


typedef Queue* Queue_Handle;
typedef FDQueue* FDQueue_Handle;


void Queue_Init(Queue *queue, uint8_t *buffer, uint16_t message_size, uint16_t queue_size);
Queue_Handle Queue_Create(uint16_t message_size, uint16_t queue_size);
static bool Queue_isEmpty(Queue *queue);
static bool Queue_isFull(Queue *queue);
uint16_t Queue_GetFreeSpace(Queue *queue);
bool Queue_Send(Queue *queue,const uint8_t *message);
void Queue_SendForce(Queue *queue,const uint8_t *message);
bool Queue_Receive(Queue *queue, uint8_t *message);

bool FDQueue_Init(FDQueue *queue, uint8_t *buffer, uint16_t buffer_size, uint8_t queue_size);
FDQueue_Handle FDQueue_Create(uint16_t buffer_size, uint8_t queue_size);
bool FDQueue_isEmpty(FDQueue *queue);
bool FDQueue_isFull(FDQueue *queue);
bool FDQueue_Send(FDQueue *queue,const uint8_t *message, uint16_t message_size);
bool FDQueue_SendBegin(FDQueue *queue,const uint8_t *message, uint16_t message_size);
bool FDQueue_SendEnd(FDQueue *queue,const uint8_t *message, uint16_t message_size);
uint16_t FDQueue_Receive(FDQueue *queue, uint8_t *message, uint16_t message_size_max);
#endif //DATASTRUCTS_QUEUE_H

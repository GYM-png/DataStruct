/**
 * ==================================================
 *  @file ringBuffer.c
 *  @brief TODO 描述该文件的功能
 *  @author GYM (48060945@qq.com)
 *  @date 2025-11-26 下午10:06
 *  @version 1.0
 *  @copyright Copyright (c) 2025 GYM. All Rights Reserved.
 * ==================================================
 */


#include "messageBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define BufferASSERT(x)  do{ if(!(x)){printf("assert faild %s:%d\n", #x, __LINE__); return;}}while(0)
#define BufferASSERT_RETURN(x, ret)  do{ if(!(x)){printf("assert faild %s:%d\n", #x, __LINE__); return ret;} }while(0)

#define Buffer_Malloc(size) malloc(size)


typedef struct __RingBuffer_t
{
    uint8_t *buffer;
    uint32_t size;
    uint32_t write_index : 31;
    uint32_t write_index_mirror : 1;
    uint32_t read_index : 31;
    uint32_t read_index_mirror : 1;
    uint8_t is_message_buffer;
}RingBuffer_t;

void ringBufferInit(RingBuffer_t* ringBuffer, uint8_t* buffer, uint32_t bufferSize)
{
    BufferASSERT(ringBuffer != NULL);
    BufferASSERT(buffer != NULL);
    BufferASSERT(bufferSize > 0);
    ringBuffer->buffer = buffer;
    ringBuffer->write_index = 0;
    ringBuffer->write_index_mirror = 0;
    ringBuffer->read_index = 0;
    ringBuffer->read_index_mirror = 0;
    ringBuffer->size = bufferSize;
    ringBuffer->is_message_buffer = 0;
}

RingBuffer_t* ringBufferCreat(uint32_t buffer_size)
{
    RingBuffer_t* ringBuffer = (RingBuffer_t*)Buffer_Malloc(sizeof(RingBuffer_t));
    BufferASSERT_RETURN(ringBuffer != NULL, NULL);

    uint8_t *buffer = (uint8_t *)Buffer_Malloc(buffer_size);
    BufferASSERT_RETURN(buffer != NULL, NULL);

    ringBufferInit(ringBuffer, buffer, buffer_size);
    return ringBuffer;
}

bool ringBufferIsEmpty(RingBuffer_t* ringBuffer)
{
    BufferASSERT_RETURN(ringBuffer != NULL, true);
    return (ringBuffer->write_index == ringBuffer->read_index) && (ringBuffer->write_index_mirror == ringBuffer->read_index_mirror);
}

bool ringBufferIsFull(RingBuffer_t* ringBuffer)
{
    BufferASSERT_RETURN(ringBuffer != NULL, false);
    return (ringBuffer->write_index == ringBuffer->read_index) && (ringBuffer->write_index_mirror != ringBuffer->read_index_mirror);
}

uint32_t ringBufferGetFreeSize(RingBuffer_t* ringBuffer) 
{
    BufferASSERT_RETURN(ringBuffer != NULL, 0);
    if (ringBufferIsFull(ringBuffer))
    {
        return 0;
    }
    else if (ringBufferIsEmpty(ringBuffer))
    {
        return ringBuffer->size;
    }
    else
    {
        if (ringBuffer->write_index > ringBuffer->read_index)
        {
            return ringBuffer->size - ringBuffer->write_index + ringBuffer->read_index;
        }
        else
        {
            return ringBuffer->read_index - ringBuffer->write_index - 1;
        }
    }
}

bool ringBufferWrite(RingBuffer_t* ringBuffer, uint8_t *data, uint32_t data_size)
{
    BufferASSERT_RETURN(ringBuffer != NULL, false);
    BufferASSERT_RETURN(data != NULL, false);
    BufferASSERT_RETURN(data_size > 0, false);

    if (ringBufferIsFull(ringBuffer))
    {
        return false;
    }

    if (ringBufferGetFreeSize(ringBuffer) < data_size)
    {
        return false;
    }

    if (ringBuffer->size - ringBuffer->write_index >= data_size)
    {
        memcpy(&ringBuffer->buffer[ringBuffer->write_index], data, data_size);
        ringBuffer->write_index = (ringBuffer->write_index + data_size) % ringBuffer->size;
        if (ringBuffer->write_index == 0)
        {
            ringBuffer->write_index_mirror ^= 1;
        }
    }
    else
    {
        uint32_t first_size = ringBuffer->size - ringBuffer->write_index;
        memcpy(&ringBuffer->buffer[ringBuffer->write_index], data, first_size);
        memcpy(&ringBuffer->buffer[0], &data[first_size], data_size - first_size);
        ringBuffer->write_index = data_size - first_size;
    }
    return true;
}

void ringBufferWriteForce(RingBuffer_t* ringBuffer, uint8_t *data, uint32_t data_size)
{
    BufferASSERT(ringBuffer != NULL);
    BufferASSERT(data != NULL);
    BufferASSERT(data_size > 0);

    for (uint32_t i = 0; i < data_size; ++i)
    {
        if (ringBufferIsFull(ringBuffer))
        {
            ringBuffer->read_index = (ringBuffer->read_index + 1) % ringBuffer->size;
            ringBuffer->buffer[ringBuffer->write_index] = data[i];
            ringBuffer->write_index = (ringBuffer->write_index + 1) % ringBuffer->size;
        }
        else
        {
            ringBuffer->buffer[ringBuffer->write_index] = data[i];
            ringBuffer->write_index = (ringBuffer->write_index + 1) % ringBuffer->size;
        }
        if (ringBuffer->read_index == ringBuffer->write_index)
        {
            ringBuffer->write_index_mirror ^= 1;
        }
    }
}

uint32_t ringBufferRead(RingBuffer_t* ringBuffer, uint8_t *data, uint32_t data_size)
{
    BufferASSERT_RETURN(ringBuffer != NULL, 0);
    BufferASSERT_RETURN(data != NULL, 0);
    BufferASSERT_RETURN(data_size > 0, 0);

    if (ringBufferIsEmpty(ringBuffer))
    {
        return 0;
    }

    for (uint32_t i = 0; i < data_size; ++i)
    {
        if(ringBufferIsEmpty(ringBuffer))
        {
            return i;
        }
        else
        {
            data[i] = ringBuffer->buffer[ringBuffer->read_index];
            ringBuffer->read_index = (ringBuffer->read_index + 1) % ringBuffer->size;
            if (ringBuffer->read_index == ringBuffer->write_index)
            {
                ringBuffer->read_index_mirror ^= 1;
            }
        }
    }
    return data_size;
}

void messageBufferInit(RingBuffer_t *messge_buffer, uint8_t *buffer, uint32_t buffer_size)
{
    ringBufferInit(messge_buffer, buffer, buffer_size);
}

RingBuffer_t* messageBufferCreat(uint32_t buffer_size)
{
    return ringBufferCreat(buffer_size);
}

bool messageBufferIsEmpty(RingBuffer_t* message_buffer)
{
    return ringBufferIsEmpty(message_buffer);
}

bool messageBufferIsFull(RingBuffer_t* message_buffer)
{
    return ringBufferIsFull(message_buffer);
}

uint32_t messageBufferGetFreeSize(RingBuffer_t* message_buffer) 
{
    return ringBufferGetFreeSize(message_buffer);
}

bool messageBufferWrite(RingBuffer_t* message_buffer, uint8_t *data, uint32_t data_size)
{
    BufferASSERT_RETURN(message_buffer != NULL, false);
    BufferASSERT_RETURN(data != NULL, false);

    if (ringBufferIsFull(message_buffer))
    {
        return false;
    }

    if (ringBufferGetFreeSize(message_buffer) < data_size + sizeof(uint32_t))
    {
        return false;
    }

    ringBufferWrite(message_buffer, (uint8_t *)&data_size, sizeof(uint32_t));
    ringBufferWrite(message_buffer, data, data_size);
    return true;
}

uint32_t messageBufferRead(RingBuffer_t* message_buffer, uint8_t *data, uint32_t data_size)
{
    BufferASSERT_RETURN(message_buffer != NULL, 0);
    BufferASSERT_RETURN(data != NULL, 0);

    if (ringBufferIsEmpty(message_buffer))
    {
        return 0;
    }

    uint32_t message_size = 0;
    ringBufferRead(message_buffer, (uint8_t *)&message_size, sizeof(uint32_t));
    if (message_size > 0)
    {
        return ringBufferRead(message_buffer, data, message_size);
    }
    return 0;
}
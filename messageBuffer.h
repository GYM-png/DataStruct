/**
 * ==================================================
 *  @file messageBuffer.h
 *  @brief TODO 描述该文件的功能
 *  @author GYM (48060945@qq.com)
 *  @date 2025-11-26 下午10:06
 *  @version 1.0
 *  @copyright Copyright (c) 2025 GYM. All Rights Reserved.
 * ==================================================
 */


#ifndef DATASTRUCTS_MESSAGEBUFFER_H
#define DATASTRUCTS_MESSAGEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

struct __RingBuffer_t;
typedef struct __RingBuffer_t* ringBufferHandle_t;
typedef struct __RingBuffer_t* messageBufferHandle_t;

messageBufferHandle_t messageBufferCreat(uint32_t buffer_size);
bool messageBufferWrite(messageBufferHandle_t message_buffer, uint8_t *data, uint32_t data_size);
uint32_t messageBufferRead(messageBufferHandle_t message_buffer, uint8_t *data, uint32_t data_size);
#endif //DATASTRUCTS_MESSAGEBUFFER_H

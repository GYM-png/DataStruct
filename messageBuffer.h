/**
 * ==================================================
 *  @file       messageBuffer.h
 *  @brief      环形缓冲区 + 变长消息缓冲区完整实现（全部公开接口）
 *  @author     GYM (48060945@qq.com)
 *  @date       2025-11-26
 *  @version    1.0
 *  @copyright  Copyright (c) 2025 GYM. All Rights Reserved.
 * ==================================================
 */

#ifndef DATASTRUCTS_MESSAGEBUFFER_H
#define DATASTRUCTS_MESSAGEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

/** @brief 不透明句柄（RingBuffer 和 MessageBuffer 共用） */
typedef struct __RingBuffer_t* ringBufferHandle_t;
typedef struct __RingBuffer_t* messageBufferHandle_t;

/* ==================== 普通环形缓冲区接口 ==================== */

/**
 * @brief 使用外部提供的缓冲区初始化一个环形缓冲区（静态内存）
 *
 * @param ringBuffer   环形缓冲区对象指针（已分配好结构体空间）
 * @param buffer       外部提供的存储空间
 * @param bufferSize   存储空间大小（字节）
 */
void ringBufferInit(ringBufferHandle_t ringBuffer, uint8_t* buffer, uint32_t bufferSize);

/**
 * @brief 动态创建一个环形缓冲区（内部 malloc）
 *
 * @param buffer_size  缓冲区大小（字节）
 * @return ringBufferHandle_t 成功返回句柄，失败返回 NULL
 */
ringBufferHandle_t ringBufferCreat(uint32_t buffer_size);

/**
 * @brief 判断环形缓冲区是否为空
 *
 * @param ringBuffer  环形缓冲区句柄
 * @return true   空
 * @return false  非空
 */
bool ringBufferIsEmpty(ringBufferHandle_t ringBuffer);

/**
 * @brief 判断环形缓冲区是否已满
 *
 * @param ringBuffer  环形缓冲区句柄
 * @return true   已满
 * @return false  未满
 */
bool ringBufferIsFull(ringBufferHandle_t ringBuffer);

/**
 * @brief 获取环形缓冲区当前剩余可用空间（字节）
 *
 * @param ringBuffer  环形缓冲区句柄
 * @return uint32_t   剩余字节数
 */
uint32_t ringBufferGetFreeSize(ringBufferHandle_t ringBuffer);

/**
 * @brief 向环形缓冲区写入数据（空间不足时返回失败）
 *
 * @param ringBuffer  环形缓冲区句柄
 * @param data        待写入数据指针
 * @param data_size   待写入字节数
 * @return true   写入成功
 * @return false  空间不足或参数错误
 */
bool ringBufferWrite(ringBufferHandle_t ringBuffer, const uint8_t *data, uint32_t data_size);

/**
 * @brief 强制写入数据（空间不足时覆盖最旧数据）
 *
 * @param ringBuffer  环形缓冲区句柄
 * @param data        待写入数据指针
 * @param data_size   待写入字节数
 */
void ringBufferWriteForce(ringBufferHandle_t ringBuffer, const uint8_t *data, uint32_t data_size);

/**
 * @brief 从环形缓冲区读取数据
 *
 * @param ringBuffer  环形缓冲区句柄
 * @param data        用于接收数据的缓冲区
 * @param data_size   希望读取的最大字节数
 * @return uint32_t   实际读取的字节数（可能小于 data_size）
 */
uint32_t ringBufferRead(ringBufferHandle_t ringBuffer, uint8_t *data, uint32_t data_size);

/* ==================== 变长消息缓冲区接口 ==================== */

/**
 * @brief 创建一个消息缓冲区（内部动态分配，等价于 ringBufferCreat）
 *
 * @param buffer_size  缓冲区总字节数
 * @return messageBufferHandle_t 成功返回句柄，失败返回 NULL
 */
messageBufferHandle_t messageBufferCreat(uint32_t buffer_size);

/**
 * @brief 向消息缓冲区写入一条完整消息（自动添加 4 字节长度前缀）
 *
 * @param message_buffer  消息缓冲区句柄
 * @param data            消息数据指针
 * @param data_size       消息数据长度
 * @return true   写入成功（空间足够）
 * @return false  空间不足以容纳完整消息（含长度前缀）
 */
bool messageBufferWrite(messageBufferHandle_t message_buffer,
                        const uint8_t *data, uint32_t data_size);

/**
 * @brief 从消息缓冲区读取一条完整消息
 *
 * @param message_buffer  消息缓冲区句柄
 * @param data            用于接收消息的缓冲区
 * @param data_size       接收缓冲区最大容量
 * @return uint32_t       实际读取的消息字节数；0 表示当前无完整消息可读
 */
uint32_t messageBufferRead(messageBufferHandle_t message_buffer,
                           uint8_t *data, uint32_t data_size);

/**
 * @brief 强制写入一条完整消息（空间不足时丢弃最旧的完整消息）
 *
 * @param message_buffer  消息缓冲区句柄
 * @param data            消息数据指针
 * @param data_size       消息数据长度
 */
void messageBufferWriteForce(messageBufferHandle_t message_buffer,
                             const uint8_t *data, uint32_t data_size);

#endif // DATASTRUCTS_MESSAGEBUFFER_H

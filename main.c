#include <stdio.h>
#include "Queue.h"
#include <string.h>

void queue_test()
{
    uint8_t buffer[50 * 5] = {0};
    Queue queue;
    queueInit(&queue, buffer, 50, 5);


    uint8_t message1[50] = "Hello World1!1111";
    uint8_t message2[50] = "Hello World2!22";
    uint8_t message3[50] = "Hello World3!3";
    uint8_t message4[50] = "Hello World4!55546";
    uint8_t message5[50] = "Hello World5!555asfas5";


    queueSend(&queue, message1);
    queueSend(&queue, message2);
    queueSend(&queue, message3);
    queueSend(&queue, message4);
    queueSend(&queue, message5);
    queueSendForce(&queue, message5);


    printf("%d\n", queueGetFreeSpace(&queue));

    uint8_t messageRec[50];
    for (int i = 0; i < 10; ++i) {
        bool statue = queueReceive(&queue, messageRec);
        if (!statue) {
            break;
        }
        printf("%s\n", messageRec);
    }

    printf("***********************test2***********************************\n");

    Queue *queue_ptr = queueCreate(50, 5);
    queueSend(queue_ptr, message1);
    queueSend(queue_ptr, message2);
    queueSend(queue_ptr, message3);
    queueSend(queue_ptr, message4);
    queueSend(queue_ptr, message5);
    printf("%d\n", queueGetFreeSpace(queue_ptr));
    for (int i = 0; i < 10; ++i) {
        bool statue = queueReceive(queue_ptr, messageRec);
        if (!statue) {
            break;
        }
        printf("%s\n", messageRec);
    }
}


void fdqueue_test() {
    uint8_t buffer[1024] = {0};
#define INIT_MODE 1


#if INIT_MODE == 0
    FDQueue fdqueue = {0};
    fdQueueInit(&fdqueue, buffer, sizeof(buffer), 6);
#else
    FDQueueHandle_t fdqueue = NULL;
    fdqueue = fdQueueCreate(1025, 6);
#endif
    uint8_t message1[50] = "Hello World1!1111";
    uint8_t message2[50] = "Hello World2!22";
    uint8_t message3[50] = "Hello World3!3";
    uint8_t message4[50] = "Hello World4!55546";
    uint8_t message5[50] = "Hello World5!555asfas5";
    uint8_t message6_1[50] = "Hello ";
    uint8_t message6_2[50] = "World6!123456";
    uint8_t message7[50] = "Hello World7force132";
#if INIT_MODE == 0
    fdQueueSend(&fdqueue, message1, strlen(message1));
    fdQueueSend(&fdqueue, message2, strlen(message2));
    fdQueueSend(&fdqueue, message3, strlen(message3));
    fdQueueSend(&fdqueue, message4, strlen(message4));
    fdQueueSend(&fdqueue, message5, strlen(message5));
#else
    fdQueueSend(fdqueue, message1, strlen(message1));
    fdQueueSend(fdqueue, message2, strlen(message2));
    fdQueueSend(fdqueue, message3, strlen(message3));
    fdQueueSend(fdqueue, message4, strlen(message4));
    fdQueueSend(fdqueue, message5, strlen(message5));
    fdQueueSendBegin(fdqueue, message6_1, strlen(message6_1));
    fdQueueSendBegin(fdqueue, message6_1, strlen(message6_1));
    fdQueueSendEnd(fdqueue, message6_2, strlen(message6_2));
    printf("%d\n", strlen(message1) + strlen(message2) + strlen(message3) + strlen(message4) + strlen(message5) + strlen(message6_1) + strlen(message6_2));
    fdQueueSendForce(fdqueue, message7, strlen(message7));
#endif
    uint8_t messageRec[50];
    for (int i = 0; i < 10; ++i) {
#if INIT_MODE == 0
        uint16_t len = FDQueue_Receive(&fdqueue, messageRec, sizeof(messageRec));
#else
        uint16_t len = fdQueueReceive(fdqueue, messageRec, sizeof(messageRec));
#endif
        if (!len) {
            break;
        }
        for (uint16_t j = 0; j < len; ++j) {
            printf("%c", messageRec[j]);
        }
        printf("\n");
    }

}


int main(void) {
//    queue_test();
    fdqueue_test();
    return 0;
}

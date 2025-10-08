#include <stdio.h>
#include "Queue.h"
#include <string.h>

void queue_test()
{
    uint8_t buffer[50 * 5] = {0};
    Queue queue;
    Queue_Init(&queue, buffer, 50, 5);


    uint8_t message1[50] = "Hello World1!1111";
    uint8_t message2[50] = "Hello World2!22";
    uint8_t message3[50] = "Hello World3!3";
    uint8_t message4[50] = "Hello World4!55546";
    uint8_t message5[50] = "Hello World5!555asfas5";


    Queue_Send(&queue, message1);
    Queue_Send(&queue, message2);
    Queue_Send(&queue, message3);
    Queue_Send(&queue, message4);
    Queue_Send(&queue, message5);
    Queue_SendForce(&queue, message5);


    printf("%d\n", Queue_GetFreeSpace(&queue));

    uint8_t messageRec[50];
    for (int i = 0; i < 10; ++i) {
        bool statue = Queue_Receive(&queue, messageRec);
        if (!statue) {
            break;
        }
        printf("%s\n", messageRec);
    }

    printf("***********************test2***********************************\n");

    Queue *queue_ptr = Queue_Create(50, 5);
    Queue_Send(queue_ptr, message1);
    Queue_Send(queue_ptr, message2);
    Queue_Send(queue_ptr, message3);
    Queue_Send(queue_ptr, message4);
    Queue_Send(queue_ptr, message5);
    printf("%d\n", Queue_GetFreeSpace(queue_ptr));
    for (int i = 0; i < 10; ++i) {
        bool statue = Queue_Receive(queue_ptr, messageRec);
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
    FDQueue_Init(&fdqueue, buffer, sizeof(buffer), 6);
#else
    FDQueue_Handle fdqueue = NULL;
    fdqueue = FDQueue_Create(1024, 6);
#endif
    uint8_t message1[50] = "Hello World1!1111";
    uint8_t message2[50] = "Hello World2!22";
    uint8_t message3[50] = "Hello World3!3";
    uint8_t message4[50] = "Hello World4!55546";
    uint8_t message5[50] = "Hello World5!555asfas5";
    uint8_t message6_1[50] = "Hello ";
    uint8_t message6_2[50] = "World6!123456";
#if INIT_MODE == 0
    FDQueue_Send(&fdqueue, message1, strlen(message1));
    FDQueue_Send(&fdqueue, message2, strlen(message2));
    FDQueue_Send(&fdqueue, message3, strlen(message3));
    FDQueue_Send(&fdqueue, message4, strlen(message4));
    FDQueue_Send(&fdqueue, message5, strlen(message5));
#else
    FDQueue_Send(fdqueue, message1, strlen(message1));
    FDQueue_Send(fdqueue, message2, strlen(message2));
    FDQueue_Send(fdqueue, message3, strlen(message3));
    FDQueue_Send(fdqueue, message4, strlen(message4));
    FDQueue_Send(fdqueue, message5, strlen(message5));
    FDQueue_SendBegin(fdqueue, message6_1, strlen(message6_1));
    FDQueue_SendBegin(fdqueue, message6_1, strlen(message6_1));
    FDQueue_SendEnd(fdqueue, message6_2, strlen(message6_2));
#endif
    uint8_t messageRec[50];
    for (int i = 0; i < 10; ++i) {
#if INIT_MODE == 0
        uint16_t len = FDQueue_Receive(&fdqueue, messageRec, sizeof(messageRec));
#else
        uint16_t len = FDQueue_Receive(fdqueue, messageRec, sizeof(messageRec));
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

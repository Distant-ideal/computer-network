/*************************************************************************
	> File Name: socket_queue.c
	> Author: gongpixi
	> Mail: 1371854096@qq.com
	> Created Time: 2018年12月10日 星期一 18时27分09秒
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

typedef struct Queue {
    char **data;
    int *port;
    int head, tail, length, cnt;
    pthread_mutex_t mutex;
    pthread_cond_t is_full;
    pthread_cond_t is_empty;
} Queue;

Queue *init(int n) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->length = n; 
    q->cnt = 0;
    q->data = (char **)malloc(sizeof(char*) * n );
    q->port = (int *)malloc(sizeof(int) * n);
    q->head = 0;
    q->tail = -1;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->is_full, NULL);
    pthread_cond_init(&q->is_empty, NULL);
    return q;
}

int push (Queue *q, char *ip, int port) {
    if(q->cnt == q->length) return 0;
    q->tail += 1;
    if(q->tail >= q->length) q->tail -= q->length;
    q->cnt += 1;
    q->data[q->tail] = strdup(ip);
    q->port[q->tail] = port;
    return 1;
}

int empty(Queue *q) {
    return q->head > q->tail;
}

void pop(Queue *q) {
    //if(pthread_mutex_lock(&q->mutex) != 0) {
    //    return (void)-1;
    //}
    //if(empty(q)) return ;
    q->head += 1;
    if(q->head > q->length) q->head -= q->length;
    q->cnt -= 1;
    //if(pthread_mutex_unlock(&q->mutex) != 0) {
    //    return (void)-1;
    //} 
    return ;
}
 
int front(Queue *q) {
    if(empty(q)) return 0;
    return q->data[q->head], q->port[q->head];
}

void output(Queue *q) {
    for(int i = 0 ; i < q->cnt; i++) {
        int ind = (q->head + i) % q->length;
        printf("%s %d\n", q->data[ind], q->port[ind]);
    }
    return ;
}

/*void clear(Queue *q) {
    if(q == NULL) return ;
    free(q->data);
    free(q);
    return ;
}*/
 
int search(Queue *q, char *ip) {
    for(int i = 0; i < 20; i++) {
        if(q->data[i] == ip) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Queue *q[5];
    for(int i = 0; i < 5; i++) {
        q[i] = init(1000);
    }
    char IP[100000];
    int port;
    for(int i = 0; i < 3; i++) {
        scanf("%d", &port);
        scanf("%s", IP);
        push(q[0], IP, port);
    }
    printf("Queue:\n");
    output(q[0]);
    pop(q[0]);
    printf("Queue:\n");
    output(q[0]);
    return 0;
}

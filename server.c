/*************************************************************************
	> File Name: server.c
	> Author: gongpixi
	> Mail: 1371854096@qq.com
	> Created Time: 2018年11月13日 星期二 21时01分33秒
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
    if(pthread_mutex_lock(&q->mutex) != 0) {
        return -1;
    }
    if(q->cnt == q->length) return 0;
    q->tail += 1;
    if(q->tail >= q->length) q->tail -= q->length;
    q->cnt += 1;
    q->data[q->tail] = strdup(ip);
    q->port[q->tail] = port;
	if(pthread_mutex_unlock(&q->mutex) != 0) {
        return  -1;
    }
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
    if(pthread_mutex_lock(&q->mutex) != 0) {
        return (void)-1;
    }
    for(int i = 0 ; i < q->cnt; i++) {
        int ind = (q->head + i) % q->length;
        printf("%s %d\n", q->data[ind], q->port[ind]);
    }
	if(pthread_mutex_unlock(&q->mutex) != 0) {
        return (void)-1;
    }
    return ;
}

/*void clear(Queue *q) {
    if(q == NULL) return ;
    free(q->data);
    free(q);
    return ;
}*/
 
int client_(char *IP, int port) {
    struct sockaddr_in server_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(IP);
    bzero(&(server_addr.sin_zero), 8);
    
    int x = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    if (x < -1) return 1;
    return sockfd;
}

void *func(void *argv) {
    Queue *q = (Queue *)argv;
    output(q);
    while(1) {
        if(empty(q)) {
            sleep(2);
            continue;
        }
        //long long a = pthread_self();
        //printf("%lld:\n", a);
        output(q);
        
        int new2_fd = client_(q->data[0], q->port[0]);
        printf("%d\n", new2_fd);
        if(new2_fd < 0) {
            printf("已移除%s\n", q->data[0]);
            sleep(1);
            pop(q);
            continue;
        }
        
        int numbytes;

        for(int i = 1; i < q->cnt; i++) {
            //printf("访客%s %d已退出\n", q->data[0], q->port[0]);
            int new3_fd = client_(q->data[i], q->port[i]);
            numbytes = send(new3_fd, "正在排队!\n", 21, 0);  
        }

        //printf("123465\n") ;
        //int numbytes;
        char IP[1000];
        char buff[BUFSIZ];
        strcpy(IP, q->data[0]);
        while((numbytes = recv(new2_fd, buff, BUFSIZ, 0)) > 0) { // 接收数据
            char filename[200] = {0}, data[BUFSIZ] = {0};
            int t = buff[0] - '0', k = strlen(IP);
            mkdir(IP, 0775); // 同linux mkdir建立目录后的权限一致
            strncpy(filename, IP, k);
            filename[k] = '/';
            strncpy(filename + 1 + k, buff + 1, t);
            strncpy(data, buff + 1 + t, strlen(buff + 1 + t));
            FILE *fd;
            fd = fopen(filename, "a+");
            fwrite(data, sizeof(char), strlen(data), fd);
            fclose(fd);
            memset(buff, 0, sizeof(buff));    
        }
        printf("访客%s %d已退出\n", q->data[0], q->port[0]);
        pop(q);
        //output(q);
        sleep(2);
        close(new2_fd);

        
    }
    return NULL;
}

int search(Queue *q, char *ip) {
    for(int i = 0; i < q->cnt; i++) {
        char ip2[1000];
        strcpy(ip2, q->data[i]);
        if(ip2 == ip) {
            return 1;
        }
    }
    return 0;
}

#define max_n 2

int main(int argc, char *argv[]) {
    int n = 0, s = 0, x = -1, port;
    int fd, new_fd, numbytes;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char buff[BUFSIZ]; //存放套接字

    socklen_t struct_len;
    server_addr.sin_family = AF_INET;
    //sin.family指协议族
    server_addr.sin_port = htons(1234);
    //sin.port存储端口号
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //按照网络字节顺序存储ｉｐ地址
    bzero(&(server_addr.sin_zero), 8);
    //为了让sockeaddr和sockaddr_in两个数据保持大小相同而保留空字节
    struct_len = sizeof(struct sockaddr_in);
    //获取sockaddr缓冲区长度
    fd = socket(AF_INET, SOCK_STREAM, 0);
    //socket函数创建新的套接字，如果创建成功就返回新创建的套接字描述符失败会返回－１
    
    while(bind(fd, (struct sockaddr  *)&server_addr, struct_len) == -1);
    //如果绑定成功返回０不成功返回－１无限循环
    printf("Bind Success!\n");
    while(listen(fd, 10) == -1);
    //监听套接字上限为１０如果成功返回０失败返回－１
    printf("Listening....\n");
    //printf("123\n");
    printf("Ready for Accept,Waitting...\n");
    //printf("456");
    Queue *q[max_n];
    char IP[1000000];
    //printf("12233");
    for(int i = 0; i < max_n; i++) {
        q[i] = init(1000);
    }
    //printf("123");
    pthread_t t[max_n];
    for(int i = 0; i < max_n; i++) {
        pthread_create(&t[i], NULL, func, (void *)q[i]);
    }
    //printf("11111");
    while(1) {
        new_fd = accept(fd, (struct sockaddr *)&client_addr, &struct_len);
        printf("出现访客\n");
        port = htons(client_addr.sin_port);
        //printf("112");
        strcpy(IP, inet_ntoa(client_addr.sin_addr));
        int h = 0;
        printf("线程%d:\n", n % max_n);
		printf("%s %d\n", IP, port);
        //printf("123456789\n");
        for(int i = 0 ; i < max_n; i++) {
            if(search(q[i], IP)) {
                h = 1;
                break;
           }
        }
        if(h) continue; 
        printf("%d", n);
        output(q[n % max_n]);
        n += 1;
        push(q[n % max_n], IP, 1234);
    }
    return 0;
}

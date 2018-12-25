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
#include "Queue.h"
#include "search.h"
#include "Daemon.h"

#define max_n 9

int a[max_n] = {0};

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

        /*for(int i = 1; i < q->cnt; i++) {
            //printf("访客%s %d已退出\n", q->data[0], q->port[0]);
            int new3_fd = client_(q->data[i], q->port[i]);
            numbytes = send(new3_fd, "正在排队!\n", 21, 0);  
        }*/
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
            //printf("%s\n", filename);
            FILE *fd;
            char s2[100] = "caveat.log";
            int x = strlen(filename);
            char s[1000];
            char s3[10000];
            strncpy(s, filename + k + 1, x - k  + 1);
            //printf("%d\n", t);
            int x2 = strlen(buff);
            printf("buffsize: %d ", x2);
            if(x2 - t != 1) {
                if(!strcmp(s, s2)) {
                    //printf("%d %d\n", t, x);
                    //printf("%d\n", t);
                    printf("%s Alarm, Please go to %s folder to view\n", IP, IP);
                }else {
                    //printf("%d %d\n", t, x);
                    printf("%s has been sent\n", filename);    
                }
                fd = fopen(filename, "a+");
                fwrite(data, sizeof(char), strlen(data), fd);
                //printf("%s has been sent\n", IP);
                fclose(fd);
                memset(buff, 0, sizeof(buff));    
            } else {
                printf("%s Temporarily no alarm message\n", IP);
            }
        }
        printf("访客%s %d已退出\n", q->data[0], q->port[0]);
        pop(q);
        //output(q);
        /*for(int m = 0; m < max_n; m++) {
            a[m] = q[m]->cnt;
        }*/
        sleep(2);
        close(new2_fd);        
    }
    return NULL;
}

//#define max_n 9

int main(int argc, char *argv[]) {
    //mydaemon();
    create_daemon();
    while(1){
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
    printf("Ready for Accept,Waitting...\n");
    
    Queue *q[max_n];
    char IP[1000000];
    
    for(int i = 0; i < max_n; i++) {
        q[i] = init(1000);
    }
   
    //int a[max_n] = {0};

    pthread_t t[max_n];
    for(int i = 0; i < max_n; i++) {
        pthread_create(&t[i], NULL, func, (void *)q[i]);
    }
    
    //int a[max_n] = {0};
    while(1) {
        new_fd = accept(fd, (struct sockaddr *)&client_addr, &struct_len);
        //printf("出现访客\n");
        port = htons(client_addr.sin_port);
        strcpy(IP, inet_ntoa(client_addr.sin_addr));
        int h = 0;
        for(int m = 0; m < max_n; m++) {
            a[m] = q[m]->cnt;
        }
        int l = search_in(a, max_n);
        //printf("线程%d:\n", n % max_n);
		//printf("%s %d\n", IP, port);
        for(int i = 0 ; i < max_n; i++) {
            if(search(q[i], IP)) {
                h = 1;
                break;
           }
        }
        if(h) continue; 
        printf("出现访客\n");
        printf("线程%d:\n", n % max_n);
	    printf("%s %d\n", IP, port);
        printf("%d", l);
        output(q[l]);
        n += 1;
        push(q[l], IP, 1234);
        a[l] = q[l]->cnt;
    }
    }
    return 0;
}

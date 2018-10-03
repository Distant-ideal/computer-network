#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#define MAXSIZE 4096

void transfer (int sockfd, char *name, char *process) {
	char buffer[MAXSIZE] = {0};
    char buf[MAXSIZE] = {0};
    FILE *fd;
    fd = popen(process, "r");
    fread(buffer, sizeof(char), sizeof(buffer), fd);
    buf[0] = strlen(name) + '0';
    int t = buf[0] - '0';
    strncpy(buf + 1, name, t);
    strncpy(buf + 1 + strlen(name), buffer, strlen(buffer));
    send(sockfd, buf, strlen(buf), 0);
    pclose(fd);
}

int main(int argc,char *argv[]) {
    // ./a.out src
    int sockfd;
    //char buf[MAXSIZE];
    struct sockaddr_in addr;
    int SERV_PORT = atoi(argv[1]);
    char *IP = argv[2];
    //建立socket套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        printf("-1");
    }
    bzero(&addr,sizeof(addr));

    //初始化ip+port
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERV_PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    //connect将sockfd套接字描述符与服务器端的ip+port联系起来
    if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)) < 0) {
        printf("-2");
    }
    while(1) {
	transfer(sockfd, "disk.log", "./disk.sh");
    sleep(1);
	transfer(sockfd, "cpu.log", "./cpu.sh");
    sleep(1);
    transfer(sockfd, "mem.log", "./mem.sh");
    printf("Sent successfully");
        sleep(5);
    }
    close(sockfd);
    return 0;
 }

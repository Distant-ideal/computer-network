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

int main(int argc, char *argv[])
{
    int fd, new_fd, numbytes,i;
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
    while(bind(fd, (struct sockaddr *)&server_addr, struct_len) == -1);
    //如果绑定成功返回０不成功返回－１无限循环
    printf("Bind Success!\n");
    while(listen(fd, 10) == -1);
    //监听套接字上限为１０如果成功返回０失败返回－１
    printf("Listening....\n");
    printf("Ready for Accept,Waitting...\n");
    pid_t pid;
    while(1) {
        new_fd = accept(fd, (struct sockaddr *)&client_addr, &struct_len);
        //接收连接请求，成功返回客户端的文件描述符，失败返回－１
        printf("Get the Client.\n");
        numbytes = send(new_fd,"Welcome to my server\n",21,0); 
        pid = fork();
        if (pid == -1) printf("fork ERROR");
        if (pid == 0) {
            close(fd);
            char *IP;
            while((numbytes = recv(new_fd, buff, BUFSIZ, 0)) > 0) {
            	char filename[1000] = {0};
                char output[1000] = {0};
            	int port;
            	int s = buff[0] - '0';
            	FILE *fd;
                IP = inet_ntoa(client_addr.sin_addr);
		printf("%s %s\n", IP,buff);
                mkdir(IP, 0755); 
                strncpy(output, buff + 1 + s, strlen(buff + 1 + s));
                strncpy(filename, IP, strlen(IP));
                strncpy(filename + strlen(IP), "/", 1);
                strncpy(filename + strlen(IP) + 1 , buff + 1, buff[0] - '0');
                fd = fopen(filename, "a+");
                fwrite(output, sizeof(char), strlen(output), fd);
                fclose(fd);
                memset(buff, '\0', sizeof(buff));
            }
            exit(EXIT_SUCCESS);
        }
        else close(new_fd);
    }
    return 0;
}

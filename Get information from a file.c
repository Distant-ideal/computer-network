#include <stdio.h>
#include <string.h>

void find(char *filename, char *name, char *storage) {
    FILE *fp = NULL;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(filename,"r");
    if(fp == NULL) {
        printf("文件读取失败!\n");
    }
    int s = strlen(name);
    while ((read = getline(&line, &len, fp)) != -1) {
        int sum = strlen(line);
        char *str = strstr(line, name);
        if(str) {
            strncpy(storage, line + s + 1, sum - s - 2);
        }
    }
}


int main() {
    char storage[1000];
    char ip[100];
    char port[100];
    memset(ip, 0, sizeof(ip));
    memset(port, 0, sizeof(port));
    memset(storage, 0, sizeof(storage));
    find("health.cof", "ip", ip);
    printf("%s\n", ip);
    find("health.cof", "port", port);
    printf("%s\n", port);
    find("health.cof", "location", storage);
    printf("%s\n", storage);
    return 0;
}
    

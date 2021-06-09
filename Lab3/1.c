#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAXLEN 1050000

struct Pipe
{
    int fd_send;
    int fd_recv;
};

void *handle_chat(void *data)
{
    int i;
    struct Pipe *pipe = (struct Pipe *)data;
    char message_recv[MAXLEN];
    char evey_message[MAXLEN];
    char message_send[MAXLEN];
    memset(message_recv, 0, sizeof(message_recv));
    memset(evey_message, 0, sizeof(evey_message));
    memset(message_send, 0, sizeof(message_send));
    ssize_t len_recv = 0, len_send = 0;
    strcpy(message_send, "Message:");
    while (len_recv = recv(pipe->fd_send, message_recv, 1000, 0) > 0)
    {
        char *p = message_recv;
        while (*p != '\0')
        {
            if (*p == '\n')
            //发送遇到'\n'之前的数据
            {
                strncpy(evey_message, message_recv, p - message_recv + 1);
                evey_message[p - message_recv + 1] = '\0';
                //结尾添加'\0'
                strcpy(message_recv, p);
                strcat(message_send, evey_message);
                //在"Message后接截取后的单次消息"
                while (1)
                {
                    len_send = send(pipe->fd_recv, message_send, strlen(message_send), 0);
                    if (len_send >= 0 && len_send < strlen(message_send))
                        //如果单次发送的数据少于应该发送的数据
                        //再次发送未发送数据
                        strcpy(message_send, message_send + len_send);
                    else
                        break;
                }
                strcpy(message_send, "Message:");
                memset(evey_message, 0, sizeof(evey_message));
                //初始化
            }
            p++;
        }
        memset(message_recv, 0, sizeof(message_recv));
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int port = atoi(argv[1]);
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    socklen_t addr_len = sizeof(addr);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind");
        return 1;
    }
    if (listen(fd, 2))
    {
        perror("listen");
        return 1;
    }
    int fd1 = accept(fd, NULL, NULL);
    int fd2 = accept(fd, NULL, NULL);
    if (fd1 == -1 || fd2 == -1)
    {
        perror("accept");
        return 1;
    }
    pthread_t thread1, thread2;
    struct Pipe pipe1;
    struct Pipe pipe2;
    pipe1.fd_send = fd1;
    pipe1.fd_recv = fd2;
    pipe2.fd_send = fd2;
    pipe2.fd_recv = fd1;
    pthread_create(&thread1, NULL, handle_chat, (void *)&pipe1);
    pthread_create(&thread2, NULL, handle_chat, (void *)&pipe2);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
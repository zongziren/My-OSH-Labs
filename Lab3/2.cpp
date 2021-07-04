#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <queue>

#define MAXLEN 1050000
#define MAXUSER 40
using std::queue;
using std::string;
queue<string> send_queue[MAXUSER];

pthread_mutex_t send_mutex[MAXUSER];
pthread_cond_t cv[MAXUSER];
pthread_t recv_thread[MAXUSER];
pthread_t send_thread[MAXUSER];
pthread_t log_thread[MAXUSER];

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
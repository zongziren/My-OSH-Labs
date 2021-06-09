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
# OSH LAB2 实验报告 
## strace 工具 报告
- 系统调用brk()

brk(NULL)

brk() change  the location  of  the program  break,  whichdefines the end of the process's data segment.

- 系统调用access()

检查调用进程是否可以对指定的文件执行某种操作。

access("/etc/ld.so.preload", R_OK) 例如这里检查读操作

- 系统调用fstat()

由文件描述词取得文件状态

int fstat(int fildes,struct stat *buf);

fstat()用来将参数fildes所指的文件状态，复制到参数buf所指的结构中(struct stat)

- 系统调用mmap()

mmap是一种内存映射文件的方法，即将一个文件或者其它对象映射到进程的地址空间，实现文件磁盘地址和进程虚拟地址空间中一段虚拟地址的一一对映关系。

void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);

start：映射区的开始地址

length：映射区的长度

prot：期望的内存保护标志，不能与文件的打开模式冲突。是以下的某个值，可以通过or运算合理地组合在一起

flags：指定映射对象的类型，映射选项和映射页是否可以共享。它的值可以是一个或者多个以下位的组合体

fd：有效的文件描述词。如果MAP_ANONYMOUS被设定，为了兼容问题，其值应为-1

offset：被映射对象内容的起点

## 选作项目总结
- 优化开头部分

仿照bash

通过env::var()获取用户名，并且打印在每行最前方。

通过env::var()获取目录名，并且打印在每行最前方。

// 如果 curr_dir在用户home目录下 使用~/形式

- 把$开头短语理解为环境变量 

eg:echo $USER 


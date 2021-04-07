# LAB1

### 基本信息
- Author:zsr   
- Time:2021.4.7
---

### 实验环境
- VMware
- Ubuntu 20.04.1
- CPU型号名称: Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz
- 虚拟化类型：完全
---

### kernel

- 首先配置好环境，然后进行裁剪
- 初始配置生成的Image在17MB左右
- delete ARM64 Accelerated Cryptographic Algorithms 
- delete Networking support
- select Compiler optimization level (Optimize for size (-Os))
- delete Support initial ramdisk/ramfs compressed using LZMA bzip2 XZ LZO LZ4 
- delete Security options 
- delete library routines 
- delete some of the filesystem
- 生成的Image在9MB左右,可以正常运行init 
- !!!kernel hacking trace can not be delete!!!
- delete most device
- 生成的Image在5.5MB,可以正常运行init


                                         
  
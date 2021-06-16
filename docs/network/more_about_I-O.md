# 更多网络 I/O 模式

在上一节，通过学习阻塞式 I/O 及与线程或进程的配合，基本已经覆盖了 socket 编程的知识点，本节将更进一步，描述另外四种网络 I/O 的实现。

再复习一下，Unix 网络编程中，存在五种 I/O 模型，它们分别为：

- 阻塞式 I/O 模型
- 非阻塞式 I/O 模型
- I/O 复用模型
- 信号驱动式 I/O 模型
- 异步 I/O 模型

## 非阻塞式 I/O

非阻塞模式通过为文件描述符设置非阻塞标志位，从而达到在**数据准备阶段不阻塞**的效果。注意，这里强调数据准备阶段是因为，非阻塞式 I/O 在数据从内核缓冲区复制到用户空间时依然是阻塞的。非阻塞 I/O 模式仅为其设置标志位而不进行其他动作，所以使用者还需要在后续使用 write/read 等调用配合循环，手动对数据进行轮询的操作。这里首先给出一个非阻塞式 I/O 操作文件的例子。

[none_block_file](../src/network/advanced_io/none_block_file.c ':include')

可以看到写入普通文件时，无错误，一次完成

```bash
./a.out < /etc/services > tmp.file
```

而输出到终端时，会产生很多错误信息，可以看到错误代码为 11,错误消息为`Resource temporarily unavailable`。通过查阅头文件得知，11 号错误正是 EWOULDBLOCK，也即 EAGAIN，代表 Operation would block，Try again。这是因为终端程序一次可以接受的数据量一般不会达到如此之大的数据，所以会产生如此的错误。

```bash
./a.out </etc/services 2>stderr.out
```

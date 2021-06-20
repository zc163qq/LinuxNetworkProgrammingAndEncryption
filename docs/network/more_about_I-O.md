# 更多网络 I/O 模式

在上一节，通过学习阻塞式 I/O 及与线程或进程的配合，基本已经覆盖了 socket 编程的知识点，本节将更进一步，描述另外四种网络 I/O 的实现。

再复习一下，Unix 网络编程中，存在五种 I/O 模型，它们分别为：

- 阻塞式 I/O 模型
- 非阻塞式 I/O 模型
- I/O 复用模型
- 信号驱动式 I/O 模型
- 异步 I/O 模型

## 非阻塞式 I/O

非阻塞模式通过为文件描述符设置非阻塞标志位，从而达到在**数据准备阶段不阻塞**的效果。注意，这里强调数据准备阶段是因为，非阻塞式 I/O 在数据从内核缓冲区复制到用户空间时依然是阻塞的。非阻塞 I/O 模式仅为其设置标志位而不进行其他动作，所以使用者还需要在后续使用 write/read 等调用配合循环，手动对数据进行轮询的操作。

### 非阻塞的磁盘文件操作

这里首先给出一个非阻塞式 I/O 操作文件的例子。

[none_block_file](../src/network/advanced_io/none_block_file.c ':include')

可以看到写入普通文件时，无错误，一次完成

```bash
./a.out < /etc/services > tmp.file
```

而输出到终端时，会产生很多错误信息，可以看到错误代码为 11,错误消息为`Resource temporarily unavailable`。通过查阅头文件得知，11 号错误正是 EWOULDBLOCK，也即 EAGAIN，代表 Operation would block，Try again。这是因为终端程序一次可以接受的数据量一般不会达到如此之大的数据，所以会产生如此的错误。

```bash
./a.out </etc/services 2>stderr.out
```

### 非阻塞的简易网络服务器

看过非阻塞式的本地文件处理，再来看一下非阻塞式的 socket 服务端是如何实现的。其也可用上节中的循环客户端程序来测试。

[none_block_server](../src/network/advanced_io/none_block_server.c ':include')

可以看到，其可以以非阻塞的形式处理 accept 连接，读取客户端数据，以及向客户端发送数据。但是非阻塞 I/O 模式的手动实现需要轮询，这样会占用相当多的 cpu 资源。同时，在轮询间的间隔也不好掌握，所以手动实现非阻塞式 I/O 在生产中并不常见，对于较为简单的场景，阻塞式 I/O 配合多线程以及多进程更为实用。同时，如果服务端程序已经在等待一个客户端的输入，那么后续到来的客户端将被阻塞，因为此程序同时只能对一个文件描述符进行监视，这也是为什么 I/O 多路复用模型存在的重要原因。

### 非阻塞的较完善的网络服务器

如果对上面的程序加以改动，可以用一个链表加轮询的方式，实现一个以类似 select/poll 的方式，完成同时多个客户端 socket 进行服务的程序。你可以在看完 I/O 多路复用模型后再回来品味这个程序。

[none_block_server_enhance](../src/network/advanced_io/none_block_server_enhance.c ':include')

更为常用的模式是下面将描述的 I/O 复用模型。

## I/O 多路复用模型

I/O 多路复用模型最明显的好处是可以在同一个进程中监听多个文件描述符，也即一个进程同时处理多路 IO。I/O 多路复用模型最典型的实现分别为 select,poll 以及 epoll。一些资料把 epoll 比作更"高档"的实现，而 select/poll 是很"low"的实现，这样是不对的，他们都有各自应用的场景，不能一概而论，比如如果套接字大多都是活跃的状态且数量没有到达很大的量级，select 的性能甚至要好过 epoll。本节将提供源代码分别进行描述。

### select

同样，可以使用前一节的循环客户端用来测试。可以看到程序中同时监控多个文件描述符。首先将主程序中的监听描述符加入 fd_set,随后监听开始。在主循环中，当有事件发生时，遍历 fd_set，根据不同的情况做出不同的动作。此时若为 accept 则不会阻塞，因为事件已经就续。但是如果是 read 事件，在数据准备阶段不会阻塞，在 I/O 阶段仍会阻塞。

[select](../src/network/advanced_io/select.c ':include')

### poll

poll 模型大体的流程和 select 是一致的。select 使用位图形式的 fd_set，而 poll 使用数组，我们在非阻塞模型中自行实现的方式是使用单链表。除此之外，poll 使用 revents 返回修改，所以不用像 select 一样对原始的 fdset 进行备份。

[poll](../src/network/advanced_io/poll.c ':include')

### epoll

epoll 模型大体的流程和 select 以及 poll 也是一致的。epoll 解决了 select/poll 中存在的轮询以及集合拷贝的问题，在高并发连接数的情况下非常适用。同时，epoll 支持水平触发以及边缘触发两种模式。select/poll 仅支持水平触发模式。

- 水平触发: 在报告 fd 后如果其没有被处理，或者数据没有被全部读取，那么水平触发会立刻继续报告该 fd
- 边缘触发: 在报告 fd 后如果其没有被处理，或者数据没有被全部读取，那么边缘触发会在下次再报告该 fd

[epoll](../src/network/advanced_io/epoll.c ':include')

## 信号驱动式 I/O 模型

此处所说的信号驱动式 I/O 模型指的是经典的基于套接字的，使用信号的异步 I/O 机制。此种模式在如 read 等调用在从内核取回数据时依然是阻塞的状态，理论上讲不是一种真正的异步模型。此种模型在大量 I/O 操作时可能会因为信号队列溢出导致没法通知。

信号驱动 I/O 尽管对于处理 UDP 套接字来说有用，即这种信号通知意味着到达一个数据报，或者返回一个异步错误。但是，对于 TCP 而言，信号驱动的 I/O 方式近乎无用，因为导致这种通知的条件为数众多，每一个来进行判别会消耗很大资源，与前几种方式相比优势尽失。

## 异步 I/O 模型

此处所说的异步 I/O 模型指的是 UNIX 规范中所描述的通用异步 I/O 机制。区别于信号驱动式的 I/O 模型，此种模型是真正的异步 I/O 实现。这种模型与信号驱动模型的主要区别在于：信号驱动 I/O 是由内核通知应用程序何时启动一个 I/O 操作，而异步 I/O 模型是由内核通知应用程序 I/O 操作何时完成。在 Linux 中，[libaio](https://archlinux.org/packages/core/x86_64/libaio/) 提供了一套不同于 POSIX 接口的异步 I/O 接口，其目的是更加高效的利用 I/O 设备。

### aio 函数族

### kernel 5.X 时代的异步模型 io_uring

---

Ref:
[[1]](https://www.bilibili.com/video/BV1pp4y1e7xN?p=6)
[[2]](http://www.mathcs.emory.edu/~cheung/Courses/455/Syllabus/9-netw-prog/timeout6.html)

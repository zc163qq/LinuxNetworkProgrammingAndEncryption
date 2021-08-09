# Linux 网络编程与加密 WIP <!-- {docsify-ignore-all} -->

> 这是一个需要长期完成的文档，不要期望短期内完善。

本书通过讲述网络编程，密码学技术，以及需要用到的相关密码学库组件，教授一个能够自由访问互联网的通信工具。

本书针对 GNU/Linux 操作系统，全部操作均在 Arch Linux 下完成。没有安装 Linux 的同学可以先参考[Arch Linux 安装使用教程](https://archlinuxstudio.github.io/ArchLinuxTutorial/#/)安装上 Linux。本书的定位是**较为进阶**，对于特别基础的知识，将不会赘述。

- 本书特点

  - 所有代码没有使用自定义封装的函数或者头文件，均采用 Linux 标准头文件和库函数，直接拷贝即可在 Arch Linux 下编译运行。这是为了保持教程的简洁以及实用性。
  - Linux 二次元电报群:[篝ノ雾枝的咕咕小屋](https://t.me/kdwu1fan)。
  - 无废话，只给出应当掌握的部分，不会面面俱到。C 语言博大精深，如有错误欢迎直接指出。
  - 本书使用 docsify 以及 gitalk 开发，并且网站源码全部开源，可放心留言讨论。
  - 只要 C 语言依旧活跃，就会一直更新。[鼓励志愿者提交更新](/contribution.md)。
  - 本书仅支持 Linux x86_64 环境，处理器为 i7 6700k。根据最新版的 GCC 与 Glibc 更新内容。仅保证全部样例在 Arch Linux 中的可运行性。

# 1. 预处理 
  预处理指令**遇到换行则结束**，可以用\连接对行。
## 1.1 \#include预处理指令
**作用**  
将被include的文件插入在写#inculde的地方，可以嵌套 比如a include了b，b 可以include c，
a也可以再include c

**如何 避免上面c被include了两次（避免多次包含）**  
通常做法(模式)是使用
```c
#ifndef INCFILE_H_
#define INCFILE_H_
/*这里写上这个头文件的实际内容*/
#endif
```

##1.2 预处理如何搜寻被include的文件
1. 对于<>的include  
文件名是否区分大小写由系统决定。
搜寻路径是系统特定路径 注入 /usr/local/include  /usr/include

2. 对于""的include  
通常现在当前目录查找，  
如果没有找到就搜索系统include路径。  
filename可以包含路径，此时会到相应目录中查找。  

3. 指定搜寻路径  
可以使用**编译器命令行选项**。  
或者在**环境变量中加入搜索路径**，环境变量名通常是INCLUDE。  


## 1.3 \#defined运算符
src/cpu/x86/vm/jni_x86.h  
```c
#if defined(SOLARIS) || defined(LINUX)
```
等同于  
hotspot/src/share/vm/adlc/adlc.hpp:#ifdef LINUX  

查看linux系统 预定义宏  
cpp -dM /dev/null  

## 1.4 \#defined带参数的宏
可以参考文章 [C语言宏的特殊用法和几个坑](http://hbprotoss.github.io/posts/cyu-yan-hong-de-te-shu-yong-fa-he-ji-ge-keng.html) 写的非常好  
> **宏名之后带括号的**宏被认为是宏函数。用法和普通函数一样，只不过在预处理阶段，宏函数会被展开。优点是没有普通函数保存寄存器和参数传递的开销，展开后的代码有利于CPU cache的利用和指令预测，速度快。缺点是可执行代码体积大。

```c
#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
// y = min(1, 2);会被扩展成y = ((1) < (2) ? (1) : (2));
```

## 1.5 \#defined \# 与 \#\#特殊符号
>在宏体中，如果宏参数前加个#，那么在宏体扩展的时候，宏参数会被扩展成字符串的形式。如：

```c
#define WARN_IF(EXP) \
     do { if (EXP) \
             fprintf (stderr, "Warning: " #EXP "\n"); } \
     while (0)

//WARN_IF (x == 0);会被扩展成：

do { if (x == 0)
    fprintf (stderr, "Warning: " "x == 0" "\n"); }
while (0);

```
**说白了 就是会在外面加引号**

>在宏体中，如果宏体所在标示符中有##，那么在宏体扩展的时候，宏参数会被直接替换到标示符中

就是连接，
```c
#define COMMAND(NAME)  { #NAME, NAME ## _command }

struct command commands[] =
{
    COMMAND (quit),
    COMMAND (help),
    ...
};

// 替换成
struct command commands[] =
{
    { "quit", quit_command },
    { "help", help_command },
    ...
};

```
**\#\# 前后的字符会被连接  如果前或者后有宏参数则会被替换后连接**  



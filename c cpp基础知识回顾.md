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



## 1.5 \#defined \# 与 \#\#特殊符号

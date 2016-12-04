#GDB常用调试命令
s 进入方法内部
n 下一行
break java.c:1400  指定文件打断点

条件断点
set $loaderCounter = 0
break classLoader.cpp:877  if ++$loaderCounter == 282



高效调试GDB的贴子
https://metricpanda.com/tips-for-productive-debugging-with-gdb
可惜ubuntu12.04 自带的gdb版本过低（7.4） 用不了。  需要gdb版本>=7.7






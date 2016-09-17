# hotspot-src-study
hotspot-src-study. OpenJDK 1.7

用hotspot脚本调试jvm时出现
source: not found
ALT_JAVA_HOME未设置
请参考： http://blog.csdn.net/mvpme82/article/details/7615454
原因： ls -l `which sh` 提示/bin/sh -> dash

改回方法： 
命令行执行：sudo dpkg-reconfigure dash
在界面中选择no

再ls -l `which sh` 提示/bin/sh -> bash

问题列表：
1. class 文件的格式结构定义，解析 加载等
2. 内存模型 划分等 垃圾回收等
3. 线程模型

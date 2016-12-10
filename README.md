# hotspot-src-study
全代码下载地址 [http://www.java.net/download/openjdk/jdk7/promoted/b147/openjdk-7-fcs-src-b147-27_jun_2011.zip]
包含jdk等   
hotspot-src-study. OpenJDK 1.7  

编译方式：  
去hotspot-src-study/hotspot/make目录下  
./make.sh 即可编译  

##1. 目标问题列表：
1. class 文件的格式结构定义，解析 加载等   
2. 内存模型 划分等 垃圾回收等  
3. 线程模型  




##2. 阅读入口
cd ~/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg  
cp ../../../../simon_test/bin/HelloWord.class ./  

### gdb调试
cd ~/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg  
sh hotspot -gdb HelloWord  

###遇到的问题1：
用hotspot脚本调试jvm时出现  
source: not found  
ALT_JAVA_HOME未设置  
请参考： http://blog.csdn.net/mvpme82/article/details/7615454  
原因： ls -l `which sh` 提示/bin/sh -> dash  

改回方法：   
命令行执行：sudo dpkg-reconfigure dash  
在界面中选择no  

再ls -l `which sh` 提示/bin/sh -> bash  

#jvm init
##1. 执行一个java class入口处
java.c的main方法  229行  
修改hotspot的脚本即可让断点打在main方法处  
修改init_gdb函数,将原来在InitializeJVM处打断点修改成 break java.c:229  
229行是java.c main方法所在位置(行号根据实际情况修改）  
部分调试信息如下 可以看出相应参数  
##2. 尝试调一下
```c
(gdb) n
270	        original_argv[i] = argv[i];
(gdb) p i
$8 = 0
(gdb) p argv[0]
$9 = 0x7fffffffe0d6 "/home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg/gamma"
(gdb) p argv[1]
$10 = 0x7fffffffe153 "HelloWord"


(gdb) p ifn
$1 = {CreateJavaVM = 0, GetDefaultJavaVMInitArgs = 0}
//此时ifn内部的CreateJavaVM    GetDefaultJavaVMInitArgs  都是null
```
InvocationFunctions ifn  
InvocationFunctions是一个struct  
```c
typedef struct {
    CreateJavaVM_t CreateJavaVM;
    GetDefaultJavaVMInitArgs_t GetDefaultJavaVMInitArgs;
} InvocationFunctions;
```

第一步
**CreateExecutionEnvironment**  

**LoadJavaVM(jvmpath, &ifn)**  
此方法 非常重要  
对InvocationFunctions ifn中两个属性进行了赋值  

对于java.h中  
```c
typedef jint (JNICALL *GetDefaultJavaVMInitArgs_t)(void *args);
```
的理解：
```c
/*
 * JNICALL 是一个宏 定义在 vm/jni_x86.h 文件中可以发现 <br/>
 * 至于java.h如何关联到jni_x86.h文件的，参见 《jvm整体备忘.md》1.1 JVM如何搞定平台差异 的方式1
 * 如果是linux或者solaris平台 JNICALL 为空 其余平台的则是 __stdcall <br/>
 * 关于__stdcall 可以参见： msdn ,可以理解成一个关键字，用于函数返回类型和函数之间 <br/>
 * 这样就好理解了 typedef还是定义了一个指向函数的类型GetDefaultJavaVMInitArgs_t <br/>
 * 函数的返回值是jint ， 参数列表是void *args <br/>
 *
 */

```
##3. LoadJavaVM 方法分析
```c
// 
jboolean
LoadJavaVM(const char *jvmpath, InvocationFunctions *ifn)
{
#ifdef GAMMA
    /* JVM is directly linked with gamma launcher; no dlopen() */
    ifn->CreateJavaVM = JNI_CreateJavaVM;
    ifn->GetDefaultJavaVMInitArgs = JNI_GetDefaultJavaVMInitArgs;
    return JNI_TRUE;
#else
// ...
```
此时完成对ifn这个结构体中两个元素的赋值，即java.h中定义的InvocationFunctions结构体  
追踪JNI_GetDefaultJavaVMInitArgs 什么时候声明的

```c
ifn->GetDefaultJavaVMInitArgs = (GetDefaultJavaVMInitArgs_t)
        dlsym(libjvm, "JNI_GetDefaultJavaVMInitArgs")
```  
用dlsym 打开动态链接库中的定义，JNI_CreateJavaVM相同玩法  
这样很容易找到 jni.cpp 3277行定义了JNI_GetDefaultJavaVMInitArgs  
jni.cpp 3302行定义了JNI_CreateJavaVM  

分析了jni.cpp 3277行定义的JNI_GetDefaultJavaVMInitArgs  
发现其主要做了两件事：
1. jni版本的处理与检查  
2. java栈大小设置与检查

分析jni.cpp 3302行定义的JNI_CreateJavaVM  
TODO: Atomic::xchg函数作用  
往下跟，切换至src/share/vm/runtime/thread.cpp:3012 Threads::create_vm方法创建vm  
ostream_init()  




**SetClassPath**



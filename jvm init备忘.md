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
create_vm 处理的逻辑  
1. ostream_init()  
2. process_sun_java_launcher_properties  
3. os::init()  
4. init_system_properties  
5. JDK_Version_init  
6. init_version_specific_system_properties  
7. parse(args);  
8. 是否需要在启动的时pause PauseAtStartup   
9. 是否需要在启动时初始化libraries init_libraries_at_startup  
10. 是否需要在启动时初始化agent init_agents_at_startup  
11. ThreadLocalStorage::init()  
12. vm_init_globals()  
13. 创建JavaThread  
14. 主线程处理相关细节 堆栈等  
main_thread->record_stack_base_and_size();  
  main_thread->initialize_thread_local_storage();  
  main_thread->set_active_handles(JNIHandleBlock::allocate_block());  
main_thread->create_stack_guard_pages();  
15. 创建java级的同步子系统 ObjectMonitor::Initialize()  
16. 初始化全局模块 init_globals();  
17. main_thread->cache_global_variables(); 这个动作是在堆创建之后
18. 维护线程锁  将当前主线程加入Threads维护  
19. JvmtiExport::transition_pending_onload_raw_monitors();？？  
20. Universe::heap()->prepare_for_verify();  
    Universe::verify();  
21. VMThread::create();// 不同于上面的JavaThread  、
os::create_thread(vmthread, os::vm_thread)  
MutexLocker ml(Notify_lock);  
      os::start_thread(vmthread);  
22. 进入虚拟机启动阶段  JvmtiExport::enter_start_phase();  
23. 虚拟机启动后处理阶段 JvmtiExport::post_vm_start();  
  1. 若需要初始化libraries create_vm_init_libraries();  
  2. 是否需要初始化java.lang.String  initialize_class(vmSymbols::java_lang_String(), CHECK_0);  
  3. 处理AggressiveOpts开关？？  
  4. 处理UseStringCache开关  
  5. 处理InitializeJavaLangSystem开发  
  6. 处理InitializeJavaLangExceptionsErrors开关  
  7. 初始化java_lang_Compiler initialize_class(vmSymbols::java_lang_Compiler(), CHECK_0);  
  8. 重置vm 信息字段 reset_vm_info_property
  9. quicken_jni_functions ??
  10. 设置启动完成标志并记录  set_init_completed  Management::record_vm_init_completed()
  11. java系统的classloader SystemDictionary::compute_java_system_loader(THREAD);
  12. 未定义SERIALGC 的GC处理
24. 进入live阶段 JvmtiExport::enter_live_phase();
25. os::signal_init();
26. 如果没有禁用attach机制，初始化attach listener，，，AttachListener::init();
27. create_vm_init_libraries 23.1已经有了，又来一次？？
28. 虚拟机初始化后处理阶段 JvmtiExport::post_vm_initialized();
29. Chunk::start_chunk_pool_cleaner_task();？？
30. 编译器初始化  CompileBroker::compilation_init(); 
31. Management::initialize(THREAD);管理相关 初始化
32. 就profile相关开关进行处理 jni相关检查开启
33. BiasedLocking::init();？？
34. 检查是否可以jvm初始化的钩子插入，若可以 调用hook
35. 是否存在PeriodicTask::num_tasks()  若存在 则启动WatcherThread  WatcherThread::start();
36. os::init_3();？？


create_vm 处理的逻辑  
1. ostream_init()  
主要是是初始化tty(分配一个char*内存 type是C_HEAP)  用于输出日志等  
2. process_sun_java_launcher_properties  
看代码发现是处理-Dsun.java.launcher= 和-Dsun.java.launcher.pid=参数的  
部分调试信息  
```c
Threads::create_vm (args=0x7ffff7fe5dd0, canTryAgain=0x7ffff7fe5d9f) at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/thread.cpp:3018
3018	  Arguments::process_sun_java_launcher_properties(args);
(gdb) s
Arguments::process_sun_java_launcher_properties (args=0x7ffff7fe5dd0) at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/arguments.cpp:137
137	  for (int index = 0; index < args->nOptions; index++) {
(gdb) p args
$6 = (JavaVMInitArgs *) 0x7ffff7fe5dd0
(gdb) n
138	    const JavaVMOption* option = args->options + index;
(gdb) n
141	    if (match_option(option, "-Dsun.java.launcher=", &tail)) {
(gdb) p option
$7 = (const JavaVMOption *) 0x60a710
(gdb) p *option
$8 = {optionString = 0x60a6b0 "-Djava.class.path=/usr/jdk1.6.0_45/jre/lib/rt.jar:.", extraInfo = 0x0}
...
(gdb) p *option
$9 = {optionString = 0x60a760 "-Dsun.java.command=HelloWord", extraInfo = 0x0}
...
(gdb) p *option
$10 = {optionString = 0x406bea "-Dsun.java.launcher=gamma", extraInfo = 0x0}
...
(gdb) p *option
$12 = {optionString = 0x60a790 "-Dsun.java.launcher.pid=2812", extraInfo = 0x0}

```  
classpath 此时被转换成了-Djava.class.path=...HelloWord被转成了-Dsun.java.command=...  
匹配到sun.java.launcher时， _sun_java_launcher = strdup(launcher);  这个strdup干什么的？？  
匹配到sun.java.launcher.pid时， 将线程id记录到_sun_java_launcher_pid  

3. os::init()  
a. 设置_initial_pid  
逻辑是：  
如果java_launcher_pid 存在 则用之  否则 用getpid()函数获取  
pid_t是什么类型 在哪定义的？系统的吗？ TODO  
b. sysconf(_SC_CLK_TCK);  ？？ 同步时钟？？  
c. 初始化随机数的种子 init_random(1234567);
d. ThreadCritical初始化  这个类干嘛用的？？TODO  
e. 设置页大小  
f. 初始化系统信息  
  设置CPU个数  
  读取物理内存  
g. Linux::_main_thread = pthread_self();  
h. 初始化时钟 Linux::clock_init();  
优先加载 librt.so.1，加载不到就加载 librt.so 初始化_clock_gettime函数  
i. initial_time_count = os::elapsed_counter(); ???  
j. pthread_mutex_init(&dl_mutex, NULL); ???  

4. init_system_properties  
向PropertyList_add添加字段元素
java.vm.specification.name  Java Virtual Machine Specification  
java.vm.version  VM_Version::vm_release()  
java.vm.name  VM_Version::vm_name()  
java.vm.info  VM_Version::vm_info_string()  
java.ext.dirs java.endorsed.dirs sun.boot.library.path java.library.path java.home sun.boot.class.path  这几个都是设置的NULL
java.class.path 设置的是空  

查找jvm动态链接库路径  
```c
(gdb) n
389	        Arguments::set_dll_dir(dll_path);
(gdb) p dll_path
$32 = 0x7ffff0001908 "/usr/jdk1.6.0_45/jre/lib/amd64
...
405	        Arguments::set_java_home(home_path);
(gdb) p home_path
$35 = 0x7ffff00019a8 "/usr/jdk1.6.0_45/jre"
...
(gdb) p ld_library_path
$49 = 0x7ffff0001dc8 "/home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg:/usr/jdk1.6.0_45/jre/lib/amd64:/usr/java/packages/lib/amd64:/usr/lib64:/lib64:/li"...
...
463	        Arguments::set_ext_dirs(buf);
(gdb) p buf
$50 = 0x7ffff0001fe8 "/usr/jdk1.6.0_45/jre/lib/ext:/usr/java/packages/lib/ext"
...
(gdb) n
471	        Arguments::set_endorsed_dirs(buf);
(gdb) p buf
$51 = 0x7ffff00020c8 "/usr/jdk1.6.0_45/jre/lib/endorsed"

```


5. JDK_Version_init  
os::native_java_library 加载java.dll/so 获取jdk的版本并检查  
用工具查看java.dll 发现jdk中涉及的native方法有大部分定义在这里  

6. init_version_specific_system_properties  
java.vm.specification.vendor Sun Microsystems Inc.  
java.vm.specification.version 1.0  

```c
201	      new SystemProperty("java.vm.specification.vendor",  spec_vendor, false));
(gdb) p spec_vendor
$55 = 0x7ffff7614c50 "Sun Microsystems Inc."
```
7. parse(args);  
jvm path --> saved_jvm_path  
-XX:Flags=  
-XX:+PrintVMOptions // 打印-XX:参数  
-XX:-PrintVMOptions  
-XX:+IgnoreUnrecognizedVMOptions  
-XX:-IgnoreUnrecognizedVMOptions  
-XX:+PrintFlagsInitial  
-XX:+PrintFlagsWithComments  



8. 是否需要在启动的时pause PauseAtStartup   
PauseAtStartup 标志什么时候赋值？？ TODO
默认情况下是false

补 os::init_2();  
9. 是否需要在启动时初始化libraries init_libraries_at_startup  
convert_vm_init_libraries_to_agents();  

10. 是否需要在启动时初始化agent init_agents_at_startup  
create_vm_init_agents  
标志虚拟机进入JVMTI_PHASE_ONLOAD阶段  
迭代Arguments::agents()  agents方法返回_agentList，其类型是AgentLibraryList，是个链表   
先有由AgentLibrary查找并load 最后返回OnLoadEntry_t， 这个方法的实现是这个步骤的主要逻辑的体现。   
待准备用例调试。**TODO**  


_agentList在哪里处理准备的  
  arguments.hpp  add_init_agent  
  arguments.hpp  add_loaded_agent  
  arguments.hpp  convert_library_to_agent  
   
add_init_agent 是被Arguments::parse_each_vm_init_arg 调用  
用于处理  
-javaagent:  
-agentlib:  
-agentpath:  
参数，  
javaagent 对应AgentLibrary的name被设置为"instrument"  
其余两个参数 对应AgentLibrary的name设置值来自于参数选项配置值  

add_loaded_agent 是被jvmtiExport.cpp的load_agent_library  
  attachListener.cpp AttachOperationFunctionInfo


11. ThreadLocalStorage::init()  


12. vm_init_globals()  
1. check_ThreadShadow TODO...  
需要弄明白byte_offset_of函数  

2. basic_types_init  
除了java的8种基本类型外 还有  
junit intx jobject u1 u2 u4   
intx的解释，globalDefinitions.hpp 201行：   
// intx and uintx are the 'extended' int and 'extended' unsigned int types;  
// they are 32bit wide on a 32-bit platform, and 64bit wide on a 64bit platform.  
jobject 是指向_jobject的指针  jni.h  
```c
typedef jubyte  u1;
typedef jushort u2;
typedef juint   u4;

typedef uint8_t  jubyte;
typedef uint16_t jushort;
typedef uint32_t juint;

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int		uint32_t;
```
但是BasicType又做了如下定义  
```c
enum BasicType {
  T_BOOLEAN  =  4,
  T_CHAR     =  5,
  T_FLOAT    =  6,
  T_DOUBLE   =  7,
  T_BYTE     =  8,
  T_SHORT    =  9,
  T_INT      = 10,
  T_LONG     = 11,
  T_OBJECT   = 12,
  T_ARRAY    = 13,
  T_VOID     = 14,
  T_ADDRESS  = 15,
  T_NARROWOOP= 16,
  T_CONFLICT = 17, // for stack value type with conflicting contents
  T_ILLEGAL  = 99
};
```

先检查每种类型的size是否正确，再检查部分类型的范围是否正确  

90-95 检查??  TODO  

100-129 检查BasicType和type2field是否能对上  

对junit和HeapWord的size做检查  必须是2的幂  

JavaPriority1_To_OSPriority   这个是干什么用的?  

对UseCompressedOops标志处理 Oops启用压缩  
heapOopSize  LogBytesPerHeapOop  LogBitsPerHeapOop  BytesPerHeapOop  BitsPerHeapOop  

3. eventlog_init  
EventBuffer::init();  
size  默认2000  
buffer是 NEW_C_HEAP_ARRAY创建  

4. mutex_init  
tty_lock  
CGC_lock  
STS_init_lock  
相关GC的lock  
后面还有若干 具体参照 mutexLocker.cpp 169-270行  
lock有两种  一是Mutex  另一是  Monitor  

5. chunkpool_init  
ChunkPool::initialize();  
_large_pool  _medium_pool  _small_pool ??TODO  

6. perfMemory_init  
是否启用了UsePerfData标志  
PerfMemory::initialize();  

为性区域分配内存 32768  
...   


13. 创建JavaThread  
主要是分配一个Thread类型的指针  
```c
((Thread*) aligned_addr)->_real_malloc_address = real_malloc_addr
```
设置主线程状态：  
_thread_in_vm // running in VM  



14. 主线程处理相关细节 堆栈等  
```c
main_thread->record_stack_base_and_size();  
main_thread->initialize_thread_local_storage();  
main_thread->set_active_handles(JNIHandleBlock::allocate_block());  
main_thread->create_stack_guard_pages();  
```

1. main_thread->record_stack_base_and_size()  
关键靠os_linux_x86.cpp  current_stack_region处理   
pthread_getattr_np  
pthread_self  函数 [http://blog.csdn.net/xsckernel/article/details/8543377]   
pthread_getattr_np//获取线程属性。常用形式：pthread_getattr_np(pthread_self(),&attr1);  
```c
int rslt = pthread_getattr_np(pthread_self(), &attr);
```
获取当前线程的属性放置在attr中  
pthread_attr_getstack 获取线程的栈地址 已经栈大小 [http://www.educity.cn/linux/1241371.html]  
```c
pthread_attr_getstack(&attr, (void **)bottom, size) != 0
```
最后用pthread_attr_destroy函数  destory &attr，至此完成了static void current_stack_region(address * bottom, size_t * size) 函数入参的赋值  
完成了stack的base address和size的设置  

2. main_thread->initialize_thread_local_storage()  
将thread的index 与当前的thread映射  
下面是用到的系统的函数的细节  
index靠pthread_key_create创建  pthread_key_create 也是系统函数 配合上面的系统函数使用  主要作用是实现ThreadLocal  
[http://blog.csdn.net/lmh12506/article/details/8452700]  
pthread_setspecific系统函数 [http://blog.csdn.net/lwfcgz/article/details/37570667]  
pthread_getpecific和pthread_setspecific实现同一个线程中不同函数间共享数据的一种很好的方式  

3. main_thread->set_active_handles(JNIHandleBlock::allocate_block());  
JNIHandleBlock::allocate_block() 创建一个JNIHandleBlock指针  
直接 new JNIHandleBlock  
```c
class JNIHandleBlock : public CHeapObj
```
继承自CHeapObj   new操作符号重载  也就是为JNIHandleBlock分配内存  
分配成功后为main_thread 设置set_active_handles

4. main_thread->create_stack_guard_pages();  
判断OS是否具备stack的page保护机制，如果没有就不结束了  
ubuntu有  
最终本质上通过mmap实现  两个关键参数 一个是地址  一个是size  
部分调试信息如下：
```c
Threads::create_vm (args=0x7ffff7fe5dd0, canTryAgain=0x7ffff7fe5d9f)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/thread.cpp:3091
3091	  if (!main_thread->set_as_starting_thread()) {
(gdb) n
3101	  main_thread->create_stack_guard_pages();
(gdb) s
JavaThread::create_stack_guard_pages (this=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/thread.cpp:2258
2258	  if (! os::uses_stack_guard_pages() || _stack_guard_state != stack_guard_unused) return;
(gdb) p os::uses_stack_guard_pages() 
Couldn't find method os::uses_stack_guard_pages
(gdb) n
2259	  address low_addr = stack_base() - stack_size();
(gdb) p os::uses_stack_guard_pages() 
$10 = true
(gdb) n
2260	  size_t len = (StackYellowPages + StackRedPages) * os::vm_page_size();
(gdb) n
2262	  int allocate = os::allocate_stack_guard_pages();
(gdb) p len
$11 = 12288
(gdb) p StackYellowPages
$12 = 2
(gdb) p StackRedPages
$13 = 1
(gdb) p os::vm_page_size()
$14 = 4096
(gdb) n
2265	  if (allocate && !os::create_stack_guard_pages((char *) low_addr, len)) {
(gdb) p allocate
$15 = 1
(gdb) s
os::create_stack_guard_pages (addr=0x7ffff7ee6000 "", size=12288)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/os/linux/vm/os_linux.cpp:2716
2716	  bool chk_bounds = NOT_DEBUG(os::Linux::is_initial_thread()) DEBUG_ONLY(true);
(gdb) n
2717	  if (chk_bounds && get_stack_bounds(&stack_extent, &stack_base)) {
(gdb) n
2724	  return os::commit_memory(addr, size);
(gdb) p addr
$16 = 0x7ffff7ee6000 ""
(gdb) p size
$17 = 12288
(gdb) s
os::commit_memory (addr=0x7ffff7ee6000 "", size=12288, exec=false)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/os/linux/vm/os_linux.cpp:2462
2462	  int prot = exec ? PROT_READ|PROT_WRITE|PROT_EXEC : PROT_READ|PROT_WRITE;
(gdb) n
2464	                                   MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0);
(gdb) p prot
$18 = 3
(gdb) n
2465	  return res != (uintptr_t) MAP_FAILED;
(gdb) n
2466	}
(gdb) n
os::create_stack_guard_pages (addr=0x7ffff7ee6000 "", size=12288)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/os/linux/vm/os_linux.cpp:2725
2725	}

```

15. 创建java级的同步子系统 ObjectMonitor::Initialize()  
设置InitializationCompleted 标志位为1  

16. 初始化全局模块 init_globals();  
启了一大堆东西 直接贴代码吧  
```c

jint init_globals() {
  HandleMark hm;
  management_init();
  bytecodes_init();
  classLoader_init();
  codeCache_init();
  VM_Version_init();
  stubRoutines_init1();
  jint status = universe_init();  // dependent on codeCache_init and stubRoutines_init
  if (status != JNI_OK)
    return status;

  interpreter_init();  // before any methods loaded
  invocationCounter_init();  // before any methods loaded
  marksweep_init();
  accessFlags_init();
  templateTable_init();
  InterfaceSupport_init();
  SharedRuntime::generate_stubs();
  universe2_init();  // dependent on codeCache_init and stubRoutines_init
  referenceProcessor_init();
  jni_handles_init();
#ifndef VM_STRUCTS_KERNEL
  vmStructs_init();
#endif // VM_STRUCTS_KERNEL

  vtableStubs_init();
  InlineCacheBuffer_init();
  compilerOracle_init();
  compilationPolicy_init();
  VMRegImpl::set_regName();

  if (!universe_post_init()) {
    return JNI_ERR;
  }
  javaClasses_init();  // must happen after vtable initialization
  stubRoutines_init2(); // note: StubRoutines need 2-phase init

  // Although we'd like to, we can't easily do a heap verify
  // here because the main thread isn't yet a JavaThread, so
  // its TLAB may not be made parseable from the usual interfaces.
  if (VerifyBeforeGC && !UseTLAB &&
      Universe::heap()->total_collections() >= VerifyGCStartAt) {
    Universe::heap()->prepare_for_verify();
    Universe::verify();   // make sure we're starting with a clean slate
  }


```
1. management_init()  
```c
Management::init();
ThreadService::init();
RuntimeService::init();
ClassLoadingService::init();
```
主要是设置一系列的管理用的变量和计数器 比如thread的“started” “live”...  
_classes_loaded_count 计数器等等  

2. bytecodes_init()  
Bytecodes::initialize();  
初始化JVM规范定义的字节码指令  

3. classLoader_init()  
ClassLoader::initialize()  
同样是先创建一堆用于性能监控的计数器与变量 比如parse class的时间，link class的时间等等  

load_zip_library  
到/usr/jdk1.6.0_45/jre/lib/amd64/下找libzip.so并加载  
windows下是靠zip.dll  

setup_bootstrap_search_path  
```c
Arguments::get_sysclasspath()
(gdb) p sys_class_path  
$27 = 0x7ffff002edf8 "/usr/jdk1.6.0_45/jre/lib/resources.jar:/usr/jdk1.6.0_45/jre/lib/rt.jar:/usr/jdk1.6.0_45/jre/lib/sunrsasign.jar:/usr/jdk1.6.0_45/jre/lib/jsse.jar:/usr/jdk1.6.0_45/jre/lib/jce.jar:/usr/jdk1.6.0_45/jre/l"
```
如果TraceClassLoading 并开启了调试 此时会打印load的信息  
最后会把这些classpath信息维护到ClassLoader的lastEnrty链表上  

处理LazyBootClassLoader  
LazyBootClassLoader 这个标识干什么的  
读取/usr/jdk1.6.0_45/jre/lib/meta-index里面的包名  
注意这个配置文件的编写  #!@ 开头  
把需要延迟加载的放到LazyClassPathEntry TODO 阅读更多的关于延迟加载的信息  

4. codeCache_init();  
......




17. main_thread->cache_global_variables(); 这个动作是在堆创建之后
18. 维护线程锁  将当前主线程加入Threads维护  
19. JvmtiExport::transition_pending_onload_raw_monitors();？？  
20. Universe::heap()->prepare_for_verify();  
    Universe::verify();  
21. VMThread::create();// 不同于上面的JavaThread  、
os::create_thread(vmthread, os::vm_thread)  
MutexLocker ml(Notify_lock);  
      os::start_thread(vmthread);  
22. 进入虚拟机启动阶段  JvmtiExport::enter_start_phase();  
23. 虚拟机启动后处理阶段 JvmtiExport::post_vm_start();  
  1. 若需要初始化libraries create_vm_init_libraries();  
  2. 是否需要初始化java.lang.String  initialize_class(vmSymbols::java_lang_String(), CHECK_0);  
  3. 处理AggressiveOpts开关？？  
  4. 处理UseStringCache开关  
  5. 处理InitializeJavaLangSystem开发  
  6. 处理InitializeJavaLangExceptionsErrors开关  
  7. 初始化java_lang_Compiler initialize_class(vmSymbols::java_lang_Compiler(), CHECK_0);  
  8. 重置vm 信息字段 reset_vm_info_property
  9. quicken_jni_functions ??
  10. 设置启动完成标志并记录  set_init_completed  Management::record_vm_init_completed()
  11. java系统的classloader SystemDictionary::compute_java_system_loader(THREAD);
  12. 未定义SERIALGC 的GC处理
24. 进入live阶段 JvmtiExport::enter_live_phase();
25. os::signal_init();
26. 如果没有禁用attach机制，初始化attach listener，，，AttachListener::init();
27. create_vm_init_libraries 23.1已经有了，又来一次？？
28. 虚拟机初始化后处理阶段 JvmtiExport::post_vm_initialized();
29. Chunk::start_chunk_pool_cleaner_task();？？
30. 编译器初始化  CompileBroker::compilation_init(); 
31. Management::initialize(THREAD);管理相关 初始化
32. 就profile相关开关进行处理 jni相关检查开启
33. BiasedLocking::init();？？
34. 检查是否可以jvm初始化的钩子插入，若可以 调用hook
35. 是否存在PeriodicTask::num_tasks()  若存在 则启动WatcherThread  WatcherThread::start();
36. os::init_3();？？




**SetClassPath**



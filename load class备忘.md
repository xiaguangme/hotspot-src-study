##2. 阅读
cd ~/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg  
cp ../../../../simon_test/bin/HelloWord.class ./  

### gdb调试
cd ~/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg  
sh hotspot -gdb HelloWord  

从FileParser.cpp 入手分析   
```c
break classFileParser.cpp:2829  

(gdb) p (name.print())
Symbol: 'java/lang/Object' count 2$18 = void  
```
发现第一个加载的就是java/lang/Object  

用gdb的bt命令观察调用栈  
```c
(gdb) bt
#0  ClassFileParser::parseClassFile (this=0x7ffff7fe5370, name=0x7ffff004f068, class_loader=..., protection_domain=..., host_klass=..., 
    cp_patches=0x0, parsed_name=..., verify=false, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/classFileParser.cpp:2829
#1  0x00007ffff6fe8572 in ClassFileParser::parseClassFile (this=0x7ffff7fe5370, name=0x7ffff004f068, class_loader=..., protection_domain=..., 
    parsed_name=..., verify=false, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/classFileParser.hpp:284
#2  0x00007ffff6fe65b6 in ClassLoader::load_classfile (h_name=0x7ffff004f068, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/classLoader.cpp:917
#3  0x00007ffff7541ee6 in SystemDictionary::load_instance_class (class_name=0x7ffff004f068, class_loader=..., __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1359
#4  0x00007ffff753fe6b in SystemDictionary::resolve_instance_class_or_null (name=0x7ffff004f068, class_loader=..., protection_domain=..., 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:755
#5  0x00007ffff753e835 in SystemDictionary::resolve_or_null (class_name=0x7ffff004f068, class_loader=..., protection_domain=..., 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:203
#6  0x00007ffff753e38d in SystemDictionary::resolve_or_fail (class_name=0x7ffff004f068, class_loader=..., protection_domain=..., 
    throw_error=true, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:145
#7  0x00007ffff753e6a9 in SystemDictionary::resolve_or_fail (class_name=0x7ffff004f068, throw_error=true, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:186
#8  0x00007ffff7543595 in SystemDictionary::initialize_wk_klass (id=SystemDictionary::Object_klass_knum, init_opt=0, 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1915
#9  0x00007ffff75436b8 in SystemDictionary::initialize_wk_klasses_until (limit_id=SystemDictionary::Cloneable_klass_knum, 
    start_id=@0x7ffff7fe5964: SystemDictionary::Object_klass_knum, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1931
#10 0x00007ffff7546578 in SystemDictionary::initialize_wk_klasses_through (end_id=SystemDictionary::Class_klass_knum, 
    start_id=@0x7ffff7fe5964: SystemDictionary::Object_klass_knum, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.hpp:421
#11 0x00007ffff75437af in SystemDictionary::initialize_preloaded_classes (__the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1954
#12 0x00007ffff75434a8 in SystemDictionary::initialize (__the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1886
#13 0x00007ffff7590d71 in Universe::genesis (__the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/memory/universe.cpp:348
#14 0x00007ffff7592fb3 in universe2_init ()
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/memory/universe.cpp:1003
---Type <return> to continue, or q <return> to quit---
#15 0x00007ffff71ba205 in init_globals ()
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/init.cpp:109
#16 0x00007ffff75763a6 in Threads::create_vm (args=0x7ffff7fe5dd0, canTryAgain=0x7ffff7fe5d9f)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/thread.cpp:3107
#17 0x00007ffff7244e13 in JNI_CreateJavaVM (vm=0x7ffff7fe5e20, penv=0x7ffff7fe5e28, args=0x7ffff7fe5dd0)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/prims/jni.cpp:3356
#18 0x0000000000404a93 in InitializeJVM (pvm=0x7ffff7fe5e20, penv=0x7ffff7fe5e28, ifn=0x7ffff7fe5e10)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/tools/launcher/java.c:1288
#19 0x000000000040339e in JavaMain (_args=0x7fffffffbbe0)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/tools/launcher/java.c:423
#20 0x00007ffff6744e9a in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0
#21 0x00007ffff647231d in clone () from /lib/x86_64-linux-gnu/libc.so.6

```



##classloader

jni.cpp 386-387行  寻找class  
```c
JNI_ENTRY(jclass, jni_FindClass(JNIEnv *env, const char *name))  
  JNIWrapper("FindClass");
```


instanceKlassHandle  

通过宏 DEF_KLASS_HANDLE(instanceKlass         , oop_is_instance_slow ) 定义出来的类  
继承自 KlassHandle 代码如下  
```c
#define DEF_KLASS_HANDLE(type, is_a)             \  
  class type##Handle : public KlassHandle {  
```
KlassHandle又继承了Handle  
class KlassHandle: public Handle  

在  
```c
  instanceKlassHandle k (THREAD, thread->security_get_caller_class(0));
  if (k.not_null()) {
    loader = Handle(THREAD, k->class_loader());
    // Special handling to make sure JNI_OnLoad and JNI_OnUnload are executed
    // in the correct class context.
    if (loader.is_null() &&
        k->name() == vmSymbols::java_lang_ClassLoader_NativeLibrary()) {
      JavaValue result(T_OBJECT);
      JavaCalls::call_static(&result, k,
                                      vmSymbols::getFromClass_name(),
                                      vmSymbols::void_class_signature(),
                                      thread);
      if (HAS_PENDING_EXCEPTION) {
        Handle ex(thread, thread->pending_exception());
        CLEAR_PENDING_EXCEPTION;
        THROW_HANDLE_0(ex);
      }
      oop mirror = (oop) result.get_jobject();
      loader = Handle(THREAD,
        instanceKlass::cast(java_lang_Class::as_klassOop(mirror))->class_loader());
      protection_domain = Handle(THREAD,
        instanceKlass::cast(java_lang_Class::as_klassOop(mirror))->protection_domain());
    }
  } else {
    // We call ClassLoader.getSystemClassLoader to obtain the system class loader.
    loader = Handle(THREAD, SystemDictionary::java_system_loader());
```  
在jni.cpp 386行findclass这段处理中  
k.not_null() 逻辑简单 就是 instanceKlassHandle中的_handle是否为null  
调试逻辑：  
```c
Handle::not_null (this=0x7ffff7fe4f40)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/handles.hpp:102
102	  bool    not_null() const                       { return _handle != NULL; }
(gdb) p _handle
$6 = (oop *) 0x7ffff00299d8
```
代码如下：
```c
class Handle VALUE_OBJ_CLASS_SPEC {
 private:
  oop* _handle;

 protected:
  oop     obj() const                            { return _handle == NULL ? (oop)NULL : *_handle; }
  oop     non_null_obj() const                   { assert(_handle != NULL, "resolving NULL handle"); return *_handle; }

 public:
  // Constructors
  Handle()                                       { _handle = NULL; }
  Handle(oop obj);
#ifndef ASSERT
  Handle(Thread* thread, oop obj);
#else
  // Don't inline body with assert for current thread
  Handle(Thread* thread, oop obj);
#endif // ASSERT

  // General access
  oop     operator () () const                   { return obj(); }
  oop     operator -> () const                   { return non_null_obj(); }
  bool    operator == (oop o) const              { return obj() == o; }
  bool    operator == (const Handle& h) const          { return obj() == h.obj(); }

  // Null checks
  bool    is_null() const                        { return _handle == NULL; }
  bool    not_null() const                       { return _handle != NULL; }

  // Debugging
  void    print()                                { obj()->print(); }

  // Direct interface, use very sparingly.
  // Used by JavaCalls to quickly convert handles and to create handles static data structures.
  // Constructor takes a dummy argument to prevent unintentional type conversion in C++.
  Handle(oop *handle, bool dummy)                { _handle = handle; }

  // Raw handle access. Allows easy duplication of Handles. This can be very unsafe
  // since duplicates is only valid as long as original handle is alive.
  oop* raw_value()                               { return _handle; }
  static oop raw_resolve(oop *handle)            { return handle == NULL ? (oop)NULL : *handle; }
};
```

VALUE_OBJ_CLASS_SPEC是个宏 其实就是定义Handle的父类  

\#define VALUE_OBJ_CLASS_SPEC    : public _ValueObj  
那么这个_handle是oop类型  

oop又是啥  
oopsHierarchy.hpp中有定义：  
```c
typedef class oopDesc*                            oop;

instanceKlassHandle  可以用name方法查看是哪个class
k->name()

(gdb) p k->name()->print()
Symbol: 'java/lang/System' count 20$9 = void

```

find_class_from_class_loader  
jvm.cpp:3978  



第一个加载的是java.lang.Object  
Threads::create_vm-->init.cpp  init_globals-->SystemDictionary.initialize-->initialize_preloaded_classes  
看调试信息：  
```c
(gdb) break systemDictionary.cpp:145
Breakpoint 2 at 0x7ffff753e345: file /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp, line 145.
(gdb) c
Continuing.
[New Thread 0x7ffff5049700 (LWP 7684)]
[New Thread 0x7ffff4f48700 (LWP 7685)]

Breakpoint 2, SystemDictionary::resolve_or_fail (class_name=0x7ffff004f068, class_loader=..., protection_domain=..., throw_error=true, 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:145
145	  klassOop klass = resolve_or_null(class_name, class_loader, protection_domain, THREAD);
(gdb) p name
No symbol "name" in current context.
(gdb) p class_name->print()
Symbol: 'java/lang/Object' count 1$1 = void
(gdb) bt
#0  SystemDictionary::resolve_or_fail (class_name=0x7ffff004f068, class_loader=..., protection_domain=..., throw_error=true, 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:145
#1  0x00007ffff753e679 in SystemDictionary::resolve_or_fail (class_name=0x7ffff004f068, throw_error=true, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:186
#2  0x00007ffff7543565 in SystemDictionary::initialize_wk_klass (id=SystemDictionary::Object_klass_knum, init_opt=0, 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1915
#3  0x00007ffff7543688 in SystemDictionary::initialize_wk_klasses_until (limit_id=SystemDictionary::Cloneable_klass_knum, 
    start_id=@0x7ffff7fe5964: SystemDictionary::Object_klass_knum, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1931
#4  0x00007ffff7546548 in SystemDictionary::initialize_wk_klasses_through (end_id=SystemDictionary::Class_klass_knum, 
    start_id=@0x7ffff7fe5964: SystemDictionary::Object_klass_knum, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.hpp:421
#5  0x00007ffff754377f in SystemDictionary::initialize_preloaded_classes (__the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1954
#6  0x00007ffff7543478 in SystemDictionary::initialize (__the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1886
#7  0x00007ffff7590d41 in Universe::genesis (__the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/memory/universe.cpp:348
#8  0x00007ffff7592f83 in universe2_init ()
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/memory/universe.cpp:1003
#9  0x00007ffff71ba1d9 in init_globals ()
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/init.cpp:109
#10 0x00007ffff7576376 in Threads::create_vm (args=0x7ffff7fe5dd0, canTryAgain=0x7ffff7fe5d9f)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/runtime/thread.cpp:3107
#11 0x00007ffff7244de7 in JNI_CreateJavaVM (vm=0x7ffff7fe5e20, penv=0x7ffff7fe5e28, args=0x7ffff7fe5dd0)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/prims/jni.cpp:3356
#12 0x0000000000404a93 in InitializeJVM (pvm=0x7ffff7fe5e20, penv=0x7ffff7fe5e28, ifn=0x7ffff7fe5e10)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/tools/launcher/java.c:1288
#13 0x000000000040339e in JavaMain (_args=0x7fffffffbbe0)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/tools/launcher/java.c:423
#14 0x00007ffff6744e9a in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0
#15 0x00007ffff647231d in clone () from /lib/x86_64-linux-gnu/libc.so.6
#16 0x0000000000000000 in ?? ()
(gdb) 

(gdb) p class_anme
No symbol "class_anme" in current context.
(gdb) p class_name
$3 = (Symbol *) 0x7ffff004f068
(gdb) p class_name->print()
Symbol: 'java/lang/Object' count 1$4 = void
(gdb) 
Symbol: 'java/lang/Object' count 1$5 = void
(gdb) 
```
第一个加载的类是 java/lang/Object
FieldType::is_obj(class_name)  是false  这方法什么意思  


klassOop SystemDictionary::resolve_instance_class_or_null  
加载类的细节在此 比较复杂   
755 行  
```c
    if (!class_has_been_loaded) {

      // Do actual loading
      k = load_instance_class(name, class_loader, THREAD);


(gdb) n
755	      k = load_instance_class(name, class_loader, THREAD);
(gdb) p class_loader
$14 = {_handle = 0x0}
(gdb) n
765	      if (UnsyncloadClass || (class_loader.is_null())) {
(gdb) p k
$15 = {<KlassHandle> = {<Handle> = {_handle = 0x7ffff0029b18}, <No data fields>}, <No data fields>}
(gdb) p k->name()
$16 = (Symbol *) 0x7ffff004f068
(gdb) p k->name()->print()
Symbol: 'java/lang/Object' count 4$17 = void
(gdb) 

-----
Breakpoint 2, SystemDictionary::load_instance_class (class_name=0x7ffff004f068, class_loader=..., __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1345
1345	  instanceKlassHandle nh = instanceKlassHandle(); // null Handle
(gdb) p class_name->print()
Symbol: 'java/lang/Object' count 2$1 = void
(gdb) n
1346	  if (class_loader.is_null()) {
(gdb) n 
1350	    instanceKlassHandle k;
(gdb) 
1352	      PerfTraceTime vmtimer(ClassLoader::perf_shared_classload_time());
(gdb) 
1353	      k = load_shared_class(class_name, class_loader, THREAD);
(gdb) 
1356	    if (k.is_null()) {
(gdb) 
1358	      PerfTraceTime vmtimer(ClassLoader::perf_sys_classload_time());
(gdb) 
1359	      k = ClassLoader::load_classfile(class_name, CHECK_(nh));
(gdb) 
1372	    if (!k.is_null()) {
(gdb) pk
Undefined command: "pk".  Try "help".
(gdb) p k
$2 = {<KlassHandle> = {<Handle> = {_handle = 0x7ffff0029b18}, <No data fields>}, <No data fields>}
(gdb) p k->name()->print()
Symbol: 'java/lang/Object' count 4$4 = void
(gdb) 
```

回去接着调  
instanceKlassHandle ClassLoader::load_classfile  

在加载HelloWord时的加载堆栈：  
```c
Symbol: 'HelloWord' count 4
Breakpoint 3, ClassLoader::load_classfile (h_name=0x7ffff0182cc8, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/classLoader.cpp:877
877	  ThreadProfilerMark tpm(ThreadProfilerMark::classLoaderRegion);
(gdb) bt
#0  ClassLoader::load_classfile (h_name=0x7ffff0182cc8, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/classLoader.cpp:877
#1  0x00007ffff7541ee6 in SystemDictionary::load_instance_class (class_name=0x7ffff0182cc8, class_loader=..., __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:1359
#2  0x00007ffff753fe6b in SystemDictionary::resolve_instance_class_or_null (name=0x7ffff0182cc8, class_loader=..., protection_domain=..., 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:755
#3  0x00007ffff753e835 in SystemDictionary::resolve_or_null (class_name=0x7ffff0182cc8, class_loader=..., protection_domain=..., 
    __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:203
#4  0x00007ffff753e888 in SystemDictionary::resolve_or_null (class_name=0x7ffff0182cc8, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/systemDictionary.cpp:208
#5  0x00007ffff72718ba in JVM_FindClassFromBootLoader (env=0x7ffff0028a00, name=0x7ffff7fe50e0 "HelloWord")
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/prims/jvm.cpp:726
```

从最初的调用栈可以看出 load clas发起于JVM_FindClassFromBootLoader   
再往上追 发现是来自于java代码 jdk的ClassLoader java.lang.ClassLoader.findBootstrapClass(String)  
findBootstrapClass是一个native方法，实现写在ClassLoader.c里  
```c
/*
 * Returns NULL if class not found.
 */
JNIEXPORT jclass JNICALL
Java_java_lang_ClassLoader_findBootstrapClass(JNIEnv *env, jobject loader,
                                              jstring classname)
{
    char *clname;
    jclass cls = 0;
    char buf[128];

    if (classname == NULL) {
        return 0;
    }

    clname = getUTF(env, classname, buf, sizeof(buf));
    if (clname == NULL) {
        JNU_ThrowOutOfMemoryError(env, NULL);
        return NULL;
    }
    VerifyFixClassname(clname);

    if (!VerifyClassname(clname, JNI_TRUE)) {  /* expects slashed name */
        goto done;
    }

    cls = JVM_FindClassFromBootLoader(env, clname);

 done:
    if (clname != buf) {
        free(clname);
    }

    return cls;
}
```
java代码与c代码怎么对接的？  
下面就要看jni了。  
	




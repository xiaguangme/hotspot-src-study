#1.  enum WKID定义推导
```c
 #define WK_KLASS_ENUM_NAME(kname)    kname##_knum
 
 enum WKID {
    NO_WKID = 0,
 #define WK_KLASS_ENUM(name, ignore_s, ignore_o) WK_KLASS_ENUM_NAME(name),
    WK_KLASSES_DO(WK_KLASS_ENUM)
 #undef WK_KLASS_ENUM
 FIRST_WKID = NO_WKID + 1
  };	
	
	
// ===>	
	
	 #define WK_KLASS_ENUM(name, ignore_s, ignore_o) name##_knum,
    WK_KLASSES_DO(WK_KLASS_ENUM)
    #undef WK_KLASS_ENUM
	
	
// ===>	
	
	 #define WK_KLASS_ENUM(name, ignore_s, ignore_o) name##_knum,
    WK_KLASS_ENUM(Object_klass,                 java_lang_Object,               Pre) \
	WK_KLASS_ENUM(String_klass,                 java_lang_String,               Pre) \
...
    #undef WK_KLASS_ENUM
	
// ===>		
    #define WK_KLASS_ENUM(name, ignore_s, ignore_o) name##_knum,
	Object_klass_knum,
	String_klass_knum,
        ...
    #undef WK_KLASS_ENUM
	
// ===>		
    #define WK_KLASS_ENUM(name, ignore_s, ignore_o) name##_knum,
	Object_klass_knum,
	String_klass_knum,
...
    #undef WK_KLASS_ENUM
// ===>		
	  enum WKID {
    NO_WKID = 0,

    Object_klass_knum,
	String_klass_knum,
...
    WKID_LIMIT,

    FIRST_WKID = NO_WKID + 1
  };
```



#2. 加载java.lang.Object过程解析 
通过systemDictionary.hpp 94行WK_KLASSES_DO定义的类列表(这些类会被预加载)结合enum WKID的定义，会用在initialize_wk_klasses_until迭代加载完所有的需要预加载的类  

SystemDictionary::initialize_preloaded_classes  
第一步 先加载至Class_klass  (表示end，以systemDictionary.hpp 94行WK_KLASSES_DO定义的类列表为顺序)， 即包括  Object_klass String_klass Class_klass  
1. 由宏定义变换出 class对应的WKID对象 id SystemDictionary::initialize_wk_klass 1900行 往前看调用栈。  
2. 分析wk_init_info数组推导 参见3.wk_init_info数组推导  
3. 根据sid值到Symbol* vmSymbols::_symbols[vmSymbols::SID_LIMIT];表中查找symbol  
_symbols什么时候初始化 逻辑是什么？  
```c
Symbol* sym = SymbolTable::new_symbol(string, CHECK);  
// ===>	
 const char* string = &vm_symbol_bodies[0];
// ===>	
static const char* vm_symbol_bodies = VM_SYMBOLS_DO(VM_SYMBOL_BODY, VM_ALIAS_IGNORE);
// ===>	
#define VM_SYMBOL_BODY(name, string) string "\0"
#define VM_ALIAS_IGNORE(id, id2)  

#define VM_SYMBOLS_DO(template, do_alias)                                                         \
  /* commonly used class names */                                                                 \
  template(java_lang_System,                          "java/lang/System")                         \
  template(java_lang_Object,                          "java/lang/Object")                         \
  template(java_lang_Class,                           "java/lang/Class")                          \
  template(java_lang_String,                          "java/lang/String")                         \
...
// ===>	
// "java/lang/System""\0""java/lang/Object""\0"
// \0 分隔  将VM_SYMBOLS_DO 中所有的 以斜线表示的类名都拼接到一起 放入vm_symbol_bodies数组中用  
        
```
什么时候调用这个逻辑:  init_globals() --> universe2_init()   
初步理解Symbol就是字符串的一个包装  业务语义为 里面有类似java/lang/System这样的全路径类名  

4. 接下来 
```c
klassOop*    klassp = &_well_known_klasses[id]; // 先尝试从_well_known_klasses获取 如果没有 则会在1915行load

(*klassp) = resolve_or_fail(symbol, true, CHECK_0); // load required class

```

5. 交由ClassLoader 
```c
 k = ClassLoader::load_classfile(class_name, CHECK_(nh));
```

6. 根据java/lang/Object 怎么找到class文件的
classLoader.cpp 874 行 instanceKlassHandle ClassLoader::load_classfile...  
是靠ClassPathEntry 这个封装  
```c
Breakpoint 2, ClassLoader::load_classfile (h_name=0x7ffff0053178, __the_thread__=0x7ffff0028800)
    at /home/simomme/01.co/01.opensource/04.openjdk7/hotspot-src-study/hotspot/src/share/vm/classfile/classLoader.cpp:884
warning: Source file is more recent than executable.
884	  char* name = st.as_string();
(gdb) p name
$1 = 0x7ffff7fe4380 "\240C\376\367\377\177"
(gdb) p h_name->as_utf8()
$2 = 0x7ffff0029598 "java/lang/Object"
(gdb) n
887	  ClassFileStream* stream = NULL;
(gdb) 
888	  int classpath_index = 0;
(gdb) 
892	                               PerfClassTraceTime::CLASS_LOAD);
(gdb) 
893	    ClassPathEntry* e = _first_entry;
(gdb) 
894	    while (e != NULL) {
(gdb) p e
$3 = (ClassPathEntry *) 0x7ffff002ef28
(gdb) p name
$4 = 0x7ffff00295b8 "java/lang/Object.class"
(gdb) p e->name()
$5 = 0x7ffff002f000 "/usr/jdk1.6.0_45/jre/lib/resources.jar"
(gdb) p e->next()->name()
$6 = 0x7ffff002f170 "/usr/jdk1.6.0_45/jre/lib/rt.jar"
(gdb) p e->next()->next()->name()
$7 = 0x7ffff002f290 "/usr/jdk1.6.0_45/jre/lib/jsse.jar"
(gdb) p e->next()->next()->next()->name()
$8 = 0x7ffff002f3b0 "/usr/jdk1.6.0_45/jre/lib/jce.jar"
(gdb) p e->next()->next()->next()->next()->name()
$9 = 0x7ffff002f4d0 "/usr/jdk1.6.0_45/jre/lib/charsets.jar"
(gdb) p e->next()->next()->next()->next()->next()        
$10 = (ClassPathEntry *) 0x0
(gdb) 

```
ClassPathEntry 的三个实现  
\-\-ClassPathDirEntry  
\-\-ClassPathZipEntry  
\-\-LazyClassPathEntry  
此处加载java/lang/Object的是LazyClassPathEntry实现，但在下一步会最终转换成ClassPathZipEntry  
关注ClassPathZipEntry::open_stream方法  
ClassPathZipEntry::open_stream方法中通过zip的动态库的ZIP_FindEntry方法取查找java/lang/Object.class

7. load完之后进行parse
instanceKlassHandle ClassFileParser::parseClassFile(...)  
返回值是instanceKlassHandle  
依赖ClassFileStream* cfs = stream(); 进行读取  
读取魔法数字  版本号  
读取常量池 最终就是形成constantPoolHandle cp  
读取存取标志 读取父类等并进行校验 比如只允许java.lang.Object没有父类  
读取接口 objArrayHandle local_interfaces  
解析方法  parse_methods objArrayHandle methods  
... 
instanceKlassHandle 这一类的handle单独分析  

#3. wk_init_info数组推导
```c
static const short wk_init_info[] = {
  #define WK_KLASS_INIT_INFO(name, symbol, option) \
    ( ((int)vmSymbols::VM_SYMBOL_ENUM_NAME(symbol) \
          << SystemDictionary::CEIL_LG_OPTION_LIMIT) \
      | (int)SystemDictionary::option ),
  WK_KLASSES_DO(WK_KLASS_INIT_INFO)
  #undef WK_KLASS_INIT_INFO
  0
};

//===>
#define WK_KLASS_INIT_INFO(name, symbol, option)  ( ((int)vmSymbols::VM_SYMBOL_ENUM_NAME(symbol)  << SystemDictionary::CEIL_LG_OPTION_LIMIT) | (int)SystemDictionary::option ),

//===>
#define WK_KLASSES_DO(template)                                               \
  /* well-known classes */                                                    \
  template(Object_klass,                 java_lang_Object,               Pre) \
  template(String_klass,                 java_lang_String,               Pre)

//===>
( ((int)vmSymbols::VM_SYMBOL_ENUM_NAME(java_lang_Object)  << SystemDictionary::CEIL_LG_OPTION_LIMIT) | (int)SystemDictionary::Pre ),
( ((int)vmSymbols::VM_SYMBOL_ENUM_NAME(java_lang_String)  << SystemDictionary::CEIL_LG_OPTION_LIMIT) | (int)SystemDictionary::Pre ),

//===>
// 结合 
#define VM_SYMBOL_ENUM_NAME(name)    name##_enum

//===>
( ((int)java_lang_Object_enum  << SystemDictionary::CEIL_LG_OPTION_LIMIT) | (int)SystemDictionary::Pre ),
( ((int)java_lang_String_enum  << SystemDictionary::CEIL_LG_OPTION_LIMIT) | (int)SystemDictionary::Pre ),

//===>
( ((int)java_lang_Object_enum  << 4) | 0 ),
( ((int)java_lang_String_enum  << 4) | 0 ),

//===> 
// 结合 vmSymbols.hpp
  enum SID {
    NO_SID = 0,

    #define VM_SYMBOL_ENUM(name, string) VM_SYMBOL_ENUM_NAME(name),
    VM_SYMBOLS_DO(VM_SYMBOL_ENUM, VM_ALIAS_IGNORE)
    #undef VM_SYMBOL_ENUM

    SID_LIMIT,

    #define VM_ALIAS_ENUM(name, def) VM_SYMBOL_ENUM_NAME(name) = VM_SYMBOL_ENUM_NAME(def),
    VM_SYMBOLS_DO(VM_SYMBOL_IGNORE, VM_ALIAS_ENUM)
    #undef VM_ALIAS_ENUM

    FIRST_SID = NO_SID + 1
  };

//===> 
// 先把上面 这段推导完  结合
// vmSymbols.hpp
#define VM_SYMBOL_ENUM_NAME(name)    name##_enum

#define VM_SYMBOLS_DO(template, do_alias)                                                         \
  /* commonly used class names */                                                                 \
  template(java_lang_System,                          "java/lang/System")                         \
  template(java_lang_Object,                          "java/lang/Object")                         \
  template(java_lang_Class,  
...

//===> 
java_lang_System_enum, // 1
java_lang_Object_enum, // 2
java_lang_Class_enum, // 3
java_lang_String_enum, //4
...
//===> 
( (2  << 4) | 0 ),
( (4  << 4) | 0 ),
( (3  << 4) | 0 ),
( (9  << 4) | 0 ),

//===> 
//| 0 不会对数值产生影响
上面的值类推出来就是 wk_init_info数组的值

```






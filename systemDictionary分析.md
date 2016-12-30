#1.  enum WKID定义推导
```c
 #define WK_KLASS_ENUM_NAME(kname)    kname##_knum
 
 #define WK_KLASS_ENUM(name, ignore_s, ignore_o) WK_KLASS_ENUM_NAME(name),
    WK_KLASSES_DO(WK_KLASS_ENUM)
    #undef WK_KLASS_ENUM
	
	
	
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

    WKID_LIMIT,

    FIRST_WKID = NO_WKID + 1
  };
```

通过systemDictionary.hpp 94行WK_KLASSES_DO定义的类列表(这些类会被预加载)结合enum WKID的定义，会用在initialize_wk_klasses_until迭代加载完所有的需要预加载的类  

SystemDictionary::initialize_preloaded_classes  
第一步 先加载至Class_klass  (表示end，以systemDictionary.hpp 94行WK_KLASSES_DO定义的类列表为顺序)， 即包括  Object_klass String_klass Class_klass  

加载过程  
```c
klassOop*    klassp = &_well_known_klasses[id]; // 先尝试从_well_known_klasses获取 如果没有 则会在1915行load

(*klassp) = resolve_or_fail(symbol, true, CHECK_0); // load required class
```






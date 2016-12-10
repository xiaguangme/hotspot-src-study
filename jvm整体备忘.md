# 1. JVM如何搞定平台差异
## 1.1 方式1
**将不同平台的实现写在不同的文件中，再依赖c/cpp的条件预处理达到目的。**  
看如下示例代码  
share/tools/launcher/java.h  
```c
#include "jni.h"
```

src/share/vm/prims/jni.h  
```c
#include "jni_md.h"
```

/hotspot/src/share/vm/prims/jni_md.h  
```c
#ifdef TARGET_ARCH_x86
# include "jni_x86.h"
#endif
#ifdef TARGET_ARCH_sparc
# include "jni_sparc.h"
#endif
#ifdef TARGET_ARCH_zero
# include "jni_zero.h"
#endif
#ifdef TARGET_ARCH_arm
# include "jni_arm.h"
#endif
#ifdef TARGET_ARCH_ppc
# include "jni_ppc.h"
#endif
```

经上述代码可以很清晰的看到  
java.h需要include jni，通过过include jni.h 再include jni_md.h达到准确引用哪个平台的目的  
jni_md.h 通过条件预处理达到引用哪个具体实现的目的。  
诸如针对x86架构的具体实现就是写在 jni_x86.h中。  

至于TARGET_ARCH_x86在哪里定义的  
对于此次调试的应该在这里  
hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg/flags.make:TARGET_DEFINES += -DTARGET_ARCH_$(Platform_arch)  
hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg/flags.make:Platform_arch = x86  







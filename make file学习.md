# 1. 指定宏
在编译时指定hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg/flags.make:Platform_sysdefs = -DLINUX
hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg/flags.make:TARGET_DEFINES += -DTARGET_ARCH_$(Platform_arch)  
hotspot/build/hotspot_debug/linux_amd64_compiler2/jvmg/flags.make:Platform_arch = x86  

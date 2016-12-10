/*
 * Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */


#ifndef _JAVA_H_
#define _JAVA_H_

/*
 * Get system specific defines.
 */
#include "jni.h"
#include "java_md.h"
#include "jli_util.h"

/*
 * Pointers to the needed JNI invocation API, initialized by LoadJavaVM.<br/>
 * JNICALL 是一个宏 定义在 vm/jni_x86.h 文件中可以发现 <br/>
 * 至于java.h如何关联到jni_x86.h文件的，参见 《jvm整体备忘.md》1.1 JVM如何搞定平台差异 的方式1
 * 如果是linux或者solaris平台 JNICALL 为空 其余平台的则是 __stdcall <br/>
 * 关于__stdcall 可以参见： msdn ,可以理解成一个关键字，用于函数返回类型和函数之间 <br/>
 * 这样就好理解了 typedef还是定义了一个指向函数的类型GetDefaultJavaVMInitArgs_t <br/>
 * 函数的返回值是jint ， 参数列表是void *args <br/>
 *
 */
typedef jint (JNICALL *CreateJavaVM_t)(JavaVM **pvm, void **env, void *args);
typedef jint (JNICALL *GetDefaultJavaVMInitArgs_t)(void *args);

typedef struct {
    CreateJavaVM_t CreateJavaVM;
    GetDefaultJavaVMInitArgs_t GetDefaultJavaVMInitArgs;
} InvocationFunctions;

/*
 * Prototypes for launcher functions in the system specific java_md.c.
 */

jboolean
LoadJavaVM(const char *jvmpath, InvocationFunctions *ifn);

void
GetXUsagePath(char *buf, jint bufsize);

jboolean
GetApplicationHome(char *buf, jint bufsize);

const char *
GetArch();

void CreateExecutionEnvironment(int *_argc,
                                       char ***_argv,
                                       char jrepath[],
                                       jint so_jrepath,
                                       char jvmpath[],
                                       jint so_jvmpath,
                                       char **original_argv);

/*
 * Report an error message to stderr or a window as appropriate.  The
 * flag always is set to JNI_TRUE if message is to be reported to both
 * strerr and windows and set to JNI_FALSE if the message should only
 * be sent to a window.
 */
void ReportErrorMessage(char * message, jboolean always);
void ReportErrorMessage2(char * format, char * string, jboolean always);

/*
 * Report an exception which terminates the vm to stderr or a window
 * as appropriate.
 */
void ReportExceptionDescription(JNIEnv * env);

jboolean RemovableMachineDependentOption(char * option);
void PrintMachineDependentOptions();

const char *jlong_format_specifier();
/*
 * Block current thread and continue execution in new thread
 */
int ContinueInNewThread(int (JNICALL *continuation)(void *),
                        jlong stack_size, void * args);

/* sun.java.launcher.* platform properties. */
void SetJavaLauncherPlatformProps(void);

/*
 * Functions defined in java.c and used in java_md.c.
 */
jint ReadKnownVMs(const char *jrepath, char * arch, jboolean speculative);
char *CheckJvmType(int *argc, char ***argv, jboolean speculative);
void AddOption(char *str, void *info);

/*
 * Make launcher spit debug output.
 */
extern jboolean _launcher_debug;

#endif /* _JAVA_H_ */

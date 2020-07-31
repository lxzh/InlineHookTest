//
// Created by lxzh123 on 2018/3/23.
//
#include "util/common.h"
#include "native.h"
#include "inlineHook.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include "fake_dlfcn.h"

#define JNIREG_CLASS "com/lxzh123/inlinehooktest/Hooker"//指定要注册的类
static bool (*old_IsMagicValid)(uint32_t magic) = NULL;
static bool new_IsMagicValid(uint32_t magic) {
    LOGD("new_IsMagicValid magic:%d", magic);
    return old_IsMagicValid(magic);
}

void printPlatform() {
#if defined(__LP64__)
    LOGD("printPlatform __LP64__");
#endif
#if defined(__aarch64__)
    LOGD("printPlatform __aarch64__");
#endif
#if defined(__x86_64__)
    LOGD("printPlatform __x86_64__");
#endif
#if defined(__x86_64)
    LOGD("printPlatform __x86_64");
#endif
#if defined(__arm__)
    LOGD("printPlatform __arm__");
#endif
#if defined(__i386__)
    LOGD("printPlatform __i386__");
#endif
}

void printMaps() {
    FILE *maps;
    char buff[256];
    int k, fd = -1, found = 0;
    maps = fopen("/proc/self/maps", "r");
    if (!maps) LOGE("failed to open maps");
    while (!found && fgets(buff, sizeof(buff), maps)) {
        LOGE("%s", buff);
    }
    fclose(maps);
}

void hookMagicValid(void *handle) {
    LOGD("hookMagicValid start");
    void *is_magic_valid_addr = ndk_dlsym(handle, "_ZN3art13DexFileLoader12IsMagicValidEj");
    if (is_magic_valid_addr == NULL) {
        LOGD("Error: unable to find the Symbol : ");
        return;
    }
    if (registerInlineHook((uint32_t) is_magic_valid_addr, (uint32_t) new_IsMagicValid,
                           (uint32_t **) &old_IsMagicValid ) != ELE7EN_OK) {
        LOGD("register1 hook failed!");
        return;
    } else {
        LOGD("register1 hook success!");
    }
    if (inlineHook((uint32_t) is_magic_valid_addr) != ELE7EN_OK) {
        LOGD("register2 hook failed!");
        return;
    } else {
        LOGD("register2 hook success!");
    }
    LOGD("hookMagicValid end");
}

int (*old_puts)(const char *) = NULL;

int new_puts(const char *string) {
    LOGD("inlineHook puts success");
    old_puts(string);
}

int hookPuts() {
#if defined(__aarch64__)
    A64HookFunction((void *)puts, (void *)new_puts, (void **)&old_puts);
#else
    if (registerInlineHook((uint32_t) puts, (uint32_t) new_puts, (uint32_t **) &old_puts) != ELE7EN_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) puts) != ELE7EN_OK) {
        return -1;
    }
    return 0;
#endif
}

int unhookPuts() {
    if (inlineUnHook((uint32_t) puts) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

void myprint(const char * str) {
    LOGD("myprint old 1 str=%s", str);
    LOGD("myprint old 2 str=%s", str);
    LOGD("myprint old 3 str=%s", str);
    LOGD("myprint old 4 str=%s", str);
    LOGD("myprint old 5 str=%s", str);
    LOGD("myprint old 6 str=%s", str);
    LOGD("myprint old 7 str=%s", str);
    LOGD("myprint old 8 str=%s", str);
    LOGD("myprint old 9 str=%s", str);
    LOGD("myprint old 10 str=%s", str);
    LOGD("myprint old 11 str=%s", str);
    LOGD("myprint old 12 str=%s", str);
    LOGD("myprint old 13 str=%s", str);
    LOGD("myprint old 14 str=%s", str);
    LOGD("myprint old 15 str=%s", str);
    LOGD("myprint old 16 str=%s", str);
    LOGD("myprint old 17 str=%s", str);
    LOGD("myprint old 18 str=%s", str);
    LOGD("myprint old 19 str=%s", str);
    LOGD("myprint old 20 str=%s", str);
}

void (*old_print)(const char * str);

void new_print(const char *str) {
    LOGD("myprint new str=%s", str);
//    myprint(str);
    old_print(str);
}

void hookPrint() {
#if defined(__aarch64__)
//    char *elf_name = "libhooker.so";
//    void *handle = dlopen(elf_name, RTLD_LAZY);
//    if (handle == NULL) {
//        LOGD("Error: unable to find the SO : %s", elf_name);
//        return;
//    }
//    LOGD("dlopen %s success", elf_name);
//    char *symbol_name = "myprint";
//    void *open_common_addr = dlsym(handle, symbol_name);
//    if (open_common_addr == NULL) {
//        LOGD("Error: unable to find the Symbol : ");
//        return;
//    }
//    void *real_addr = reinterpret_cast<void *>(myprint);
//    LOGD("hookPrint: real_addr=%p open_common_addr=%p", real_addr, open_common_addr);
//    A64HookFunction((void *)symbol_name, (void *)new_print, (void **)myprint);
    A64HookFunction((void *)myprint, (void *)new_print, (void **)&old_print);
#else
    if (registerInlineHook((uint32_t) puts, (uint32_t) new_puts, (uint32_t **) &old_puts) != ELE7EN_OK) {
        return;
    }
    if (inlineHook((uint32_t) puts) != ELE7EN_OK) {
        return;
    }
#endif
}

int unhookPrint() {
#if defined(__aarch64__)

#else
    if (inlineUnHook((uint32_t) myprint) != ELE7EN_OK) {
        return -1;
    }
#endif
    return 0;
}

void hookTest() {
    myprint("test");
    hookPrint();
    myprint("test");
    unhookPrint();
    myprint("test");
}

JNIEXPORT void JNICALL hook(JNIEnv *env, jclass obj) {
    LOGD("hook start");
    static bool is_hook = false;
    if (is_hook) {
        LOGD("hooked ignore");
        return;
    }
//    printPlatform();
    hookTest();
    LOGD("hook init complete");
    is_hook = true;
}

JNIEXPORT void JNICALL dump(JNIEnv *env, jclass obj, jstring packageName) {
    LOGD("hook start");
    static bool is_hook = false;
    char *p = (char *) env->GetStringUTFChars(packageName, 0);
    if (is_hook) {
        LOGD("hooked ignore");
        return;
    }
    init_package_name(p);
    printPlatform();
    LOGD("hook start 1");
//    ndk_init(env);
//    void *handle = ndk_dlopen("libart.so", RTLD_NOW);
    char *elf_name = get_open_elf_name();
    void *handle = dlopen_ex(elf_name, RTLD_NOW);
    if (handle == NULL) {
        LOGD("Error: unable to find the SO : %s", elf_name);
        return;
    }
    LOGD("dlopen libart.so success");
//    hookMagicValid(handle);
//    hookTest();
///*
//    void *open_common_addr = ndk_dlsym(handle, get_open_function_flag());
    char *symbol_name = get_open_function_flag();
    void *open_common_addr = dlsym_ex(handle, symbol_name);
    if (open_common_addr == NULL) {
        LOGD("Error: unable to find the Symbol : ");
        return;
    }
//    printMaps();
    LOGD("loaded so finished, start hook");
#if defined(__aarch64__)
    A64HookFunction(open_common_addr, get_new_open_function_addr(), get_old_open_function_addr());
    LOGD("loaded so: libart.so");
#elif defined(__arm__)
    if (registerInlineHook((uint32_t) open_common_addr, (uint32_t) get_new_open_function_addr(),
                           (uint32_t **) get_old_open_function_addr()) != ELE7EN_OK) {
        LOGD("register1 hook failed!");
        return;
    } else {
        LOGD("register1 hook success!");
    }
    if (inlineHook((uint32_t) open_common_addr) != ELE7EN_OK) {
        LOGD("register2 hook failed!");
        return;
    } else {
        LOGD("register2 hook success!");
    }
    LOGD("loaded so: libart.so");
#endif
//*/
    LOGD("hook init complete");
    is_hook = true;
}

JNIEXPORT void JNICALL printMap(JNIEnv *env, jclass obj) {
    (void)env;
    (void)obj;
//    printMaps();
    myprint("test");
}

static JNINativeMethod gMethods[] = {
        {"hook", "()V", (void *) hook},
        {"dump", "(Ljava/lang/String;)V", (void *) dump},
        {"printMap", "()V", (void *) printMap}
};

JNIEXPORT int JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass javaClass = env->FindClass(JNIREG_CLASS);
    if (javaClass == NULL) {
        return JNI_ERR;
    }
    if (env->RegisterNatives(javaClass, gMethods, NELEM(gMethods)) < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
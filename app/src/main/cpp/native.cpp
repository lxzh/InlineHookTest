//
// Created by lxzh123 on 2018/3/23.
//
#include "util/common.h"
#include "native.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include "fake_dlfcn.h"
#include "test_class.h"

//
//#if defined(__LP64__) || defined(__aarch64__) || defined(__x86_64__) || defined(__x86_64)
//#include "A64InlineHook/And64InlineHook.hpp"
//#else
//#include "inlineHook/inlineHook.h"
//#endif

#ifdef __aarch64__
#include "A64Inlinehook/And64InlineHook.hpp"
#else
#include "Substrate/SubstrateHook.h"
#endif


#define JNIREG_CLASS "com/lxzh123/inlinehooktest/Hooker"//指定要注册的类

static inline void
hook_function(void *addr, void *new_func, void **old_func) {
#ifdef __aarch64__
    LOGD("hook_function with A64HookFunction");
    A64HookFunction(addr, new_func, old_func);
#else
    LOGD("hook_function with MSHookFunction");
    MSHookFunction(addr, new_func, old_func);
#endif

}


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
//
//void hookMagicValid(void *handle) {
//    LOGD("hookMagicValid start");
//    void *is_magic_valid_addr = ndk_dlsym(handle, "_ZN3art13DexFileLoader12IsMagicValidEj");
//    if (is_magic_valid_addr == NULL) {
//        LOGD("Error: unable to find the Symbol : ");
//        return;
//    }
//    if (registerInlineHook((uint32_t) is_magic_valid_addr, (uint32_t) new_IsMagicValid,
//                           (uint32_t **) &old_IsMagicValid ) != ELE7EN_OK) {
//        LOGD("register1 hook failed!");
//        return;
//    } else {
//        LOGD("register1 hook success!");
//    }
//    if (inlineHook((uint32_t) is_magic_valid_addr) != ELE7EN_OK) {
//        LOGD("register2 hook failed!");
//        return;
//    } else {
//        LOGD("register2 hook success!");
//    }
//    LOGD("hookMagicValid end");
//}

int (*old_puts)(const char *) = NULL;

int new_puts(const char *string) {
    LOGD("inlineHook puts success");
    return old_puts(string);
}

//int hookPuts() {
//#if defined(__aarch64__)
//    A64HookFunction((void *)puts, (void *)new_puts, (void **)&old_puts);
//#else
//    if (registerInlineHook((uint32_t) puts, (uint32_t) new_puts, (uint32_t **) &old_puts) != ELE7EN_OK) {
//        return -1;
//    }
//    if (inlineHook((uint32_t) puts) != ELE7EN_OK) {
//        return -1;
//    }
//    return 0;
//#endif
//}
//
//int unhookPuts() {
//    if (inlineUnHook((uint32_t) puts) != ELE7EN_OK) {
//        return -1;
//    }
//
//    return 0;
//}

void myprint(const char * str) {
    LOGD("myprint old 1 str=%s", str);
    LOGD("myprint old 2 str=%s", str);
    LOGD("myprint old 3 str=%s", str);
}

void (*old_print)(const char * str);

void new_print(const char *str) {
    LOGD("myprint new str=%s", str);
//    myprint(str);
    old_print(str);
}

void *(*old_cppprint)(void *testClass, int value);
void *new_cppprint(void *testClass, int value) {
    LOGD("new_cppprint value=%u %p", value, testClass);
    return old_cppprint(testClass, value);
}

void hookPrint() {
    hook_function((void *)myprint, (void *)new_print, (void **)&old_print);

    char *elf_name = "libhooker.so";
    void *handle = dlopen(elf_name, RTLD_LAZY);
    if (handle == NULL) {
        LOGD("Error: unable to find the SO : %s", elf_name);
        return;
    }
    LOGD("dlopen %s success", elf_name);
    char *symbol_name = "_ZN9TestClass10printValueEi";
    void *print_value_addr = dlsym(handle, symbol_name);
    if (print_value_addr == NULL) {
        LOGD("Error: unable to find the Symbol : ");
        return;
    }
    hook_function(print_value_addr, (void *)new_cppprint, (void **)&old_cppprint);

//#if defined(__aarch64__)

//    A64HookFunction((void *)myprint, (void *)new_print, (void **)&old_print);
//#else
//    if (registerInlineHook((uint32_t) puts, (uint32_t) new_puts, (uint32_t **) &old_puts) != ELE7EN_OK) {
//        return;
//    }
//    if (inlineHook((uint32_t) puts) != ELE7EN_OK) {
//        return;
//    }
//#endif
}

int unhookPrint() {
#if defined(__aarch64__)

#else
//    if (inlineUnHook((uint32_t) myprint) != ELE7EN_OK) {
//        return -1;
//    }
#endif
    return 0;
}

void hookTest() {
    TestClass testClass;
    myprint("test");
    LOGD("printValue1:%s", (char *)testClass.printValue(1));
    hookPrint();
    myprint("test");
    LOGD("printValue1:%s", (char *)testClass.printValue(2));
//    unhookPrint();
//    myprint("test");
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
void *
(*old_q_open_with_magic)(void *, uint32_t, int, void *,
                         bool,
                         bool,
                         void *,
                         void *);

void *new_q_open_with_magic(void *ArtDexFile_this, uint32_t magic, int fd,
                            void *location,
                            bool verify,
                            bool verify_checksum,
                            void *error_msg,
                            void *dex_files) {
    LOGI("new_q_open_with_magic2 enter");
    LOGI("new_q_open_with_magic2 ArtDexFile_this=%p, magic=%u, fd=%p, data_base=%p, location=%s, "
         "location_checksum=%u, oat_dex_file=%u, verify=%u, verify_checksum=%u", ArtDexFile_this,
         magic, fd,
         location, verify, verify_checksum);
    return old_q_open_with_magic(ArtDexFile_this, magic, fd, location, verify, verify_checksum,
                                 error_msg, dex_files);
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
    char *elf_name = get_open_elf_name();
    void *handle = dlopen_ex(elf_name, RTLD_NOW);
    if (handle == NULL) {
        LOGD("Error: unable to find the SO : %s", elf_name);
        return;
    }
    LOGD("dlopen %s success", elf_name);
//    hookMagicValid(handle);
//    hookTest();
///*
//    void *open_common_addr = ndk_dlsym(handle, get_open_function_flag());
    char *symbol_name = get_open_symbol_name();
    void *open_common_addr = dlsym_ex(handle, symbol_name);
    if (open_common_addr == NULL) {
        LOGD("Error: unable to find the Symbol:%s", symbol_name);
        return;
    }
    LOGD("loaded so finished, start hook");
    void *new_func_addr = get_new_open_addr();
    void **old_func_addr = get_old_open_addr();
    LOGD("hook new_func_addr=%p, old_func_addr=%p", new_func_addr, old_func_addr);
    hook_function(open_common_addr, new_func_addr, old_func_addr);

//    hook_function(open_common_addr, (void *)new_q_open_common, (void **)&old_q_open_common);

//    void *open_with_magic_addr = dlsym_ex(handle, "tDexFileLoader13OpenWithMagicEjiRKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEEbbPS7_PNS1_6vectorINS1_10unique_ptrIKNS_7DexFileENS1_14default_deleteISE_EEEENS5_ISH_EEEE");
//    hook_function(open_common_addr, (void *)new_q_open_with_magic, (void **)&old_q_open_with_magic);
//    printMaps();

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
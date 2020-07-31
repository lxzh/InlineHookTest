#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <jni.h>
#include <android/log.h>
#include "xhook.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <string.h>

#define TAG "xhooker"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO, TAG, FORMAT, ##__VA_ARGS__);
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, FORMAT, ##__VA_ARGS__);
#define LOGV(FORMAT, ...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, FORMAT, ##__VA_ARGS__);
#define LOGW(FORMAT, ...) __android_log_print(ANDROID_LOG_WARN, TAG, FORMAT, ##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, FORMAT, ##__VA_ARGS__);
//
//int (*old_open)(const char *pathname, int flags, mode_t mode);
//
//FILE *(*old_fopen)(const char *path, const char *mode);
//
//FILE *(*old_popen)(const char *path, const char *mode);
//
//char *(*old_strcpy)(char *destination, const char *source);
//
//int (*old_strcmp)(const char *str1, const char *str2);
//
//int (*old_sscanf)(const char *s, const char *format, ...);
//
//const char *(*old_strstr)(const char *str1, const char *str2);
//
//size_t (*old_strlen)(const char *str);
//
//void *(*old_memcpy)(void *destination, const void *source, size_t num);
//
//void *(*old_mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset);
//
//void *(*old_malloc)(size_t size);
//
//pid_t (*old_fork)(void);
//
//int (*old_sp_get)(const char *name, char *value);
//
//void (*old_epic_memcpy)(JNIEnv *env, jclass, jlong src, jlong dest, jint length);
//
//void (*old_epic_memput)(JNIEnv *env, jclass, jbyteArray src, jlong dest);
//
//int new_open(const char *pathname, int flags, mode_t mode) {
//    LOGD("new_open pathname=%s flags=%d", pathname, flags);
//    int rtn = old_open(pathname, flags, mode);
//    LOGD("new_open rtn=%d", rtn);
//    return rtn;
//}
//
//FILE *new_fopen(const char *path, const char *mode) {
//    LOGD("new_fopen path=%s mode=%d", path, *mode);
//    FILE *rtn = old_fopen(path, mode);
////    LOGD("new_fopen rtn=%d", (int)rtn);
//    return rtn;
//}
//
//FILE *new_popen(const char *path, const char *mode) {
//    LOGD("new_popen path=%s mode=%d", path, *mode);
//    FILE *rtn = old_popen(path, mode);
////    LOGD("new_fopen rtn=%d", (int)rtn);
//    return rtn;
//}
//
//char *new_strcpy(char *destination, const char *source) {
//    LOGD("new_strcpy destination=%s source=%s", destination, source);
//    char *rtn = old_strcpy(destination, source);
////    LOGD("new_strcpy rtn=%s", rtn);
//    return rtn;
//}
//
//int new_strcmp(const char *str1, const char *str2) {
//    LOGD("new_strcmp destination=%s source=%s", str1, str2);
//    int rtn = old_strcmp(str1, str2);
//    LOGD("new_strcmp rtn=%d", rtn);
//    return rtn;
//}
//
//int new_sscanf(const char *s, const char *format, ...) {
//    va_list args;
//    va_start(args, format);
//    int rtn = old_sscanf(s, format, args);
//    LOGD("new_sscanf s=%s rtn=%d format=%s", s, rtn, format);
//    va_end(args);
//    return rtn;
//}
//
//const char *new_strstr(const char *str1, const char *str2) {
//    const char *rtn = old_strstr(str1, str2);
//    LOGD("new_strstr str1=%s str2=%s rtn=%s", str1, str2, rtn);
//    return rtn;
//}
//
//size_t new_strlen(const char *str) {
//    size_t rtn = old_strlen(str);
//    LOGD("new_strlen str=%s rtn=%lld", str, (unsigned long long) rtn);
//    return rtn;
//}
//
//void *new_memcpy(void *destination, const void *source, size_t num) {
//    LOGD("new_memcpy destination=%s source=%s num=%d", destination, source, (int) num);
//    old_memcpy(destination, source, num);
//    return destination;
//}
//
//void *new_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset) {
//    void *addr = old_mmap(start, length, prot, flags, fd, offset);
//    LOGD("new_mmap length=%lld prot=%d flags=%d fd=%d", (unsigned long long) length, prot, flags,
//         fd);
//    return addr;
//}
//
//void *new_malloc(size_t size) {
//    LOGD("new_malloc size=%lld", (unsigned long long) size);
//    return old_malloc(size);
//}
//
//pid_t new_fork(void) {
//    LOGD("new_fork");
//    return old_fork();
//}
//
//int new_sp_get(const char *name, char *value) {
//    LOGD("[+] new__system_property_get name=%s value=%s", name, value);
////    if (strstr(name, "ro.serialno")) {
////        return strlen(strcpy(value, "AABBCC"));
////    } else {
////        return old__system_property_get(name, value);
////    }
//    return old_sp_get(name, value);
//}
//
//void new_epic_memcpy(JNIEnv *env, jclass jclz, jlong src, jlong dest, jint length) {
//    LOGD("new_epic_memcpy src=%s dest=%s length=%d", (char *) src, (char *) dest, (int) length);
//    old_epic_memcpy(env, jclz, src, dest, length);
//}
//
//void new_epic_memput(JNIEnv *env, jclass jclz, jbyteArray src, jlong dest) {
//    LOGD("new_epic_memput");
//    old_epic_memput(env, jclz, src, dest);
//}

//static int my_system_log_print(int prio, const char *tag, const char *fmt, ...) {
//    va_list ap;
//    char buf[1024];
//    int r;
//
//    snprintf(buf, sizeof(buf), "[%s] %s", (NULL == tag ? "" : tag), (NULL == fmt ? "" : fmt));
//
//    va_start(ap, fmt);
//    r = __android_log_vprint(prio, "xhooker1", buf, ap);
//    va_end(ap);
//    return r;
//}
//
//static int my_xhooker_log_print(int prio, const char *tag, const char *fmt, ...) {
//    va_list ap;
//    char buf[1024];
//    int r;
//
//    snprintf(buf, sizeof(buf), "[%s] %s", (NULL == tag ? "" : tag), (NULL == fmt ? "" : fmt));
//
//    va_start(ap, fmt);
//    r = __android_log_vprint(prio, "xhooker1", buf, ap);
//    va_end(ap);
//    return r;
//}

int (*old_puts)(const char *) = NULL;

int new_puts(const char *string) {
    LOGD("DumpDex:hook puts success, input string:%s", string);
    return old_puts(string);
}

void put111(const char *string) {
    LOGD("DumpDex:put111 string %s", string);
}

void (*old_puts111)(const char *);

void new_puts111(const char *string) {
    LOGD("DumpDex:new_puts111 string=%s", string);
    old_puts111(string);
}

static void *new_thread_func(void *arg)
{
    (void)arg;
    unsigned int i = 0;

    while(1)
    {
        LOGD("DumpDex:new_thread_func put111 success. %u\n", i);
        put111("DumpDex:new_thread_func puts111 ***************");
        i++;
        sleep(1);
    }

    return NULL;
}

int (*old_IsMagicValid)(uint32_t magic) = NULL;
int new_IsMagicValid(uint32_t magic) {
    LOGD("new_IsMagicValid magic:%d", magic);
    return old_IsMagicValid(magic);
}


void Java_com_lxzh123_hooker_XHooker_start(JNIEnv *env, jobject obj) {
    (void) env;
    (void) obj;

//    xhook_register(".*/libmainNative\\.so$",  "mmap", testmmap,  old_mmap);
//    xhook_register("^/system/.*\\.so$",  "__android_log_print", my_system_log_print,  NULL);
//    xhook_register("^/vendor/.*\\.so$",  "__android_log_print", my_system_log_print,  NULL);
//    xhook_register(".*/libxhooker\\.so$", "__android_log_print", my_xhooker_log_print, NULL);
//    xhook_refresh(0);    // 同步或一步处理。这里是同步处理。
//    LOGD("Java_com_lxzh123_hooker_XHooker_start");
//    xhook_register(".*/libtongdun\\.so$", "open",   new_open,   (void **) (&old_open));
//    xhook_register(".*/libtongdun\\.so$", "fopen",  new_fopen,  (void **) (&old_fopen));
//    xhook_register(".*/libtongdun\\.so$", "strcpy", new_strcpy, (void **) (&old_strcpy));
//    xhook_register(".*/libtongdun\\.so$", "strcmp", new_strcmp, (void **) (&old_strcmp));
//    xhook_register(".*/libtongdun\\.so$", "memcpy", new_memcpy, (void **) (&old_memcpy));
//    xhook_register("^/system/.*\\.so$", "open", new_open, (void **) (&old_open));
//
//    char *so1 = ".*/libtongdun.so$";
//    char *so2 = "/data/app/com.lxzh123.tddemo-2/lib/.*.so$";
//
//    xhook_register(so2, "open", new_open, (void **) (&old_open));
////    xhook_register(so1, "fopen", new_fopen, (void **) (&old_fopen));
//    xhook_register(so2, "fopen", new_fopen, (void **) (&old_fopen));
//    xhook_register(so2, "popen", new_popen, (void **) (&old_popen));
//    xhook_register(so2, "strcpy", new_strcpy, (void **) (&old_strcpy));
////    xhook_register(so2, "strcmp", new_strcmp, (void **) (&old_strcmp));
////    xhook_register(so2, "sscanf", new_sscanf, (void **) (&old_sscanf));
//    xhook_register(so2, "strstr", new_strstr, (void **) (&old_strstr));
////    xhook_register(so2, "strlen", new_strlen, (void **) (&old_strlen));
//    xhook_register(so2, "mmap", new_mmap, (void **) (&old_mmap));
//    xhook_register(so2, "fork", new_fork, (void **) (&old_fork));
//
//    xhook_register(so1, "__system_property_get", new_sp_get, (void **) (&old_sp_get));
//    xhook_register(so2, "epic_memcpy", new_epic_memcpy, (void **) (&old_epic_memcpy));
//    xhook_register(so2, "epic_memput", new_epic_memput, (void **) (&old_epic_memput));
//
//    xhook_register(so1, "malloc", new_malloc, (void **) (&old_malloc));
//    xhook_register(so2, "malloc", new_malloc, (void **) (&old_malloc));

//     xhook_register(".*.so$", "open", new_open, (void **) (&old_open)));
//     xhook_register(".*.so$", "fopen", new_fopen, (void **) (&old_fopen)));
//     xhook_register(".*.so$", "popen", new_popen, (void **) (&old_popen)));
//     xhook_register(".*.so$", "strcpy", new_strcpy, (void **) (&old_strcpy)));
//     xhook_register(".*.so$", "strcmp", new_strcmp, (void **) (&old_strcmp)));
//     xhook_register(".*.so$", "mmap", new_mmap, (void **) (&old_mmap)));
//     xhook_register(".*.so$", "fork", new_fork, (void **) (&old_fork)));
//     xhook_register(".*.so$", "__system_property_get", new_system_property_get,
//                   (void **) (&old_system_property_get)));
//    xhook_register("^/system/.*\\.so$", "memcpy", new_fork, (void **) (&old_fork));
//    xhook_refresh(1);

//    xhook_register("^/system/.*\\.so$",  "fopen", my_system_log_print,  NULL);
    //xhook_register("^/system/.*\\.so$",  "fopen", my_system_log_print,  NULL);

    //xhook_register("^/system/.*\\.so$",  "__android_log_print", my_system_log_print,  NULL);
    //xhook_register("^/vendor/.*\\.so$",  "__android_log_print", my_system_log_print,  NULL);
    //xhook_register(".*/libtest\\.so$", "__android_log_print", my_libtest_log_print, NULL);

    //just for testing
    //xhook_ignore(".*/liblog\\.so$", "__android_log_print"); //ignore __android_log_print in liblog.so
    //xhook_ignore(".*/libjavacore\\.so$", NULL); //ignore all hooks in libjavacore.so


    LOGD("DumpDex: hook start");
    put111("DumpDex:test xhook 1***************");
    xhook_register(".*.so$", "put", new_puts, (void **) (&old_puts));
    xhook_register(".*.so$", "put111", new_puts111, (void **) (&old_puts111));
    xhook_register(".*.so$", "_ZN3art13DexFileLoader12IsMagicValidEj", new_IsMagicValid, (void **) (&old_IsMagicValid));

    LOGD("DumpDex: hook 1");
    xhook_refresh(1);
    LOGD("DumpDex: hook 2");
    put111("DumpDex:test xhook 2***************");
    LOGD("DumpDex: hook 3");
//    pthread_t tid;
//    pthread_create(&tid, NULL, &new_thread_func, NULL);
}

JNIEXPORT void JNICALL Java_com_lxzh123_hooker_XHooker_load
        (JNIEnv *env, jobject obj) {
    (void) env;
    (void) obj;
    puts("DumpDex:load puts ***************");
    put111("DumpDex:load puts111 ***************");
}

JNIEXPORT jint JNICALL Java_com_lxzh123_hooker_XHooker_refresh
        (JNIEnv *env, jobject obj, jboolean async) {
    (void) env;
    (void) obj;
    return xhook_refresh(async ? 1 : 0);
}

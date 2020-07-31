//
// Created by lxzh123 on 2018/4/8.
//


#include "deviceutils.h"
#include "fileutils.h"
#include "common.h"

const static long DEX_MIN_LEN = 102400L;
static int sdk_int = 0;

void init_sdk_init() {
    if (sdk_int != 0) {
        return;
    }
    char sdk[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.sdk", sdk);
    sdk_int = atoi(sdk);
}

bool is_arm_64() {
#if defined(__aarch64__)
    return true;
#else
    return false;
#endif
}

bool is_r() {
    return sdk_int == 30;
}

bool is_q() {
    return sdk_int == 29;
}

bool is_pie() {
    return sdk_int == 28;
}

bool is_oreo() {
    return sdk_int == 27 || sdk_int == 26;
}

bool is_nougat() {
    return sdk_int == 25 || sdk_int == 24;
}

bool is_marshmallow() {
    return sdk_int == 23;
}

char *get_open_elf_name() {
    init_sdk_init();
    if(is_q()) {
        return "libdexfile.so";
    } else {
        return "libart.so";
    }
}

char *get_open_function_flag() {
    init_sdk_init();
    if (is_arm_64()) {
        if (is_pie() || is_q()) {
            return "_ZN3art13DexFileLoader10OpenCommonEPKhmS2_mRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPKNS_10OatDexFileEbbPS9_NS3_10unique_ptrINS_16DexFileContainerENS3_14default_deleteISH_EEEEPNS0_12VerifyResultE";
        }
        if (is_oreo()) {
            return "_ZN3art7DexFile10OpenCommonEPKhmRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPKNS_10OatDexFileEbbPS9_PNS0_12VerifyResultE";
        }
        if (is_nougat() || is_marshmallow()) {
            return "_ZN3art7DexFile10OpenMemoryEPKhmRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_10OatDexFileEPS9_";
        }
    } else {
        if (is_pie() || is_q()) {
            return "_ZN3art13DexFileLoader10OpenCommonEPKhjS2_jRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPKNS_10OatDexFileEbbPS9_NS3_10unique_ptrINS_16DexFileContainerENS3_14default_deleteISH_EEEEPNS0_12VerifyResultE";
        }

        if (is_oreo()) {
            return "_ZN3art7DexFile10OpenCommonEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPKNS_10OatDexFileEbbPS9_PNS0_12VerifyResultE";
        }

        if (is_nougat() || is_marshmallow()) {
            return "_ZN3art7DexFile10OpenMemoryEPKhjRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEjPNS_6MemMapEPKNS_10OatDexFileEPS9_";
        }
    }
    return "";
}

//64位
/////////////////////
#if defined(__LP64__) || defined(__aarch64__) || defined(__x86_64__) || defined(__x86_64)
static void *
(*old_arm64_open_common)(uint8_t *, size_t, void *, uint32_t, void *, bool, bool, void *, void *);

static void *new_arm64_open_common(uint8_t *base, size_t size, void *location,
                                   uint32_t location_checksum, void *oat_dex_file,
                                   bool verify,
                                   bool verify_checksum,
                                   void *error_meessage, void *verify_result) {
    LOGI("new_arm64_open_common base=%u, size=%u, location=%u, location_checksum=%u", base,
         size, location, location_checksum);
    if (size < DEX_MIN_LEN) {
        LOGE("size=%u", size);
    } else {
        save_dex_file(base, size);
    }
    void *result = old_arm64_open_common(base, size, location, location_checksum,
                                         oat_dex_file, verify, verify_checksum,
                                         error_meessage,
                                         verify_result);
    return result;
}

static void *
(*old_arm64_q_open_common)(uint8_t *, size_t, const uint8_t *,
                             size_t, void *,
                             uint32_t, void *,
                             bool,
                             bool,
                             void *,
                             void *,
                             void *);
#define __make_rwx(p, n)           ::mprotect(__ptr_align(p), \
                                                  __page_align(__uintval(p) + n) != __page_align(__uintval(p)) ? __page_align(n) + __page_size : __page_align(n), \
                                                  PROT_READ | PROT_WRITE | PROT_EXEC)
static void *new_arm64_q_open_common(uint8_t *base, size_t size, const uint8_t *data_base,
                                       size_t data_size, void *location,
                                       uint32_t location_checksum, void *oat_dex_file,
                                       bool verify,
                                       bool verify_checksum,
                                       void *error_msg,
                                       void *container,
                                       void *verify_result) {
    LOGI("new_arm64_q_open_common base=%u, size=%u, location=%u, location_checksum=%u", base,
         size, location, location_checksum);
    if (size < DEX_MIN_LEN) {
        LOGE("size=%u", size);
    } else {
        save_dex_file(base, size);
    }
//    void *result = old_arm64_q_open_common(base, size, data_base, data_size, location,
//                                         location_checksum,
//                                         oat_dex_file, verify, verify_checksum,
//                                         error_msg, container,
//                                         verify_result);
//    return result;
    return NULL;
}
/////////////////////



/////////////////////
static void *(*old_arm64_open_memory)(uint8_t *base,
                                      size_t size, void *location,
                                      uint32_t location_checksum,
                                      void *mem_map,
                                      void *oat_dex_file, void *error_msg);

static void *
(new_arm64_open_memory)(uint8_t *base, size_t size, void *location,
                        uint32_t location_checksum, void *mem_map,
                        void *oat_dex_file, void *error_msg) {
    LOGI("new_arm64_open_memory base=%p, size=%u, location=%p, location_checksum=%u", base,
         size, location, location_checksum);
    if (size > DEX_MIN_LEN) {
        save_dex_file(base, size);
    }
//    return NULL;
    return (*old_arm64_open_memory)(base, size, location, location_checksum, mem_map,
                                    oat_dex_file, error_msg);
}
/////////////////////

//32位
#else

/////////////////////
static void *
(*old_nougat_open_memory)(void *DexFile_thiz, uint8_t *base, size_t size, void *location,
                          uint32_t location_checksum, void *mem_map, void *oat_dex_file,
                          void *error_msg);

static void *
(new_nougat_open_memory)(void *DexFile_thiz, uint8_t *base, size_t size, void *location,
                         uint32_t location_checksum, void *mem_map, void *oat_dex_file,
                         void *error_msg) {
    LOGI("new_nougat_open_memory DexFile_thiz=%u base=%u, size=%u, location=%u, location_checksum=%u, oat_dex_file=%u",
            (uint32_t)DexFile_thiz, base, size, location, location_checksum, (uint32_t)oat_dex_file);
    if (size > DEX_MIN_LEN) {
        save_dex_file(base, size);
    }
    return (*old_nougat_open_memory)(DexFile_thiz, base, size, location, location_checksum, mem_map,
                                     oat_dex_file, error_msg);
}
/////////////////////

/////////////////////
static void *(*old_opencommon)(void *DexFile_thiz, uint8_t *base, size_t size, void *location,
                               uint32_t location_checksum, void *oat_dex_file, bool verify,
                               bool verify_checksum,
                               void *error_meessage, void *verify_result);


static void *new_opencommon(void *DexFile_thiz, uint8_t *base, size_t size, void *location,
                            uint32_t location_checksum, void *oat_dex_file, bool verify,
                            bool verify_checksum,
                            void *error_meessage, void *verify_result) {
    LOGI("new_opencommon DexFile_thiz=%u base=%u, size=%u, location=%u, location_checksum=%u",
            (uint32_t)DexFile_thiz, base, size, location, location_checksum);
    if (size > DEX_MIN_LEN) {
        save_dex_file(base, size);
    }
    return (*old_opencommon)(DexFile_thiz, base, size, location, location_checksum,
                             oat_dex_file, verify, verify_checksum, error_meessage,
                             verify_result);
}
/////////////////////


static void *(*old_pie_open_memory)(void *DexFile_thiz,
                                    uint8_t *base,
                                    size_t size,
                                    uint8_t *data_base,
                                    size_t data_size,
                                    void *location,
                                    uint32_t location_checksum,
                                    void *oat_dex_file,
                                    bool verify,
                                    bool verify_checksum,
                                    void *error_msg,
                                    void *container,
                                    void *verify_result);

static void *(new_pie_open_memory)(void *DexFile_thiz,
                                   uint8_t *base,
                                   size_t size,
                                   uint8_t *data_base,
                                   size_t data_size,
                                   void *location,
                                   uint32_t location_checksum,
                                   void *oat_dex_file,
                                   bool verify,
                                   bool verify_checksum,
                                   void *error_msg,
                                   void *container,
                                   void *verify_result) {
    LOGI("new_pie_open_memory DexFile_thiz=%u base=%u, size=%u, location=%u, location_checksum=%u",
            (uint32_t)DexFile_thiz, base, size, location, location_checksum);
    if (size > DEX_MIN_LEN) {
        save_dex_file(base, size);
    }
    return (*old_pie_open_memory)(DexFile_thiz,
                                  base,
                                  size,
                                  data_base,
                                  data_size,
                                  location,
                                  location_checksum,
                                  oat_dex_file,
                                  verify,
                                  verify_checksum,
                                  error_msg,
                                  container,
                                  verify_result);
}
#endif

void **get_old_open_function_addr() {
#if defined(__LP64__) || defined(__aarch64__) || defined(__x86_64__) || defined(__x86_64)
        if (is_oreo()) {
            return reinterpret_cast<void **>(&old_arm64_open_common);
        } else if(is_q()) {
            return reinterpret_cast<void **>(&old_arm64_q_open_common);
        } else {
            return reinterpret_cast<void **>(&old_arm64_open_memory);
        }
#else
        if (is_oreo()) {
            return reinterpret_cast<void **>(&old_opencommon);
        } else {
            if (is_pie() || is_q()) {
                return reinterpret_cast<void **>(&old_pie_open_memory);
            }
            return reinterpret_cast<void **>(&old_nougat_open_memory);
        }
#endif
}

void *get_new_open_function_addr() {
    LOGI("get_new_open_function_addr is_arm_64=%u, is_marshmallow=%u, is_nougat=%u, is_oreo=%u, is_pie=%u, is_q=%u, is_r=%u",
         is_arm_64(), is_marshmallow(), is_nougat(), is_oreo(), is_pie(), is_q(), is_r());
#if defined(__LP64__) || defined(__aarch64__) || defined(__x86_64__) || defined(__x86_64)
        if (is_oreo()) {
            return reinterpret_cast<void *>(new_arm64_open_common);
        } else if(is_q()) {
            return reinterpret_cast<void *>(new_arm64_q_open_common);
        } else {
            return reinterpret_cast<void *>(new_arm64_open_memory);
        }
#else
        if (is_oreo()) {
            return reinterpret_cast<void *>(new_opencommon);
        } else {
            if (is_pie() || is_q()) {
                return reinterpret_cast<void *>(new_pie_open_memory);
            }
            return reinterpret_cast<void *>(new_nougat_open_memory);
        }
#endif
}

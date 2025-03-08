#ifndef _UTILS_H_
#define _UTILS_H_

// -------------------------------
// Printing Macros
// -------------------------------
#define RED           "\033[31m"
#define GREEN         "\033[32m"
#define YELLOW        "\033[33m"
#define BLUE          "\033[34m"
#define PURPLE        "\033[35m"
#define CYAN          "\033[36m"
#define WHITE         "\033[37m"
#define BRIGHT_YELLOW "\033[38;5;214m"    
#define RESET_COLOR   "\033[0m"

#define WARNING_COLOR BRIGHT_YELLOW
#define ERROR_COLOR   RED
#define DEBUG_COLOR   PURPLE
#define TODO_COLOR    CYAN

#define COLOR_STR(str, COLOR) COLOR str RESET_COLOR
#define WARNING_LOG(format, ...) printf(COLOR_STR("WARNING:" __FILE__ ":%u: ", BRIGHT_YELLOW) format, __LINE__, ##__VA_ARGS__)
#define TODO(msg) printf(COLOR_STR("TODO: " __FILE__ ":%u: ", TODO_COLOR) msg "\n", __LINE__), assert(FALSE)

#ifdef _DEBUG
	#define DEBUG_LOG(format, ...) printf(COLOR_STR("DEBUG:" __FILE__ ":%u: ", DEBUG_COLOR) format, __LINE__, ##__VA_ARGS__)
#else 
    #define DEBUG_LOG(...)
#endif //_DEBUG

#include "./str_error.h"
#define PERROR_LOG(format, ...) printf(COLOR_STR("WARNING:" __FILE__ ":%u: ", BRIGHT_YELLOW) format ", because: " COLOR_STR("'%s'", BRIGHT_YELLOW) ".\n", __LINE__, ##__VA_ARGS__, str_error())

#define BOOL2STR(cond) (cond ? COLOR_STR("TRUE", GREEN) : COLOR_STR("FALSE", RED))
#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define UNUSED_FUNCTION __attribute__((unused))
#define PACKED_STRUCT __attribute__((packed))
#define CAST_PTR(ptr, type) ((type*) (ptr))
#define UNUSED_VAR(var) (void) var
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define FALSE       0
#define TRUE        1

#define BE_CONVERT(ptr_val, size) be_to_le(ptr_val, size)
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
 	
	UNUSED_FUNCTION static void be_to_le(void* ptr_val, size_t size) {
        for (size_t i = 0; i < size / 2; ++i) {
            unsigned char temp = CAST_PTR(ptr_val, unsigned char)[i];
            CAST_PTR(ptr_val, unsigned char)[i] = CAST_PTR(ptr_val, unsigned char)[size - 1 - i];
            CAST_PTR(ptr_val, unsigned char)[size - 1 - i] = temp;
        }
        return;
    }

#else
    #define be_to_le(ptr_val, size)
#endif // CHECK_ENDIANNESS

#define SAFE_FREE(ptr) do { if ((ptr) != NULL) free(ptr), (ptr) = NULL; } while(0)
#define MULTI_FREE(...) 														\
	do {																		\
		void* ptrs[] = { NULL, ##__VA_ARGS__ };							 		\
		for (unsigned int i = 0; i < ARR_SIZE(ptrs); ++i) SAFE_FREE(ptrs[i]);	\
	} while(0)

#define mem_set(ptr, value, size) mem_set_var(ptr, value, size, sizeof(unsigned char))
#define mem_set_32(ptr, value, size) mem_set_var(ptr, value, size * sizeof(unsigned int), sizeof(unsigned int))
#define mem_set_64(ptr, value, size) mem_set_var(ptr, value, size * sizeof(unsigned long long int), sizeof(unsigned long long int))
static void mem_set_var(void* ptr, int value, size_t size, size_t val_size) {
	if (ptr == NULL) return;
	for (size_t i = 0; i < size; ++i) CAST_PTR(ptr, unsigned char)[i] = CAST_PTR(&value, unsigned char)[i % val_size]; 
	return;
}

static void* mem_cpy(void* dest, const void* src, size_t size) {
	if (dest == NULL || src == NULL) return NULL;
	for (size_t i = 0; i < size; ++i) CAST_PTR(dest, unsigned char)[i] = CAST_PTR(src, unsigned char)[i];
	return dest;
}

static void mem_move(void* dest, const void* src, size_t size) {
    if (dest == NULL || src == NULL || size == 0) return;
    
	unsigned char* temp = (unsigned char*) calloc(size, sizeof(unsigned char));
	
	for (size_t i = 0; i < size; ++i) *CAST_PTR(temp + i, unsigned char) = *CAST_PTR(CAST_PTR(src, unsigned char) + i, unsigned char); 
    for (size_t i = 0; i < size; ++i) *CAST_PTR(CAST_PTR(dest, unsigned char) + i, unsigned char) = *CAST_PTR(temp + i, unsigned char);
    
	SAFE_FREE(temp);
    
    return;
}

UNUSED_FUNCTION static int mem_n_cmp(const void* ptr1, const void* ptr2, size_t n) {
    // Null Checks
    if (ptr1 == NULL && ptr2 == NULL) return 0;
    if (ptr1 == NULL) return -1;
    else if (ptr2 == NULL) return 1;

    unsigned char* a = (unsigned char*) ptr1;
    unsigned char* b = (unsigned char*) ptr2;

	while (n--) if (*a++ != *b++) return *(a - 1) - *(b - 1);

	return 0;
}

UNUSED_FUNCTION static size_t str_len(const char* str) {
    if (str == NULL) return 0;
	size_t i = 0;
    while (*str++) ++i;
    return i;
}

UNUSED_FUNCTION static int str_n_cmp(const char* str1, const char* str2, size_t n) {
    // Null Checks
    if (str1 == NULL && str2 == NULL) return 0;
    if (str1 == NULL) return -1;
    else if (str2 == NULL) return 1;

    size_t i = 0;
    while ((str1[i] != '\0' || str2[i] != '\0') && i < n) {
        if (str1[i] != str2[i]) return str1[i] - str2[i];
        ++i;
    }

	return 0;
}

#endif //_UTILS_H_


/*
 * Copyright (C) 2025 TheProgxy <theprogxy@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _QCOW_UTILS_H_
#define _QCOW_UTILS_H_

#ifdef _QCOW_PRINTING_UTILS_
// -------------------------------
// Printing Macros
// -------------------------------
#define RED           "\033[31m"
#define GREEN         "\033[32m"
#define PURPLE        "\033[35m"
#define CYAN          "\033[36m"
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

#endif //_QCOW_PRINTING_UTILS_

/* -------------------------------------------------------------------------------------------------------- */
// --------
//  Macros 
// --------
#define QCOW_MASK_BITS_INTERVAL(a, b) (((char)(a) >= (char)(b)) ? (QCOW_MASK_BITS_PRECEDING(a) & ~QCOW_MASK_BITS_PRECEDING(b)) : 0)
#define QCOW_BOOL2STR(cond) (cond ? COLOR_STR("TRUE", GREEN) : COLOR_STR("FALSE", RED))
#define QCOW_MASK_BITS_PRECEDING(bit_index) ((1ULL << (bit_index)) - 1)
#define QCOW_ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define QCOW_CAST_PTR(ptr, type) ((type*) (ptr))

#ifndef _QCOW_CUSTOM_ALLOCATORS_
	#define qcow_calloc  calloc
	#define qcow_realloc realloc
	#define qcow_free    free
#else
	#if !defined(qcow_calloc) || !defined(qcow_realloc) || !defined(qcow_free)
		#error "If using custom allocators all qcow_calloc, qcow_realloc and qcow_free must be defined."
		#include <stophere>
	#endif // check definitions
#endif //_QCOW_CUSTOM_ALLOCATORS_

#ifndef MIN
	#define MIN(a, b) (a < b ? a : b)
	#define MAX(a, b) (a > b ? a : b)
#endif //MIN

#ifndef UNUSED_FUNCTION
	#define UNUSED_FUNCTION  __attribute__((unused))
	#define PACKED_STRUCT    __attribute__((packed))
	#define UNUSED_VAR(var) (void) var
#endif //UNUSED_FUNCTION

#ifndef FALSE
	#define FALSE 0
	#define TRUE  1
#endif //FALSE

/* -------------------------------------------------------------------------------------------------------- */
// ------------------
//  Macros Functions
// ------------------
#define QCOW_BE_CONVERT(ptr_val, size) qcow_be_to_le(ptr_val, size)
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

	UNUSED_FUNCTION static void qcow_be_to_le(void* ptr_val, size_t size) {
        for (size_t i = 0; i < size / 2; ++i) {
            unsigned char temp = QCOW_CAST_PTR(ptr_val, unsigned char)[i];
            QCOW_CAST_PTR(ptr_val, unsigned char)[i] = QCOW_CAST_PTR(ptr_val, unsigned char)[size - 1 - i];
            QCOW_CAST_PTR(ptr_val, unsigned char)[size - 1 - i] = temp;
        }
        return;
    }

#else
    #define qcow_be_to_le(ptr_val, size)
#endif // CHECK_ENDIANNESS

#define QCOW_SAFE_FREE(ptr) do { if ((ptr) != NULL) qcow_free(ptr), (ptr) = NULL; } while(0)
#define QCOW_MULTI_FREE(...) 															\
	do {																				\
		void* ptrs[] = { NULL, ##__VA_ARGS__ };							 				\
		for (unsigned int i = 0; i < QCOW_ARR_SIZE(ptrs); ++i) QCOW_SAFE_FREE(ptrs[i]);	\
	} while(0)

/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
#define mem_set(ptr, value, size) mem_set_var(ptr, value, size, sizeof(unsigned char))
#define mem_set_32(ptr, value, size) mem_set_var(ptr, value, size * sizeof(unsigned int), sizeof(unsigned int))
#define mem_set_64(ptr, value, size) mem_set_var(ptr, value, size * sizeof(unsigned long long int), sizeof(unsigned long long int))
static void mem_set_var(void* ptr, int value, size_t size, size_t val_size);
static void* mem_cpy(void* dest, const void* src, size_t size);
static void mem_move(void* dest, const void* src, size_t size);
UNUSED_FUNCTION static int mem_n_cmp(const void* ptr1, const void* ptr2, size_t n);
UNUSED_FUNCTION static size_t str_len(const char* str);
UNUSED_FUNCTION static int str_n_cmp(const char* str1, const char* str2, size_t n);

/* -------------------------------------------------------------------------------------------------------- */
#ifdef _QCOW_UTILS_IMPLEMENTATION_
static void mem_set_var(void* ptr, int value, size_t size, size_t val_size) {
	if (ptr == NULL) return;
	for (size_t i = 0; i < size; ++i) QCOW_CAST_PTR(ptr, unsigned char)[i] = QCOW_CAST_PTR(&value, unsigned char)[i % val_size]; 
	return;
}

static void* mem_cpy(void* dest, const void* src, size_t size) {
	if (dest == NULL || src == NULL) return NULL;
	for (size_t i = 0; i < size; ++i) QCOW_CAST_PTR(dest, unsigned char)[i] = QCOW_CAST_PTR(src, unsigned char)[i];
	return dest;
}

static void mem_move(void* dest, const void* src, size_t size) {
    if (dest == NULL || src == NULL || size == 0) return;
    
	unsigned char* temp = (unsigned char*) qcow_calloc(size, sizeof(unsigned char));
	
	for (size_t i = 0; i < size; ++i) *QCOW_CAST_PTR(temp + i, unsigned char) = *QCOW_CAST_PTR(QCOW_CAST_PTR(src, unsigned char) + i, unsigned char); 
    for (size_t i = 0; i < size; ++i) *QCOW_CAST_PTR(QCOW_CAST_PTR(dest, unsigned char) + i, unsigned char) = *QCOW_CAST_PTR(temp + i, unsigned char);
    
	QCOW_SAFE_FREE(temp);
    
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

#endif //_QCOW_UTILS_IMPLEMENTATION_

#endif //_QCOW_UTILS_H_


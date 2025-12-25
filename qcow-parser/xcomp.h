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
#ifndef _XCOMP_H_
#define _XCOMP_H_
#include <stdio.h>
#include <stdlib.h>
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

#ifndef _XCOMP_UTILS_H_
#define _XCOMP_UTILS_H_

#ifdef _XCOMP_PRINTING_UTILS_
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

#ifndef _XCOMP_NO_PERROR_
	#include "./str_error.h"
	#define PERROR_LOG(format, ...) printf(COLOR_STR("WARNING:" __FILE__ ":%u: ", BRIGHT_YELLOW) format ", because: " COLOR_STR("'%s'", BRIGHT_YELLOW) ".\n", __LINE__, ##__VA_ARGS__, str_error())
#endif // _XCOMP_NO_PERROR_

#endif //_XCOMP_PRINTING_UTILS_

/* -------------------------------------------------------------------------------------------------------- */
// --------
//  Macros 
// --------
#define XCOMP_MASK_BITS_INTERVAL(a, b) (((char)(a) >= (char)(b)) ? (XCOMP_MASK_BITS_PRECEDING(a) & ~XCOMP_MASK_BITS_PRECEDING(b)) : 0)
#define XCOMP_BOOL2STR(cond) (cond ? COLOR_STR("TRUE", GREEN) : COLOR_STR("FALSE", RED))
#define XCOMP_MASK_BITS_PRECEDING(bit_index) ((1ULL << (bit_index)) - 1)
#define XCOMP_ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define XCOMP_CAST_PTR(ptr, type) ((type*) (ptr))

#ifndef _XCOMP_CUSTOM_ALLOCATORS_
	#define xcomp_calloc  calloc
	#define xcomp_realloc realloc
	#define xcomp_free    free
#else
	#if !defined(xcomp_calloc) || !defined(xcomp_realloc) || !defined(xcomp_free)
		#error "If using custom allocators all xcomp_calloc, xcomp_realloc and xcomp_free must be defined."
		#include <stophere>
	#endif // check definitions
#endif //_XCOMP_CUSTOM_ALLOCATORS_

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
#define XCOMP_BE_CONVERT(ptr_val, size) xcomp_be_to_le(ptr_val, size)
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

	UNUSED_FUNCTION static void xcomp_be_to_le(void* ptr_val, size_t size) {
        for (size_t i = 0; i < size / 2; ++i) {
            unsigned char temp = XCOMP_CAST_PTR(ptr_val, unsigned char)[i];
            XCOMP_CAST_PTR(ptr_val, unsigned char)[i] = XCOMP_CAST_PTR(ptr_val, unsigned char)[size - 1 - i];
            XCOMP_CAST_PTR(ptr_val, unsigned char)[size - 1 - i] = temp;
        }
        return;
    }

#else
    #define xcomp_be_to_le(ptr_val, size)
#endif // CHECK_ENDIANNESS

#define XCOMP_SAFE_FREE(ptr) do { if ((ptr) != NULL) xcomp_free(ptr), (ptr) = NULL; } while(0)
#define XCOMP_MULTI_FREE(...) 															\
	do {																				\
		void* ptrs[] = { NULL, ##__VA_ARGS__ };							 				\
		for (unsigned int i = 0; i < XCOMP_ARR_SIZE(ptrs); ++i) XCOMP_SAFE_FREE(ptrs[i]);	\
	} while(0)

/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
#define mem_set(ptr, value, size)    mem_set_var(ptr, value, size, sizeof(unsigned char))
#define mem_set_32(ptr, value, size) mem_set_var(ptr, value, size, sizeof(unsigned int))
#define mem_set_64(ptr, value, size) mem_set_var(ptr, value, size, sizeof(unsigned long long int))
static void mem_set_var(void* ptr, int value, size_t size, size_t val_size);
static void* mem_cpy(void* dest, const void* src, size_t size);
static void mem_move(void* dest, const void* src, size_t size);
UNUSED_FUNCTION static int mem_n_cmp(const void* ptr1, const void* ptr2, size_t n);
UNUSED_FUNCTION static size_t str_len(const char* str);
UNUSED_FUNCTION static int str_n_cmp(const char* str1, const char* str2, size_t n);

/* -------------------------------------------------------------------------------------------------------- */
#ifdef _XCOMP_UTILS_IMPLEMENTATION_
static void mem_set_var(void* ptr, int value, size_t size, size_t val_size) {
	if (ptr == NULL) return;
	for (size_t i = 0; i < size; ++i) XCOMP_CAST_PTR(ptr, unsigned char)[i] = XCOMP_CAST_PTR(&value, unsigned char)[i % val_size]; 
	return;
}

static void* mem_cpy(void* dest, const void* src, size_t size) {
	if (dest == NULL || src == NULL) return NULL;
	for (size_t i = 0; i < size; ++i) XCOMP_CAST_PTR(dest, unsigned char)[i] = XCOMP_CAST_PTR(src, unsigned char)[i];
	return dest;
}

static void mem_move(void* dest, const void* src, size_t size) {
    if (dest == NULL || src == NULL || size == 0) return;
    
	unsigned char* temp = (unsigned char*) xcomp_calloc(size, sizeof(unsigned char));
	
	for (size_t i = 0; i < size; ++i) *XCOMP_CAST_PTR(temp + i, unsigned char) = *XCOMP_CAST_PTR(XCOMP_CAST_PTR(src, unsigned char) + i, unsigned char); 
    for (size_t i = 0; i < size; ++i) *XCOMP_CAST_PTR(XCOMP_CAST_PTR(dest, unsigned char) + i, unsigned char) = *XCOMP_CAST_PTR(temp + i, unsigned char);
    
	XCOMP_SAFE_FREE(temp);
    
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

#endif //_XCOMP_UTILS_IMPLEMENTATION_

#endif //_XCOMP_UTILS_H_

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

#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

// -------------------
//  Macros Definition
// -------------------
#define IS_EOS(bit_stream) ((bit_stream) -> byte_pos == (bit_stream) -> size)
#define IS_REVERSED_EOS(bit_stream) ((bit_stream) -> byte_pos == 0 && (bit_stream) -> bit_pos == 0)
#define REWIND_BIT_STREAM(bit_stream) ((bit_stream) -> byte_pos = 0, (bit_stream) -> bit_pos = 0, (bit_stream) -> error = 0)
#define CREATE_BIT_STREAM(data_stream, data_size) (BitStream) { .stream = data_stream, .size = data_size, .byte_pos = 0, .bit_pos = 0, .bit_lower_limit = 0, .error = 0 }
#define CREATE_REVERSED_BIT_STREAM(data_stream, data_size, data_lower_limit) (BitStream) { .stream = data_stream, .size = data_size, .byte_pos = data_size - 1, .bit_pos = 8, .bit_lower_limit = data_lower_limit, .error = 0 }
#define PRINT_BIT_STREAM_INFO(bit_stream) DEBUG_LOG("%s: byte_pos: %u, bit_pos: %d, size: %u, error: %u, current_byte: 0x%X.\n", #bit_stream, (bit_stream) -> byte_pos, (bit_stream) -> bit_pos, (bit_stream) -> size, (bit_stream) -> error, ((bit_stream) -> stream)[(bit_stream) -> byte_pos])

#define BITSTREAM_IO_ERROR 1
#define SAFE_NEXT_BIT_READ(bit_stream, value, ...) 									\
	0;																				\
	do {																			\
		value = bitstream_read_next_bit((bit_stream));								\
		 if ((bit_stream) -> error) {												\
	 	 	XCOMP_MULTI_FREE(__VA_ARGS__);												\
		 	WARNING_LOG("An error occurred while reading from the bitstream.\n");	\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
	} while (0)

#define SAFE_BITS_READ(bit_stream, value, nb_bits, ...) 							\
	0;																				\
	do {																			\
		value = bitstream_read_bits((bit_stream), (nb_bits));						\
		 if ((bit_stream) -> error) {												\
	 	 	XCOMP_MULTI_FREE(__VA_ARGS__);												\
		 	WARNING_LOG("An error occurred while reading from the bitstream.\n");	\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
	} while (0)

#define SAFE_BYTE_READ(bit_stream, size, nmemb, var, ...) 							\
	NULL;																			\
	do {																			\
		void* _tmp = bitstream_read_bytes((bit_stream), (size), (nmemb));			\
		 if (_tmp == NULL) {														\
	 	 	XCOMP_MULTI_FREE(__VA_ARGS__);												\
		 	WARNING_LOG("An error occurred while reading from the bitstream.\n");	\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
		 var = _tmp;																\
	} while (0)

#define SAFE_BYTE_READ_WITH_CAST(bit_stream, size, nmemb, type, var, def_val, ...)  \
	def_val;																		\
	do {																			\
		void* _tmp = bitstream_read_bytes((bit_stream), (size), (nmemb));			\
		 if (_tmp == NULL) {														\
	 	 	XCOMP_MULTI_FREE(__VA_ARGS__);												\
		 	WARNING_LOG("An error occurred while reading from the bitstream.\n");	\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
		 var = *XCOMP_CAST_PTR(_tmp, type);												\
	} while (0)

#define SAFE_NEXT_BIT_WRITE(bit_stream, bit, ...) 									\
	do {																			\
		bitstream_write_next_bit((bit_stream), (bit));								\
		if ((bit_stream) -> error) {												\
	 	 	XCOMP_MULTI_FREE(__VA_ARGS__);												\
		 	WARNING_LOG("An error occurred while writing to the bitstream.\n");		\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
	} while (0)

#define SAFE_BIT_WRITE(bit_stream, value, nb_bits, ...) 							\
	do {																			\
		bitstream_write_bits((bit_stream), (value), (nb_bits));						\
		if ((bit_stream) -> error) {												\
	 	 	__VA_ARGS__;															\
		 	WARNING_LOG("An error occurred while writing to the bitstream.\n");		\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
	} while (0)

#define SAFE_REV_BIT_WRITE(bit_stream, value, nb_bits, ...) 						\
	do {																			\
		bitstream_write_bits_reversed((bit_stream), (value), (nb_bits));			\
		if ((bit_stream) -> error) {												\
	 	 	__VA_ARGS__;															\
		 	WARNING_LOG("An error occurred while writing to the bitstream.\n");		\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
	} while (0)

#define SAFE_BYTE_WRITE(bit_stream, size, nmemb, var, ...) 							\
	do {																			\
		bitstream_write_bytes((bit_stream), (size), (nmemb), var);					\
		 if ((bit_stream) -> error) {												\
	 	 	XCOMP_MULTI_FREE(__VA_ARGS__);												\
		 	WARNING_LOG("An error occurred while writing to the bitstream.\n");		\
		 	return -BITSTREAM_IO_ERROR;												\
		 }																			\
	} while (0)

/* ---------------------------------------------------------------------------------------------------------- */
// ---------
//  Structs
// ---------
typedef struct PACKED_STRUCT BitStream {
	unsigned char* stream;
	unsigned int size;
	char bit_lower_limit;
	unsigned int byte_pos;
	char bit_pos;
	unsigned char error;
} BitStream;

/* ---------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
static void* bitstream_read_bytes(BitStream* bit_stream, unsigned int size, unsigned int nmemb);
static unsigned char bitstream_read_next_bit(BitStream* bit_stream);
static unsigned long long int bitstream_read_bits(BitStream* bit_stream, unsigned char n_bits);
static void* reversed_bitstream_read_bytes(BitStream* reversed_bit_stream, unsigned int size, unsigned int nmemb);
static unsigned char reversed_bitstream_read_next_bit(BitStream* reversed_bit_stream);
UNUSED_FUNCTION static unsigned long long int reversed_bitstream_read_bits(BitStream* reversed_bit_stream, unsigned char n_bits);
UNUSED_FUNCTION static void bitstream_unread_bit(BitStream* bit_stream);
UNUSED_FUNCTION static void skip_to_next_byte(BitStream* bit_stream);
static void resize_bit_stream(BitStream* bit_stream);
static void bitstream_write_next_bit(BitStream* bit_stream, unsigned char bit);
UNUSED_FUNCTION static void bitstream_write_bits_reversed(BitStream* bit_stream, unsigned long long int bits, unsigned char n_bits);
UNUSED_FUNCTION static void bitstream_write_bits(BitStream* bit_stream, unsigned long long int bits, unsigned char n_bits);
UNUSED_FUNCTION static void deallocate_bit_stream(BitStream* bit_stream);

/* ---------------------------------------------------------------------------------------------------------- */

// TODO: All this functions can be further optimized for performance.
static void* bitstream_read_bytes(BitStream* bit_stream, unsigned int size, unsigned int nmemb) {
	if (bit_stream -> error) {
		WARNING_LOG("Bitstream error bytes.\n"); 
		return NULL;
	}

	unsigned int tot_size = size * nmemb;
	if (bit_stream -> bit_pos != 0) {
		tot_size--;
		(bit_stream -> byte_pos)++;
	}

	if (bit_stream -> byte_pos + tot_size > bit_stream -> size) {
		bit_stream -> error = 1;
		WARNING_LOG("Bitstream gen error bytes, size: %u, nmemb: %u.\n", size, nmemb); 
		PRINT_BIT_STREAM_INFO(bit_stream);
		return NULL;
	}

	bit_stream -> bit_pos = 0;
	unsigned int old_pos = bit_stream -> byte_pos;
	bit_stream -> byte_pos += tot_size;
	return bit_stream -> stream + old_pos;
}

static unsigned char bitstream_read_next_bit(BitStream* bit_stream) {
    if (bit_stream -> error) {
		WARNING_LOG("Bitstream error bits.\n"); 
		return 0;
	}
	
	if (bit_stream -> byte_pos >= bit_stream -> size) {
		bit_stream -> error = 1;
		WARNING_LOG("Gen bit error:\n");
		PRINT_BIT_STREAM_INFO(bit_stream);
		return 0;
	}

	unsigned char bit_value = (bit_stream -> stream[bit_stream -> byte_pos] >> bit_stream -> bit_pos) & 1;
	(bit_stream -> bit_pos)++;

	if (bit_stream -> bit_pos > 7) bitstream_read_bytes(bit_stream, sizeof(unsigned char), 1);

    return bit_value;
}

static unsigned long long int bitstream_read_bits(BitStream* bit_stream, unsigned char n_bits) {
	if (n_bits > sizeof(unsigned long long int) * 8) {
		bit_stream -> error = 1;
		WARNING_LOG("Tried to read more than %lu bits: %u\n", sizeof(unsigned long long int) * 8, n_bits);
		return 0;
	}

	unsigned long long int bits = 0;
    for (unsigned char i = 0; i < n_bits; ++i) {
        bits += bitstream_read_next_bit(bit_stream) << i;
		if (bit_stream -> error) {
			WARNING_LOG("Bitstream error n_bits.\n"); 
			return 0;
		}
    }

    return bits;
}

static void* reversed_bitstream_read_bytes(BitStream* reversed_bit_stream, unsigned int size, unsigned int nmemb) {
	if (reversed_bit_stream -> error) {
		WARNING_LOG("Reversed Bitstream error bytes.\n"); 
		return NULL;
	}

	long int tot_size = size * nmemb;
	if (reversed_bit_stream -> bit_pos != 8 && reversed_bit_stream -> byte_pos > 0) {
		tot_size--;
		(reversed_bit_stream -> byte_pos)--;
	}
	
	if (reversed_bit_stream -> byte_pos - tot_size < 0) {
		reversed_bit_stream -> error = 1;
		WARNING_LOG("Reversed Bitstream gen error bytes, size: %u, nmemb: %u.\n", size, nmemb); 
		PRINT_BIT_STREAM_INFO(reversed_bit_stream);
		return NULL;
	}

	reversed_bit_stream -> bit_pos = 8;
	unsigned int old_pos = reversed_bit_stream -> byte_pos;
	reversed_bit_stream -> byte_pos -= tot_size; 

	return reversed_bit_stream -> stream + old_pos;
}

static unsigned char reversed_bitstream_read_next_bit(BitStream* reversed_bit_stream) {
	if (reversed_bit_stream -> error) {
		WARNING_LOG("Reversed Bitstream error bits.\n"); 
		return 0;
	}
	
	if (reversed_bit_stream -> byte_pos == 0 && (reversed_bit_stream -> bit_pos <= 0 && reversed_bit_stream -> bit_pos >= reversed_bit_stream -> bit_lower_limit)) {
		(reversed_bit_stream -> bit_pos)--;
		return 0;
	} else if (reversed_bit_stream -> byte_pos == 0 && reversed_bit_stream -> bit_pos < reversed_bit_stream -> bit_lower_limit) {
		reversed_bit_stream -> error = 1;
		WARNING_LOG("Reversed Gen bit error:\n");
		PRINT_BIT_STREAM_INFO(reversed_bit_stream);
		return 0;
	} 

	if (reversed_bit_stream -> bit_pos == 0) reversed_bitstream_read_bytes(reversed_bit_stream, sizeof(unsigned char), 1);
	
    unsigned char bit_value = (reversed_bit_stream -> stream[reversed_bit_stream -> byte_pos] >> (reversed_bit_stream -> bit_pos - 1)) & 1;
    (reversed_bit_stream -> bit_pos)--;
	
	return bit_value;
}

UNUSED_FUNCTION static unsigned long long int reversed_bitstream_read_bits(BitStream* reversed_bit_stream, unsigned char n_bits) {
	if (n_bits > sizeof(unsigned long long int) * 8) {
		reversed_bit_stream -> error = 1;
		WARNING_LOG("Tried to read more than %lu bits: %u\n", sizeof(unsigned long long int) * 8, n_bits);
		return 0;
	}

	unsigned long long int bits = 0;
    for (unsigned char i = 0; i < n_bits; ++i) {
        bits += reversed_bitstream_read_next_bit(reversed_bit_stream) << (n_bits - i - 1);
    }

    return bits;
}

UNUSED_FUNCTION static void bitstream_unread_bit(BitStream* bit_stream) {
	if (bit_stream -> error) {
		WARNING_LOG("Bitsream error unread_bit.\n");
		return;
	}

	if (bit_stream -> bit_pos) {
	   	(bit_stream -> bit_pos)--;
		return;
	} else if (bit_stream -> byte_pos) {
		(bit_stream -> byte_pos)--;
		bit_stream -> bit_pos = 7;
		return;
	} 
	
	bit_stream -> error = 1;
	WARNING_LOG("Cannot unread a new bit_stream.\n");

	return;
}

UNUSED_FUNCTION static void skip_to_next_byte(BitStream* bit_stream) {
	if (bit_stream -> error) {
		WARNING_LOG("Bitstream error skip.\n");
		return;
	}

	if (bit_stream -> byte_pos >= bit_stream -> size) {
		bit_stream -> error = 1;
		WARNING_LOG("Bitstream gen error skip.\n");
		return;
	}

	bit_stream -> bit_pos = 0;
	(bit_stream -> byte_pos)++;

	return;
}

static void resize_bit_stream(BitStream* bit_stream) {
	if (bit_stream -> error) {
		WARNING_LOG("BitStream error resize stream.\n");
		return;
	}

	bit_stream -> stream = realloc(bit_stream -> stream, bit_stream -> size * sizeof(unsigned char));
	if (bit_stream -> stream == NULL) {
		WARNING_LOG("Failed to reallocate the stream to %u.\n", bit_stream -> size);
		bit_stream -> error = 1;
		return;
	}
	
	mem_set(bit_stream -> stream + bit_stream -> byte_pos, 0, bit_stream -> size - bit_stream -> byte_pos);

	return;
}

UNUSED_FUNCTION static void bitstream_bit_copy(BitStream* dest_bit_stream, BitStream* src_bit_stream) {
	if (dest_bit_stream -> error || src_bit_stream -> error) {
		WARNING_LOG("BitStream error bit copy.\n");
		return;
	}

	unsigned long long int bits_cnt = src_bit_stream -> byte_pos * 8 + src_bit_stream -> bit_pos;
	for (unsigned long long int i = 0; i < bits_cnt; ++i) {
		bitstream_write_next_bit(dest_bit_stream, (src_bit_stream -> stream)[(i - (i % 8)) / 8] >> (i % 8));
		if (dest_bit_stream -> error) {
			WARNING_LOG("BitStream gen error bit copy.\n");
			return;
		}
	}

	return;
}

UNUSED_FUNCTION static void bitstream_write_bytes(BitStream* bit_stream, unsigned int size, unsigned int nmemb, const void* src) {
	if (bit_stream -> error) {
		WARNING_LOG("BitStream error write next bit.\n");
		return;
	}
	
	if (bit_stream -> bit_pos != 0) {
		bit_stream -> bit_pos = 0;
		(bit_stream -> byte_pos)++;
	}

	// Update the size of the bitstream
	unsigned int original_byte_pos = bit_stream -> byte_pos;
	bit_stream -> byte_pos += size * nmemb;
	
	if (bit_stream -> byte_pos >= bit_stream -> size) {
		bit_stream -> size = bit_stream -> byte_pos;
		resize_bit_stream(bit_stream);
		if (bit_stream -> error) {
			WARNING_LOG("Failed to resize the stream.\n");
			return;
		}
	}

	mem_cpy(bit_stream -> stream + original_byte_pos, src, size * nmemb);

	return;
}

static void bitstream_write_next_bit(BitStream* bit_stream, unsigned char bit) {
	if (bit_stream -> error) {
		WARNING_LOG("BitStream error write next bit.\n");
		return;
	}
	
	if (bit_stream -> bit_pos == 8) {
		bit_stream -> bit_pos = 0;
		(bit_stream -> byte_pos)++;
	}
	
	if (bit_stream -> byte_pos >= bit_stream -> size) {
		bit_stream -> size = bit_stream -> byte_pos + 1;
		resize_bit_stream(bit_stream);
		if (bit_stream -> error) {
			WARNING_LOG("Failed to resize the stream.\n");
			return;
		}
	}

	(bit_stream -> stream)[bit_stream -> byte_pos] |= bit << (bit_stream -> bit_pos);
	(bit_stream -> bit_pos)++;

	return;
}

UNUSED_FUNCTION static void bitstream_write_bits_reversed(BitStream* bit_stream, unsigned long long int bits, unsigned char n_bits) {
	if (n_bits > sizeof(unsigned long long int) * 8) {
		bit_stream -> error = 1;
		WARNING_LOG("Tried to write more than %lu bits: %u\n", sizeof(unsigned long long int) * 8, n_bits);
		return;
	}

	unsigned int mask = 1 << (n_bits - 1);
	for (unsigned char i = 0; i < n_bits; ++i, mask >>= 1) {
		bitstream_write_next_bit(bit_stream, (bits & mask) >> (n_bits - 1 - i));
		if (bit_stream -> error) {
			WARNING_LOG("An error occurred while writing %u bits reversed to the stream.\n", n_bits);
			return;
		}
	}

	return;
}

UNUSED_FUNCTION static void bitstream_write_bits(BitStream* bit_stream, unsigned long long int bits, unsigned char n_bits) {
	if (n_bits > sizeof(unsigned long long int) * 8) {
		bit_stream -> error = 1;
		WARNING_LOG("Tried to write more than %lu bits: %u\n", sizeof(unsigned long long int) * 8, n_bits);
		return;
	}

	unsigned int mask = 1;
	for (unsigned char i = 0; i < n_bits; ++i, mask <<= 1) {
		bitstream_write_next_bit(bit_stream, (bits & mask) >> i);
		if (bit_stream -> error) {
			WARNING_LOG("An error occurred while writing %u bits to the stream.\n", n_bits);
			return;
		}
	}

	return;
}

UNUSED_FUNCTION static void deallocate_bit_stream(BitStream* bit_stream) {
	XCOMP_SAFE_FREE(bit_stream -> stream);
	bit_stream -> stream = NULL;
	bit_stream -> byte_pos = 0;
	bit_stream -> bit_pos = 0;
	bit_stream -> size = 0;
	return;
}

#endif //_BITSTREAM_H_

/* Copyright (C) 1991-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
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
// -----------------
//  Constant Values 
// -----------------
/* -------------------------------------------------------------------------------------------------------- */
// -------
//  Enums
// -------
typedef enum PACKED_STRUCT ZstdError {
    ZSTD_NO_ERROR,
    ZSTD_IO_ERROR,
    ZSTD_RESERVED,
    ZSTD_TABLE_LOG_TOO_LARGE,
    ZSTD_CORRUPTED_DATA,
    ZSTD_MAX_SYMBOL_VALUE_TOO_SMALL,
    ZSTD_TOO_MANY_LITERALS,
    ZSTD_CHECKSUM_FAIL,
    ZSTD_INVALID_MAGIC,
    ZSTD_RESERVED_FIELD,
    ZSTD_UNSUPPORTED_FEATURE,
 ZSTD_DECOMPRESSED_SIZE_MISMATCH,
    ZSTD_TODO
} ZstdError;
static const char* zstd_errors_str[] = {
    "ZSTD_NO_ERROR",
    "ZSTD_IO_ERROR",
    "ZSTD_RESERVED",
    "ZSTD_TABLE_LOG_TOO_LARGE",
    "ZSTD_CORRUPTED_DATA",
    "ZSTD_MAX_SYMBOL_VALUE_TOO_SMALL",
    "ZSTD_TOO_MANY_LITERALS",
    "ZSTD_CHECKSUM_FAIL",
    "ZSTD_INVALID_MAGIC",
    "ZSTD_RESERVED_FIELD",
    "ZSTD_UNSUPPORTED_FEATURE",
 "ZSTD_DECOMPRESSED_SIZE_MISMATCH",
    "ZSTD_TODO"
};
typedef enum PACKED_STRUCT LiteralsBlockType { RAW_LITERALS_BLOCK, RLE_LITERALS_BLOCK, COMPRESSED_LITERALS_BLOCK, TREELESS_LITERALS_BLOCK } LiteralsBlockType;
typedef enum PACKED_STRUCT BlockType { RAW_BLOCK, RLE_BLOCK, COMPRESSED_BLOCK, RESERVED_TYPE } BlockType;
typedef enum PACKED_STRUCT CompressionMode { PREDEFINED_MODE, RLE_MODE, FSE_COMPRESSED_MODE, REPEAT_MODE } CompressionMode;
 static const char* literals_blocks_type_str[] = { "RAW_LITERALS_BLOCK", "RLE_LITERALS_BLOCK", "COMPRESSED_LITERALS_BLOCK", "TREELESS_LITERALS_BLOCK" };
 static const char* block_types_str[] = { "RAW_BLOCK", "RLE_BLOCK", "COMPRESSED_BLOCK", "RESERVED_TYPE" };
 static const char* compression_modes_str[] = { "PREDEFINED_MODE", "RLE_MODE", "FSE_COMPRESSED_MODE", "REPEAT_MODE" };
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
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Resources: zstd <http://github.com/facebook/zstd> (original repo) *
 *                 <https://datatracker.ietf.org/doc/html/rfc8878>   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
/* -------------------------------------------------------------------------------------------------------- */
// ------------------
//  Macros Functions
// ------------------
typedef unsigned int u32;
typedef unsigned long long u64;
_Static_assert(sizeof(u32) == 4, "u32 must be 4 bytes");
_Static_assert(sizeof(u64) == 8, "u64 must be 8 bytes");
/* -------------------------------------------------------------------------------------------------------- */
// -----------------
//  Constant Values
// -----------------
static const u64 PRIME64_1 = 0x9E3779B185EBCA87ULL;
static const u64 PRIME64_2 = 0xC2B2AE3D27D4EB4FULL;
static const u64 PRIME64_3 = 0x165667B19E3779F9ULL;
static const u64 PRIME64_4 = 0x85EBCA77C2B2AE63ULL;
static const u64 PRIME64_5 = 0x27D4EB2F165667C5ULL;
/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
static inline u64 xxround(u64 acc_n, u64 lane_n);
static inline u64 merge_accumulator(u64 acc, u64 acc_n);
u64 xxhash64(unsigned char* lane, unsigned int byte_size, u64 seed);
/* -------------------------------------------------------------------------------------------------------- */
static inline u64 xxround(u64 acc_n, u64 lane_n) {
  acc_n += lane_n * PRIME64_2;
  acc_n = __builtin_stdc_rotate_left(acc_n, 31);
  return acc_n * PRIME64_1;
}
static inline u64 merge_accumulator(u64 acc, u64 acc_n) {
  acc ^= xxround(0, acc_n);
  return (acc * PRIME64_1) + PRIME64_4;
}
u64 xxhash64(unsigned char* lane, unsigned int byte_size, u64 seed) {
 u64 acc = 0;
 unsigned int remaining_size = byte_size;
 if (byte_size >= 32) {
  u64 accs[4] = {0};
  accs[0] = seed + PRIME64_1 + PRIME64_2;
  accs[1] = seed + PRIME64_2;
  accs[2] = seed;
  accs[3] = seed - PRIME64_1;
  while (remaining_size >= 32) {
   for (unsigned int i = 0; i < 4; ++i, lane += sizeof(u64)) {
    accs[i] = xxround(accs[i], *XCOMP_CAST_PTR(lane, u64));
   }
   remaining_size -= 32;
  }
  acc = __builtin_stdc_rotate_left(accs[0], 1) + __builtin_stdc_rotate_left(accs[1], 7) + __builtin_stdc_rotate_left(accs[2], 12) + __builtin_stdc_rotate_left(accs[3], 18);
  acc = merge_accumulator(acc, accs[0]);
  acc = merge_accumulator(acc, accs[1]);
  acc = merge_accumulator(acc, accs[2]);
  acc = merge_accumulator(acc, accs[3]);
 } else acc = seed + PRIME64_5;
  acc += byte_size;
 while (remaining_size >= 8) {
      acc ^= xxround(0, *XCOMP_CAST_PTR(lane, u64));
      acc = __builtin_stdc_rotate_left(acc, 27) * PRIME64_1 + PRIME64_4;
      lane += sizeof(u64), remaining_size -= sizeof(u64);
 }
 if (remaining_size >= 4) {
      acc ^= (*XCOMP_CAST_PTR(lane, u32) * PRIME64_1);
      acc = __builtin_stdc_rotate_left(acc, 23) * PRIME64_2 + PRIME64_3;
   lane += sizeof(u32), remaining_size -= sizeof(u32);
 }
 while (remaining_size > 0) {
      acc ^= (*lane++) * PRIME64_5;
      acc = __builtin_stdc_rotate_left(acc, 11) * PRIME64_1;
   remaining_size--;
 }
 acc ^= (acc >> 33);
 acc *= PRIME64_2;
 acc ^= (acc >> 29);
 acc *= PRIME64_3;
 acc ^= (acc >> 32);
 return acc;
}
// TODO: Implement the compression
// -----------------
//  Constant Values
// -----------------
/* -------------------------------------------------------------------------------------------------------- */
// ---------
//  Structs
// ---------
/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
/// NOTE: the stream will be always deallocated both in case of failure and success.
/// 	  Furthermore, the function allocates the returned stream of bytes, so that
/// 	  once it's on the hand of the caller, it's responsible to manage that memory.
unsigned char* zstd_deflate(unsigned char* stream, unsigned int size, unsigned int* compressed_len, int* zstd_err);
/* -------------------------------------------------------------------------------------------------------- */
unsigned char* zstd_deflate(unsigned char* stream, unsigned int size, unsigned int* compressed_len, int* zstd_err) {
 *compressed_len = 0;
 *zstd_err = ZSTD_TODO;
 printf("size: %u\n", size);
 printf("Implement compression algorithm.\n");
 XCOMP_SAFE_FREE(stream);
 return NULL;
}
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
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Resources: zstd <http://github.com/facebook/zstd> (original repo) *
 *                 <https://datatracker.ietf.org/doc/html/rfc8878>   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
// TODO: Rewrite the comments for better orienting external supporters
// TODO: Rewrite the warnings msgs to have a compact style
// TODO: Note that as most of CPUs use little-endian, and that the ZSTD format follows that, it could be unjustified the use of LE_CONVERT to convert to LE from BE, in rare cases.
/* -------------------------------------------------------------------------------------------------------- */
// -------------------
//  Macros Definition
// -------------------
/* -------------------------------------------------------------------------------------------------------- */
// -------------------------------
//  Predefined Distributions Data
// -------------------------------
typedef enum DistributionData {
 LL_MAX_LOG = 9,
 ML_MAX_LOG = 9,
 OL_MAX_LOG = 8,
 PRED_LL_TABLE_LOG = 6,
 PRED_ML_TABLE_LOG = 6,
 PRED_OL_TABLE_LOG = 5
} DistributionData;
static const short int ll_pred_frequencies[] = {
 4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
 3, 2, 1, 1, 1, 1, 1, -1, -1, -1, -1
};
static const short int ml_pred_frequencies[] = {
 1, 4, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1,
 -1, -1, -1, -1
};
static const short int ol_pred_frequencies[] = {
 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1
};
static const unsigned int ll_codes[36][2] = {
 {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0},
 {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0}, {16, 1}, {18, 1},
 {20, 1}, {22, 1}, {24, 2}, {28, 2}, {32, 3}, {40, 3}, {48, 4}, {64, 6},
 {128, 7}, {256, 8}, {512, 9}, {1024, 10}, {2048, 11}, {4096, 12}, {8192, 13},
 {16384, 14}, {32768, 15}, {65536, 16}
};
static const unsigned int ml_codes[53][2] = {
 {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
 {12, 0}, {13, 0}, {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0},
 {20, 0}, {21, 0}, {22, 0}, {23, 0}, {24, 0}, {25, 0}, {26, 0}, {27, 0},
 {28, 0}, {29, 0}, {30, 0}, {31, 0}, {32, 0}, {33, 0}, {34, 0}, {35, 1},
 {37, 1}, {39, 1}, {41, 1}, {43, 2}, {47, 2}, {51, 3}, {59, 3}, {67, 4},
 {83, 4}, {99, 5}, {131, 7}, {259, 8}, {515, 9}, {1027, 10}, {2051, 11},
 {4099, 12}, {8195, 13}, {16387, 14}, {32771, 15}, {65539, 16}
};
/* -------------------------------------------------------------------------------------------------------- */
// ---------
//  Structs
// ---------
typedef struct PACKED_STRUCT LengthCode {
 unsigned int value;
 unsigned int num_bits;
} LengthCode;
typedef struct PACKED_STRUCT FrameHeaderDescriptor {
 unsigned char dictionary_id_flag: 2;
 unsigned char content_checksum_flag: 1;
 unsigned char reserved: 1;
 unsigned char unused: 1;
 unsigned char single_segment_flag: 1;
 unsigned char frame_content_size_flag: 2;
} FrameHeaderDescriptor;
typedef struct PACKED_STRUCT WindowDescriptor {
 unsigned char mantissa: 3;
 unsigned char exponent: 4;
} WindowDescriptor;
typedef struct PACKED_STRUCT BlockHeader {
 unsigned char last_block: 1;
 BlockType block_type: 2;
 unsigned int block_size: 21;
} BlockHeader;
typedef struct PACKED_STRUCT SymbolCompressionModes {
 unsigned char reserved: 2;
 CompressionMode match_len_mode: 2;
 CompressionMode offset_mode: 2;
 CompressionMode literals_len_mode: 2;
} SymbolCompressionModes;
typedef struct FSETableEntry {
 unsigned char symbol; // Decoded symbol
    unsigned char nb_bits; // Bits to read for next state
    unsigned short int baseline; // Base for next state calculation
} FSETableEntry;
typedef struct PACKED_STRUCT LiteralsSectionHeader {
 unsigned int regenerated_size;
 unsigned int compressed_size;
 LiteralsBlockType literals_block_type;
 unsigned char streams_cnt;
} LiteralsSectionHeader;
typedef struct PACKED_STRUCT ZSTDHfEntry {
 unsigned char symbol;
 unsigned char nb_bits;
} ZSTDHfEntry;
typedef struct SequenceSection {
 FSETableEntry* ll_fse_table;
 unsigned char ll_table_log;
 short int ll_rle;
 FSETableEntry* ml_fse_table;
 unsigned char ml_table_log;
 short int ml_rle;
 FSETableEntry* ol_fse_table;
 unsigned char ol_table_log;
 short int ol_rle;
} SequenceSection;
typedef struct PACKED_STRUCT Sequence {
 unsigned int ll_value;
 unsigned int ml_value;
 unsigned int ol_value;
} Sequence;
typedef struct Workspace {
 unsigned int* offset_history;
 unsigned char* frame_buffer;
 unsigned int frame_buffer_len;
 SequenceSection sequence_section;
 ZSTDHfEntry* hf_literals;
 unsigned char table_log;
 unsigned char* literals;
 unsigned int literals_cnt;
 Sequence* sequences;
 unsigned int sequence_len;
 unsigned int hf_tree_desc_size;
 unsigned char max_nb_bits;
} Workspace;
/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
static unsigned char highest_bit(unsigned long long int val);
static void print_fhd(FrameHeaderDescriptor fhd);
static void deallocate_workspace(Workspace* workspace);
static void deallocate_sequence_section(SequenceSection* sequence_section);
static int calc_baseline_and_numbits(unsigned int num_states_total, unsigned int num_states_symbol, unsigned int state_number, unsigned short int* a, unsigned char* b);
static int read_probabilities(BitStream* compressed_bit_stream, unsigned char table_log, unsigned char max_symbol, short int** frequencies, unsigned short int* probabilities_cnt);
static int fse_build_table(unsigned char table_log, short int* frequencies, unsigned short int probabilities_cnt, FSETableEntry** fse_table);
static int read_weights(BitStream* compressed_bit_stream, unsigned char* table_log, unsigned char** weights, unsigned short int* weights_cnt);
static int build_huff_table(BitStream* compressed_bit_stream, Workspace* workspace);
static int huff_decode_stream(BitStream* literals_stream, unsigned short int hf_literals_size, unsigned int regenerated_size, Workspace* workspace);
static int decode_literals(BitStream* compressed_bit_stream, Workspace* workspace, LiteralsSectionHeader lsh);
static int parse_literals_section(BitStream* compressed_bit_stream, Workspace* workspace);
static int decode_sequences(BitStream* sequence_compressed_bit_stream, Workspace* workspace);
static int parse_sequence_section(BitStream* compressed_bit_stream, Workspace* workspace);
static unsigned int update_off_history(unsigned int* offset_history, unsigned int offset, unsigned int ll_value);
static int sequence_execution(Workspace* workspace);
static int decompress_block(BitStream* compressed_bit_stream, Workspace* workspace);
static int parse_block(BitStream* bit_stream, Workspace* workspace, unsigned int block_maximum_size);
static int parse_frames(BitStream* bit_stream, unsigned char** decompressed_data, unsigned int* decompressed_data_length);
/// NOTE: the stream will be always deallocated both in case of failure and success.
/// 	  Furthermore, the function allocates the returned stream of bytes, so that
/// 	  once it's on the hand of the caller, it's responsible to manage that memory.
unsigned char* zstd_inflate(unsigned char* stream, unsigned int size, unsigned int* decompressed_data_length, int* zstd_err);
/* ---------------------------------------------------------------------------------------------------------- */
// ---------------------------
//  General Utilities Section
// ---------------------------
static unsigned char highest_bit(unsigned long long int val) {
 return 32 - __builtin_clz(val);
}
static void print_fhd(FrameHeaderDescriptor fhd) {
 (void) fhd;
 DEBUG_LOG("FrameHeaderDescriptor: (0x%X)\n", *XCOMP_CAST_PTR(&fhd, unsigned char));
    DEBUG_LOG(" - frame_content_size_flag: %u\n", fhd.frame_content_size_flag);
    DEBUG_LOG(" - single_segment_flag: %u\n", fhd.single_segment_flag);
    DEBUG_LOG(" - unused: %u\n", fhd.unused);
    DEBUG_LOG(" - reserved: %u\n", fhd.reserved);
    DEBUG_LOG(" - content_checksum_flag: %u\n", fhd.content_checksum_flag);
    DEBUG_LOG(" - dictionary_id_flag: %u\n", fhd.dictionary_id_flag);
 return;
}
static void deallocate_workspace(Workspace* workspace) {
 XCOMP_SAFE_FREE(workspace -> frame_buffer);
 XCOMP_SAFE_FREE(workspace -> sequence_section.ll_fse_table);
 XCOMP_SAFE_FREE(workspace -> sequence_section.ml_fse_table);
 XCOMP_SAFE_FREE(workspace -> sequence_section.ol_fse_table);
 XCOMP_SAFE_FREE(workspace -> hf_literals);
 XCOMP_SAFE_FREE(workspace -> literals);
 XCOMP_SAFE_FREE(workspace -> sequences);
 return;
}
static void deallocate_sequence_section(SequenceSection* sequence_section) {
 if (sequence_section -> ll_fse_table) XCOMP_SAFE_FREE(sequence_section -> ll_fse_table);
 if (sequence_section -> ml_fse_table) XCOMP_SAFE_FREE(sequence_section -> ml_fse_table);
 if (sequence_section -> ol_fse_table) XCOMP_SAFE_FREE(sequence_section -> ol_fse_table);
 return;
}
// ---------------------------------------
//  Literals Parsing and Decoding Section
// ---------------------------------------
// TODO: revise a simpler method to substitute this function, cause A is stolen, B is for sure too computationally expensive
static int calc_baseline_and_numbits(unsigned int num_states_total, unsigned int num_states_symbol, unsigned int state_number, unsigned short int* a, unsigned char* b) {
    if (num_states_symbol == 0) {
        *a = 0, *b = 0;
  return ZSTD_NO_ERROR;
    }
 unsigned int num_state_slices = (1U << (highest_bit(num_states_symbol) - 1) == num_states_symbol) ? num_states_symbol : 1U << highest_bit(num_states_symbol); //always power of two
    unsigned int num_double_width_state_slices = num_state_slices - num_states_symbol; //leftovers to the power of two need to be distributed
    unsigned int num_single_width_state_slices = num_states_symbol - num_double_width_state_slices; //these will not receive a double width slice of states
    unsigned int slice_width = num_states_total / num_state_slices; //size of a single width slice of states
 if (slice_width == 0) {
  WARNING_LOG("Slice width cannot be 0.\n");
  return -ZSTD_CORRUPTED_DATA;
 }
 unsigned int num_bits = highest_bit(slice_width) - 1; //number of bits needed to read for one slice
    if (state_number < num_double_width_state_slices) {
        unsigned int baseline = num_single_width_state_slices * slice_width + state_number * slice_width * 2;
        *a = baseline, *b = num_bits + 1;
    } else {
        unsigned int index_shifted = state_number - num_double_width_state_slices;
        *a = (index_shifted * slice_width), *b = num_bits;
    }
 return ZSTD_NO_ERROR;
}
static int read_probabilities(BitStream* compressed_bit_stream, unsigned char table_log, unsigned char max_symbol, short int** frequencies, unsigned short int* probabilities_cnt) {
 if (table_log > 15) return -ZSTD_TABLE_LOG_TOO_LARGE;
 int remaining = (1 << table_log) + 1;
 *frequencies = (short int*) xcomp_realloc(*frequencies, (255 + 1U) * sizeof(short int));
 if (*frequencies == NULL) {
  WARNING_LOG("Failed to allocate frequencies.\n");
  return -ZSTD_IO_ERROR;
 }
 mem_set(*frequencies, 0, (255 + 1U) * sizeof(short int));
 *probabilities_cnt = 0;
 unsigned short int freq_cum_sum = 0;
 unsigned char zero_repeat = FALSE;
 while (TRUE) {
  if (zero_repeat) {
   unsigned char repeat = bitstream_read_bits(compressed_bit_stream, 2);
   *probabilities_cnt += repeat;
   if (repeat == 3) continue;
  }
  unsigned char nb_bits = highest_bit(remaining);
  unsigned short int max = (1 << (nb_bits - 1)) - 1;
  unsigned short int low_threshold = ((1 << nb_bits) - 1) - remaining;
  short int value = bitstream_read_bits(compressed_bit_stream, nb_bits);
  short int small_count = value & max;
  if (small_count < low_threshold) {
   bitstream_unread_bit(compressed_bit_stream);
   value = small_count;
  } else if (value > max) value -= low_threshold;
  value--; // Prediction = value - 1
  if (value < -1 || remaining <= 1) {
   XCOMP_SAFE_FREE(frequencies);
   WARNING_LOG("Predictions cannot be less than 1: %d\n", value);
   return -ZSTD_CORRUPTED_DATA;
  }
  (*frequencies)[(*probabilities_cnt)++] = value;
  freq_cum_sum += abs(value);
  remaining -= abs(value);
  zero_repeat = !value;
  if (*probabilities_cnt > max_symbol + 1 || freq_cum_sum >= (1 << table_log)) break;
 }
 if (*probabilities_cnt > max_symbol + 1) {
  XCOMP_SAFE_FREE(*frequencies);
  WARNING_LOG("Probabilities_cnt %u > %u max_symbol_value.\n", *probabilities_cnt, max_symbol + 1);
  return -ZSTD_MAX_SYMBOL_VALUE_TOO_SMALL;
 } else if (freq_cum_sum != (1 << table_log)) {
  XCOMP_SAFE_FREE(*frequencies);
  WARNING_LOG("Freq_cum_sum %u != %u expected frequencies count.\n", freq_cum_sum, (1 << table_log));
  return -ZSTD_CORRUPTED_DATA;
 }
 *frequencies = (short int*) xcomp_realloc(*frequencies, *probabilities_cnt * sizeof(short int));
 if (*frequencies == NULL) {
  WARNING_LOG("Failed to allocate frequencies.\n");
  return -ZSTD_IO_ERROR;
 }
 if (compressed_bit_stream -> bit_pos) skip_to_next_byte(compressed_bit_stream); // Any remaining bit within the last byte is simply unused.
 return -ZSTD_NO_ERROR;
}
static int fse_build_table(unsigned char table_log, short int* frequencies, unsigned short int probabilities_cnt, FSETableEntry** fse_table) {
 if (table_log > 15) return -ZSTD_TABLE_LOG_TOO_LARGE;
 // Build the decoding table from those probabilites
 unsigned short int table_size = 1 << table_log;
 *fse_table = (FSETableEntry*) xcomp_realloc(*fse_table, table_size * sizeof(FSETableEntry));
 if (*fse_table == NULL) {
  WARNING_LOG("Failed to allocate the fse table.\n");
  return -ZSTD_IO_ERROR;
 }
 // Start by assigning the -1 (also called "less than 1") probabilities' symbols, to the bottom of the table
 unsigned short int negative_index = table_size;
 for (unsigned short int i = 0; i < probabilities_cnt; ++i) {
  if (frequencies[i] == -1) {
   negative_index--;
   (*fse_table)[negative_index].symbol = i;
   (*fse_table)[negative_index].baseline = 0;
   (*fse_table)[negative_index].nb_bits = table_log;
  }
 }
 unsigned short int update_pos = (table_size >> 1) + (table_size >> 3) + 3;
 unsigned short int tab_pos = 0;
 for (unsigned short int i = 0; i < probabilities_cnt; ++i) {
  for (short int j = 0; j < frequencies[i]; ++j) {
   (*fse_table)[tab_pos].symbol = i;
   tab_pos = (tab_pos + update_pos) & (table_size - 1);
   while (tab_pos >= negative_index) tab_pos = (tab_pos + update_pos) & (table_size - 1);
  }
 }
 if (tab_pos != 0) {
  XCOMP_SAFE_FREE(*fse_table);
  WARNING_LOG("Tab pos didn't go back to 0: %u.\n", tab_pos);
  return -ZSTD_CORRUPTED_DATA;
 }
 unsigned int* symbol_counter = (unsigned int*) xcomp_calloc(probabilities_cnt, sizeof(unsigned int));
 if (symbol_counter == NULL) {
  XCOMP_SAFE_FREE(*fse_table);
  WARNING_LOG("Failed to allocate symbol counter buffer.\n");
  return -ZSTD_IO_ERROR;
 }
 for (unsigned short int i = 0; i < negative_index; ++i) {
  unsigned char symbol = (*fse_table)[i].symbol;
  short int prob = frequencies[symbol];
  unsigned int symbol_count = symbol_counter[symbol];
  calc_baseline_and_numbits(table_size, prob, symbol_count, &((*fse_table)[i].baseline), &((*fse_table)[i].nb_bits));
  if ((*fse_table)[i].nb_bits > table_log) {
   XCOMP_MULTI_FREE(*fse_table, symbol_counter);
   WARNING_LOG("An error occurred while building the fse table.\n");
   return -ZSTD_CORRUPTED_DATA;
  }
  symbol_counter[symbol]++;
 }
 XCOMP_SAFE_FREE(symbol_counter);
 return ZSTD_NO_ERROR;
}
static int read_weights(BitStream* compressed_bit_stream, unsigned char* table_log, unsigned char** weights, unsigned short int* weights_cnt) {
 int err = ZSTD_NO_ERROR;
 unsigned char header_byte = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, header_byte, 0);
 // Use FSE-Decoding	
 if (header_byte < 128) {
  unsigned char* compressed_literals_stream = SAFE_BYTE_READ(compressed_bit_stream, sizeof(unsigned char), header_byte, compressed_literals_stream);
  BitStream literals_compressed_bit_stream = CREATE_BIT_STREAM(compressed_literals_stream, header_byte);
  *table_log = bitstream_read_bits(&literals_compressed_bit_stream, 4) + 5;
  if (*table_log > 6) {
   WARNING_LOG("Table log exceeds the maximum value of 6: %u.\n", *table_log);
   return -ZSTD_CORRUPTED_DATA;
  }
  short int* frequencies = NULL;
  unsigned short int probabilities_cnt = 0;
  if ((err = read_probabilities(&literals_compressed_bit_stream, *table_log, 255, &frequencies, &probabilities_cnt)) < 0) {
   WARNING_LOG("An error occurred while reading the probabilities distribution.\n");
   return err;
  }
  FSETableEntry* fse_table = NULL;
  if ((err = fse_build_table(*table_log, frequencies, probabilities_cnt, &fse_table))) {
   XCOMP_SAFE_FREE(frequencies);
   WARNING_LOG("An error occurred while building the FSE Table.\n");
   return err;
  }
  XCOMP_SAFE_FREE(frequencies);
  // Decode the fse encoded weights
  BitStream weights_compressed_bit_stream = CREATE_REVERSED_BIT_STREAM(literals_compressed_bit_stream.stream + literals_compressed_bit_stream.byte_pos, header_byte - literals_compressed_bit_stream.byte_pos, -(*table_log));
  do { unsigned char bit_val = reversed_bitstream_read_next_bit(&weights_compressed_bit_stream); if (((&weights_compressed_bit_stream) -> bit_pos == 6) && ((&weights_compressed_bit_stream) -> byte_pos < (&weights_compressed_bit_stream) -> size - 1)) { WARNING_LOG("Padding zeros cannot be more than 7.\n"); err = -ZSTD_CORRUPTED_DATA; break; } else if (bit_val) break; } while(TRUE);
  if (err < 0) {
   XCOMP_SAFE_FREE(fse_table);
   return err;
  }
  unsigned short int even_state = reversed_bitstream_read_bits(&weights_compressed_bit_stream, *table_log);
  unsigned short int odd_state = reversed_bitstream_read_bits(&weights_compressed_bit_stream, *table_log);
  while (TRUE) {
   *weights = (unsigned char*) xcomp_realloc(*weights, sizeof(unsigned char) * (*weights_cnt + 2));
   if (*weights == NULL) {
    XCOMP_SAFE_FREE(fse_table);
    WARNING_LOG("Failed to xcomp_reallocate the weights.\n");
    return -ZSTD_IO_ERROR;
   }
   (*weights)[*weights_cnt] = fse_table[even_state].symbol;
   even_state = (fse_table)[even_state].baseline + reversed_bitstream_read_bits(&weights_compressed_bit_stream, (fse_table)[even_state].nb_bits);
   (*weights_cnt)++;
   if ((*weights)[*weights_cnt - 1] > 11) {
    XCOMP_MULTI_FREE(fse_table, *weights);
    WARNING_LOG("An error occurred while decoding the encoded weights.\n");
    return -ZSTD_CORRUPTED_DATA;
   }
   if (weights_compressed_bit_stream.bit_pos < 0) {
    (*weights)[(*weights_cnt)++] = fse_table[odd_state].symbol;
    if ((*weights)[*weights_cnt - 1] > 11) {
     XCOMP_MULTI_FREE(fse_table, *weights);
     WARNING_LOG("An error occurred while decoding the encoded weights.\n");
     return -ZSTD_CORRUPTED_DATA;
    }
    break;
   }
   (*weights)[(*weights_cnt)++] = fse_table[odd_state].symbol;
   odd_state = (fse_table)[odd_state].baseline + reversed_bitstream_read_bits(&weights_compressed_bit_stream, (fse_table)[odd_state].nb_bits);
   if ((*weights)[*weights_cnt - 1] > 11) {
    XCOMP_MULTI_FREE(fse_table, *weights);
    WARNING_LOG("An error occurred while decoding the encoded weights.\n");
    return -ZSTD_CORRUPTED_DATA;
   }
   if (weights_compressed_bit_stream.bit_pos < 0) {
    *weights = (unsigned char*) xcomp_realloc(*weights, sizeof(unsigned char) * (++(*weights_cnt)));
    if (*weights == NULL) {
     XCOMP_SAFE_FREE(fse_table);
     WARNING_LOG("Failed to xcomp_reallocate the weights.\n");
     return -ZSTD_IO_ERROR;
    }
    (*weights)[*weights_cnt - 1] = fse_table[even_state].symbol;
    if ((*weights)[*weights_cnt - 1] > 11) {
     XCOMP_MULTI_FREE(fse_table, *weights);
     WARNING_LOG("An error occurred while decoding the encoded weights.\n");
     return -ZSTD_CORRUPTED_DATA;
    }
    break;
   }
  }
  XCOMP_SAFE_FREE(fse_table);
  if (*weights_cnt > 255) {
   XCOMP_SAFE_FREE(*weights);
   return -ZSTD_TOO_MANY_LITERALS;
  }
 } else {
  *weights_cnt = header_byte - 127;
  *weights = (unsigned char*) xcomp_realloc(*weights, sizeof(unsigned char) * (*weights_cnt));
  if (*weights == NULL) {
   WARNING_LOG("Failed to xcomp_reallocate the weights.\n");
   return -ZSTD_IO_ERROR;
  }
  unsigned char temp_byte = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, temp_byte, 0, *weights);
  for (unsigned short int i = 0; i < *weights_cnt; ++i) {
   if ((i % 2) == 0) (*weights)[i] = (temp_byte >> 4) & 0x0F;
   else {
    (*weights)[i] = temp_byte & 0x0F;
    if ((i + 1) < *weights_cnt) {
     temp_byte = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, temp_byte, 0, *weights);
    }
   }
  }
 }
 return ZSTD_NO_ERROR;
}
static int build_huff_table(BitStream* compressed_bit_stream, Workspace* workspace) {
 int err = 0;
 unsigned short int weights_cnt = 0;
 unsigned char* weights = NULL;
 workspace -> hf_tree_desc_size = compressed_bit_stream -> byte_pos;
 if ((err = read_weights(compressed_bit_stream, &(workspace -> table_log), &weights, &weights_cnt)) < 0) {
  WARNING_LOG("An error occurred while reading the weights!\n");
  return err;
 }
 workspace -> hf_tree_desc_size = compressed_bit_stream -> byte_pos - workspace -> hf_tree_desc_size;
 unsigned int exp_weights_cnt = 0;
 for (unsigned short int i = 0; i < weights_cnt; ++i) {
  if (weights[i] > 0) exp_weights_cnt += 1U << (weights[i] - 1);
 }
 // Infer the last weight and reconstruct the table for decoding of Huffman *state-based* Coding
 // NOTE for Adventurers: Don't be fooled by those monkeys at Meta that specify "Huffman Tree" in their RFC 8878 (sponsored as official reference of ZSTD), and instead use a state-based approach
 // Furthermore, thanks to "zstd-rs" (at "https://github.com/KillingSpark/zstd-rs") for showing what they were actually doing inside their jungle mess of code.
 workspace -> max_nb_bits = highest_bit(exp_weights_cnt);
 weights = xcomp_realloc(weights, sizeof(unsigned char) * (++weights_cnt));
 if (weights == NULL) {
  WARNING_LOG("Failed to xcomp_reallocate the weights.\n");
  return -ZSTD_IO_ERROR;
 }
 weights[weights_cnt - 1] = highest_bit((1 << workspace -> max_nb_bits) - exp_weights_cnt); // Add the hidden literal and its weight
 unsigned short int hf_literals_size = 1 << workspace -> max_nb_bits;
 unsigned short int hf_literals_cnt = 0;
 workspace -> hf_literals = (ZSTDHfEntry*) xcomp_realloc(workspace -> hf_literals, hf_literals_size * sizeof(ZSTDHfEntry));
 if (workspace -> hf_literals == NULL) {
  XCOMP_SAFE_FREE(weights);
  WARNING_LOG("Failed to xcomp_reallocate the huff table for literals.\n");
  return -ZSTD_IO_ERROR;
 }
 mem_set(workspace -> hf_literals, 0, hf_literals_size * sizeof(ZSTDHfEntry));
 for (unsigned short int i = 0; i < weights_cnt; ++i) {
  if (weights[i] == 0) continue;
  unsigned short int j = 0;
  unsigned char nb_bits = workspace -> max_nb_bits + 1 - weights[i];
  while (j < hf_literals_size && ((workspace -> hf_literals)[j].nb_bits >= nb_bits && (workspace -> hf_literals)[j].symbol < i)) ++j;
  unsigned short int symbols_cnt = 1 << (weights[i] - 1);
  if (j < hf_literals_cnt) mem_move(workspace -> hf_literals + j + symbols_cnt, workspace -> hf_literals + j, (hf_literals_cnt - j) * sizeof(ZSTDHfEntry));
  for (unsigned short int s = j; s < j + symbols_cnt; ++s) (workspace -> hf_literals)[s].symbol = i, (workspace -> hf_literals)[s].nb_bits = nb_bits;
  hf_literals_cnt += symbols_cnt;
 }
 XCOMP_SAFE_FREE(weights);
 if (hf_literals_size != hf_literals_cnt) {
  XCOMP_SAFE_FREE(workspace -> hf_literals);
  WARNING_LOG("Mismatch in the hf literals size: %u != %u (expected size).\n", hf_literals_size, hf_literals_cnt);
  return -ZSTD_CORRUPTED_DATA;
 }
 return ZSTD_NO_ERROR;
}
static int huff_decode_stream(BitStream* literals_stream, unsigned short int hf_literals_size, unsigned int regenerated_size, Workspace* workspace) {
 int err = 0;
 do { unsigned char bit_val = reversed_bitstream_read_next_bit(literals_stream); if (((literals_stream) -> bit_pos == 6) && ((literals_stream) -> byte_pos < (literals_stream) -> size - 1)) { WARNING_LOG("Padding zeros cannot be more than 7.\n"); err = -ZSTD_CORRUPTED_DATA; break; } else if (bit_val) break; } while(TRUE);
 if (err < 0) return err;
 unsigned short int hf_state = reversed_bitstream_read_bits(literals_stream, workspace -> max_nb_bits);
 while ((literals_stream -> bit_pos > -(workspace -> max_nb_bits)) && (workspace -> literals_cnt < regenerated_size)) {
  (workspace -> literals)[(workspace -> literals_cnt)++] = (workspace -> hf_literals)[hf_state].symbol;
  hf_state = ((hf_state << (workspace -> hf_literals)[hf_state].nb_bits) & (hf_literals_size - 1)) | reversed_bitstream_read_bits(literals_stream, (workspace -> hf_literals)[hf_state].nb_bits);
 }
 if (literals_stream -> byte_pos > 0) {
  WARNING_LOG("Expected empty bitstream, but got still %u bytes inside.\n", literals_stream -> byte_pos);
  return -ZSTD_CORRUPTED_DATA;
 }
 return ZSTD_NO_ERROR;
}
static int decode_literals(BitStream* compressed_bit_stream, Workspace* workspace, LiteralsSectionHeader lsh) {
 // Decode the literals from the stream/streams
 int err = 0;
 if (lsh.literals_block_type == COMPRESSED_LITERALS_BLOCK) {
  if ((err = build_huff_table(compressed_bit_stream, workspace)) < 0) {
   WARNING_LOG("Failed to build the huffman table.\n");
   return -ZSTD_CORRUPTED_DATA;
  }
 } else {
  workspace -> hf_tree_desc_size = 0;
  if (workspace -> hf_literals == NULL) {
   WARNING_LOG("Uninitialized hf_literals for treeless compressed block, either a IO error, or the stream is corrupted.\n");
   return -ZSTD_CORRUPTED_DATA;
  }
 }
 unsigned short int hf_literals_size = 1 << workspace -> max_nb_bits;
 unsigned int total_streams_size = lsh.compressed_size - workspace -> hf_tree_desc_size;
 if (lsh.streams_cnt == 1) {
  unsigned char* literals_stream = SAFE_BYTE_READ(compressed_bit_stream, sizeof(unsigned char), total_streams_size, literals_stream, workspace -> hf_literals);
  BitStream literals_bit_stream = CREATE_REVERSED_BIT_STREAM(literals_stream, total_streams_size, -(workspace -> max_nb_bits));
  if (((err = huff_decode_stream(&literals_bit_stream, hf_literals_size, lsh.regenerated_size, workspace)) < 0) || (workspace -> literals_cnt != lsh.regenerated_size)) {
   XCOMP_SAFE_FREE(workspace -> hf_literals);
   WARNING_LOG("An error occurred while decoding the literals huff encoded stream.\n");
   return err;
  }
 } else {
  unsigned short int streams_size[4] = {0};
  unsigned char* streams_size_data = SAFE_BYTE_READ(compressed_bit_stream, sizeof(unsigned short int), 3, streams_size_data, workspace -> hf_literals);
  mem_cpy(streams_size, streams_size_data, sizeof(unsigned short int) * 3);
  streams_size[3] = (total_streams_size - 6 - streams_size[0] - streams_size[1] - streams_size[2]);
  for (unsigned char i = 0; i < 4; ++i) {
   unsigned char* literals_sub_stream = SAFE_BYTE_READ(compressed_bit_stream, sizeof(unsigned char), streams_size[i], literals_sub_stream, workspace -> hf_literals);
   BitStream literals_sub_bit_stream = CREATE_REVERSED_BIT_STREAM(literals_sub_stream, streams_size[i], -(workspace -> max_nb_bits));
   if (compressed_bit_stream -> error) {
    XCOMP_SAFE_FREE(workspace -> hf_literals);
    WARNING_LOG("Not enough data for the streams.\n");
    return -ZSTD_CORRUPTED_DATA;
   } else if (((err = huff_decode_stream(&literals_sub_bit_stream, hf_literals_size, lsh.regenerated_size, workspace)) < 0) || (literals_sub_bit_stream.bit_pos != -(workspace -> max_nb_bits))) {
    XCOMP_SAFE_FREE(workspace -> hf_literals);
    WARNING_LOG("An error occurred while decoding the literals huff encoded in the substream '%u'.\n", i + 1);
    return -ZSTD_CORRUPTED_DATA;
   }
  }
  if (workspace -> literals_cnt != lsh.regenerated_size) {
   XCOMP_SAFE_FREE(workspace -> hf_literals);
   WARNING_LOG("Size mismatch between literals_cnt and the expected regenerated size: %u != %u .\n", workspace -> literals_cnt, lsh.regenerated_size);
   return -ZSTD_CORRUPTED_DATA;
  }
 }
 return err;
}
static int parse_literals_section(BitStream* compressed_bit_stream, Workspace* workspace) {
 LiteralsSectionHeader lsh = {0};
 lsh.literals_block_type = bitstream_read_bits(compressed_bit_stream, 2);
 DEBUG_LOG("literals_block_type: '%s'\n", literals_blocks_type_str[lsh.literals_block_type]);
 unsigned char size_format = bitstream_read_bits(compressed_bit_stream, 2);
 int err = 0;
 if (lsh.literals_block_type == RAW_LITERALS_BLOCK || lsh.literals_block_type == RLE_LITERALS_BLOCK) {
  if (size_format == 0 || size_format == 2) lsh.regenerated_size = (bitstream_read_bits(compressed_bit_stream, 4) << 1) + (size_format >> 1);
  else if (size_format == 1) lsh.regenerated_size = bitstream_read_bits(compressed_bit_stream, 12);
  else lsh.regenerated_size = bitstream_read_bits(compressed_bit_stream, 20);
  DEBUG_LOG("regenerated_size: %u\n", lsh.regenerated_size);
  workspace -> literals_cnt = 0;
  XCOMP_SAFE_FREE(workspace -> literals);
  workspace -> literals = (unsigned char*) xcomp_calloc(lsh.regenerated_size, sizeof(unsigned char));
  if (workspace -> literals == NULL) {
   WARNING_LOG("Failed to allocate literals buffer.\n");
   return -ZSTD_IO_ERROR;
  }
  if (lsh.literals_block_type == RAW_LITERALS_BLOCK) {
   unsigned char* raw_literals_block_data = SAFE_BYTE_READ(compressed_bit_stream, sizeof(unsigned char), lsh.regenerated_size, raw_literals_block_data);
   mem_cpy(workspace -> literals, raw_literals_block_data, lsh.regenerated_size);
  } else {
   unsigned char rle_literal_val = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, rle_literal_val, 0);
   mem_set(workspace -> literals, rle_literal_val, lsh.regenerated_size);
  }
 } else {
  lsh.regenerated_size = bitstream_read_bits(compressed_bit_stream, ((size_format == 0 || size_format == 1) ? 10 : (size_format == 2 ? 14 : 18)));
  lsh.compressed_size = bitstream_read_bits(compressed_bit_stream, ((size_format == 0 || size_format == 1) ? 10 : (size_format == 2 ? 14 : 18)));
  lsh.streams_cnt = size_format == 0 ? 1 : 4;
  XCOMP_SAFE_FREE(workspace -> literals);
  workspace -> literals_cnt = 0;
  workspace -> literals = (unsigned char*) xcomp_calloc(lsh.regenerated_size, sizeof(unsigned char));
  if (workspace -> literals == NULL) {
   WARNING_LOG("Failed to allocate literals buffer.\n");
   return -ZSTD_IO_ERROR;
  } else if ((err = decode_literals(compressed_bit_stream, workspace, lsh)) < 0) {
   WARNING_LOG("An error occurred while decoding the literals.\n");
   return err;
  }
 }
 workspace -> literals_cnt = lsh.regenerated_size;
 return ZSTD_NO_ERROR;
}
// ---------------------------------------
//  Sequence Parsing and Decoding Section
// ---------------------------------------
static int decode_sequences(BitStream* sequence_compressed_bit_stream, Workspace* workspace) {
 unsigned int ll_state = 0;
 unsigned int ol_state = 0;
 unsigned int ml_state = 0;
 if (workspace -> sequence_section.ll_rle == -1) ll_state = reversed_bitstream_read_bits(sequence_compressed_bit_stream, workspace -> sequence_section.ll_table_log);
 if (workspace -> sequence_section.ol_rle == -1) ol_state = reversed_bitstream_read_bits(sequence_compressed_bit_stream, workspace -> sequence_section.ol_table_log);
 if (workspace -> sequence_section.ml_rle == -1) ml_state = reversed_bitstream_read_bits(sequence_compressed_bit_stream, workspace -> sequence_section.ml_table_log);
 for (unsigned int i = 0; i < workspace -> sequence_len; ++i) {
  unsigned int ll_code = workspace -> sequence_section.ll_rle == -1 ? (workspace -> sequence_section.ll_fse_table)[ll_state].symbol : workspace -> sequence_section.ll_rle;
  unsigned int ml_code = workspace -> sequence_section.ml_rle == -1 ? (workspace -> sequence_section.ml_fse_table)[ml_state].symbol : workspace -> sequence_section.ml_rle;
  unsigned int ol_code = workspace -> sequence_section.ol_rle == -1 ? (workspace -> sequence_section.ol_fse_table)[ol_state].symbol : workspace -> sequence_section.ol_rle;
  LengthCode ll_actual_codes = *XCOMP_CAST_PTR(ll_codes[ll_code], LengthCode);
  LengthCode ml_actual_codes = *XCOMP_CAST_PTR(ml_codes[ml_code], LengthCode);
  if (ol_code > 31) {
   WARNING_LOG("Offset Length code cannot be bigger than %u: %u\n", 31, ol_code);
   return -ZSTD_CORRUPTED_DATA;
  }
        unsigned int offset = reversed_bitstream_read_bits(sequence_compressed_bit_stream, ol_code) + (1U << ol_code);
  unsigned int ml_add = reversed_bitstream_read_bits(sequence_compressed_bit_stream, ml_actual_codes.num_bits);
  unsigned int ll_add = reversed_bitstream_read_bits(sequence_compressed_bit_stream, ll_actual_codes.num_bits);
  if (offset == 0) {
   WARNING_LOG("Offset cannot be 0.\n");
   return -ZSTD_CORRUPTED_DATA;
  }
  (workspace -> sequences)[i] = (Sequence) {
   .ll_value = ll_actual_codes.value + ll_add,
   .ml_value = ml_actual_codes.value + ml_add,
   .ol_value = offset
  };
  if ((i + 1) < workspace -> sequence_len) {
   if (workspace -> sequence_section.ll_rle == -1) ll_state = (workspace -> sequence_section.ll_fse_table)[ll_state].baseline + reversed_bitstream_read_bits(&*sequence_compressed_bit_stream, (workspace -> sequence_section.ll_fse_table)[ll_state].nb_bits);
   if (workspace -> sequence_section.ml_rle == -1) ml_state = (workspace -> sequence_section.ml_fse_table)[ml_state].baseline + reversed_bitstream_read_bits(&*sequence_compressed_bit_stream, (workspace -> sequence_section.ml_fse_table)[ml_state].nb_bits);
   if (workspace -> sequence_section.ol_rle == -1) ol_state = (workspace -> sequence_section.ol_fse_table)[ol_state].baseline + reversed_bitstream_read_bits(&*sequence_compressed_bit_stream, (workspace -> sequence_section.ol_fse_table)[ol_state].nb_bits);
  }
  if (sequence_compressed_bit_stream -> error) {
   XCOMP_SAFE_FREE(workspace -> sequences);
   WARNING_LOG("Tried to read after the end of the stream.\n");
   return -ZSTD_CORRUPTED_DATA;
  }
 }
 if (!IS_REVERSED_EOS(sequence_compressed_bit_stream)) {
  XCOMP_SAFE_FREE(workspace -> sequences);
  WARNING_LOG("Stream not empty.\n");
  PRINT_BIT_STREAM_INFO(sequence_compressed_bit_stream);
  return -ZSTD_CORRUPTED_DATA;
 }
 return ZSTD_NO_ERROR;
}
static int parse_sequence_section(BitStream* compressed_bit_stream, Workspace* workspace) {
 workspace -> sequence_len = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, workspace -> sequence_len, 0);
 if (workspace -> sequence_len > 127) {
  unsigned char first_byte = workspace -> sequence_len;
  if (first_byte < 255) {
   workspace -> sequence_len = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, workspace -> sequence_len, 0);
  } else {
   workspace -> sequence_len = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 2, unsigned short int, workspace -> sequence_len, 0);
  }
  workspace -> sequence_len += first_byte < 255 ? ((first_byte - 128) << 8) : 0x7F00;
 }
 DEBUG_LOG("sequence_len: %u\n", workspace -> sequence_len);
 if (workspace -> sequence_len == 0) {
  if (!IS_EOS(compressed_bit_stream)) {
   WARNING_LOG("Expected end of stream, but the bitstream is not empty: %u bytes left.\n", compressed_bit_stream -> size - compressed_bit_stream -> byte_pos);
   return -ZSTD_CORRUPTED_DATA;
  }
  return ZSTD_NO_ERROR;
 }
 SymbolCompressionModes symbol_compression_modes = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(SymbolCompressionModes), 1, SymbolCompressionModes, symbol_compression_modes, {0});
 if (symbol_compression_modes.reserved != 0) {
  WARNING_LOG("Use of symbol compression mode reserved field.\n");
  return -ZSTD_RESERVED_FIELD;
 }
 DEBUG_LOG("SymbolCompressionModes: (0x%X)\n", *XCOMP_CAST_PTR(&symbol_compression_modes, unsigned char));
 DEBUG_LOG(" - literals_length_mode: '%s'\n", compression_modes_str[symbol_compression_modes.literals_len_mode]);
 DEBUG_LOG(" - offset_mode: '%s'\n", compression_modes_str[symbol_compression_modes.offset_mode]);
 DEBUG_LOG(" - match_length_mode: '%s'\n", compression_modes_str[symbol_compression_modes.match_len_mode]);
 // Build the FSE table for each type from predefined distribution or build it from decoded distribution, for RLE recover the single value that compose the entire table
 // For Repeat nothing has to be done, at the moment we are not able to take track of previously used tables
 // Furthermore, for Repeat just check that it can be used, like if this is not the first block and similars
 int err = 0;
 SequenceSection* sequence_section = &(workspace -> sequence_section);
 do { switch (symbol_compression_modes.literals_len_mode) { case PREDEFINED_MODE: { if (sequence_section -> ll_fse_table != NULL) XCOMP_SAFE_FREE(sequence_section -> ll_fse_table); if ((err = fse_build_table(PRED_LL_TABLE_LOG, (short int*)ll_pred_frequencies, XCOMP_ARR_SIZE(ll_pred_frequencies), &(sequence_section -> ll_fse_table))) < 0) { WARNING_LOG("An error occurred while building the table for predefined.\n"); return err; } sequence_section -> ll_table_log = PRED_LL_TABLE_LOG; sequence_section -> ll_rle = -1; break; } case FSE_COMPRESSED_MODE: { if (sequence_section -> ll_fse_table != NULL) XCOMP_SAFE_FREE(sequence_section -> ll_fse_table); sequence_section -> ll_table_log = bitstream_read_bits(compressed_bit_stream, 4) + 5; if (sequence_section -> ll_table_log > LL_MAX_LOG) { WARNING_LOG("Table log exceeds the maximum value of %u: %u.\n", LL_MAX_LOG, sequence_section -> ll_table_log); return -ZSTD_CORRUPTED_DATA; } short int* frequencies = NULL; unsigned short int probabilities_cnt = 0; if ((err = read_probabilities(compressed_bit_stream, sequence_section -> ll_table_log, 35, &frequencies, &probabilities_cnt)) < 0) { WARNING_LOG("An error occurred while reading the probabilities for predefined.\n"); return err; } if ((err = fse_build_table(sequence_section -> ll_table_log, frequencies, probabilities_cnt, &(sequence_section -> ll_fse_table))) < 0) { XCOMP_SAFE_FREE(frequencies); WARNING_LOG("An error occurred while building the table for predefined.\n"); return err; } XCOMP_SAFE_FREE(frequencies); sequence_section -> ll_rle = -1; break; } case RLE_MODE: { sequence_section -> ll_rle = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, sequence_section -> ll_rle, 0); break; } case REPEAT_MODE: { if (sequence_section -> ll_fse_table == NULL) { WARNING_LOG("Uninitialized " "sequence_section -> ll_fse_table" " either due to a previous IO error, or the stream is actually corrupted.\n"); return -ZSTD_CORRUPTED_DATA; } break; } } } while(FALSE);
 do { switch (symbol_compression_modes.offset_mode) { case PREDEFINED_MODE: { if (sequence_section -> ol_fse_table != NULL) XCOMP_SAFE_FREE(sequence_section -> ol_fse_table); if ((err = fse_build_table(PRED_OL_TABLE_LOG, (short int*)ol_pred_frequencies, XCOMP_ARR_SIZE(ol_pred_frequencies), &(sequence_section -> ol_fse_table))) < 0) { WARNING_LOG("An error occurred while building the table for predefined.\n"); return err; } sequence_section -> ol_table_log = PRED_OL_TABLE_LOG; sequence_section -> ol_rle = -1; break; } case FSE_COMPRESSED_MODE: { if (sequence_section -> ol_fse_table != NULL) XCOMP_SAFE_FREE(sequence_section -> ol_fse_table); sequence_section -> ol_table_log = bitstream_read_bits(compressed_bit_stream, 4) + 5; if (sequence_section -> ol_table_log > OL_MAX_LOG) { WARNING_LOG("Table log exceeds the maximum value of %u: %u.\n", OL_MAX_LOG, sequence_section -> ol_table_log); return -ZSTD_CORRUPTED_DATA; } short int* frequencies = NULL; unsigned short int probabilities_cnt = 0; if ((err = read_probabilities(compressed_bit_stream, sequence_section -> ol_table_log, 31, &frequencies, &probabilities_cnt)) < 0) { WARNING_LOG("An error occurred while reading the probabilities for predefined.\n"); return err; } if ((err = fse_build_table(sequence_section -> ol_table_log, frequencies, probabilities_cnt, &(sequence_section -> ol_fse_table))) < 0) { XCOMP_SAFE_FREE(frequencies); WARNING_LOG("An error occurred while building the table for predefined.\n"); return err; } XCOMP_SAFE_FREE(frequencies); sequence_section -> ol_rle = -1; break; } case RLE_MODE: { sequence_section -> ol_rle = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, sequence_section -> ol_rle, 0); break; } case REPEAT_MODE: { if (sequence_section -> ol_fse_table == NULL) { WARNING_LOG("Uninitialized " "sequence_section -> ol_fse_table" " either due to a previous IO error, or the stream is actually corrupted.\n"); return -ZSTD_CORRUPTED_DATA; } break; } } } while(FALSE);
 do { switch (symbol_compression_modes.match_len_mode) { case PREDEFINED_MODE: { if (sequence_section -> ml_fse_table != NULL) XCOMP_SAFE_FREE(sequence_section -> ml_fse_table); if ((err = fse_build_table(PRED_ML_TABLE_LOG, (short int*)ml_pred_frequencies, XCOMP_ARR_SIZE(ml_pred_frequencies), &(sequence_section -> ml_fse_table))) < 0) { WARNING_LOG("An error occurred while building the table for predefined.\n"); return err; } sequence_section -> ml_table_log = PRED_ML_TABLE_LOG; sequence_section -> ml_rle = -1; break; } case FSE_COMPRESSED_MODE: { if (sequence_section -> ml_fse_table != NULL) XCOMP_SAFE_FREE(sequence_section -> ml_fse_table); sequence_section -> ml_table_log = bitstream_read_bits(compressed_bit_stream, 4) + 5; if (sequence_section -> ml_table_log > ML_MAX_LOG) { WARNING_LOG("Table log exceeds the maximum value of %u: %u.\n", ML_MAX_LOG, sequence_section -> ml_table_log); return -ZSTD_CORRUPTED_DATA; } short int* frequencies = NULL; unsigned short int probabilities_cnt = 0; if ((err = read_probabilities(compressed_bit_stream, sequence_section -> ml_table_log, 52, &frequencies, &probabilities_cnt)) < 0) { WARNING_LOG("An error occurred while reading the probabilities for predefined.\n"); return err; } if ((err = fse_build_table(sequence_section -> ml_table_log, frequencies, probabilities_cnt, &(sequence_section -> ml_fse_table))) < 0) { XCOMP_SAFE_FREE(frequencies); WARNING_LOG("An error occurred while building the table for predefined.\n"); return err; } XCOMP_SAFE_FREE(frequencies); sequence_section -> ml_rle = -1; break; } case RLE_MODE: { sequence_section -> ml_rle = SAFE_BYTE_READ_WITH_CAST(compressed_bit_stream, sizeof(unsigned char), 1, unsigned char, sequence_section -> ml_rle, 0); break; } case REPEAT_MODE: { if (sequence_section -> ml_fse_table == NULL) { WARNING_LOG("Uninitialized " "sequence_section -> ml_fse_table" " either due to a previous IO error, or the stream is actually corrupted.\n"); return -ZSTD_CORRUPTED_DATA; } break; } } } while(FALSE);
 unsigned int sequence_stream_size = compressed_bit_stream -> size - compressed_bit_stream -> byte_pos;
 unsigned char* sequence_compressed_stream = SAFE_BYTE_READ(compressed_bit_stream, sizeof(unsigned char), sequence_stream_size, sequence_compressed_stream);
 BitStream sequence_compressed_bit_stream = CREATE_REVERSED_BIT_STREAM(sequence_compressed_stream, sequence_stream_size, 0);
 do { unsigned char bit_val = reversed_bitstream_read_next_bit(&sequence_compressed_bit_stream); if (((&sequence_compressed_bit_stream) -> bit_pos == 6) && ((&sequence_compressed_bit_stream) -> byte_pos < (&sequence_compressed_bit_stream) -> size - 1)) { WARNING_LOG("Padding zeros cannot be more than 7.\n"); err = -ZSTD_CORRUPTED_DATA; break; } else if (bit_val) break; } while(TRUE);
 if (err < 0) {
  WARNING_LOG("An error occurred while parsing the sequence section.\n");
  return err;
 }
 // Parse the FSE Table as defined in decode_sequences_with_rle in zstd-rs, the price of few branches is feasible as at the moment we value more readability.
 // At each pass it will decode a Sequence, so we need a struct for the Sequences
 XCOMP_SAFE_FREE(workspace -> sequences);
 workspace -> sequences = (Sequence*) xcomp_calloc(workspace -> sequence_len, sizeof(Sequence));
 if (workspace -> sequences == NULL) {
  WARNING_LOG("Failed to allocate sequences buffer.\n");
  return -ZSTD_IO_ERROR;
 } else if ((err = decode_sequences(&sequence_compressed_bit_stream, workspace)) < 0) {
  deallocate_sequence_section(sequence_section);
  WARNING_LOG("An error occurred while decoding the sequences.\n");
  return err;
 }
 return ZSTD_NO_ERROR;
}
// ----------------------------
//  Sequence Execution Section
// ----------------------------
static unsigned int update_off_history(unsigned int* offset_history, unsigned int offset, unsigned int ll_value) {
 unsigned int actual_offset = 0;
 if (offset > 3) actual_offset = offset - 3;
 else {
  if (ll_value > 0) actual_offset = offset_history[offset - 1];
  else actual_offset = offset == 3 ? offset_history[0] - 1 : offset_history[offset];
 }
 if (offset >= 3 || (offset == 2 && ll_value == 0)) {
  offset_history[2] = offset_history[1];
     offset_history[1] = offset_history[0];
  offset_history[0] = actual_offset;
 } else if ((offset == 1 && ll_value == 0) || (offset == 2 && ll_value > 0)) {
  offset_history[1] = offset_history[0];
        offset_history[0] = actual_offset;
 }
 return actual_offset;
}
static int sequence_execution(Workspace* workspace) {
 if (workspace -> sequence_len == 0) {
  mem_cpy(workspace -> frame_buffer + workspace -> frame_buffer_len, workspace -> literals, workspace -> literals_cnt);
  workspace -> frame_buffer_len += workspace -> literals_cnt;
  return ZSTD_NO_ERROR;
 }
 unsigned int literals_ind = 0;
 unsigned int sequence_cnt = 0;
 unsigned int original_frame_len = workspace -> frame_buffer_len;
 unsigned int* offset_history = workspace -> offset_history;
 if (offset_history == NULL) {
  WARNING_LOG("Uninitialized offset history.\n");
  return -ZSTD_IO_ERROR;
 }
 for (unsigned int i = 0; i < workspace -> sequence_len; ++i) {
  Sequence sequence = workspace -> sequences[i];
  if (sequence.ll_value > 0) {
   if (sequence.ll_value + literals_ind > workspace -> literals_cnt) {
    WARNING_LOG("Literals length value makes index out of range (%u > %u).\n", (sequence.ll_value + literals_ind), workspace -> literals_cnt);
    return -ZSTD_CORRUPTED_DATA;
   }
   mem_cpy(workspace -> frame_buffer + workspace -> frame_buffer_len, workspace -> literals + literals_ind, sequence.ll_value);
   literals_ind += sequence.ll_value;
   workspace -> frame_buffer_len += sequence.ll_value;
   sequence_cnt += sequence.ll_value;
  }
  unsigned int offset = update_off_history(offset_history, sequence.ol_value, sequence.ll_value);
  if (offset == 0) {
   WARNING_LOG("Actual offset cannot be zero.\n");
   return -ZSTD_CORRUPTED_DATA;
  }
  if (sequence.ml_value > 0) {
   unsigned int current_pos = workspace -> frame_buffer_len;
   if (((long int) current_pos - offset) < 0LL) {
    WARNING_LOG("Offset makes negative index into literals: %ld.\n", ((long int) current_pos - offset));
    return -ZSTD_CORRUPTED_DATA;
   }
   for (unsigned int i = 0; i < sequence.ml_value; ++i) (workspace -> frame_buffer)[(workspace -> frame_buffer_len)++] = (workspace -> frame_buffer)[current_pos + i - offset];
   sequence_cnt += sequence.ml_value;
  }
 }
 if (literals_ind < workspace -> literals_cnt) {
  mem_cpy(workspace -> frame_buffer + workspace -> frame_buffer_len, workspace -> literals + literals_ind, workspace -> literals_cnt - literals_ind);
  workspace -> frame_buffer_len += workspace -> literals_cnt - literals_ind;
  sequence_cnt += workspace -> literals_cnt - literals_ind;
 }
 if (sequence_cnt != (workspace -> frame_buffer_len - original_frame_len)) {
  WARNING_LOG("Size mismatch between sequence_cnt: %u and the decoded len: %u\n", sequence_cnt, (workspace -> frame_buffer_len - original_frame_len));
  return -ZSTD_CORRUPTED_DATA;
 }
 return ZSTD_NO_ERROR;
}
// ----------------------------------------------
//  Block and Frame Parsing and Decoding Section
// ----------------------------------------------
static int decompress_block(BitStream* compressed_bit_stream, Workspace* workspace) {
 int err = 0;
 if ((err = parse_literals_section(compressed_bit_stream, workspace)) < 0) {
  WARNING_LOG("An error occurred while parsing the literals section.\n");
  return err;
 }
 // Use the Literals to decode the sequence section
 if ((err = parse_sequence_section(compressed_bit_stream, workspace))) {
  WARNING_LOG("An error occurred while parsing the sequence section.\n");
  return err;
 }
 if ((err = sequence_execution(workspace)) < 0) {
  WARNING_LOG("An error occurred while performing sequence execution.\n");
  return err;
 }
 return ZSTD_NO_ERROR;
}
static int parse_block(BitStream* bit_stream, Workspace* workspace, unsigned int block_maximum_size) {
 BlockHeader block_header = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(BlockHeader), 1, BlockHeader, block_header, {0});
 DEBUG_LOG("BlockHeader: (0x%X)\n", *XCOMP_CAST_PTR(&block_header, unsigned int));
 DEBUG_LOG(" - last_block: %u\n", block_header.last_block);
 DEBUG_LOG(" - block_type: '%s'\n", block_types_str[block_header.block_type]);
 DEBUG_LOG(" - block_size: %u\n", block_header.block_size);
 if (block_header.block_type == RESERVED_TYPE) return -ZSTD_RESERVED;
 if (block_header.block_type == RAW_BLOCK) {
  if (workspace -> frame_buffer_len + block_header.block_size) {
   workspace -> frame_buffer = (unsigned char*) xcomp_realloc(workspace -> frame_buffer, (workspace -> frame_buffer_len + block_header.block_size) * sizeof(unsigned char));
   if (workspace -> frame_buffer == NULL) {
    WARNING_LOG("Failed to xcomp_reallocate frame buffer.\n");
    return -ZSTD_IO_ERROR;
   }
   unsigned char* raw_block_data = SAFE_BYTE_READ(bit_stream, sizeof(unsigned char), block_header.block_size, raw_block_data);
   mem_cpy(workspace -> frame_buffer + workspace -> frame_buffer_len, raw_block_data, block_header.block_size * sizeof(unsigned char));
   workspace -> frame_buffer_len += block_header.block_size;
  }
 } else if (block_header.block_type == RLE_BLOCK) {
  workspace -> frame_buffer = (unsigned char*) xcomp_realloc(workspace -> frame_buffer, (workspace -> frame_buffer_len + block_header.block_size) * sizeof(unsigned char));
  if (workspace -> frame_buffer == NULL) {
   WARNING_LOG("Failed to xcomp_reallocate frame buffer.\n");
   return -ZSTD_IO_ERROR;
  }
  unsigned char rle_val = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(unsigned char), 1, unsigned char, rle_val, 0);
  mem_set(workspace -> frame_buffer + workspace -> frame_buffer_len, rle_val, block_header.block_size * sizeof(unsigned char));
  workspace -> frame_buffer_len += block_header.block_size;
 } else {
  workspace -> frame_buffer = (unsigned char*) xcomp_realloc(workspace -> frame_buffer, (workspace -> frame_buffer_len + block_maximum_size) * sizeof(unsigned char));
  if (workspace -> frame_buffer == NULL) {
   WARNING_LOG("Failed to xcomp_reallocate frame buffer.\n");
   return -ZSTD_IO_ERROR;
  }
  unsigned char* compressed_stream = SAFE_BYTE_READ(bit_stream, sizeof(unsigned char), block_header.block_size, compressed_stream);
  BitStream compressed_bit_stream = CREATE_BIT_STREAM(compressed_stream, block_header.block_size);
  int err = 0;
  if ((err = decompress_block(&compressed_bit_stream, workspace)) < 0) {
   XCOMP_SAFE_FREE(workspace -> frame_buffer);
   WARNING_LOG("An error occurred while decompressing the block.\n");
   return err;
  }
  workspace -> frame_buffer = (unsigned char*) xcomp_realloc(workspace -> frame_buffer, workspace -> frame_buffer_len * sizeof(unsigned char));
  if (workspace -> frame_buffer == NULL && workspace -> frame_buffer_len != 0) {
   WARNING_LOG("Failed to xcomp_reallocate frame buffer.\n");
   return -ZSTD_IO_ERROR;
  }
 }
 return block_header.last_block; // Return the information to the frame parser
}
static int parse_frames(BitStream* bit_stream, unsigned char** decompressed_data, unsigned int* decompressed_data_length) {
 unsigned int magic = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(unsigned int), 1, unsigned int, magic, 0);
 DEBUG_LOG("magic: 0x%X\n", magic);
 if (0x184D2A50 <= magic && magic <= 0x184D2A5F) {
  unsigned int frame_len = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(unsigned int), 1, unsigned int, frame_len, 0);
  DEBUG_LOG("Skipping 'skippable frame' with length %u found!\n", frame_len);
  unsigned char* skipped_data = SAFE_BYTE_READ(bit_stream, sizeof(unsigned char), frame_len, skipped_data);
  UNUSED_VAR(skipped_data);
  return ZSTD_NO_ERROR;
 }
 if (magic != 0xFD2FB528) {
  WARNING_LOG("Invalid magic: 0x%X != 0x%X (the expected magic)\n", magic, 0xFD2FB528);
  return -ZSTD_INVALID_MAGIC;
 }
 // Frame Header
 FrameHeaderDescriptor fhd = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(FrameHeaderDescriptor), 1, FrameHeaderDescriptor, fhd, {0});
 print_fhd(fhd);
 if (fhd.reserved != 0) {
  WARNING_LOG("Used frame header descriptor reserved field.\n");
  return -ZSTD_RESERVED_FIELD;
 }
 unsigned long long int window_size = 0;
 if (!fhd.single_segment_flag) {
  WindowDescriptor window_descriptor = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(WindowDescriptor), 1, WindowDescriptor, window_descriptor, {0});
  unsigned char window_log = 10 + window_descriptor.exponent;
  unsigned long long int window_base = 1 << window_log;
  unsigned long long int window_add = (window_base / 8) * window_descriptor.mantissa;
  window_size = window_base + window_add;
 }
 if (fhd.dictionary_id_flag != 0) {
  WARNING_LOG("Sorry, but this decoder implementation does not support the use of dictionaries.\n");
  return -ZSTD_UNSUPPORTED_FEATURE;
 }
 unsigned char frame_content_size_len = (fhd.frame_content_size_flag == 0 ? fhd.single_segment_flag : 1 << fhd.frame_content_size_flag);
 DEBUG_LOG("frame_content_size_len: %u\n", frame_content_size_len);
 unsigned long long int frame_content_size = 0;
 if (frame_content_size_len) {
  unsigned char* frame_content_size_data = SAFE_BYTE_READ(bit_stream, frame_content_size_len, 1, frame_content_size_data);
  mem_cpy(&frame_content_size, frame_content_size_data, frame_content_size_len);
  if (frame_content_size_len == 2) frame_content_size += 256;
 }
 if (fhd.single_segment_flag) window_size = frame_content_size;
 DEBUG_LOG("Frame Header:\n");
 DEBUG_LOG(" - window_size: %llu\n", window_size);
 DEBUG_LOG(" - frame_content_size: %llu\n", frame_content_size);
 // Define the workspace for this frame
 unsigned int offset_history[] = {1, 4, 8};
 Workspace workspace = {0};
 workspace.offset_history = offset_history;
 int err = 0;
 unsigned int blocks_cnt = 0;
 do {
  DEBUG_LOG("Parsing data block num %u\n", blocks_cnt);
  // Previous decoded data, up to a distance of Window_Size, or the beginning of the Frame, whichever is smaller. Single_Segment_Flag will be set in the latter case.
  if ((err = parse_block(bit_stream, &workspace, MAX(window_size, (128 * 1024)))) < 0) {
   deallocate_workspace(&workspace);
   WARNING_LOG("An error occurred while decoding a block.\n");
   return err;
  }
  blocks_cnt++;
 } while (err != 1);
 if (fhd.content_checksum_flag) {
  unsigned int frame_checksum = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(unsigned int), 1, unsigned int, frame_checksum, 0);
  DEBUG_LOG("frame checksum: 0x%X\n", frame_checksum);
  u64 decoded_checksum = xxhash64(workspace.frame_buffer, workspace.frame_buffer_len, 0) & 0xFFFFFFFF;
  if (decoded_checksum != frame_checksum) {
   DEBUG_LOG("data(%u): '%.*s'\n", workspace.frame_buffer_len, workspace.frame_buffer_len, workspace.frame_buffer);
   deallocate_workspace(&workspace);
   WARNING_LOG("The checksum of the frame doesn't match with the one found at the end of the frame (0x%llX != 0x%X).\n", decoded_checksum, frame_checksum);
   return -ZSTD_CHECKSUM_FAIL;
  }
 }
 if (workspace.frame_buffer_len) {
  *decompressed_data = (unsigned char*) xcomp_realloc(*decompressed_data, (*decompressed_data_length + workspace.frame_buffer_len) * sizeof(unsigned char));
  if (*decompressed_data == NULL) {
   deallocate_workspace(&workspace);
   WARNING_LOG("Failed to xcomp_reallocate the decompressed data buffer.\n");
   return -ZSTD_IO_ERROR;
  }
  mem_cpy(*decompressed_data + *decompressed_data_length, workspace.frame_buffer, workspace.frame_buffer_len);
  *decompressed_data_length += workspace.frame_buffer_len;
 }
 deallocate_workspace(&workspace);
 return ZSTD_NO_ERROR;
}
/* ---------------------------------------------------------------------------------------------------------- */
unsigned char* zstd_inflate(unsigned char* stream, unsigned int size, unsigned int* decompressed_data_length, int* zstd_err) {
 unsigned char* decompressed_data = (unsigned char*) xcomp_calloc(1, sizeof(unsigned char));
 if (decompressed_data == NULL) {
  XCOMP_SAFE_FREE(stream);
  WARNING_LOG("Failed to allocate decompressed data buffer.\n");
  *zstd_err = -ZSTD_IO_ERROR;
  return ((unsigned char*) "An error occurred while initializing the buffer for the decompressed data.\n");
 }
 unsigned int frames_cnt = 0;
 BitStream bit_stream = CREATE_BIT_STREAM(stream, size);
 unsigned long int expected_decompression_size = (*decompressed_data_length > 0) ? *decompressed_data_length : 0x1FFFFFFFF;
 *decompressed_data_length = 0;
 do {
  DEBUG_LOG("Parsing frame num %u:\n", frames_cnt);
  if ((*zstd_err = parse_frames(&bit_stream, &decompressed_data, decompressed_data_length))) {
   XCOMP_MULTI_FREE(stream, decompressed_data);
   return ((unsigned char*) "An error occurred while parsing the frame.\n");
  }
  frames_cnt++;
 } while (!IS_EOS(&bit_stream) && !bit_stream.error && *decompressed_data_length < expected_decompression_size);
 if (expected_decompression_size != 0x1FFFFFFFF && *decompressed_data_length != expected_decompression_size) {
  *zstd_err = -ZSTD_DECOMPRESSED_SIZE_MISMATCH;
  return ((unsigned char*) "Decompressed size doesn't match the expected decompressed size.\n");
 }
 *zstd_err = 0;
 XCOMP_SAFE_FREE(stream);
 return decompressed_data;
}
/* Copyright (C) 1991-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
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
// -----------------
//  Constant Values 
// -----------------
/* -------------------------------------------------------------------------------------------------------- */
// -------
//  Enums
// -------
typedef enum PACKED_STRUCT ZlibError {
    ZLIB_NO_ERROR,
    ZLIB_IO_ERROR,
    ZLIB_CORRUPTED_DATA,
    ZLIB_INVALID_LEN_CHECKSUM,
    ZLIB_INVALID_COMPRESSION_TYPE,
    ZLIB_INVALID_DECODED_VALUE,
    ZLIB_INVALID_LENGTH,
    ZLIB_TODO
} ZlibError;
static const char* zlib_errors_str[] = {
    "ZLIB_NO_ERROR",
    "ZLIB_IO_ERROR",
    "ZLIB_CORRUPTED_DATA",
    "ZLIB_INVALID_LEN_CHECKSUM",
    "ZLIB_INVALID_COMPRESSION_TYPE",
    "ZLIB_INVALID_DECODED_VALUE",
    "ZLIB_INVALID_LENGTH",
    "ZLIB_TODO"
};
typedef enum PACKED_STRUCT BType { NO_COMPRESSION, COMPRESSED_FIXED_HF, COMPRESSED_DYNAMIC_HF, RESERVED } BType;
static const char* btypes_str[] = { "NO_COMPRESSION", "COMPRESSED_FIXED_HF", "COMPRESSED_DYNAMIC_HF", "RESERVED" };
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
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Resources: deflate <https://www.ietf.org/rfc/rfc1951.txt> *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// TODO: Possibly create hf_tree struct to contain all the length - size couples, generally reduce the size of functions' declaration
// TODO: Write better comments and error messages
// -----------------
//  Constant Values
// -----------------
/* -------------------------------------------------------------------------------------------------------- */
// ------------------
//  Static variables
// ------------------
static const unsigned short int fixed_hf_distances_table[] = {
 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE,
 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};
static const unsigned char fixed_hf_distances_lengths[] = {
 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
 0x5, 0x5
};
static const unsigned short int fixed_hf_literals_table[] = {
 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53,
 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83,
 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B,
 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
 0x190, 0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199,
 0x19A, 0x19B, 0x19C, 0x19D, 0x19E, 0x19F, 0x1A0, 0x1A1, 0x1A2, 0x1A3,
 0x1A4, 0x1A5, 0x1A6, 0x1A7, 0x1A8, 0x1A9, 0x1AA, 0x1AB, 0x1AC, 0x1AD,
 0x1AE, 0x1AF, 0x1B0, 0x1B1, 0x1B2, 0x1B3, 0x1B4, 0x1B5, 0x1B6, 0x1B7,
 0x1B8, 0x1B9, 0x1BA, 0x1BB, 0x1BC, 0x1BD, 0x1BE, 0x1BF, 0x1C0, 0x1C1,
 0x1C2, 0x1C3, 0x1C4, 0x1C5, 0x1C6, 0x1C7, 0x1C8, 0x1C9, 0x1CA, 0x1CB,
 0x1CC, 0x1CD, 0x1CE, 0x1CF, 0x1D0, 0x1D1, 0x1D2, 0x1D3, 0x1D4, 0x1D5,
 0x1D6, 0x1D7, 0x1D8, 0x1D9, 0x1DA, 0x1DB, 0x1DC, 0x1DD, 0x1DE, 0x1DF,
 0x1E0, 0x1E1, 0x1E2, 0x1E3, 0x1E4, 0x1E5, 0x1E6, 0x1E7, 0x1E8, 0x1E9,
 0x1EA, 0x1EB, 0x1EC, 0x1ED, 0x1EE, 0x1EF, 0x1F0, 0x1F1, 0x1F2, 0x1F3,
 0x1F4, 0x1F5, 0x1F6, 0x1F7, 0x1F8, 0x1F9, 0x1FA, 0x1FB, 0x1FC, 0x1FD,
 0x1FE, 0x1FF, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB,
 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xC0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7
};
static const unsigned char fixed_hf_literals_lengths[] = {
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
 0x9, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x8, 0x8, 0x8, 0x8, 0x8,
 0x8, 0x8, 0x8
};
/* -------------------------------------------------------------------------------------------------------- */
// ---------
//  Structs
// ---------
typedef struct Match {
 unsigned short int literal;
 unsigned char distance;
 unsigned char length_diff;
 unsigned short int distance_diff;
} Match;
typedef struct HFNode {
    unsigned short int symbol;
    unsigned int freq;
} HFNode;
typedef struct RLEStream {
 unsigned char value;
 unsigned char repeat_cnt;
} RLEStream;
typedef struct HFTree {
 unsigned char* lengths;
 unsigned short int* table;
 unsigned short int size;
 unsigned char is_fixed;
} HFTree;
/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
static int length_distance_encoding(const unsigned char* data_stream, unsigned int data_stream_size, Match** distance_encoding, unsigned int* distance_encoding_cnt);
static void update_hf_nodes(HFNode new_node, HFNode* hf_nodes, unsigned int hf_nodes_cnt);
static int build_hf_table(HFTree* hf_tree);
static int generate_hf_tree(unsigned short int* data_stream, unsigned int data_stream_size, HFTree* hf_tree);
static int rle_encoding(RLEStream** rle_encoded, unsigned short int* rle_encoded_size, HFTree hf_literals, HFTree hf_distances);
static int generate_hf_trees(Match* distance_encoded, unsigned int distance_encoded_size, BitStream* buffer, HFTree* hf_literals, HFTree* hf_distances);
static int hf_encode_block(HFTree hf_literals, HFTree hf_distances, Match* distance_encoding, unsigned int distance_encoding_cnt, BitStream* buffer);
static int encode_uncompressed_block(BitStream* compressed_bit_stream, unsigned char* data_buffer, unsigned int data_buffer_len, unsigned char is_final) ;
static int hf_compressed_block(BType method, BitStream* buffer, Match* distance_encoding, unsigned int distance_encoding_cnt, unsigned char is_final);
static int compress_block(BitStream* compressed_bit_stream, unsigned char* data_buffer, unsigned int data_buffer_len, unsigned char is_final);
/// NOTE: the stream will be always deallocated both in case of failure and success.
/// 	  Furthermore, the function allocates the returned stream of bytes, so that
/// 	  once it's on the hand of the caller, it's responsible to manage that memory.
unsigned char* zlib_deflate(unsigned char* data_buffer, unsigned int data_buffer_len, unsigned int* compressed_data_len, int* zlib_err);
/* -------------------------------------------------------------------------------------------------------- */
static void deallocate_hf_tree(HFTree* hf_tree) {
 XCOMP_SAFE_FREE(hf_tree -> lengths);
 XCOMP_SAFE_FREE(hf_tree -> table);
 return;
}
static int length_distance_encoding(const unsigned char* data_stream, unsigned int data_stream_size, Match** distance_encoding, unsigned int* distance_encoding_cnt) {
 *distance_encoding_cnt = MIN(data_stream_size, 3);
 *distance_encoding = (Match*) xcomp_realloc(*distance_encoding, *distance_encoding_cnt * sizeof(Match));
 if (*distance_encoding == NULL) {
  WARNING_LOG("Failed to xcomp_reallocate buffer for distance_encoding.\n");
  return -ZLIB_IO_ERROR;
 }
 for (unsigned char i = 0; i < *distance_encoding_cnt; ++i) (*distance_encoding)[i].literal = data_stream[i];
 unsigned short int cur_len = 0;
 for (long int i = 3; i < data_stream_size; i += cur_len) {
  unsigned short int found = 0;
  cur_len = 3;
  for (int j = 0; (j < i - cur_len) && (i < data_stream_size - cur_len); ++j) {
   if (mem_n_cmp((const char*) (data_stream + i), (const char*) (data_stream + j), cur_len) == 0) {
    cur_len++;
    found = i - j;
    if (cur_len == 259) break;
    continue;
   }
  }
  cur_len--;
  *distance_encoding = (Match*) xcomp_realloc(*distance_encoding, sizeof(Match) * (++(*distance_encoding_cnt)));
  if (*distance_encoding == NULL) {
   WARNING_LOG("Failed to xcomp_reallocate buffer for distance_encoding.\n");
   return -ZLIB_IO_ERROR;
  }
  (*distance_encoding)[*distance_encoding_cnt - 1] = (Match) {0};
  if (cur_len > 2) {
   const unsigned short int length_base_values[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
   unsigned char len_ind = 0;
   for (; len_ind < 29; ++len_ind) {
    if (cur_len < length_base_values[len_ind]) {
     --len_ind;
     break;
    } else if (cur_len == length_base_values[len_ind]) break;
   }
   (*distance_encoding)[*distance_encoding_cnt - 1].length_diff = cur_len - length_base_values[len_ind];
   (*distance_encoding)[*distance_encoding_cnt - 1].literal = 257 + len_ind;
   const unsigned short int dist_base_values[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
   unsigned char dist_ind = 0;
   for (; dist_ind < 30; ++dist_ind) {
    if (found < dist_base_values[dist_ind]) {
     --dist_ind;
     break;
    } else if (found == dist_base_values[dist_ind]) break;
   }
   dist_ind = MIN(dist_ind, 29);
   (*distance_encoding)[*distance_encoding_cnt - 1].distance_diff = found - dist_base_values[dist_ind];
   (*distance_encoding)[*distance_encoding_cnt - 1].distance = dist_ind;
  } else {
   (*distance_encoding)[*distance_encoding_cnt - 1].literal = data_stream[i];
   cur_len = 1;
  }
 }
 // Append the block delimiter
 *distance_encoding = (Match*) xcomp_realloc(*distance_encoding, sizeof(Match) * (++(*distance_encoding_cnt)));
 if (*distance_encoding == NULL) {
  WARNING_LOG("Failed to xcomp_reallocate buffer for distance_encoding.\n");
  return -ZLIB_IO_ERROR;
 }
 (*distance_encoding)[*distance_encoding_cnt - 1] = (Match) {0};
 (*distance_encoding)[*distance_encoding_cnt - 1].literal = 256;
 return ZLIB_NO_ERROR;
}
static void update_hf_nodes(HFNode new_node, HFNode* hf_nodes, unsigned int hf_nodes_cnt) {
 for (unsigned short int i = 0; i < hf_nodes_cnt - 1; ++i) {
  if ((hf_nodes[i].freq > new_node.freq) || (hf_nodes[i].freq == new_node.freq && hf_nodes[i].symbol > new_node.symbol)) {
   // Move one to the left the others and insert it
   mem_move(hf_nodes + i + 1, hf_nodes + i, (hf_nodes_cnt - 1 - i) * sizeof(HFNode));
   hf_nodes[i] = new_node;
   return;
  }
 }
 hf_nodes[hf_nodes_cnt - 1] = new_node;
 return;
}
static int build_hf_table(HFTree* hf_tree) {
 hf_tree -> table = (unsigned short int*) xcomp_calloc(hf_tree -> size, sizeof(unsigned short int));
 if (hf_tree -> table == NULL) {
  WARNING_LOG("Failed to allocate buffer for hf_tree.\n");
  return -ZLIB_IO_ERROR;
 }
 unsigned int bl_cnts[16] = {0};
 for (unsigned short int i = 0; i < hf_tree -> size; ++i) bl_cnts[(hf_tree -> lengths)[i]]++;
 bl_cnts[0] = 0;
 unsigned short int mins[16] = {0};
 for (unsigned char i = 1; i < 16; ++i) mins[i] = (mins[i - 1] + bl_cnts[i - 1]) << 1;
 for (unsigned short int i = 0; i < hf_tree -> size; ++i) if ((hf_tree -> lengths)[i]) (hf_tree -> table)[i] = mins[(hf_tree -> lengths)[i]]++;
 return ZLIB_NO_ERROR;
}
// Compute Huffman code lengths from a frequency table
static int generate_hf_tree(unsigned short int* data_stream, unsigned int data_stream_size, HFTree* hf_tree) {
 unsigned int frequencies[288] = {0};
 for (unsigned int i = 0; i < data_stream_size; ++i) frequencies[data_stream[i]]++;
 HFNode hf_nodes[288] = {0}; // Heap for building the tree
 unsigned short int hf_nodes_cnt = 0;
 for (unsigned short int i = 0; i < 288; ++i) {
  if (frequencies[i]) {
   HFNode new_node = (HFNode) { .symbol = i, .freq = frequencies[i] };
   update_hf_nodes(new_node, hf_nodes, ++hf_nodes_cnt);
   hf_tree -> size = MAX(hf_tree -> size, i + 1);
  }
 }
 if (hf_tree -> size == 0) {
  hf_tree -> table = (unsigned short int*) xcomp_calloc(MAX(hf_tree -> size, 1), sizeof(unsigned short int));
  hf_tree -> lengths = (unsigned char*) xcomp_calloc(MAX(hf_tree -> size, 1), sizeof(unsigned char));
  return ZLIB_NO_ERROR;
 }
    // Build Huffman tree (iterative method)
 unsigned int parent_size = hf_tree -> size;
 unsigned int* parent = (unsigned int*) xcomp_calloc(parent_size, sizeof(unsigned int));
 if (parent == NULL) {
  WARNING_LOG("Failed to allocate buffer for parent.\n");
  return -ZLIB_IO_ERROR;
 }
    while (hf_nodes_cnt > 1) {
        // Take two smallest nodes
        HFNode left = hf_nodes[0];
        HFNode right = hf_nodes[1];
        // Remove them from heap
        mem_move(hf_nodes, hf_nodes + 2, (hf_nodes_cnt - 2) * sizeof(HFNode));
        hf_nodes_cnt--;
        // Create a new merged node
        HFNode new_node = (HFNode) { .symbol = parent_size++, .freq = left.freq + right.freq};
  parent = (unsigned int*) xcomp_realloc(parent, sizeof(unsigned int) * parent_size);
  if (parent == NULL) {
   WARNING_LOG("Failed to xcomp_reallocate buffer for parent.\n");
   return -ZLIB_IO_ERROR;
  }
  parent[parent_size - 1] = 0;
        // Assign parents for tree traversal
        parent[left.symbol] = new_node.symbol;
        parent[right.symbol] = new_node.symbol;
  update_hf_nodes(new_node, hf_nodes, hf_nodes_cnt);
    }
    // Assign bit-lengths from depths
    hf_tree -> lengths = (unsigned char*) xcomp_calloc(hf_tree -> size, sizeof(unsigned char));
 if (hf_tree -> lengths == NULL) {
  WARNING_LOG("Failed to allocate buffer for hf_lengths.\n");
  return -ZLIB_IO_ERROR;
 }
 for (unsigned short int i = 0; i < hf_tree -> size; i++) {
        unsigned int node = i;
        while (parent[node]) {
   ((hf_tree -> lengths)[i])++;
            node = parent[node];
        }
    }
 XCOMP_SAFE_FREE(parent);
 int err = 0;
 if ((err = build_hf_table(hf_tree)) < 0) {
  XCOMP_SAFE_FREE(hf_tree -> lengths);
  WARNING_LOG("An error occurred while building the hf table.\n");
  return err;
 }
 return ZLIB_NO_ERROR;
}
static int rle_encoding(RLEStream** rle_encoded, unsigned short int* rle_encoded_size, HFTree hf_literals, HFTree hf_distances) {
 *rle_encoded = (RLEStream*) xcomp_calloc(hf_literals.size + hf_distances.size, sizeof(RLEStream));
 if (*rle_encoded == NULL) {
  WARNING_LOG("Failed to allocate buffer for rle_encoded.\n");
  return -ZLIB_IO_ERROR;
 }
 unsigned char previous_code = (hf_literals.lengths)[0];
 *rle_encoded_size = 0;
 (*rle_encoded)[(*rle_encoded_size)++].value = (hf_literals.lengths)[0];
 unsigned char zero_cnt = 0;
 unsigned char repeat_cnt = 0;
 for (unsigned short int i = 1; i < hf_literals.size; ++i) {
  if ((hf_literals.lengths)[i] == 0) {
   for (zero_cnt = 1; zero_cnt < 138; ++i, ++zero_cnt) if (i + 1 >= hf_literals.size || (hf_literals.lengths)[i + 1] != 0) break;
   if (zero_cnt < 138 && i == hf_literals.size) break;
   else if (zero_cnt == 2) (*rle_encoded)[(*rle_encoded_size)++].value = (hf_literals.lengths)[i - 1];
  } else if ((hf_literals.lengths)[i] == previous_code) {
   for (repeat_cnt = 1; repeat_cnt < 6; ++i, ++repeat_cnt) if (i + 1 >= hf_literals.size || (hf_literals.lengths)[i + 1] != previous_code) break;
   if (repeat_cnt < 6 && i == hf_literals.size) break;
   else if (repeat_cnt == 2) (*rle_encoded)[(*rle_encoded_size)++].value = (hf_literals.lengths)[i - 1];
  }
  if (repeat_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = repeat_cnt, (*rle_encoded)[*rle_encoded_size].value = 16;
  else if (zero_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = zero_cnt, (*rle_encoded)[*rle_encoded_size].value = 17 + (zero_cnt > 10), previous_code = 0;
  else (*rle_encoded)[*rle_encoded_size].value = (hf_literals.lengths)[i], previous_code = (hf_literals.lengths)[i];
  (*rle_encoded_size)++;
  repeat_cnt = 0, zero_cnt = 0;
 }
 if ((repeat_cnt || zero_cnt) && (previous_code != (hf_distances.lengths)[0])) {
  if (repeat_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = repeat_cnt, (*rle_encoded)[(*rle_encoded_size)++].value = 16;
  else if (zero_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = zero_cnt, (*rle_encoded)[(*rle_encoded_size)++].value = 17 + (zero_cnt > 10), previous_code = 0;
  else {
   unsigned char limit = MAX(repeat_cnt, zero_cnt);
   if (zero_cnt) previous_code = 0;
   for (unsigned char i = 0; i < limit; ++i, ++(*rle_encoded_size)) (*rle_encoded)[*rle_encoded_size].repeat_cnt = 0, (*rle_encoded)[*rle_encoded_size].value = previous_code;
  }
 } else if ((repeat_cnt == 1 || zero_cnt == 1) && (previous_code == (hf_distances.lengths)[0] && previous_code != (hf_distances.lengths)[1])) {
  if (zero_cnt) previous_code = 0;
  (*rle_encoded)[*rle_encoded_size].repeat_cnt = 0, (*rle_encoded)[(*rle_encoded_size)++].value = previous_code;
 }
 for (unsigned short int i = 0; i < hf_distances.size; ++i) {
  if ((hf_distances.lengths)[i] == 0) {
   for (zero_cnt = 1; zero_cnt < 138; ++i, ++zero_cnt) if (i + 1 >= hf_distances.size || (hf_distances.lengths)[i + 1] != 0) break;
   if (zero_cnt < 138 && i == hf_distances.size) break;
   else if (zero_cnt == 2) (*rle_encoded)[(*rle_encoded_size)++].value = (hf_distances.lengths)[i - 1];
  } else if ((hf_distances.lengths)[i] == previous_code) {
   for (repeat_cnt = 1; repeat_cnt < 6; ++i, ++repeat_cnt) if (i + 1 >= hf_distances.size || (hf_distances.lengths)[i + 1] != previous_code) break;
   if (repeat_cnt < 6 && i == hf_distances.size) break;
   else if (repeat_cnt == 2) (*rle_encoded)[(*rle_encoded_size)++].value = (hf_distances.lengths)[i - 1];
  }
  if (repeat_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = repeat_cnt, (*rle_encoded)[*rle_encoded_size].value = 16;
  else if (zero_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = zero_cnt, (*rle_encoded)[*rle_encoded_size].value = 17 + (zero_cnt > 10), previous_code = 0;
  else (*rle_encoded)[*rle_encoded_size].repeat_cnt = 0, (*rle_encoded)[*rle_encoded_size].value = (hf_distances.lengths)[i], previous_code = (hf_distances.lengths)[i];
  (*rle_encoded_size)++;
  repeat_cnt = 0, zero_cnt = 0;
 }
 if (repeat_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = repeat_cnt, (*rle_encoded)[(*rle_encoded_size)++].value = 16;
 else if (zero_cnt >= 3) (*rle_encoded)[*rle_encoded_size].repeat_cnt = zero_cnt, (*rle_encoded)[(*rle_encoded_size)++].value = 17 + (zero_cnt > 10), previous_code = 0;
 else {
  unsigned char limit = MAX(repeat_cnt, zero_cnt);
  if (zero_cnt) previous_code = 0;
  for (unsigned char i = 0; i < limit; ++i, ++(*rle_encoded_size)) (*rle_encoded)[*rle_encoded_size].repeat_cnt = 0, (*rle_encoded)[*rle_encoded_size].value = previous_code;
 }
 *rle_encoded = (RLEStream*) xcomp_realloc(*rle_encoded, (*rle_encoded_size) * sizeof(RLEStream));
 if (*rle_encoded == NULL) {
  WARNING_LOG("Failed to xcomp_reallocate buffer for rle_encoded.\n");
  return -ZLIB_IO_ERROR;
 }
 return ZLIB_NO_ERROR;
}
static int generate_hf_trees(Match* distance_encoded, unsigned int distance_encoded_size, BitStream* buffer, HFTree* hf_literals, HFTree* hf_distances) {
 unsigned int distance_size = 0;
 unsigned short int* literals_data = (unsigned short int*) xcomp_calloc(distance_encoded_size, sizeof(unsigned short int));
 if (literals_data == NULL) {
  WARNING_LOG("Failed to allocate buffer for literals_data.\n");
  return -ZLIB_IO_ERROR;
 }
 unsigned short int* distance_data = (unsigned short int*) xcomp_calloc(distance_encoded_size, sizeof(unsigned short int));
 if (distance_data == NULL) {
  XCOMP_SAFE_FREE(literals_data);
  WARNING_LOG("Failed to allocate buffer for distance_data.\n");
  return -ZLIB_IO_ERROR;
 }
 for (unsigned int i = 0; i < distance_encoded_size; ++i) {
  literals_data[i] = distance_encoded[i].literal;
  if (literals_data[i] > 256) distance_data[distance_size++] = distance_encoded[i].distance;
 }
 if (distance_size == 0) XCOMP_SAFE_FREE(distance_data);
 else {
  distance_data = (unsigned short int*) xcomp_realloc(distance_data, distance_size * sizeof(unsigned short int));
  if (distance_data == NULL) {
   XCOMP_SAFE_FREE(literals_data);
   WARNING_LOG("Failed to xcomp_reallocate buffer for distance_data.\n");
   return -ZLIB_IO_ERROR;
  }
 }
 // Generate the tables
 int err = 0;
 if ((err = generate_hf_tree(literals_data, distance_encoded_size, hf_literals)) < 0) {
  XCOMP_MULTI_FREE(literals_data, distance_data);
  WARNING_LOG("An error occurred while generating the hf_tree for literals.\n");
  return err;
 }
 if ((err = generate_hf_tree(distance_data, distance_size, hf_distances)) < 0) {
  deallocate_hf_tree(hf_literals);
  XCOMP_MULTI_FREE(literals_data, distance_data);
  WARNING_LOG("An error occurred while generating the hf_tree for distances.\n");
  return err;
 }
 XCOMP_MULTI_FREE(literals_data, distance_data);
 RLEStream* rle_encoded = NULL;
 unsigned short int rle_encoded_size = 0;
 if ((err = rle_encoding(&rle_encoded, &rle_encoded_size, *hf_literals, *hf_distances)) < 0) {
  do { HFTree* hf_trees[] = { NULL,hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE);
  WARNING_LOG("An error occurred while rle_encoding.\n");
  return err;
 }
 unsigned short int* rle_encoded_data = (unsigned short int*) xcomp_calloc(rle_encoded_size, sizeof(unsigned short int));
 if (rle_encoded_data == NULL) {
  do { HFTree* hf_trees[] = { NULL,hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE);
  XCOMP_SAFE_FREE(rle_encoded);
  WARNING_LOG("Failed to allocate buffer for rle_encoded_data.\n");
  return -ZLIB_IO_ERROR;
 }
 for (unsigned short int i = 0; i < rle_encoded_size; ++i) rle_encoded_data[i] = rle_encoded[i].value;
 HFTree hf_tree = { .size = 19 };
 if ((err = generate_hf_tree(rle_encoded_data, rle_encoded_size, &hf_tree)) < 0) {
  do { HFTree* hf_trees[] = { NULL,hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE);
  XCOMP_MULTI_FREE(rle_encoded, rle_encoded_data);
  WARNING_LOG("An error occurred while generating the hf_tree for the previous hf.\n");
  return err;
 }
 XCOMP_SAFE_FREE(rle_encoded_data);
 SAFE_BIT_WRITE(buffer, MAX(257, hf_literals -> size) - 257, 5, do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
 SAFE_BIT_WRITE(buffer, MAX(1, hf_distances -> size) - 1, 5, do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
 const unsigned char order_of_code_lengths[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
 unsigned char order_size = 18;
 for (; order_size > 3; --order_size) if ((hf_tree.lengths)[order_of_code_lengths[order_size]] > 0) break;
 SAFE_BIT_WRITE(buffer, (order_size + 1) - 4, 4, do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
 for (unsigned char i = 0; i <= order_size; ++i) SAFE_BIT_WRITE(buffer, (hf_tree.lengths)[order_of_code_lengths[i]], 3, do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
 for (unsigned short int i = 0; i < rle_encoded_size; ++i) {
  unsigned char value = rle_encoded[i].value;
  unsigned char repeat_cnt = rle_encoded[i].repeat_cnt;
  SAFE_REV_BIT_WRITE(buffer, (hf_tree.table)[value], (hf_tree.lengths)[value], do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
  if (repeat_cnt) {
   if (value == 16) SAFE_BIT_WRITE(buffer, repeat_cnt - 3, 2, do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
   else if (value == 17) SAFE_BIT_WRITE(buffer, repeat_cnt - 3, 3,do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
   else SAFE_BIT_WRITE(buffer, repeat_cnt - 11, 7, do { do { HFTree* hf_trees[] = { NULL,&hf_tree, hf_literals, hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE); XCOMP_SAFE_FREE(rle_encoded); } while (FALSE));
  }
 }
 deallocate_hf_tree(&hf_tree);
 XCOMP_SAFE_FREE(rle_encoded);
 return ZLIB_NO_ERROR;
}
static int hf_encode_block(HFTree hf_literals, HFTree hf_distances, Match* distance_encoding, unsigned int distance_encoding_cnt, BitStream* buffer) {
    const unsigned char lenghts_extra_bits[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    const unsigned char distances_extra_bits[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
 for (unsigned int i = 0; i < distance_encoding_cnt; ++i) {
  unsigned short int literal = distance_encoding[i].literal;
  SAFE_REV_BIT_WRITE(buffer, (hf_literals.table)[literal], (hf_literals.lengths)[literal]);
  if (literal > 256) {
   unsigned char distance = distance_encoding[i].distance;
   SAFE_BIT_WRITE(buffer, distance_encoding[i].length_diff, lenghts_extra_bits[literal - 257]);
   SAFE_REV_BIT_WRITE(buffer, (hf_distances.table)[distance], (hf_distances.lengths)[distance]);
   SAFE_BIT_WRITE(buffer, distance_encoding[i].distance_diff, distances_extra_bits[distance]);
  }
 }
 return ZLIB_NO_ERROR;
}
static int encode_uncompressed_block(BitStream* compressed_bit_stream, unsigned char* data_buffer, unsigned int data_buffer_len, unsigned char is_final) {
 SAFE_BIT_WRITE(compressed_bit_stream, is_final, 3);
 unsigned short int buffer_len = data_buffer_len & 0xFFFF;
 XCOMP_BE_CONVERT(&buffer_len, sizeof(unsigned short int));
 SAFE_BYTE_WRITE(compressed_bit_stream, sizeof(unsigned short int), 1, &buffer_len);
 buffer_len = ~buffer_len;
 SAFE_BYTE_WRITE(compressed_bit_stream, sizeof(unsigned short int), 1, &buffer_len);
 XCOMP_BE_CONVERT(data_buffer, data_buffer_len);
 SAFE_BYTE_WRITE(compressed_bit_stream, sizeof(unsigned char), data_buffer_len, data_buffer);
 return ZLIB_NO_ERROR;
}
static int hf_compressed_block(BType method, BitStream* buffer, Match* distance_encoding, unsigned int distance_encoding_cnt, unsigned char is_final) {
 int err = 0;
 // Calculate the Huffman Tree/Table
 SAFE_NEXT_BIT_WRITE(buffer, is_final, distance_encoding);
 SAFE_BIT_WRITE(buffer, method, 2, XCOMP_SAFE_FREE(distance_encoding));
 HFTree hf_literals = {0};
 HFTree hf_distances = {0};
 if (method == COMPRESSED_DYNAMIC_HF) {
  if ((err = generate_hf_trees(distance_encoding, distance_encoding_cnt, buffer, &hf_literals, &hf_distances)) < 0) {
   WARNING_LOG("An error occurred while generating hf_tables.\n");
   return err;
  }
 } else {
  hf_literals.table = (unsigned short int*) fixed_hf_literals_table; hf_literals.lengths = (unsigned char*) fixed_hf_literals_lengths; hf_literals.size = 286; hf_literals.is_fixed = TRUE;;
  hf_distances.table = (unsigned short int*) fixed_hf_distances_table; hf_distances.lengths = (unsigned char*) fixed_hf_distances_lengths; hf_distances.size = 30; hf_distances.is_fixed = TRUE;;
 }
 // Huffman encode the block, encapsulating it into a DEFLATE block (append block header, encoded data plus the encoded '256' to signal the end of the block)
 if ((err = hf_encode_block(hf_literals, hf_distances, distance_encoding, distance_encoding_cnt, buffer)) < 0) {
  if (!hf_literals.is_fixed) do { HFTree* hf_trees[] = { NULL,&hf_literals, &hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE);
  WARNING_LOG("An error occurred while encoding the block.\n");
  return err;
 }
 if (!hf_literals.is_fixed) do { HFTree* hf_trees[] = { NULL,&hf_literals, &hf_distances }; for (unsigned int i = 1; i < XCOMP_ARR_SIZE(hf_trees); ++i) deallocate_hf_tree(hf_trees[i]); } while (FALSE);
 return ZLIB_NO_ERROR;
}
static int compress_block(BitStream* compressed_bit_stream, unsigned char* data_buffer, unsigned int data_buffer_len, unsigned char is_final) {
 int err = 0;
 Match* distance_encoding = NULL;
 unsigned int distance_encoding_cnt = 0;
 if ((err = length_distance_encoding(data_buffer, data_buffer_len, &distance_encoding, &distance_encoding_cnt)) < 0) {
  WARNING_LOG("An error occurred while performing the length-distance encoding.\n");
  return err;
 }
 // Static compression
 BitStream fixed_block_bit_stream = CREATE_BIT_STREAM(NULL, 0);
 if ((err = hf_compressed_block(COMPRESSED_FIXED_HF, &fixed_block_bit_stream, distance_encoding, distance_encoding_cnt, is_final)) < 0) {
  XCOMP_SAFE_FREE(distance_encoding);
  WARNING_LOG("An error occurred while compressing the block using FIXED_HF.\n");
  return err;
 }
 // Dynamic compression
 BitStream dynamic_block_bit_stream = CREATE_BIT_STREAM(NULL, 0);
 if ((err = hf_compressed_block(COMPRESSED_DYNAMIC_HF, &dynamic_block_bit_stream, distance_encoding, distance_encoding_cnt, is_final)) < 0) {
  XCOMP_SAFE_FREE(distance_encoding);
  deallocate_bit_stream(&fixed_block_bit_stream);
  WARNING_LOG("An error occurred while compressing the block using DYNAMIC_HF.\n");
  return err;
 }
 XCOMP_SAFE_FREE(distance_encoding);
 // Fallback no compression
 if (fixed_block_bit_stream.size > data_buffer_len + 5 && dynamic_block_bit_stream.size > data_buffer_len + 5) {
  deallocate_bit_stream(&fixed_block_bit_stream);
  deallocate_bit_stream(&dynamic_block_bit_stream);
  printf("compression_method: '%s', decompressed_size: %u\n", btypes_str[NO_COMPRESSION], data_buffer_len);
  if ((err = encode_uncompressed_block(compressed_bit_stream, data_buffer, data_buffer_len, is_final)) < 0) {
   WARNING_LOG("An error occurred while encoding the uncompressed block.\n");
   return err;
  }
  return ZLIB_NO_ERROR;
 }
 unsigned char is_fixed_better = fixed_block_bit_stream.size <= dynamic_block_bit_stream.size;
 printf("compression_method: '%s', decompressed_size: %u\n", btypes_str[is_fixed_better ? COMPRESSED_FIXED_HF : COMPRESSED_DYNAMIC_HF], data_buffer_len);
 BitStream block_bit_stream = is_fixed_better ? fixed_block_bit_stream : dynamic_block_bit_stream;
 if (is_fixed_better) deallocate_bit_stream(&dynamic_block_bit_stream);
 else deallocate_bit_stream(&fixed_block_bit_stream);
 bitstream_bit_copy(compressed_bit_stream, &block_bit_stream);
 if (compressed_bit_stream -> error) {
  if (!is_fixed_better) deallocate_bit_stream(&dynamic_block_bit_stream);
  else deallocate_bit_stream(&fixed_block_bit_stream);
  WARNING_LOG("Failed to bit copy the block bitstream into the compressed bitstream.\n");
  return -ZLIB_IO_ERROR;
 }
 // Deallocate the one the bit_stream actually used
 if (!is_fixed_better) deallocate_bit_stream(&dynamic_block_bit_stream);
 else deallocate_bit_stream(&fixed_block_bit_stream);
 return ZLIB_NO_ERROR;
}
unsigned char* zlib_deflate(unsigned char* data_buffer, unsigned int data_buffer_len, unsigned int* compressed_data_len, int* zlib_err) {
 *compressed_data_len = 0;
 BitStream compressed_bit_stream = CREATE_BIT_STREAM(NULL, 0);
 *zlib_err = -ZLIB_NO_ERROR;
 // Fragment the data in block of WINDOW_SIZE
 unsigned int buffer_offset = 0;
 unsigned int block_cnt = 0;
 while (data_buffer_len >= 0x7FFF) {
  DEBUG_LOG("Block %u: is_final: %u, ", ++block_cnt, data_buffer_len == 0x7FFF);
  if ((*zlib_err = compress_block(&compressed_bit_stream, data_buffer + buffer_offset, 0x7FFF, data_buffer_len == 0x7FFF)) < 0) {
   XCOMP_SAFE_FREE(data_buffer);
   deallocate_bit_stream(&compressed_bit_stream);
   return ((unsigned char*) "An error occurred while compressing the block.\n");
  }
  data_buffer_len -= 0x7FFF;
  buffer_offset += 0x7FFF;
 }
 if (data_buffer_len > 0) {
  DEBUG_LOG("Block %u: is_final: 1, ", ++block_cnt);
  if ((*zlib_err = compress_block(&compressed_bit_stream, data_buffer + buffer_offset, data_buffer_len, TRUE)) < 0) {
   XCOMP_SAFE_FREE(data_buffer);
   deallocate_bit_stream(&compressed_bit_stream);
   return ((unsigned char*) "An error occurred while compressing the block.\n");
  }
 }
 XCOMP_SAFE_FREE(data_buffer);
 *compressed_data_len = compressed_bit_stream.size;
 return compressed_bit_stream.stream;
}
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
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Resources: deflate <https://www.ietf.org/rfc/rfc1951.txt> *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// -------------------
//  Macros Definition
// -------------------
/* ---------------------------------------------------------------------------------------------------------- */
// ---------
//  Structs
// ---------
typedef struct HFTable {
    unsigned short int** values;
    unsigned short int* min_codes;
    unsigned short int* max_codes;
    unsigned char* lengths;
    unsigned short int size;
    unsigned char max_bit_length;
 unsigned char is_fixed_hf;
} HFTable;
/* -------------------------------------------------------------------------------------------------------- */
// ------------------
//  Static Variables
// ------------------
// Fixed huffman literal/lengths codes
static const unsigned short int fixed_val_ptr[] = {0, 256, 0, 280, 144};
static const unsigned short int fixed_mins[] = {0, 0x00, 0x30, 0xC0, 0x190};
static const unsigned short int fixed_maxs[] = {0, 0x18, 0xC0, 0xC8, 0x200};
static const unsigned short int fixed_distance_val_ptr[] = {0, 0x00};
static const unsigned short int fixed_distance_mins[] = {0, 0x00};
static const unsigned short int fixed_distance_maxs[] = {0, 0x20};
/* ---------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------
static void deallocate_hf_table(HFTable* hf);
static int generate_codes(HFTable* hf);
static int decode_hf(BitStream* bit_stream, unsigned short int code, HFTable hf);
static int decode_lengths(BitStream* bit_stream, HFTable decoder_hf, HFTable* literals_hf, HFTable* distance_hf);
static int copy_data(unsigned char** dest, unsigned int* index, unsigned short int length, unsigned short int distance);
static int get_length(BitStream* bit_stream, unsigned short int value);
static int get_distance(BitStream* bit_stream, unsigned short int value);
static int read_uncompressed_data(BitStream* bit_stream, unsigned char** decompressed_data, unsigned int* decompressed_data_length);
static int decode_dynamic_huffman_tables(BitStream* bit_stream, HFTable* literals_hf, HFTable* distance_hf);
/// NOTE: the stream will be always deallocated both in case of failure and success.
/// 	  Furthermore, the function allocates the returned stream of bytes, so that
/// 	  once it's on the hand of the caller, it's responsible to manage that memory.
unsigned char* zlib_inflate(unsigned char* stream, unsigned int size, unsigned int* decompressed_data_length, int* zlib_err);
/* ---------------------------------------------------------------------------------------------------------- */
static void deallocate_hf_table(HFTable* hf) {
 if (hf -> is_fixed_hf) return;
 for (unsigned char i = 1; i <= hf -> max_bit_length; ++i) XCOMP_SAFE_FREE((hf -> values)[i]);
    XCOMP_SAFE_FREE(hf -> values);
    XCOMP_SAFE_FREE(hf -> min_codes);
    XCOMP_SAFE_FREE(hf -> max_codes);
    XCOMP_SAFE_FREE(hf -> lengths);
    hf -> max_bit_length = 0;
    hf -> size = 0;
    return;
}
static int generate_codes(HFTable* hf) {
 if (hf -> max_bit_length == 0) {
  hf -> max_bit_length = 0; do { for (unsigned short int i = 0; i < (hf -> size); ++i) { (hf -> max_bit_length) = ((hf -> max_bit_length) < (hf -> lengths)[i]) ? (hf -> lengths)[i] : (hf -> max_bit_length); } } while (FALSE);
 }
 unsigned char bl_count[16] = {0};
 for (unsigned short int i = 0; i < hf -> size; ++i) bl_count[(hf -> lengths)[i]]++;
 bl_count[0] = 0;
    hf -> values = (unsigned short int**) xcomp_calloc(hf -> max_bit_length + 1, sizeof(unsigned short int*));
    if (hf -> values == NULL) {
  WARNING_LOG("Failed to allocate buffer for hf -> values.\n");
  return -ZLIB_IO_ERROR;
 }
 for (unsigned char i = 1; i <= hf -> max_bit_length; ++i) {
        (hf -> values)[i] = (unsigned short int*) xcomp_calloc(bl_count[i], sizeof(unsigned short int));
  if ((hf -> values)[i] == NULL) {
   WARNING_LOG("Failed to allocate buffer for hf -> values[%u].\n", i);
   return -ZLIB_IO_ERROR;
  }
 }
 // Find the minimum and maximum code values for each bit_length 
    hf -> min_codes = (unsigned short int*) xcomp_calloc(hf -> max_bit_length + 1, sizeof(unsigned short int));
    if (hf -> min_codes == NULL) {
  WARNING_LOG("Failed to allocate buffer for hf -> min_codes.\n");
  return -ZLIB_IO_ERROR;
 }
 hf -> max_codes = (unsigned short int*) xcomp_calloc(hf -> max_bit_length + 1, sizeof(unsigned short int));
    if (hf -> max_codes == NULL) {
  WARNING_LOG("Failed to allocate buffer for hf -> max_codes.\n");
  return -ZLIB_IO_ERROR;
 }
    for (unsigned char bits = 1; bits <= hf -> max_bit_length; ++bits) {
        (hf -> min_codes)[bits] = ((hf -> min_codes)[bits - 1] + bl_count[bits - 1]) << 1;
        (hf -> max_codes)[bits] = (hf -> min_codes)[bits];
    }
 // Compute the code for each entry, and updating the max_code each time accordingly.
    // In this way we sort of allocate a code for each entry in order (as lexicographical order is required), as we already know the base for each bit_length (min_code).
 unsigned char* values_index = (unsigned char*) xcomp_calloc(hf -> max_bit_length + 1, sizeof(unsigned char));
    if (values_index == NULL) {
  WARNING_LOG("Failed to allocate buffer for values_index.\n");
  return -ZLIB_IO_ERROR;
 }
 for (unsigned short int i = 0; i < hf -> size; ++i) {
        if ((hf -> lengths)[i] != 0) {
            unsigned char value_bit_len = (hf -> lengths)[i];
            ((hf -> max_codes)[value_bit_len])++;
            (hf -> values)[value_bit_len][values_index[value_bit_len]++] = i;
  }
    }
    XCOMP_SAFE_FREE(values_index);
    return ZLIB_NO_ERROR;
}
static int decode_hf(BitStream* bit_stream, unsigned short int code, HFTable hf) {
 if (hf.is_fixed_hf) {
  // Read the first n common first bits, if bit_length is 4 means that is the literals hf so the base is 6 common bits, otherwise is 4 for distance hf
  for (unsigned char i = 0; i < (hf.max_bit_length == 4 ? 6 : 4); ++i) {
   unsigned char next_bit = SAFE_NEXT_BIT_READ(bit_stream, next_bit);
   code = (code << 1) + next_bit;
  }
 }
 for (unsigned char i = 1; i <= hf.max_bit_length; ++i) {
  // Check if the code is inside this bit_length group, by checking if it's less than the maximum code for this bit_length.
  if (hf.max_codes[i] > code) {
   // Return the corresponding decoded value from the hf tree, subtracting the min code for this bit length to get the offset of the entry.
   return (hf.is_fixed_hf) ? (code - hf.min_codes[i] + hf.values[0][i]) : hf.values[i][code - hf.min_codes[i]];
        }
  // If the code does not match, then add a bit and check with the next bit_length
        // If the hf used is the literals fixed one, then we need to do the following as the 8 bit-length group is divided in two sub-groups (0 - 143) and (280 - 287), preventing it to read the next_bit and skip the length
        if (i != 2 || hf.is_fixed_hf != 2) {
   unsigned char next_bit = SAFE_NEXT_BIT_READ(bit_stream, next_bit);
   code = (code << 1) + next_bit;
  }
        if (bit_stream -> error) break;
    }
 // Return max value to signal error during operation.
    return -ZLIB_INVALID_DECODED_VALUE;
}
static int decode_lengths(BitStream* bit_stream, HFTable decoder_hf, HFTable* literals_hf, HFTable* distance_hf) {
    literals_hf -> lengths = (unsigned char*) xcomp_calloc(literals_hf -> size, sizeof(unsigned char));
 if (literals_hf -> lengths == NULL) {
  WARNING_LOG("Failed to allocate buffer for literals_hf -> lengths.\n");
  return -ZLIB_IO_ERROR;
 }
 distance_hf -> lengths = (unsigned char*) xcomp_calloc(distance_hf -> size, sizeof(unsigned char));
    if (distance_hf -> lengths == NULL) {
  XCOMP_SAFE_FREE(literals_hf -> lengths);
  WARNING_LOG("Failed to allocate buffer for distance_hf -> lengths.\n");
  return -ZLIB_IO_ERROR;
 }
 unsigned short int index = 0;
    while (index < (literals_hf -> size + distance_hf -> size)) {
  unsigned short int code = SAFE_NEXT_BIT_READ(bit_stream, code);
        int value = decode_hf(bit_stream, code, decoder_hf);
  if (value < 0) {
   XCOMP_MULTI_FREE(literals_hf -> lengths, distance_hf -> lengths);
   WARNING_LOG("Corrupted encoded lengths.\n");
   return value;
  }
  // NOTE: the operation index - literals_hf -> size, is required to normalize the index, as the values are read sequentially
        if (value < 16) {
   // 0 - 15: Represent code lengths of 0 - 15.
   // Hence, we first fill the literals and then the distances (look block order below)
            if (index < literals_hf -> size) {
    (literals_hf -> lengths)[index] = value;
    literals_hf -> max_bit_length = MAX(literals_hf -> max_bit_length, value);
   } else {
    (distance_hf -> lengths)[index - literals_hf -> size] = value;
    distance_hf -> max_bit_length = MAX(distance_hf -> max_bit_length, value);
   }
   index++;
        } else if (value == 16) {
   // 16: Copy the previous code length 3 - 6 times.
   // Determine how many times to repeat
   unsigned char count = SAFE_BITS_READ(bit_stream, count, 2);
            count += 3;
   // Fill the literals and then the distances (look block order below)
            unsigned char value = (index < literals_hf -> size) ? (literals_hf -> lengths)[index - 1] : (distance_hf -> lengths)[index - literals_hf -> size - 1];
            for (unsigned char i = 0; i < count; ++i, ++index) {
                if (index < literals_hf -> size) (literals_hf -> lengths)[index] = value;
                else (distance_hf -> lengths)[index - literals_hf -> size] = value;
            }
        } else if (value == 17) {
   // 17: Repeat a code length of 0 for 3 - 10 times. (3 bits of length).
   unsigned char count = SAFE_BITS_READ(bit_stream, count, 3);
            count += 3;
   for (unsigned char i = 0; i < count; ++i, ++index) {
                if (index < literals_hf -> size) (literals_hf -> lengths)[index] = 0;
                else (distance_hf -> lengths)[index - literals_hf -> size] = 0;
            }
        } else if (value == 18) {
   // 18: Repeat a code length of 0 for 11 - 138 times (7 bits of length).
   unsigned char count = SAFE_BITS_READ(bit_stream, count, 7);
            count += 11;
   for (unsigned char i = 0; i < count; ++i, ++index) {
                if (index < literals_hf -> size) (literals_hf -> lengths)[index] = 0;
                else (distance_hf -> lengths)[index - literals_hf -> size] = 0;
            }
        } else {
   WARNING_LOG("Invalid value decoded: %u must be less than 19.\n", value); return -ZLIB_CORRUPTED_DATA;
  }
    }
    return ZLIB_NO_ERROR;
}
/// Move backwards distance bytes in the output stream, and copy length bytes from this position to the output stream
static int copy_data(unsigned char** dest, unsigned int* index, unsigned short int length, unsigned short int distance) {
    *dest = (unsigned char*) xcomp_realloc(*dest, sizeof(unsigned char) * ((*index) + length));
 if (*dest == NULL) {
  WARNING_LOG("Failed to xcomp_reallocate buffer for dest.\n");
  return -ZLIB_IO_ERROR;
 }
 if ((long int) *index - distance < 0) {
  WARNING_LOG("Invalid distance, which makes buffer pointer negative: %d, (index: %u, distance: %u)\n", *index - distance, *index, distance);
  return -ZLIB_CORRUPTED_DATA;
 }
 mem_cpy(*dest + *index, *dest + *index - distance, sizeof(unsigned char) * length);
 (*index) += length;
 return ZLIB_NO_ERROR;
}
/// Get length from table defined in the specification.
static int get_length(BitStream* bit_stream, unsigned short int value) {
    const unsigned short int base_values[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
    const unsigned char extra_bits[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    unsigned short int length = base_values[value - 257]; // 257 is the base value of the table, so it's used to normalize the value.
 unsigned char extra = SAFE_BITS_READ(bit_stream, extra, extra_bits[value - 257]); // Some of the entries require to read additional bits.
    return (length + extra);
}
/// Similarly, as above, we perform an analog lookup operation.
static int get_distance(BitStream* bit_stream, unsigned short int value) {
    const unsigned short int base_values[] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
    const unsigned char extra_bits[] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
    unsigned short int distance = base_values[value]; // The distance values start from 0 in the table.
    unsigned short int extra = SAFE_BITS_READ(bit_stream, extra, extra_bits[value]); // Similarly some entries require additional bits to be read.
    return (distance + extra);
}
static int read_uncompressed_data(BitStream* bit_stream, unsigned char** decompressed_data, unsigned int* decompressed_data_length) {
    // Read the length and its one-complement, and check if there's corruption
 skip_to_next_byte(bit_stream);
 unsigned short int length = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(unsigned short int), 1, unsigned short int, length, 0);
 unsigned short int length_c = SAFE_BYTE_READ_WITH_CAST(bit_stream, sizeof(unsigned short int), 1, unsigned short int, length_c, 0);
 XCOMP_BE_CONVERT(&length, sizeof(unsigned short int));
    XCOMP_BE_CONVERT(&length_c, sizeof(unsigned short int));
 unsigned short int check = ((length ^ length_c) + 1) & 0xFFFF;
    if (check) {
  WARNING_LOG("Invalid checksum: ((0x%X ^ 0x%X) + 1 = 0x%X) which is not equal to 0.\n", length, length_c, check);
  return -ZLIB_INVALID_LEN_CHECKSUM;
 } else if (length + bit_stream -> byte_pos > bit_stream -> size) {
  WARNING_LOG("Invalid length: %u would make bitstream pointer read out of bound.\n", length);
  PRINT_BIT_STREAM_INFO(bit_stream);
  return -ZLIB_CORRUPTED_DATA;
 }
 // Read length bytes from the stream
    *decompressed_data = (unsigned char*) xcomp_realloc(*decompressed_data, sizeof(unsigned char) * (*decompressed_data_length + length));
 if (mem_cpy(*decompressed_data + *decompressed_data_length, bitstream_read_bytes(bit_stream, sizeof(unsigned char), length), sizeof(unsigned char) * length) == NULL) {
  WARNING_LOG("Failed to xcomp_reallocate buffer for decompressed data.\n");
  return -ZLIB_IO_ERROR;
 }
 XCOMP_BE_CONVERT(*decompressed_data + *decompressed_data_length, length);
    *decompressed_data_length += length;
 return ZLIB_NO_ERROR;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * The format of the block:
 *  5 Bits: HLIT, # of Literal/Length codes - 257 (257 - 286)
 *  5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
 *  4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
 *
 * (HCLEN + 4) x 3 bits: code lengths for the code length
 * alphabet given just above, in the order: 16, 17, 18,
 *  0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
 *
 * These code lengths are interpreted as 3-bit integers
 * (0-7); as above, a code length of 0 means the
 * corresponding symbol (literal/length or distance code
 * length) is not used.
 *
 * HLIT + 257 code lengths for the literal/length alphabet,
 * encoded using the code length Huffman code
 *
 * HDIST + 1 code lengths for the distance alphabet,
 * encoded using the code length Huffman code
 *
 * The actual compressed data of the block, encoded using the literal/length and distance Huffman codes
 *
 * The literal/length symbol 256 (end of data), encoded using the literal/length Huffman code
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int decode_dynamic_huffman_tables(BitStream* bit_stream, HFTable* literals_hf, HFTable* distance_hf) {
 HFTable decoder_hf = (HFTable) {0}; // Huffman Tree used for decoding the Literals and Distances Huffman Trees
 literals_hf -> size = SAFE_BITS_READ(bit_stream, literals_hf -> size, 5);
    literals_hf -> size += 257;
 distance_hf -> size = SAFE_BITS_READ(bit_stream, distance_hf -> size, 5);
    distance_hf -> size += 1;
 decoder_hf.size = SAFE_BITS_READ(bit_stream, decoder_hf.size, 4);
    decoder_hf.size += 4;
    // Retrieve the length to build the huffman tree to decode the other two huffman trees (Literals and Distance)
    const unsigned char order_of_code_lengths[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    decoder_hf.lengths = (unsigned char*) xcomp_calloc(19, sizeof(unsigned char));
 if (decoder_hf.lengths == NULL) {
  WARNING_LOG("Failed to allocate buffer for decoder_hf lengths.\n");
  return -ZLIB_IO_ERROR;
 }
 // Retrieve the length of each code, using the array to match the fixed order of the codes. Furthermore, each one of the length is 3-bit long.
    for (unsigned char i = 0; i < decoder_hf.size; ++i) {
  (decoder_hf.lengths)[order_of_code_lengths[i]] = SAFE_BITS_READ(bit_stream, (decoder_hf.lengths)[order_of_code_lengths[i]], 3);
 }
 decoder_hf.size = 19; // The real size of the lengths is 19 as the amount allocated, value used for allocation
 // Build the huffman tree from the lengths
    int err = 0;
    if ((err = generate_codes(&decoder_hf)) < 0) {
  deallocate_hf_table(&decoder_hf);
  WARNING_LOG("An error occurred while generating the codes for the decoder_hf table.\n");
  return err;
 }
 // Decode the bit_lengths for both the Huffman Trees
 if ((err = decode_lengths(bit_stream, decoder_hf, literals_hf, distance_hf)) < 0) {
  deallocate_hf_table(&decoder_hf);
  WARNING_LOG("An error occurred while decoding the lengths.\n");
  return err;
 }
    deallocate_hf_table(&decoder_hf);
 // Again this values are required to be set as they are used for the allocation
    if (literals_hf -> size < 286) {
  literals_hf -> lengths = xcomp_realloc(literals_hf -> lengths, sizeof(unsigned char) * 286);
  if (literals_hf -> lengths == NULL) {
   XCOMP_SAFE_FREE(distance_hf -> lengths);
   WARNING_LOG("Failed to xcomp_reallocate the buffer for literals_hf -> lenghts.\n");
   return -ZLIB_IO_ERROR;
  }
  mem_set(literals_hf -> lengths + literals_hf -> size, 0, sizeof(unsigned char) * (286 - literals_hf -> size));
  literals_hf -> size = 286;
 }
 if (distance_hf -> size < 30) {
  distance_hf -> lengths = xcomp_realloc(distance_hf -> lengths, sizeof(unsigned char) * 30);
  if (distance_hf -> lengths == NULL) {
   XCOMP_SAFE_FREE(literals_hf -> lengths);
   WARNING_LOG("Failed to xcomp_reallocate the buffer for distance_hf -> lenghts.\n");
   return -ZLIB_IO_ERROR;
  }
  mem_set(distance_hf -> lengths + distance_hf -> size, 0, sizeof(unsigned char) * (30 - distance_hf -> size));
  distance_hf -> size = 30;
 }
    if ((err = generate_codes(literals_hf)) < 0) {
  XCOMP_SAFE_FREE(distance_hf -> lengths);
  deallocate_hf_table(literals_hf);
  WARNING_LOG("An error occurred while generating the codes for the literals_hf table.\n");
  return err;
 }
 XCOMP_SAFE_FREE(literals_hf -> lengths);
 if ((err = generate_codes(distance_hf)) < 0) {
  do { deallocate_hf_table(literals_hf); deallocate_hf_table(distance_hf); } while (FALSE);
  WARNING_LOG("An error occurred while generating the codes for the distance_hf table.\n");
  return err;
 }
 XCOMP_SAFE_FREE(distance_hf -> lengths);
    return ZLIB_NO_ERROR;
}
static int decode_compressed_block(BType compression_method, BitStream* bit_stream, unsigned char** decompressed_data, unsigned int* decompressed_data_length) {
 int err = 0;
 HFTable literals_hf = (HFTable) {0};
 HFTable distance_hf = (HFTable) {0};
 if (compression_method == COMPRESSED_FIXED_HF) {
  unsigned short int* literals_val_ptr = (unsigned short int*) fixed_val_ptr; (literals_hf) = (HFTable) { .values = &literals_val_ptr, .min_codes = (unsigned short int*) fixed_mins, .max_codes = (unsigned short int*) fixed_maxs, .size = 286, .max_bit_length = 4, .is_fixed_hf = 2 };
  unsigned short int* distance_val_ptr = (unsigned short int*) fixed_distance_val_ptr; (distance_hf) = (HFTable) { .values = &distance_val_ptr, .min_codes = (unsigned short int*) fixed_distance_mins, .max_codes = (unsigned short int*) fixed_distance_maxs, .size = 30, .max_bit_length = 1, .is_fixed_hf = TRUE };
 } else if ((err = decode_dynamic_huffman_tables(bit_stream, &literals_hf, &distance_hf))) {
  WARNING_LOG("An error occurred during dynamic HF table decoding.\n");
  return err;
 }
 // Decode compressed data block
 unsigned short int code = 0;
 int decoded_value = 0;
 int decoded_distance = 0;
 while (!bit_stream -> error) {
  code = bitstream_read_next_bit(bit_stream);
  if ((bit_stream) -> error) { do { deallocate_hf_table((&literals_hf)); deallocate_hf_table((&distance_hf)); } while (FALSE); return -ZLIB_IO_ERROR; };
  // Decode the literal/length value
  if ((decoded_value = decode_hf(bit_stream, code, literals_hf)) < 0) {
   do { deallocate_hf_table(&literals_hf); deallocate_hf_table(&distance_hf); } while (FALSE);
   WARNING_LOG("An error occurred while decoding the code.\n");
   return decoded_value;
  }
  if (decoded_value < 256) {
   // If literal/length value < 256: copy value (literal/length byte) to output stream
   *decompressed_data = (unsigned char*) xcomp_realloc(*decompressed_data, sizeof(unsigned char) * (*decompressed_data_length + 1));
   if (*decompressed_data == NULL) {
    do { deallocate_hf_table(&literals_hf); deallocate_hf_table(&distance_hf); } while (FALSE);
    WARNING_LOG("Failed to xcomp_reallocate buffer for decompressed data.\n");
    return -ZLIB_IO_ERROR;
   }
   (*decompressed_data)[*decompressed_data_length] = decoded_value;
   (*decompressed_data_length)++;
  } else if (decoded_value == 256) {
   break;
  } else {
   unsigned short int length = get_length(bit_stream, decoded_value);
   int distance_code = bitstream_read_next_bit(bit_stream);
   if ((bit_stream) -> error) { do { deallocate_hf_table((&literals_hf)); deallocate_hf_table((&distance_hf)); } while (FALSE); return -ZLIB_IO_ERROR; };
   // Decode the distance value
   if ((decoded_distance = decode_hf(bit_stream, distance_code, distance_hf)) < 0) {
    do { deallocate_hf_table(&literals_hf); deallocate_hf_table(&distance_hf); } while (FALSE);
    WARNING_LOG("An error occurred while decoding the code.\n");
    return decoded_value;
   }
   // Move backwards distance bytes in the output stream, and copy length bytes from this position to the output stream
   unsigned short int distance = get_distance(bit_stream, decoded_distance);
   if ((err = copy_data(decompressed_data, decompressed_data_length, length, distance)) < 0) {
    do { deallocate_hf_table(&literals_hf); deallocate_hf_table(&distance_hf); } while (FALSE);
    WARNING_LOG("An error occurred while copying data from the decoded data.\n");
    return err;
   }
  }
 }
 do { deallocate_hf_table(&literals_hf); deallocate_hf_table(&distance_hf); } while (FALSE);
 return ZLIB_NO_ERROR;
}
unsigned char* zlib_inflate(unsigned char* stream, unsigned int size, unsigned int* decompressed_data_length, int* zlib_err) {
    // Initialize decompressed data
    unsigned char* decompressed_data = (unsigned char*) xcomp_calloc(1, sizeof(unsigned char));
    if (decompressed_data == NULL) {
  XCOMP_SAFE_FREE(stream);
  *zlib_err = -ZLIB_IO_ERROR;
  return ((unsigned char*) "Failed to allocate buffer for decompressed_data.\n");
 }
 BitStream bit_stream = CREATE_BIT_STREAM(stream, size);
 *decompressed_data_length = 0;
    unsigned char final = 0;
 BType compression_method = 0;
 unsigned int block_cnt = 0;
 unsigned int old_decompressed_size = 0;
 while (!final) {
  // Read header bits
  do { final = bitstream_read_next_bit(&bit_stream); if ((&bit_stream) -> error) { (*zlib_err) = -ZLIB_IO_ERROR; return ((unsigned char*) "An error occurred while reading from the bitstream.\n"); } compression_method = bitstream_read_bits((&bit_stream), 2); if ((&bit_stream) -> error) { (*zlib_err) = -ZLIB_IO_ERROR; return ((unsigned char*) "An error occurred while reading from the bitstream.\n"); } } while (FALSE);
  block_cnt++;
  DEBUG_LOG("Block %u: is_final: %u, compression_method: '%s', ", block_cnt, final, btypes_str[compression_method]);
        if (compression_method == NO_COMPRESSION) {
   if ((*zlib_err = read_uncompressed_data(&bit_stream, &decompressed_data, decompressed_data_length)) < 0) {
    XCOMP_MULTI_FREE(stream, decompressed_data);
    return ((unsigned char*) "corrupted compressed block\n");
            }
            continue;
        } else if (compression_method == RESERVED) {
   XCOMP_MULTI_FREE(stream, decompressed_data);
            return ((unsigned char*) "invalid compression type\n");
        }
        // Decode compressed data block
  if ((*zlib_err = decode_compressed_block(compression_method, &bit_stream, &decompressed_data, decompressed_data_length)) < 0) {
   XCOMP_MULTI_FREE(stream, decompressed_data);
   return ((unsigned char*) "An error occurred while decompressing the block.\n");
  }
  printf("decompressed_size: %u\n", *decompressed_data_length - old_decompressed_size);
  old_decompressed_size = *decompressed_data_length;
 }
 *zlib_err = ZLIB_NO_ERROR;
 XCOMP_SAFE_FREE(stream);
    return decompressed_data;
}

#endif /* _XCOMP_H_ */

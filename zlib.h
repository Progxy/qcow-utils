#ifndef _ZLIB_H_
#define _ZLIB_H_

// -----------------
//  Constant Values 
// -----------------
#define HF_LITERALS_SIZE   286
#define HF_DISTANCE_SIZE   30
#define HF_TABLE_SIZE      19

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

#include "./zlib_compress.h"
#include "./zlib_decompress.h"

#endif // _ZLIB_H_

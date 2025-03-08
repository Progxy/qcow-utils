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

#ifndef _QCOW_PARSER_H_
#define _QCOW_PARSER_H_

#include "./utils.h"
#include "./zlib.h"
#include "./zstd.h" // TODO: Note that ZSTD is missing a compressor

/* -------------------------------------------------------------------------------------------------------- */
// -----------------
//  Constant Values
// -----------------
#define QCOW_HEADER_HEADER_START 8
#define DEFAULT_REF_CNT_BITS     16
#define SHARED_FIELDS_SIZE       52
#define QCOW_HEADER2_SIZE        72
#define QCOW_HEADER3_EXT         104 - QCOW_HEADER2_SIZE
#define L2_ENTRY_SIZE            8
#define L2_EXTENDED_ENTRY_SIZE   16

/* -------------------------------------------------------------------------------------------------------- */
// ------------------
//  Macros Functions
// ------------------
#define BE_CONVERT(ptr_val, size) be_to_le(ptr_val, size)
#define IS_COMPRESSED_CLUSTER(img_offset)((img_offset >> 62) & 1) 
#define MASK_BITS_PRECEDING(bit_index) ((1ULL << (bit_index)) - 1)
#define GET_IMAGE_OFFSET(offset) (offset & MASK_BITS_INTERVAL(56, 9))
#define MASK_BITS_INTERVAL(a, b) (((char)(a) >= (char)(b)) ? (MASK_BITS_PRECEDING(a) & ~MASK_BITS_PRECEDING(b)) : 0)
#define FLOORING(dividend, divisor) (((dividend) - ((dividend) % (divisor))) / (divisor))
#define CEILING(dividend, divisor) (((dividend) - ((dividend) % (divisor))) / (divisor) + (((dividend) % (divisor)) > 0)) 
#define IS_CLUSTER_ALIGNED(cluster_offset, img_file_base, cluster_size) ((((cluster_offset) - (img_file_base)) % (cluster_size)) == 0)
#define IMG_OFFSET_INFO(img_offset) DEBUG_LOG("0x%lX: img_offset: 0x%llX, is_compressed_cluster: '%s'.\n", img_offset, img_offset & MASK_BITS_INTERVAL(56, 9), BOOL2STR(IS_COMPRESSED_CLUSTER(img_offset))) 

/* -------------------------------------------------------------------------------------------------------- */
// -------
//  Enums
// -------
typedef enum PACKED_STRUCT QCowErrors { 
    QCOW_NO_ERROR = 0, 
    QCOW_IO_ERROR, 
    QCOW_INVALID_OFFSET, 
    QCOW_UNALLOCATED_CLUSTER, 
    QCOW_DEFLATE_ERROR, 
    QCOW_INVALID_MAGIC, 
    QCOW_INVALID_VERSION, 
    QCOW_CLUSTER_BITS_TOO_SMALL, 
    QCOW_INVALID_COMPRESSION_TYPE, 
    QCOW_USE_OF_RESERVED_FIELD, 
    QCOW_UNALLOCATED_L1_TABLE, 
    QCOW_CORRUPTED_IMAGE, 
    QCOW_INVALID_REF_CNT_ORDER, 
    QCOW_MISSING_COMPRESSION_TYPE_FIELD, 
	QCOW_EMPTY_BACKING_FILE,	
    QCOW_INVALID_BACKING_FILE_OFFSET,
	QCOW_UNINITIALIZED_ERDF,
	QCOW_INVALID_SUBCLUSTER_BITMAP,
	QCOW_UNALIGNED_CLUSTER,
	QCOW_TODO 
} QCowErrors;

static const char* qcow_errors_str[] = { 
    "QCOW_NO_ERROR", 
    "QCOW_IO_ERROR", 
    "QCOW_INVALID_OFFSET", 
    "QCOW_UNALLOCATED_CLUSTER", 
    "QCOW_DEFLATE_ERROR", 
    "QCOW_INVALID_MAGIC", 
    "QCOW_INVALID_VERSION", 
    "QCOW_CLUSTER_BITS_TOO_SMALL", 
    "QCOW_INVALID_COMPRESSION_TYPE", 
    "QCOW_USE_OF_RESERVED_FIELD", 
    "QCOW_UNALLOCATED_L1_TABLE", 
    "QCOW_CORRUPTED_IMAGE", 
    "QCOW_INVALID_REF_CNT_ORDER", 
    "QCOW_MISSING_COMPRESSION_TYPE_FIELD", 
	"QCOW_EMPTY_BACKING_FILE",	
    "QCOW_INVALID_BACKING_FILE_OFFSET",
	"QCOW_UNINITIALIZED_ERDF",
	"QCOW_INVALID_SUBCLUSTER_BITMAP",
	"QCOW_UNALIGNED_CLUSTER",
    "QCOW_TODO" 
};

typedef enum PACKED_STRUCT QCowExtType {
    HEADER_EXT_END = 0,
    BACKING_FILE_NAME,
    FEATURE_NAME_TABLE,
    BITMAPS_EXTENSION,
    ENCRYPTION_HEADER,
    EXTERNAL_FILE_NAME,
    UNKNOWN_EXTENSION 
} QCowExtType;
static const char* qcow_ext_types_strs[] = { "HEADER_EXT_END", "BACKING_FILE_NAME", "FEATURE_NAME_TABLE", "BITMAPS_EXTENSION", "ENCRYPTION_HEADER", "EXTERNAL_FILE_NAME", "UNKNOWN_EXTENSION" };

/* deflate <https://www.ietf.org/rfc/rfc1951.txt> */
/* zstd <http://github.com/facebook/zstd> */
typedef enum PACKED_STRUCT CompressionType {DEFLATE = 0, ZSTD = 1} CompressionType;
static const char* compression_type_str[] = { "DEFLATE", "ZSTD" };

/* -------------------------------------------------------------------------------------------------------- */
// ---------
//  Structs
// ---------
typedef struct PACKED_STRUCT QCowHeader {
    char magic[4];                    // 0 - 3: QCOW magic string ("QFI\xfb")
    uint32_t version;                 // 4 - 7: Version number (valid values are 2 and 3)
    uint64_t backing_file_offset;     // 8 - 15: Offset to the backing file name
    uint32_t backing_file_name_size;  // 16 - 19: Length of the backing file name
    uint32_t cluster_bits;            // 20 - 23: Bits for addressing within a cluster
    uint64_t size;                    // 24 - 31: Virtual disk size in bytes
    uint32_t crypt_method;            // 32 - 35: 0 for no encryption, 1 for AES encryption
    uint32_t l1_size;                 // 36 - 39: Number of entries in the active L1 table
    uint64_t l1_table_offset;         // 40 - 47: Offset of the active L1 table
    uint64_t refcount_table_offset;   // 48 - 55: Offset of the refcount table
    uint32_t refcount_table_clusters; // 56 - 59: Clusters occupied by the refcount table
    uint32_t nb_snapshots;            // 60 - 63: Number of snapshots in the image
    uint64_t snapshots_offset;        // 64 - 71: Offset of the snapshot table
	uint64_t incompatible_features;   // Bytes 72 - 79
    uint64_t compatible_features;     // Bytes 80 - 87
    uint64_t autoclear_features;      // Bytes 88 - 95
    uint32_t refcount_order;          // Bytes 96 - 99
    uint32_t header_length;           // Bytes 100 - 103
} QCowHeader;

typedef struct PACKED_STRUCT QCowHeaderExtension {
	QCowExtType ext_type;
	uint32_t ext_length;
	uint8_t* data;
} QCowHeaderExtension;

typedef struct PACKED_STRUCT QCowMetadata {
    CompressionType compression_type;
    uint64_t backing_file_offset;
    uint32_t backing_file_name_size; 
	uint32_t cluster_bits;
    uint64_t size;
    uint32_t crypt_method; 
	uint32_t l1_size;
	uint64_t l1_table_offset;
	uint64_t refcount_table_offset;
	uint32_t refcount_table_clusters; 
	uint64_t cluster_size;
	uint32_t refcount_block_entries;
	uint32_t table_cluster_entries;
	uint32_t refcount_table_size;
	uint8_t refcount_bytes;
	void** refcount_table;
	void** l1_table;
	uint8_t l2_entries_size;
	FILE* img_file;
	long long int img_size;
	long long int img_file_base;
	FILE* backing_file;
	long long int backing_file_size;
	FILE* clusters_file;
	long long int clusters_file_size;
	long long int clusters_file_base;
	uint8_t use_erdf;
	uint8_t use_extended_l2_entries;
} QCowMetadata;

typedef struct PACKED_STRUCT SubclusterInfo {
	uint32_t alloc_status;
	uint32_t reads_as_zero;
} SubclusterInfo;

/* -------------------------------------------------------------------------------------------------------- */
// ------------------------
//  Functions Declarations
// ------------------------

// TODO: Add support for ZSTD compression.
// TODO: Add support for snapshots: it requires the nb_snapshots and the offset of the table, also requires a bit of rethinking of the entire system, as it needs to parse the entries in the snapshot table,
// 		 somehow expose them to the end user, and also once choosen to activate its l1_table, sort of initializing the entire qcow metadata structure.
// TODO: Add support for bitmaps: requires extension header and the check of the autoclear field (bit 0).
// TODO: Add support for crypt method: it requires checking for the crypt method used, and also the extension header for informations.

// TODO: After implementing most of the aforementioned support the code needs to be tidied up.
// TODO: Rewrite the error messages for more clarity, and also add comments to better explain how everything works.
static inline QCowExtType to_qcow_ext_type(uint32_t val);
static inline int write_at(FILE* file, uint64_t offset, const void* data, size_t size, size_t nmemb);
static inline int read_at(FILE* file, uint64_t offset, void* data, size_t size, size_t nmemb);
static inline int zero_out_at(FILE* file, uint64_t offset, uint64_t n);
static inline void deallocate_qcow_metadata(QCowMetadata* qcow_metadata);
static inline void format_qcow_header(QCowHeader* qcow_header, unsigned char version);
static inline void dump_qcow_header(const QCowHeader* qcow_header);
static inline void dump_qcow_header_extension(const QCowHeaderExtension* qcow_header_ext);
static int parse_qcow_ext(QCowHeaderExtension** qcow_exts, FILE* file);
static int parse_ref_cnt_table(QCowMetadata* qcow_metadata);
static int parse_l1_table(QCowMetadata* qcow_metadata);
static int parse_qcow_header(QCowMetadata* qcow_metadata, QCowHeader* qcow_header);
static int init_qcow_img(QCowMetadata* qcow_metadata, const char* path_qcow);
static int parse_qcow(QCowMetadata* qcow_metadata, const char* path_qcow);
static inline int get_ref_cnt(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* ref_cnt);
static int allocate_ref_cnt_table(QCowMetadata qcow_metadata, uint64_t refcount_table_index);
static int update_ref_cnt(QCowMetadata qcow_metadata, uint64_t offset, uint64_t new_ref_cnt);
static inline int lba_to_img_offset(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* img_offset, SubclusterInfo* subcluster_info);
static int allocate_l2_table(QCowMetadata qcow_metadata, uint64_t l1_index);
static int set_lba_at_img_offset(QCowMetadata qcow_metadata, uint64_t offset, uint64_t new_entry, SubclusterInfo new_subcluster_info);
static int extend_img_file(FILE* file, uint64_t n, uint64_t file_boundary_base, uint64_t boundary, uint64_t* end_pos);
static inline int find_unallocated_cluster(QCowMetadata qcow_metadata, uint64_t* offset);
static int alloc_cluster(QCowMetadata qcow_metadata, uint64_t* cluster_offset);
static int cow_alloc_cluster(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* cluster_offset);
static int write_compressed_cluster(QCowMetadata qcow_metadata, uint64_t img_offset, unsigned int* recompressed_cluster_size, unsigned int compressed_cluster_size, unsigned char* cluster, unsigned int cluster_data_size);
static int read_compressed_cluster(QCowMetadata qcow_metadata, FILE* file, uint64_t* cluster_offset, unsigned char** clusters, unsigned int *cluster_data_size, unsigned int* compressed_clusters_size);
static int get_lba_img_offset_for_write(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* img_offset, SubclusterInfo* subcluster_info);
int qwrite(const void* data, unsigned int size, size_t nmemb, unsigned int offset, QCowMetadata qcow_metadata) ;
int qread(void* ptr, size_t size, size_t nmemb, unsigned int offset, QCowMetadata qcow_metadata);

/* -------------------------------------------------------------------------------------------------------- */

static inline QCowExtType to_qcow_ext_type(uint32_t val) {
    const uint32_t qcow_ext_types[] = { 0x00000000, 0xE2792ACA, 0x6803F857, 0x23852875, 0x0537BE77, 0x44415441, 0x00000001 };
    for (int i = 0; i < (int) ARR_SIZE(qcow_ext_types); ++i) {
        if (val == qcow_ext_types[i]) return ((QCowExtType) i);
    }
    return UNKNOWN_EXTENSION;
}

static inline int write_at(FILE* file, uint64_t offset, const void* data, size_t size, size_t nmemb) {
	int ret = fseek(file, offset, SEEK_SET);							
	if (ret < 0) {															
		WARNING_LOG("Failed to seek at pos: 0x%lX\n", offset);			
		return -QCOW_IO_ERROR;												
	}							

	if (fwrite(data, size, nmemb, file) != nmemb) { 	
		PERROR_LOG("Failed to write %lu bytes at pos 0x%lX", size * nmemb, offset);											
		return -QCOW_IO_ERROR;												
	}

	return QCOW_NO_ERROR;																	
}

static inline int read_at(FILE* file, uint64_t offset, void* data, size_t size, size_t nmemb) {
	int ret = fseek(file, offset, SEEK_SET);							
	if (ret < 0) {															
		WARNING_LOG("Failed to seek at pos: 0x%lX\n", offset);			
		return -QCOW_IO_ERROR;												
	}								

	if (fread(data, size, nmemb, file) != nmemb) { 	
		PERROR_LOG("Failed to read %lu bytes at pos 0x%lX", size * nmemb, offset);											
		return -QCOW_IO_ERROR;												
	}			
	
	return QCOW_NO_ERROR;
}

static inline int zero_out_at(FILE* file, uint64_t offset, uint64_t n) {
	if (offset) {													
		int ret = fseek(file, offset, SEEK_SET);					
		if (ret < 0) {												
			WARNING_LOG("Failed to seek at pos: 0x%lX\n", offset);	
			return -QCOW_IO_ERROR;									
		}															
	}									

	unsigned char zero[] = {0};										
	for (unsigned int i = 0; i < n; ++i) {							
		if (fwrite(zero, sizeof(unsigned char), 1, file) != 1) {	
			PERROR_LOG("Failed to write zero at pos 0x%lX", offset + i);								
			return -QCOW_IO_ERROR;									
		}															
	}					

	return QCOW_NO_ERROR;
}

static inline long long int fsize(FILE* file) {
	int ret = fseek(file, 0, SEEK_END);
	if (ret < 0) {
		PERROR_LOG("Failed to seek to the end of the file.\n");
		return -QCOW_IO_ERROR;
	}

	long long int pos = ftell(file);
	if (pos < 0) {
		PERROR_LOG("Failed to read the current pos.\n");
		return -QCOW_IO_ERROR;
	}

	if ((ret = fseek(file, 0, SEEK_SET)) < 0) {
		PERROR_LOG("Failed to seek to the start of the file.\n");
		return -QCOW_IO_ERROR;
	}

	return pos;
}

static inline void deallocate_qcow_metadata(QCowMetadata* qcow_metadata) {
	for (unsigned int i = 0; i < qcow_metadata -> refcount_table_size; ++i) SAFE_FREE(qcow_metadata -> refcount_table[i]);
	SAFE_FREE(qcow_metadata -> refcount_table);
	
	for (unsigned int i = 0; i < qcow_metadata -> l1_size; ++i) SAFE_FREE(qcow_metadata -> l1_table[i]);
	SAFE_FREE(qcow_metadata -> l1_table);
	
	if (qcow_metadata -> img_file != qcow_metadata -> clusters_file) fclose(qcow_metadata -> clusters_file);
	qcow_metadata -> clusters_file = NULL;

	if (qcow_metadata -> img_file) fclose(qcow_metadata -> img_file);
   	qcow_metadata -> img_file = NULL;

	if (qcow_metadata -> backing_file) fclose(qcow_metadata -> backing_file);
	qcow_metadata -> backing_file = NULL;

	return;
}

static inline void format_qcow_header(QCowHeader* qcow_header, unsigned char version) {
	if (version == 2) {
		BE_CONVERT(&qcow_header -> version, sizeof(qcow_header -> version));
		BE_CONVERT(&qcow_header -> backing_file_offset, sizeof(qcow_header -> backing_file_offset));
		BE_CONVERT(&qcow_header -> backing_file_name_size, sizeof(qcow_header -> backing_file_name_size));
		BE_CONVERT(&qcow_header -> cluster_bits, sizeof(qcow_header -> cluster_bits));
		BE_CONVERT(&qcow_header -> size, sizeof(qcow_header -> size));
		BE_CONVERT(&qcow_header -> crypt_method, sizeof(qcow_header -> crypt_method));
		BE_CONVERT(&qcow_header -> l1_size, sizeof(qcow_header -> l1_size));
		BE_CONVERT(&qcow_header -> l1_table_offset, sizeof(qcow_header -> l1_table_offset));
		BE_CONVERT(&qcow_header -> refcount_table_offset, sizeof(qcow_header -> refcount_table_offset));
		BE_CONVERT(&qcow_header -> refcount_table_clusters, sizeof(qcow_header -> refcount_table_clusters));
		BE_CONVERT(&qcow_header -> nb_snapshots, sizeof(qcow_header -> nb_snapshots));
		BE_CONVERT(&qcow_header -> snapshots_offset, sizeof(qcow_header -> snapshots_offset));
	} else if (version == 3) {
		BE_CONVERT(&qcow_header -> incompatible_features, sizeof(qcow_header -> incompatible_features));
		BE_CONVERT(&qcow_header -> compatible_features, sizeof(qcow_header -> compatible_features));
		BE_CONVERT(&qcow_header -> autoclear_features, sizeof(qcow_header -> autoclear_features));
		BE_CONVERT(&qcow_header -> refcount_order, sizeof(qcow_header -> refcount_order));
		BE_CONVERT(&qcow_header -> header_length, sizeof(qcow_header -> header_length));
	}
	return;
}

static inline void dump_qcow_header(const QCowHeader* qcow_header) {
    printf(" -- QCowHeader Dump --\n");
    printf(" %-25s: '%.3s'\n", "magic", qcow_header -> magic);
    printf(" %-25s: %u\n", "version", qcow_header -> version);
    printf(" %-25s: 0x%lX\n", "backing_file_offset", qcow_header -> backing_file_offset);
    printf(" %-25s: %u\n", "backing_file_name_size", qcow_header -> backing_file_name_size);
    printf(" %-25s: %u - cluster size: %u bytes\n", "cluster_bits", qcow_header -> cluster_bits, 1 << qcow_header -> cluster_bits);
	printf(" %-25s: %.2LfGB\n", "size", qcow_header -> size / (1024.0L * 1024.0L * 1024.0L));
    printf(" %-25s: %s\n", "crypt_method", qcow_header -> crypt_method ? "AES_ENCRYPTION" : "NO_ENCRYPTION");
    printf(" %-25s: %u\n", "l1_size", qcow_header -> l1_size);
    printf(" %-25s: 0x%lX\n", "l1_table_offset", qcow_header -> l1_table_offset);
    printf(" %-25s: 0x%lX\n", "refcount_table_offset", qcow_header -> refcount_table_offset);
    printf(" %-25s: %u\n", "refcount_table_clusters", qcow_header -> refcount_table_clusters);
    printf(" %-25s: %u\n", "nb_snapshots", qcow_header -> nb_snapshots);
    printf(" %-25s: 0x%lX\n", "snapshots_offset", qcow_header -> snapshots_offset);
	printf(" %-25s: 0x%lX\n", "incompatible_features", qcow_header->incompatible_features);
    printf(" %-25s: 0x%lX\n", "compatible_features", qcow_header->compatible_features);
    printf(" %-25s: 0x%lX\n", "autoclear_features", qcow_header->autoclear_features);
    printf(" %-25s: %u\n", "refcount_order", qcow_header->refcount_order);
    printf(" %-25s: %u\n", "header_length", qcow_header->header_length);
    printf(" ---------------------\n");
	return;
}

static inline void dump_qcow_header_extension(const QCowHeaderExtension* qcow_header_ext) {
    if (!qcow_header_ext) {
        WARNING_LOG("Error: QCowHeaderExtension is NULL.\n");
        return;
    } 
    
	printf(" -- QCowHeaderExtension Dump --\n");
    
	printf("- %s\n", qcow_ext_types_strs[qcow_header_ext -> ext_type]);
	printf(" %-25s: %u\n", "ext_length", qcow_header_ext -> ext_length);

    if (qcow_header_ext -> data) {
        printf(" %-25s: [Data Start]\n", "data");
        for (uint32_t i = 0; i < qcow_header_ext -> ext_length; ++i) {
            printf("  %02X", (qcow_header_ext -> data)[i]);
            if ((i + 1) % 16 == 0 || i + 1 == qcow_header_ext -> ext_length) {
                printf("\n");
            }
        }
    } else {
        printf(" %-25s: NULL\n", "data");
    }

    printf(" -------------------------------\n");

    return;
}

static int parse_qcow_ext(QCowHeaderExtension** qcow_exts, FILE* file) {
	int exts_cnt = 0;
	while (TRUE) {
		uint32_t ext_type = 0;
		if (fread(&ext_type, sizeof(uint32_t), 1, file) != 1) {
			for (int i = 0; i < exts_cnt; ++i) SAFE_FREE((*qcow_exts) -> data); 
			SAFE_FREE(*qcow_exts);
			PERROR_LOG("An error occurred while reading the extension type");
			return -QCOW_IO_ERROR;
		}
		
		BE_CONVERT(&ext_type, sizeof(uint32_t));	
		
		QCowHeaderExtension qcow_ext_header = {0};
		qcow_ext_header.ext_type = to_qcow_ext_type(ext_type);

		if (fread(&(qcow_ext_header.ext_length), sizeof(uint32_t), 1, file) != 1) {
			for (int i = 0; i < exts_cnt; ++i) SAFE_FREE((*qcow_exts) -> data); 
			SAFE_FREE(*qcow_exts);
			PERROR_LOG("An error occurred while reading the extension length");
			return -QCOW_IO_ERROR;
		}

		BE_CONVERT(&(qcow_ext_header.ext_length), sizeof(uint32_t));	
		
		if (qcow_ext_header.ext_length) {
			qcow_ext_header.data = (unsigned char*) calloc(qcow_ext_header.ext_length, sizeof(unsigned char));
			if (qcow_ext_header.data == NULL) {
				for (int i = 0; i < exts_cnt; ++i) SAFE_FREE((*qcow_exts) -> data); 
				SAFE_FREE(*qcow_exts);
				WARNING_LOG("Failed to allocate buffer for extension header data.\n");
				return -QCOW_IO_ERROR;
			}
			
			if (fread(qcow_ext_header.data, sizeof(unsigned char), qcow_ext_header.ext_length, file) != qcow_ext_header.ext_length) {
				for (int i = 0; i < exts_cnt; ++i) SAFE_FREE((*qcow_exts) -> data); 
				SAFE_FREE(*qcow_exts);
				PERROR_LOG("An error occurred while reading the data");
				return -QCOW_IO_ERROR;
			}

			// Padding zero-data to set the current pos to the next multiple of 8
			uint64_t padding = 0;
			unsigned char padding_size = 8 - (qcow_ext_header.ext_length % 8);
			if (padding_size < 8 && fread(&padding, sizeof(unsigned char), padding_size, file) != padding_size) {
				for (int i = 0; i < exts_cnt; ++i) SAFE_FREE((*qcow_exts) -> data); 
				SAFE_FREE(*qcow_exts);
				PERROR_LOG("Failed to read the padding with size %u bytes", padding_size);
				return -QCOW_IO_ERROR;
			}

			if (padding) {
				for (int i = 0; i < exts_cnt; ++i) SAFE_FREE((*qcow_exts) -> data); 
				SAFE_FREE(*qcow_exts);
				WARNING_LOG("Padding field must be zero, but found: %lu\n", padding);
				return -QCOW_USE_OF_RESERVED_FIELD;
			}
		}
	
		DEBUG_LOG("exts_cnt: %d, ext_type: '%s', len: %u\n", exts_cnt + 1, qcow_ext_types_strs[qcow_ext_header.ext_type], qcow_ext_header.ext_length);

		if (qcow_ext_header.ext_type == HEADER_EXT_END) {
			SAFE_FREE(qcow_ext_header.data);
			break;
		} else if (qcow_ext_header.ext_type == FEATURE_NAME_TABLE || qcow_ext_header.ext_type == UNKNOWN_EXTENSION) {
			SAFE_FREE(qcow_ext_header.data);
			continue;
		}

		*qcow_exts = realloc(*qcow_exts, sizeof(QCowHeaderExtension) * (exts_cnt + 1));
		if (*qcow_exts == NULL) {
			WARNING_LOG("Failed to reallocate the buffer for qcow_exts.\n");
			return -QCOW_IO_ERROR;
		}

		(*qcow_exts)[exts_cnt++] = qcow_ext_header;
	}

	return exts_cnt;
}

static int parse_ref_cnt_table(QCowMetadata* qcow_metadata) {
	qcow_metadata -> refcount_table = calloc(qcow_metadata -> refcount_table_size, sizeof(void*));
	if (qcow_metadata -> refcount_table == NULL) {
		WARNING_LOG("Failed to allocate refcount table.\n");
		return -QCOW_IO_ERROR;
	}

	int ret = 0;
	for (unsigned int refcnt_block = 0; refcnt_block < qcow_metadata -> refcount_table_size; ++refcnt_block) {
		uint64_t refcount_block_offset = 0;
		uint64_t offset = qcow_metadata -> refcount_table_offset + refcnt_block * sizeof(uint64_t);
		if ((ret = read_at(qcow_metadata -> img_file, offset, &refcount_block_offset, sizeof(uint64_t), 1)) < 0) {
			WARNING_LOG("Failed to read the offset.\n");
			return ret;
		}

		BE_CONVERT((unsigned char*) &refcount_block_offset, sizeof(uint64_t));

		// The refcount table and its clusters are unallocated
		if (refcount_block_offset == 0) continue; 
		else if (refcount_block_offset & MASK_BITS_INTERVAL(9, 0)) {
			WARNING_LOG("Reserved bits set in refcount_block_offset: 0x%lX\n", refcount_block_offset);
			return -QCOW_USE_OF_RESERVED_FIELD;
		} else if (!IS_CLUSTER_ALIGNED(refcount_block_offset, qcow_metadata -> img_file_base, qcow_metadata -> cluster_size)) {
			WARNING_LOG("The table must be aligned to a cluster boundary.\n");
			return -QCOW_UNALIGNED_CLUSTER;
		}
	
		(qcow_metadata -> refcount_table)[refcnt_block] = calloc(qcow_metadata -> refcount_block_entries, qcow_metadata -> refcount_bytes);
		if ((qcow_metadata -> refcount_table)[refcnt_block] == NULL) {
			WARNING_LOG("Failed to allocate %u refcount block.\n", refcnt_block);
			return -QCOW_IO_ERROR;
		}	

		ret = fseek(qcow_metadata -> img_file, refcount_block_offset, SEEK_SET);
		if (ret < 0) {
			PERROR_LOG("Failed to seek at pos: 0x%lX.\n", refcount_block_offset);
			return -QCOW_IO_ERROR;
		}

		for (unsigned int i = 0; i < qcow_metadata -> refcount_block_entries; ++i) {
			if (fread(CAST_PTR((qcow_metadata -> refcount_table)[refcnt_block], unsigned char) + i * qcow_metadata -> refcount_bytes, qcow_metadata -> refcount_bytes, 1, qcow_metadata -> img_file) != 1) {
				PERROR_LOG("Failed to read the refcount");
				return -QCOW_IO_ERROR;
			}
			BE_CONVERT(CAST_PTR((qcow_metadata -> refcount_table)[refcnt_block], unsigned char) + i * qcow_metadata -> refcount_bytes, qcow_metadata -> refcount_bytes);
		}
	}

	return QCOW_NO_ERROR;
}

static int parse_l1_table(QCowMetadata* qcow_metadata) {
	qcow_metadata -> l1_table = (void**) calloc(qcow_metadata -> l1_size, sizeof(void*));
	if (qcow_metadata -> l1_table == NULL) {
		WARNING_LOG("Failed to allocate l1 table.\n");
		return -QCOW_IO_ERROR;
	}

	int ret = 0;
	for (unsigned int l2_entry = 0; l2_entry < qcow_metadata -> l1_size; ++l2_entry) {
		uint64_t l2_offset = 0;
		uint64_t offset = qcow_metadata -> l1_table_offset + l2_entry * sizeof(uint64_t);
		if ((ret = read_at(qcow_metadata -> img_file, offset, &l2_offset, sizeof(uint64_t), 1)) < 0) {
			WARNING_LOG("Failed to read the offset.\n");
			return ret;
		}
		
		BE_CONVERT((unsigned char*) &l2_offset, sizeof(uint64_t));
		
		// The l2 table and its clusters are unallocated
		if (l2_offset == 0 || (l2_offset & MASK_BITS_INTERVAL(56, 9)) == 0) continue; 
		else if ((l2_offset & MASK_BITS_INTERVAL(9, 0)) || (l2_offset & MASK_BITS_INTERVAL(63, 52))) {
			WARNING_LOG("Reserved bits set in l1_entry: 0x%lX.\n", l2_offset);
			return -QCOW_USE_OF_RESERVED_FIELD;
		} else if (!IS_CLUSTER_ALIGNED(l2_offset, qcow_metadata -> img_file_base, qcow_metadata -> cluster_size)) {
			WARNING_LOG("The table must be aligned to a cluster boundary.\n");
			return -QCOW_UNALIGNED_CLUSTER;
		}
		
		ret = fseek(qcow_metadata -> img_file, l2_offset & MASK_BITS_INTERVAL(56, 9), SEEK_SET);
		if (ret < 0) {
			PERROR_LOG("Failed to read the l2 table offset");
			return -QCOW_IO_ERROR;
		}

		(qcow_metadata -> l1_table)[l2_entry] = calloc(qcow_metadata -> table_cluster_entries, qcow_metadata -> l2_entries_size);
		if ((qcow_metadata -> l1_table)[l2_entry] == NULL) {
			WARNING_LOG("Failed to allocate %u l2 table.\n", l2_entry);
			return -QCOW_IO_ERROR;
		}	

		for (unsigned int i = 0; i < qcow_metadata -> table_cluster_entries; ++i) {
			if (fread(CAST_PTR((qcow_metadata -> l1_table)[l2_entry], unsigned char) + i * qcow_metadata -> l2_entries_size, qcow_metadata -> l2_entries_size, 1, qcow_metadata -> img_file) != 1) {
				PERROR_LOG("Failed to read the l2 table entry");
				return -QCOW_IO_ERROR;
			}
			BE_CONVERT(CAST_PTR((qcow_metadata -> l1_table)[l2_entry], unsigned char) + i * qcow_metadata -> l2_entries_size, qcow_metadata -> l2_entries_size);
		}
	}

	return QCOW_NO_ERROR;
}

static int deallocate_cluster(QCowMetadata* qcow_metadata, uint64_t cluster_offset) {
	long long int file_size = 0;
	if ((file_size = fsize(qcow_metadata -> img_file)) < 0) {
		WARNING_LOG("Failed to get the size of the image file.\n");
		return file_size;		
	}
	
	unsigned int clusters_to_copy = FLOORING(file_size - cluster_offset, qcow_metadata -> cluster_size);
	uint8_t* temp_buffer = (uint8_t*) calloc(qcow_metadata -> cluster_size, sizeof(uint8_t));
	if (temp_buffer == NULL) {
		WARNING_LOG("Failed to allocate the temp_buffer.\n");
		return -QCOW_IO_ERROR;
	}
	
	int err = 0;
	for (unsigned int i = 0; i < clusters_to_copy; ++i) { 
		uint64_t copy_size = MIN(qcow_metadata -> cluster_size, file_size - (cluster_offset + i * qcow_metadata -> cluster_size));
		if ((err = read_at(qcow_metadata -> img_file, cluster_offset + (i + 1) * qcow_metadata -> cluster_size, temp_buffer, sizeof(uint8_t), copy_size)) < 0) {
			SAFE_FREE(temp_buffer);
			WARNING_LOG("Failed to read from the image file.\n");
			return err;
		}

		if ((err = write_at(qcow_metadata -> img_file, cluster_offset + i * qcow_metadata -> cluster_size, temp_buffer, sizeof(uint8_t), copy_size)) < 0) {
			SAFE_FREE(temp_buffer);
			WARNING_LOG("Failed to copy back to the image file.\n");
			return err;
		}
	}

	SAFE_FREE(temp_buffer);
	
	if ((qcow_metadata -> img_size = fsize(qcow_metadata -> img_file)) < 0) {
		WARNING_LOG("Failed to get the size of the image file.\n");
		return qcow_metadata -> img_size;		
	}

	return QCOW_NO_ERROR;
}

static int recompute_ref_cnt(QCowMetadata* qcow_metadata) {
	// Deallocate all the ref_cnt tables
	int err = 0;
	for (unsigned int i = 0; i < qcow_metadata -> refcount_table_size; ++i) {
		if (qcow_metadata -> refcount_table[i] == NULL) continue;
		
		uint64_t ref_cnt_table_offset = 0;
		if ((err = read_at(qcow_metadata -> img_file, qcow_metadata -> refcount_table_offset + i * sizeof(uint64_t), &ref_cnt_table_offset, sizeof(uint64_t), 1)) < 0) {
			WARNING_LOG("Failed to read the refcnt_table offset.\n");
			return err;
		}

		if ((err = deallocate_cluster(qcow_metadata, ref_cnt_table_offset)) < 0) {
			WARNING_LOG("Failed to deallocate the cluster containing the ref_cnt table.\n");
			return err;
		}

		uint64_t zero_offset = 0;
		if ((err = write_at(qcow_metadata -> img_file, qcow_metadata -> refcount_table_offset + i * sizeof(uint64_t), &zero_offset, sizeof(uint64_t), 1)) < 0) {
			WARNING_LOG("Failed to update the refcnt_table offset.\n");
			return err;
		}
		
		SAFE_FREE((qcow_metadata -> refcount_table)[i]);
	}

	// Allocate the ref_cnt tables by walking through the l2_entries
	for (unsigned int l2_entry = 0; l2_entry < qcow_metadata -> l1_size; ++l2_entry) {
		if ((qcow_metadata -> l1_table)[l2_entry] == NULL) continue;
		for (unsigned int j = 0; j < qcow_metadata -> table_cluster_entries; ++j) {
			uint64_t cluster_offset = 0;
			mem_cpy(&cluster_offset, CAST_PTR((qcow_metadata -> l1_table)[l2_entry], unsigned char) + j * qcow_metadata -> l2_entries_size, sizeof(uint64_t));
			if ((cluster_offset & MASK_BITS_INTERVAL(56, 9)) == 0 && (((cluster_offset >> 63) & 1) == 0 || qcow_metadata -> backing_file == NULL)) {
				uint64_t offset = j + (l2_entry * qcow_metadata -> cluster_size * qcow_metadata -> table_cluster_entries);
				if ((err = update_ref_cnt(*qcow_metadata, offset, 1)) < 0) {
					WARNING_LOG("Failed to update the ref cnt.\n");
					return err;
				}
			}
		}
	}

	return QCOW_NO_ERROR;
}

static inline int check_version_three_features(QCowHeader* qcow_header, QCowMetadata* qcow_metadata) {
	if (qcow_header -> incompatible_features & MASK_BITS_INTERVAL(63, 5)) {
		WARNING_LOG("Use of reserved field in incompatible_features.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} else if ((qcow_header -> incompatible_features >> 1) & 1) {
		WARNING_LOG("Corrupted image.\n");
		return -QCOW_CORRUPTED_IMAGE;
	} else if (qcow_header -> compatible_features & MASK_BITS_INTERVAL(63, 1)) {
		WARNING_LOG("Use of reserved field in compatible_features.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} else if (qcow_header -> autoclear_features & MASK_BITS_INTERVAL(63, 2)) {
		WARNING_LOG("Use of reserved field in autoclear_features.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} else if (qcow_header -> refcount_order > 6) {
		WARNING_LOG("Refcount order must be less than or equal to 6, but found: %u.\n", qcow_header -> refcount_order);
		return -QCOW_INVALID_REF_CNT_ORDER;
	} else if (qcow_header -> header_length < 104 || qcow_header -> header_length % 8) {
		WARNING_LOG("Header length must be greater than or equal to 104 and must also be a multiple of 8, but found: %u.\n", qcow_header -> header_length);
		return -QCOW_CORRUPTED_IMAGE;
	} else if (!qcow_metadata -> use_erdf && ((qcow_header -> autoclear_features >> 1) & 1)) {
		WARNING_LOG("Cannot set the consistent raw external data file, when the raw external data file is not used.\n");
		return -QCOW_CORRUPTED_IMAGE;
	} else if (qcow_header -> backing_file_offset && qcow_header -> backing_file_name_size && qcow_metadata -> use_erdf) {
		WARNING_LOG("Backing file and raw external data files cannot coexist.\n");
		return -QCOW_CORRUPTED_IMAGE;
	}
	
	int err = 0;
	uint64_t compression_type_field = 0;
	unsigned char compression_type_flag = (qcow_header -> incompatible_features >> 3) & 1;
	if (qcow_header -> header_length == 104 && compression_type_flag) {
		WARNING_LOG("Missing compression type field.\n");
		return -QCOW_MISSING_COMPRESSION_TYPE_FIELD;
	} else if ((err = read_at(qcow_metadata -> img_file, 104, &compression_type_field, sizeof(uint64_t), 1)) < 0) {
		WARNING_LOG("Failed to read the compression type field.\n");
		return err;
	}
	
	qcow_metadata -> compression_type = CAST_PTR(&compression_type_field, unsigned char)[0];

	if (compression_type_field & MASK_BITS_INTERVAL(63, 1)) {
		WARNING_LOG("Use of reserved field/padding in compression type field.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} else if (compression_type_flag && qcow_metadata -> compression_type == 0) {
		WARNING_LOG("Expected a non-zero value for compression type.\n");
		return -QCOW_INVALID_COMPRESSION_TYPE;
	} else if (!compression_type_flag && qcow_metadata -> compression_type > 0) {
		WARNING_LOG("Expected non-present or zero compression type, but found: %u.\n", qcow_metadata -> compression_type);
		return -QCOW_INVALID_COMPRESSION_TYPE;
	}
	
	return QCOW_NO_ERROR;
}

static int parse_qcow_header(QCowMetadata* qcow_metadata, QCowHeader* qcow_header) {
	if (fread(qcow_header, 1, QCOW_HEADER2_SIZE, qcow_metadata -> img_file) != QCOW_HEADER2_SIZE) {
		PERROR_LOG("An error occurred while reading the qcow header");
		return -QCOW_IO_ERROR;
	}

	format_qcow_header(qcow_header, 2);

	if (qcow_header -> backing_file_offset != 0 && qcow_header -> backing_file_name_size == 0) {
		WARNING_LOG("Backing file size cannot be zero.\n");
		return -QCOW_EMPTY_BACKING_FILE;
	} else if (qcow_header -> backing_file_offset < QCOW_HEADER2_SIZE && qcow_header -> backing_file_name_size > 0) {
		WARNING_LOG("Invalid backing file offset: %lu\n", qcow_header -> backing_file_offset);
		return -QCOW_INVALID_BACKING_FILE_OFFSET;
	} else if (str_n_cmp(qcow_header -> magic, "QFI\xfb", 4) != 0) {
		WARNING_LOG("Invalid magic expected: 'QFI\xfb' found '%s'.\n", qcow_header -> magic);
		return -QCOW_INVALID_MAGIC;
	} else if (qcow_header -> version != 2 && qcow_header -> version != 3) {
		WARNING_LOG("Valid qcow2 images versions are 2 and 3, found: %u\n", qcow_header -> version);
		return -QCOW_INVALID_VERSION;
	} else if (qcow_header -> cluster_bits < 9 || qcow_header -> cluster_bits > 21) {
		WARNING_LOG("Cluster bits must be between 9 and 21 included, but found: %u\n", qcow_header -> cluster_bits);
		return -QCOW_CLUSTER_BITS_TOO_SMALL;
	} 

	if (fread(CAST_PTR(qcow_header, unsigned char) + QCOW_HEADER2_SIZE, QCOW_HEADER3_EXT, 1, qcow_metadata -> img_file) != 1) {
		PERROR_LOG("An error occurred while reading the qcow header");
		return -QCOW_IO_ERROR;
	}

	format_qcow_header(qcow_header, 3);
	
	qcow_metadata -> use_extended_l2_entries = (qcow_header -> incompatible_features >> 4) & 1; 
	if (qcow_metadata -> use_extended_l2_entries) qcow_metadata -> l2_entries_size = L2_EXTENDED_ENTRY_SIZE;
	else qcow_metadata -> l2_entries_size = L2_ENTRY_SIZE;
	qcow_metadata -> use_erdf = (qcow_header -> incompatible_features >> 2) & 1;

	int err = 0;
	if (qcow_header -> version >= 3 && (err = check_version_three_features(qcow_header, qcow_metadata)) < 0) {
		WARNING_LOG("An error occurred while checking for version >=3 features.\n");
		return err;
	}

	dump_qcow_header(qcow_header);
	printf(" Compression Type: '%s'\n", compression_type_str[qcow_metadata -> compression_type]);
    printf(" ---------------------\n");

	return QCOW_NO_ERROR;
}

static int init_qcow_img(QCowMetadata* qcow_metadata, const char* path_qcow) {
	if ((qcow_metadata -> img_file = fopen(path_qcow, "rb+")) == NULL) {
		PERROR_LOG("An error occurred while opening the qcow file");
		return -QCOW_IO_ERROR;
	}

	if ((qcow_metadata -> img_size = fsize(qcow_metadata -> img_file)) < 0) {
		WARNING_LOG("Failed to get the size of the backing file.\n");
		return qcow_metadata -> img_size; 
	}

	DEBUG_LOG("File len: %.2LfMB (0x%llX)\n", qcow_metadata -> img_size / (1024.0L * 1024.0L), qcow_metadata -> img_size);
	
	return QCOW_NO_ERROR;
}

static int init_backing_file(QCowMetadata* qcow_metadata) {
	long int old_pos = 0;
	if ((old_pos = ftell(qcow_metadata -> img_file)) < 0) {
		PERROR_LOG("Failed to get the current pos.\n");
		return -QCOW_IO_ERROR;
	}
	
	int err = 0;
	char path_backing_file[1024] = {0};
	if ((err = read_at(qcow_metadata -> img_file, qcow_metadata -> backing_file_offset, path_backing_file, sizeof(unsigned char), qcow_metadata -> backing_file_size)) < 0) {
		WARNING_LOG("Failed to read the backing file name.\n");
		return err;
	}
	
	DEBUG_LOG("Using backing file: '%.*s'.\n", (int) qcow_metadata -> backing_file_offset, path_backing_file);

	if ((qcow_metadata -> backing_file = fopen(path_backing_file, "rb")) == NULL) {
		PERROR_LOG("An error occurred while opening the backing file");
		return -QCOW_IO_ERROR;
	}
	
	if ((qcow_metadata -> backing_file_size = fsize(qcow_metadata -> backing_file)) < 0) {
		WARNING_LOG("Failed to get the size of the backing file.\n");
		return qcow_metadata -> backing_file_size; 
	}

	DEBUG_LOG("File len: %.2LfMB (0x%llX)\n", qcow_metadata -> backing_file_size / (1024.0L * 1024.0L), qcow_metadata -> backing_file_size);
	
	if ((err = fseek(qcow_metadata -> img_file, old_pos, SEEK_SET)) < 0) {
		PERROR_LOG("Failed to seek at pos: %ld\n", old_pos);
		return -QCOW_IO_ERROR;
	}

	return QCOW_NO_ERROR;
}

static int init_raw_external_data(QCowMetadata* qcow_metadata, QCowHeaderExtension qcow_header_ext) {
	DEBUG_LOG("Using raw external data file: '%.*s'.\n", (int) qcow_header_ext.ext_length, qcow_header_ext.data);
	
	if ((qcow_metadata -> clusters_file = fopen((char*) qcow_header_ext.data, "rb+")) == NULL) {
		PERROR_LOG("Failed to open the raw external data file");
		return -QCOW_IO_ERROR;
	}
	

	if ((qcow_metadata -> clusters_file_size = fsize(qcow_metadata -> clusters_file)) < 0) {
		WARNING_LOG("Failed to get the size of the raw external data file.\n");
		return qcow_metadata -> clusters_file_size;
	}
	
	qcow_metadata -> clusters_file_base = 0;

	DEBUG_LOG("File len: %.2LfMB (0x%llX)\n", qcow_metadata -> clusters_file_size / (1024.0L * 1024.0L), qcow_metadata -> clusters_file_size);

	return QCOW_NO_ERROR;
}

static int parse_qcow(QCowMetadata* qcow_metadata, const char* path_qcow) {
	int err = 0;
	if ((err = init_qcow_img(qcow_metadata, path_qcow)) < 0) {
		deallocate_qcow_metadata(qcow_metadata);
		WARNING_LOG("Failed to initialize the qcow image.\n");
		return err;
	}

	QCowHeader qcow_header = {0};
	if ((err = parse_qcow_header(qcow_metadata, &qcow_header)) < 0) {
		deallocate_qcow_metadata(qcow_metadata);
		WARNING_LOG("Failed to parse qcow_header.\n");
		return err;
	}

	if (qcow_metadata -> backing_file_name_size > 0 && (err = init_backing_file(qcow_metadata)) < 0) {
		deallocate_qcow_metadata(qcow_metadata);
		WARNING_LOG("Failed to init the backing file.\n");
		return err;
	}

	QCowHeaderExtension* qcow_header_exts = NULL;
    int header_exts_cnts = parse_qcow_ext(&qcow_header_exts, qcow_metadata -> img_file);
	if (header_exts_cnts < 0) {
		deallocate_qcow_metadata(qcow_metadata);
		WARNING_LOG("An error occurred while parsing the header extensions.\n");
		return header_exts_cnts;
	}
	
	if ((qcow_metadata -> img_file_base = fsize(qcow_metadata -> img_file)) < 0) {
		WARNING_LOG("Failed to get the image file base.\n");
		return qcow_metadata -> img_file_base;
	}

	for (int i = 0; i < header_exts_cnts; ++i) {
		if (qcow_header_exts[i].ext_type == EXTERNAL_FILE_NAME) {
			if ((err = init_raw_external_data(qcow_metadata, qcow_header_exts[i])) < 0) {
				WARNING_LOG("An error occurred while initializing the raw external data.\n");
				return err;
			}
		} else dump_qcow_header_extension(qcow_header_exts + i);
		SAFE_FREE(qcow_header_exts[i].data);
	}

	if (qcow_metadata -> clusters_file == NULL && qcow_metadata -> use_erdf) {
		WARNING_LOG("Expected an exteral raw data file, but found none.\n");
		return -QCOW_UNINITIALIZED_ERDF;
	} else {
		qcow_metadata -> clusters_file = qcow_metadata -> img_file;
		qcow_metadata -> clusters_file_size = qcow_metadata -> img_size;
		qcow_metadata -> clusters_file_base = qcow_metadata -> img_file_base;
	}

	SAFE_FREE(qcow_header_exts);

	mem_cpy(CAST_PTR(qcow_metadata, unsigned char) + sizeof(CompressionType), CAST_PTR(&qcow_header, unsigned char) + QCOW_HEADER_HEADER_START, SHARED_FIELDS_SIZE);

	qcow_metadata -> cluster_size = 1 << qcow_header.cluster_bits;
	qcow_metadata -> refcount_bytes = (1 << qcow_header.refcount_order) / 8;
	qcow_metadata -> refcount_block_entries = (qcow_metadata -> cluster_size / qcow_metadata -> refcount_bytes);
	qcow_metadata -> refcount_table_size = qcow_metadata -> refcount_table_clusters * qcow_metadata -> cluster_size / sizeof(uint64_t); 
	qcow_metadata -> table_cluster_entries = qcow_metadata -> cluster_size / sizeof(uint64_t);
	if (qcow_metadata -> use_extended_l2_entries) qcow_metadata -> table_cluster_entries /= 2;
	if ((err = parse_ref_cnt_table(qcow_metadata)) < 0) {
		deallocate_qcow_metadata(qcow_metadata);
		WARNING_LOG("Failed to parse ref_cnt_table.\n");
		return -QCOW_IO_ERROR;
	}

	if ((err = parse_l1_table(qcow_metadata)) < 0) {
		deallocate_qcow_metadata(qcow_metadata);
		WARNING_LOG("Failed to parse l1_table.\n");
		return -QCOW_IO_ERROR;
	}

	if ((qcow_header.incompatible_features & 1) && (err = recompute_ref_cnt(qcow_metadata)) < 0) {
		WARNING_LOG("Failed to recompute the ref_cnt tables.\n");
		return err;
	}

	return QCOW_NO_ERROR;
}

static inline int get_ref_cnt(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* ref_cnt) {
	unsigned int refcount_block_index = (offset / qcow_metadata.cluster_size) % qcow_metadata.refcount_block_entries;
	unsigned int refcount_table_index = (offset / qcow_metadata.cluster_size) / qcow_metadata.refcount_block_entries;
	
	if (refcount_table_index >= qcow_metadata.refcount_table_size) {
		WARNING_LOG("Invalid offset: 0x%lX\n", offset);
		return -QCOW_INVALID_OFFSET;
	} else if ((qcow_metadata.refcount_table)[refcount_table_index] == NULL) {
		WARNING_LOG("Unallocated refcount table and clusters.\n");
		return -QCOW_UNALLOCATED_CLUSTER;
	}
	
	mem_cpy(ref_cnt, CAST_PTR((qcow_metadata.refcount_table)[refcount_table_index], unsigned char) + refcount_block_index * qcow_metadata.refcount_bytes, qcow_metadata.refcount_bytes);
	
	return QCOW_NO_ERROR;
}

static int allocate_ref_cnt_table(QCowMetadata qcow_metadata, uint64_t refcount_table_index) {		
	int err = 0;
	uint64_t refcnt_block_offset = 0;
	if ((err = extend_img_file(qcow_metadata.img_file, qcow_metadata.cluster_size, qcow_metadata.img_file_base, qcow_metadata.cluster_size, &refcnt_block_offset)) < 0) {
		WARNING_LOG("Failed to extend the image file by %lu bytes.\n", qcow_metadata.cluster_size);
		return err;
	}
	
	BE_CONVERT((unsigned char*) &refcnt_block_offset, sizeof(uint64_t));
	uint64_t offset = qcow_metadata.refcount_table_offset + refcount_table_index * sizeof(uint64_t);
	if ((err = write_at(qcow_metadata.img_file, offset, &refcnt_block_offset, sizeof(uint64_t), 1)) < 0) {
		WARNING_LOG("Failed to update the ref_cnt_table index.\n");
		return err;
	}
	
	(qcow_metadata.refcount_table)[refcount_table_index] = calloc(qcow_metadata.refcount_block_entries, qcow_metadata.refcount_bytes);
	if ((qcow_metadata.refcount_table)[refcount_table_index] == NULL) {
		WARNING_LOG("Failed to allocate the new refcount block.\n");
		return -QCOW_IO_ERROR;
	}

	return QCOW_NO_ERROR;
}

static int update_ref_cnt(QCowMetadata qcow_metadata, uint64_t offset, uint64_t new_ref_cnt) {
	unsigned int refcount_block_index = (offset / qcow_metadata.cluster_size) % qcow_metadata.refcount_block_entries;
	unsigned int refcount_table_index = (offset / qcow_metadata.cluster_size) / qcow_metadata.refcount_block_entries;
	
	int err = 0;
	if (refcount_table_index >= qcow_metadata.refcount_table_size) {
		WARNING_LOG("Invalid offset: 0x%lX\n", offset);
		return -QCOW_INVALID_OFFSET;
	} else if ((qcow_metadata.refcount_table)[refcount_table_index] == NULL) {
		if ((err = allocate_ref_cnt_table(qcow_metadata, refcount_table_index)) < 0) {
			WARNING_LOG("Failed to allocate the ref_cnt_table.\n");
			return err;
		}
	}

	mem_cpy(CAST_PTR((qcow_metadata.refcount_table)[refcount_table_index], unsigned char) + refcount_block_index * qcow_metadata.refcount_bytes, &new_ref_cnt, qcow_metadata.refcount_bytes);
	
	uint64_t refcount_block_offset = 0;
	uint64_t table_offset = qcow_metadata.refcount_table_offset + refcount_table_index * sizeof(uint64_t);
	if ((err = read_at(qcow_metadata.img_file, table_offset, &refcount_block_offset, sizeof(uint64_t), 1)) < 0) {
		WARNING_LOG("Failed to read the table offset.\n");
		return err;
	} 

	BE_CONVERT((unsigned char*) &refcount_block_offset, sizeof(uint64_t));
	if (refcount_block_offset & MASK_BITS_INTERVAL(9, 0)) {
		WARNING_LOG("Reserved bits set in refcount_block_offset: 0x%lX\n", refcount_block_offset);
		return -QCOW_USE_OF_RESERVED_FIELD;
	}
	
	BE_CONVERT((unsigned char*) &new_ref_cnt, qcow_metadata.refcount_bytes);
	if ((err = write_at(qcow_metadata.img_file, refcount_block_offset + refcount_block_index * qcow_metadata.refcount_bytes, &new_ref_cnt, qcow_metadata.refcount_bytes, 1))) {
		WARNING_LOG("Failed to update the ref_cnt.\n");
		return err;
	}
	
	return QCOW_NO_ERROR;
}

static inline int lba_to_img_offset(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* img_offset, SubclusterInfo* subcluster_info) {
    uint64_t l2_index = (offset / qcow_metadata.cluster_size) % qcow_metadata.table_cluster_entries;
    uint64_t l1_index = (offset / qcow_metadata.cluster_size) / qcow_metadata.table_cluster_entries;

	if (l1_index >= qcow_metadata.l1_size) {
		WARNING_LOG("Invalid offset points to unallocated l1_table: 0x%lX\n", offset);
		return -QCOW_INVALID_OFFSET;
	} else if ((qcow_metadata.l1_table)[l1_index] == NULL) {
		WARNING_LOG("Unallocated l1 table and clusters.\n");
		return -QCOW_UNALLOCATED_L1_TABLE;
	}
	
	mem_cpy(img_offset, CAST_PTR((qcow_metadata.l1_table)[l1_index], unsigned char) + l2_index * qcow_metadata.l2_entries_size, sizeof(uint64_t));
	
	if (*img_offset == 0 || *img_offset == (1ULL << 62)) {
		WARNING_LOG("Unallocated cluster.\n");
		return -QCOW_UNALLOCATED_CLUSTER;
	} else if (!IS_COMPRESSED_CLUSTER(*img_offset) && (*img_offset & MASK_BITS_INTERVAL(56, 9)) == 0 && ((*img_offset >> 63) & 1) && !qcow_metadata.use_erdf) {
		WARNING_LOG("The cluster offset can be zero only if an external raw data file is used.\n");
		return -QCOW_INVALID_OFFSET;
	} else if (!IS_COMPRESSED_CLUSTER(*img_offset) && ((*img_offset & MASK_BITS_INTERVAL(9, 1)) || (*img_offset & MASK_BITS_INTERVAL(62, 56)))) {
		WARNING_LOG("Detected use of reserved field.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} else if (!IS_COMPRESSED_CLUSTER(*img_offset) && !IS_CLUSTER_ALIGNED(*img_offset & MASK_BITS_INTERVAL(56, 9), qcow_metadata.clusters_file_base, qcow_metadata.cluster_size)) {
		WARNING_LOG("Unaligned cluster.\n");
		return -QCOW_UNALIGNED_CLUSTER;
	}
	
	if (qcow_metadata.use_extended_l2_entries && subcluster_info != NULL) {
		mem_cpy(subcluster_info, CAST_PTR((qcow_metadata.l1_table)[l1_index], unsigned char) + l2_index * qcow_metadata.l2_entries_size + sizeof(uint64_t), sizeof(SubclusterInfo));
		if (IS_COMPRESSED_CLUSTER(*img_offset) && *CAST_PTR(subcluster_info, uint64_t) != 0) {
			WARNING_LOG("If the cluster is compressed the subcluster info should be zeroed-out, but found: %lu\n", *CAST_PTR(subcluster_info, uint64_t));
			return -QCOW_USE_OF_RESERVED_FIELD;
		}
	}	

	return QCOW_NO_ERROR;
}

static int allocate_l2_table(QCowMetadata qcow_metadata, uint64_t l1_index) {
	int err = 0;
	uint64_t l2_table_offset = 0;
	if ((err = extend_img_file(qcow_metadata.img_file, qcow_metadata.cluster_size, qcow_metadata.img_file_base, qcow_metadata.cluster_size, &l2_table_offset)) < 0) {
		WARNING_LOG("Failed to extend the image file by %lu bytes.\n", qcow_metadata.cluster_size);
		return err;
	}
	
	l2_table_offset &= MASK_BITS_INTERVAL(56, 9);
	BE_CONVERT((unsigned char*) &l2_table_offset, sizeof(uint64_t));
	uint64_t offset = qcow_metadata.l1_table_offset + l1_index * sizeof(uint64_t);
	if ((err = write_at(qcow_metadata.img_file, offset, &l2_table_offset, sizeof(uint64_t), 1))) {
		WARNING_LOG("Failed to update the l2 entry.\n");
		return err;
	}
	
	(qcow_metadata.l1_table)[l1_index] = calloc(qcow_metadata.table_cluster_entries, qcow_metadata.l2_entries_size);
	if ((qcow_metadata.l1_table)[l1_index] == NULL) {
		WARNING_LOG("Failed to allocate the new l2 table.\n");
		return -QCOW_IO_ERROR;
	}	

	return QCOW_NO_ERROR;
}

static int set_lba_at_img_offset(QCowMetadata qcow_metadata, uint64_t offset, uint64_t new_entry, SubclusterInfo new_subcluster_info) {
    uint64_t l1_index = (offset / qcow_metadata.cluster_size) / qcow_metadata.table_cluster_entries;
    uint64_t l2_index = (offset / qcow_metadata.cluster_size) % qcow_metadata.table_cluster_entries;
	
	int err = 0;
	if (l1_index >= qcow_metadata.l1_size) {
		WARNING_LOG("The l1_table referenced is out of the active table range, but I don't know how to increase it :< \n");
		return -QCOW_INVALID_OFFSET;
	} else if ((qcow_metadata.l1_table)[l1_index] == NULL) {
		if ((err = allocate_l2_table(qcow_metadata, l1_index)) < 0) {
			WARNING_LOG("Unallocated l1 table and clusters.\n");
			return err;
		}
	}

	DEBUG_LOG("new_entry: %lX\n", new_entry);
	mem_cpy(CAST_PTR((qcow_metadata.l1_table)[l1_index], unsigned char) + l2_index * qcow_metadata.l2_entries_size, &new_entry, sizeof(uint64_t));

	uint64_t l2_offset = 0;
	uint64_t l1_table_offset = qcow_metadata.l1_table_offset + l1_index * sizeof(uint64_t);
	if ((err = read_at(qcow_metadata.img_file, l1_table_offset, &l2_offset, sizeof(uint64_t), 1)) < 0) {
		WARNING_LOG("Failed to read the l2 offset.\n");
		return err;
	}

	BE_CONVERT(&l2_offset, sizeof(uint64_t));
	BE_CONVERT(&new_entry, sizeof(uint64_t));

	uint64_t l2_entry = (l2_offset & MASK_BITS_INTERVAL(56, 9)) + l2_index * qcow_metadata.l2_entries_size;
	if ((err = write_at(qcow_metadata.img_file, l2_entry, &new_entry, sizeof(uint64_t), 1)) < 0) {
		WARNING_LOG("Failed to update the l2 entry.\n");
		return err;
	}
	
	if (qcow_metadata.use_extended_l2_entries) {
		DEBUG_LOG("new_alloc_status: 0x%X, new_reads_as_zero: 0x%X\n", new_subcluster_info.alloc_status, new_subcluster_info.reads_as_zero);
		mem_cpy(CAST_PTR((qcow_metadata.l1_table)[l1_index], unsigned char) + l2_index * qcow_metadata.l2_entries_size + sizeof(uint64_t), &new_subcluster_info, sizeof(SubclusterInfo));
		BE_CONVERT(&new_subcluster_info, sizeof(SubclusterInfo));
		if (fwrite(&new_subcluster_info, sizeof(SubclusterInfo), 1, qcow_metadata.img_file) != 1) {
			PERROR_LOG("Failed to update the l2 extended entry.\n");
			return -QCOW_IO_ERROR;
		}
	}

	return QCOW_NO_ERROR;
}

static int extend_img_file(FILE* file, uint64_t n, uint64_t file_boundary_base, uint64_t boundary, uint64_t* end_pos) {
	long long int eof_pos = 0;
	if ((eof_pos = fsize(file)) < 0) {
		WARNING_LOG("Failed to get the file size.\n");
		return eof_pos;
	}

	int err = 0;
	uint64_t boundary_offset = (eof_pos - file_boundary_base) % boundary;
	if (boundary && boundary_offset) {
		if ((err = zero_out_at(file, eof_pos, boundary - boundary_offset)) < 0) {
			WARNING_LOG("Failed to zero out until reaching the requested boundary.\n");
			return err;
		}
		eof_pos += boundary - boundary_offset;
	}

	if ((err = zero_out_at(file, eof_pos, n)) < 0) {
		WARNING_LOG("Failed to zero out the allocated cluster.\n");
		return err;
	}
	
	if (end_pos != NULL) *end_pos = eof_pos + n;

	return QCOW_NO_ERROR;
}

static inline int find_unallocated_cluster(QCowMetadata qcow_metadata, uint64_t* offset) {
	for (unsigned int refcnt_block = 0; refcnt_block < qcow_metadata.refcount_table_size; ++refcnt_block) {
		for (unsigned int i = 0; i < qcow_metadata.refcount_block_entries; ++i) {
			uint64_t ref_cnt = 0;
			mem_cpy(&ref_cnt, CAST_PTR((qcow_metadata.refcount_table)[refcnt_block], unsigned char) + i, qcow_metadata.refcount_bytes);
			if (ref_cnt == 0 || (qcow_metadata.refcount_table)[refcnt_block] == NULL) {
				*offset = i + (refcnt_block * qcow_metadata.cluster_size * qcow_metadata.refcount_block_entries);
				int err = 0;
				if ((err = update_ref_cnt(qcow_metadata, *offset, 1)) < 0) {
					WARNING_LOG("Failed to update the ref_cnt.\n");
					return err;
				}
				return QCOW_NO_ERROR;
			}
		}
	}	

	WARNING_LOG("No space for allocation found: buy more disk space LOL!\n");

	return -QCOW_IO_ERROR;
}

static int alloc_cluster(QCowMetadata qcow_metadata, uint64_t* cluster_offset) {
	int err = 0;
	uint64_t offset = 0;
	if ((err = find_unallocated_cluster(qcow_metadata, &offset)) < 0) {
		WARNING_LOG("Failed to find an unallocated cluster.\n");
		return err;
	}
	
	SubclusterInfo subcluster_info = { .alloc_status = 0xFFFF, .reads_as_zero = 0 };
	if ((err = lba_to_img_offset(qcow_metadata, offset, cluster_offset, NULL)) == 0) {
		*cluster_offset = (*cluster_offset & MASK_BITS_INTERVAL(56, 9)) | (1ULL << 63);
		if ((err = set_lba_at_img_offset(qcow_metadata, offset, *cluster_offset, subcluster_info)) == 0) return QCOW_NO_ERROR;
		WARNING_LOG("Failed to update the l2 entry.\n");
		return err;
	} else if (err != -QCOW_UNALLOCATED_CLUSTER || err != -QCOW_UNALLOCATED_L1_TABLE) return err;

	uint64_t cluster_pos = 0;
	if ((err = extend_img_file(qcow_metadata.clusters_file, qcow_metadata.cluster_size, qcow_metadata.clusters_file_base, qcow_metadata.cluster_size,  &cluster_pos)) < 0) {
		WARNING_LOG("Failed to extend the image file by %lu bytes.\n", qcow_metadata.cluster_size);
		return err;
	}

	// Update the l2 entry and set the subcluster_info to allocated in case it uses l2_extended
	*cluster_offset = (cluster_pos & MASK_BITS_INTERVAL(56, 9)) | (1ULL << 63);
	if ((err = set_lba_at_img_offset(qcow_metadata, offset, *cluster_offset, subcluster_info)) < 0) {
		WARNING_LOG("Failed to update the l2 entry.\n");
		return err;
	}

	return QCOW_NO_ERROR;
}

static int cow_alloc_cluster(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* cluster_offset) {
	int err = 0;
	uint64_t original_img_offset = 0;
	if ((err = lba_to_img_offset(qcow_metadata, offset, &original_img_offset, NULL)) < 0) return err;

	// Allocate the required clusters
	unsigned int additional_sectors = 0;
	uint64_t cluster_boundary = qcow_metadata.cluster_size * IS_COMPRESSED_CLUSTER(original_img_offset);
	if (IS_COMPRESSED_CLUSTER(original_img_offset)) {
		unsigned int x = 62 - (qcow_metadata.cluster_bits - 8);
		additional_sectors = (original_img_offset & MASK_BITS_INTERVAL(62, x)) >> x;
		original_img_offset &= MASK_BITS_INTERVAL(x, 0); 
	} else original_img_offset &= MASK_BITS_INTERVAL(56, 9);
	
	uint64_t cluster_pos = 0;
	uint64_t additional_clusters = CEILING(additional_sectors, qcow_metadata.cluster_size / 512);
	uint64_t clusters_size = (1 + additional_clusters) * qcow_metadata.cluster_size;
	if ((err = extend_img_file(qcow_metadata.clusters_file, clusters_size, qcow_metadata.clusters_file_base, cluster_boundary, &cluster_pos)) < 0) {
		WARNING_LOG("Failed to extend the image file by %lu bytes.\n", clusters_size);
		return err;
	}

	// Copy the data from the old clusters
	FILE* file = qcow_metadata.clusters_file;
	if (qcow_metadata.backing_file && (original_img_offset + clusters_size) <= (uint64_t) qcow_metadata.backing_file_size) file = qcow_metadata.backing_file;
	
	unsigned char* cluster_data = (unsigned char*) calloc(clusters_size, sizeof(unsigned char));
	if ((err = read_at(file, original_img_offset, cluster_data, sizeof(unsigned char), clusters_size)) < 0) {
		SAFE_FREE(cluster_data);
		WARNING_LOG("Failed to read the cluster at offset: 0x%lX.\n", offset);
		return err;
	}
	
	// Copy the data to the newly allocated clusters
	if ((err = write_at(qcow_metadata.clusters_file, cluster_pos, cluster_data, sizeof(unsigned char), clusters_size)) < 0) {
		SAFE_FREE(cluster_data);
		WARNING_LOG("Failed to copy the cluster data.\n");
		return err;
	}

	SAFE_FREE(cluster_data);
	
	// Update the l2 entry and ref_cnt for the original offset and also set the subclusters to allocated in case of use_l2_extended
	SubclusterInfo subcluster_info = { .alloc_status = 0xFFFF, .reads_as_zero = 0 };
	*cluster_offset = (cluster_pos & MASK_BITS_INTERVAL(56, 9)) | (1ULL << 63);
	if ((err = set_lba_at_img_offset(qcow_metadata, offset, *cluster_offset, subcluster_info)) == 0) {
		WARNING_LOG("Failed to update the l2 entry.\n");
		return QCOW_NO_ERROR;
	}

	if ((err = update_ref_cnt(qcow_metadata, offset, 1)) < 0) {
		WARNING_LOG("Failed to update the ref_cnt.\n");
		return err;
	}

	return QCOW_NO_ERROR;
}

static int write_compressed_cluster(QCowMetadata qcow_metadata, uint64_t img_offset, unsigned int* recompressed_cluster_size, unsigned int compressed_cluster_size, unsigned char* cluster, unsigned int cluster_data_size) {
	int err = 0;
	unsigned char* recompressed_cluster = NULL;
	if (qcow_metadata.compression_type == DEFLATE) {
		recompressed_cluster = zlib_deflate(cluster, cluster_data_size, recompressed_cluster_size, &err);
		if (err) {
			printf(COLOR_STR("ZLIB_ERROR::%s: ", RED) "%s", zlib_errors_str[-err], recompressed_cluster);
			return -QCOW_DEFLATE_ERROR; 
		}
	} else {
		WARNING_LOG("ZSTD compression is not yet implemented.");
		return -QCOW_TODO;
	}

	if ((err = zero_out_at(qcow_metadata.clusters_file, img_offset, compressed_cluster_size)) < 0){
		SAFE_FREE(recompressed_cluster);
		return err;
	}

	if ((err = write_at(qcow_metadata.clusters_file, img_offset, recompressed_cluster, sizeof(unsigned char), *recompressed_cluster_size)) < 0) {
		SAFE_FREE(recompressed_cluster);	
		return err;
	}

	SAFE_FREE(recompressed_cluster);

	return QCOW_NO_ERROR;
}

static int read_compressed_cluster(QCowMetadata qcow_metadata, FILE* file, uint64_t* cluster_offset, unsigned char** clusters, unsigned int *cluster_data_size, unsigned int* compressed_clusters_size) {
	unsigned int x = 62 - (qcow_metadata.cluster_bits - 8);
	unsigned int additional_sectors = (*cluster_offset & MASK_BITS_INTERVAL(62, x)) >> x;
	*cluster_offset &= MASK_BITS_INTERVAL(x, 0); 
	DEBUG_LOG("img_offset: 0x%lX, additional_sectors: %u\n", *cluster_offset, additional_sectors);

	int err = 0;
	*compressed_clusters_size = qcow_metadata.cluster_size + additional_sectors * 512;
	unsigned char* compressed_clusters = (unsigned char*) calloc(*compressed_clusters_size, sizeof(unsigned char));
	if ((err = read_at(file, *cluster_offset, compressed_clusters, sizeof(unsigned char), *compressed_clusters_size)) < 0) {
		SAFE_FREE(compressed_clusters);
		WARNING_LOG("Failed to read the compressed cluster.\n");
	}

	DEBUG_LOG("Compressed virtual disk block with compression_method: '%s'.\n", compression_type_str[qcow_metadata.compression_type]);
	
	if (qcow_metadata.compression_type == DEFLATE) {
		*clusters = zlib_inflate(compressed_clusters, *compressed_clusters_size, cluster_data_size, &err);
		if (err) {
			printf(COLOR_STR("ZLIB_ERROR::%s: ", RED) "%s", zlib_errors_str[-err], *clusters);
			return -QCOW_DEFLATE_ERROR; 
		}
	} else {
		*cluster_data_size = qcow_metadata.cluster_size;
		*clusters = zstd_inflate(compressed_clusters, *compressed_clusters_size, cluster_data_size, &err);
		if (err) {
			printf(COLOR_STR("ZSTD_ERROR::%s: ", RED) "%s", zstd_errors_str[-err], *clusters);
			return -QCOW_DEFLATE_ERROR; 
		}
	}

	return QCOW_NO_ERROR;
}

static int get_lba_img_offset_for_write(QCowMetadata qcow_metadata, uint64_t offset, uint64_t* img_offset, SubclusterInfo* subcluster_info) {
	int err = 0;
	uint64_t ref_cnt = 0;
	if ((err = get_ref_cnt(qcow_metadata, offset, &ref_cnt)) < 0) return err;
	DEBUG_LOG("ref_cnt at LBA 0x%lX: %lu\n", offset, ref_cnt);
	
	if (ref_cnt == 0) {
		if ((err = alloc_cluster(qcow_metadata, img_offset)) < 0) {
			WARNING_LOG("Failed to allocate the cluster.\n");
			return err;
		}
	} else if (ref_cnt > 1) {
		if ((err = cow_alloc_cluster(qcow_metadata, offset, img_offset)) < 0) {
			WARNING_LOG("Failed to allocate the cluster.\n");
			return err;
		}
	} else {
		if ((err = lba_to_img_offset(qcow_metadata, offset, img_offset, subcluster_info)) < 0) return err;
	}

	IMG_OFFSET_INFO(*img_offset);

	return QCOW_NO_ERROR;
}

int qwrite(const void* data, unsigned int size, size_t nmemb, unsigned int offset, QCowMetadata qcow_metadata) {	
	int err = 0;
	uint64_t img_offset = 0;
	SubclusterInfo subcluster_info = {0};
	if ((err = get_lba_img_offset_for_write(qcow_metadata, offset, &img_offset, &subcluster_info)) < 0) {
		WARNING_LOG("Failed to retrieve the img_offset.\n");
		return err;
	}

	if (IS_COMPRESSED_CLUSTER(img_offset)) {
		unsigned char* cluster = NULL;
		unsigned int cluster_data_size = 0;
		unsigned int compressed_cluster_size = 0;
		if ((err = read_compressed_cluster(qcow_metadata, qcow_metadata.clusters_file, &img_offset, &cluster, &cluster_data_size, &compressed_cluster_size)) < 0) {
			WARNING_LOG("Failed to read compressed cluster at img_offset: 0x%lX\n", img_offset);
			return err;
		}

		unsigned int cluster_offset = offset % qcow_metadata.cluster_size;
		if (cluster_offset >= cluster_data_size) {
			WARNING_LOG("Invalid offset %u in cluster of size: %u.\n", cluster_offset, cluster_data_size);
			return -QCOW_IO_ERROR;
		}
		
		mem_cpy(cluster + cluster_offset, data, MIN(size * nmemb, cluster_data_size - cluster_offset));

		unsigned int recompressed_cluster_size = 0;
		if ((err = write_compressed_cluster(qcow_metadata, img_offset, &recompressed_cluster_size, compressed_cluster_size, cluster, cluster_data_size)) < 0) {
			WARNING_LOG("Failed to write back the recompressed cluster.\n");
			return err;
		}

		subcluster_info = (SubclusterInfo) {0};
		unsigned int x = 62 - (qcow_metadata.cluster_bits - 8);
		uint64_t new_additional_sectors = recompressed_cluster_size > qcow_metadata.cluster_size ? (((recompressed_cluster_size - qcow_metadata.cluster_size) / 512) & MASK_BITS_PRECEDING(61 - x)) << (x + 1) : 0;
		if ((err = set_lba_at_img_offset(qcow_metadata, offset, new_additional_sectors | img_offset | (1ULL << 62), subcluster_info))) {
			WARNING_LOG("Failed to set the new address for the modified lba.\n");
			return err;
		}
	} else {
		if (((img_offset & MASK_BITS_INTERVAL(62, 56)) != 0) || ((img_offset & MASK_BITS_INTERVAL(9, 0)) != 0)) {
			WARNING_LOG("Use of reserved field in l2 entry.\n");
			return -QCOW_USE_OF_RESERVED_FIELD;
		} else if ((img_offset & MASK_BITS_INTERVAL(56, 9)) == 0 && ((img_offset >> 63) & 1) == 0 && !qcow_metadata.use_erdf) {
			if ((err = alloc_cluster(qcow_metadata, &img_offset)) < 0) {
				WARNING_LOG("Failed to allocate the cluster.\n");
				return err;
			}
		}
		
		if (qcow_metadata.use_extended_l2_entries) {
			uint64_t subcluster_size = qcow_metadata.cluster_size / 32;
			uint64_t subcluster_pos = offset % subcluster_size;
			uint8_t subcluster_index = FLOORING(offset % qcow_metadata.cluster_size, subcluster_size);
			uint8_t subclusters_used = FLOORING(subcluster_pos + size * nmemb, subcluster_size);
			subcluster_info.alloc_status |= MASK_BITS_INTERVAL(subclusters_used + subcluster_index + 1, subcluster_index);
			subcluster_info.reads_as_zero &= MASK_BITS_INTERVAL(32, subcluster_index + subclusters_used + 1) | MASK_BITS_INTERVAL(subcluster_index, 0);
			if ((err = set_lba_at_img_offset(qcow_metadata, offset, img_offset, subcluster_info))) {
				WARNING_LOG("Failed to set the update the subcluster info for the modified lba.\n");
				return err;
			}
		}
		
		img_offset = (img_offset & MASK_BITS_INTERVAL(56, 9)) + (offset % qcow_metadata.cluster_size);
		write_at(qcow_metadata.clusters_file, img_offset, data, size, nmemb);
	}
	
	update_ref_cnt(qcow_metadata, offset, 1);
	
	return QCOW_NO_ERROR;
}

static int read_from_backing_file(void* ptr, size_t size, size_t nmemb, uint64_t cluster_offset, unsigned int offset, QCowMetadata qcow_metadata) {
	int err = 0;
	if (IS_COMPRESSED_CLUSTER(cluster_offset)) {
		unsigned char* cluster = NULL;
		unsigned int cluster_data_size = 0;
		unsigned int compressed_cluster_size = 0;
		if ((err = read_compressed_cluster(qcow_metadata, qcow_metadata.backing_file, &cluster_offset, &cluster, &cluster_data_size, &compressed_cluster_size)) < 0) {
			WARNING_LOG("Failed to read compressed cluster at cluster_offset: 0x%lX\n", cluster_offset);
			return err;
		}
		
		mem_cpy(ptr, cluster + (offset % qcow_metadata.cluster_size), MIN(cluster_data_size, size * nmemb));
		SAFE_FREE(cluster);
			
		return QCOW_NO_ERROR;
	} 

	if (((cluster_offset & MASK_BITS_INTERVAL(62, 56)) != 0) || ((cluster_offset & MASK_BITS_INTERVAL(9, 0)) != 0)) {
		WARNING_LOG("Use of reserved field in l2 entry.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} 

	cluster_offset = (cluster_offset & MASK_BITS_INTERVAL(56, 9)) + (offset % qcow_metadata.cluster_size);
	if ((err = read_at(qcow_metadata.backing_file, cluster_offset, ptr, size, nmemb)) < 0) {
		WARNING_LOG("Failed to read the cluster from the backing file.\n");
		return err;
	}

	return QCOW_NO_ERROR;
}

/// NOTE: the function expects that the ptr has been already allocated, so that it has no responsibility for its de/allocation.
int qread(void* ptr, size_t size, size_t nmemb, unsigned int offset, QCowMetadata qcow_metadata) {
	int err = 0;
	uint64_t ref_cnt = 0;
	if ((err = get_ref_cnt(qcow_metadata, offset, &ref_cnt)) < 0) return err;
	DEBUG_LOG("ref_cnt at LBA 0x%X: %lu\n", offset, ref_cnt);
	
	if (ref_cnt == 0 && qcow_metadata.backing_file == NULL) {
		mem_set(ptr, 0, size * nmemb);
		return QCOW_NO_ERROR;
	} 
	
	uint64_t img_offset = 0;
	SubclusterInfo subcluster_info = {0};
	if ((err = lba_to_img_offset(qcow_metadata, offset, &img_offset, &subcluster_info)) < 0) return err;
	IMG_OFFSET_INFO(img_offset);
	
	if (ref_cnt == 0 && (img_offset + size * nmemb) <= (uint64_t) qcow_metadata.backing_file_size) {
		if ((err = read_from_backing_file(ptr, size, nmemb, img_offset, offset, qcow_metadata)) < 0) {
			WARNING_LOG("Failed to read from the backing file at img_offset: 0x%lX\n", img_offset);
			return err;
		}
		return QCOW_NO_ERROR;
	} else if (ref_cnt == 0) {
		mem_set(ptr, 0, size * nmemb);
		return QCOW_NO_ERROR;
	}
	
	if (qcow_metadata.use_extended_l2_entries && !IS_COMPRESSED_CLUSTER(img_offset)) {
		uint64_t subcluster_size = qcow_metadata.cluster_size / 32;
		uint64_t subcluster_pos = offset % qcow_metadata.cluster_size;
		uint8_t subcluster_index = FLOORING(offset % qcow_metadata.cluster_size, subcluster_size);
		
		if ((subcluster_info.alloc_status >> subcluster_index & 1) && (subcluster_info.reads_as_zero >> subcluster_index & 1)) {
			WARNING_LOG("Allocation status and reads as zero cannot be both set to 1 for the same subcluster.\n");
			return -QCOW_INVALID_SUBCLUSTER_BITMAP;
		}
		
		if ((subcluster_info.alloc_status >> subcluster_index & 1) == 0 || (subcluster_info.reads_as_zero >> subcluster_index & 1)) {
			if (qcow_metadata.backing_file == NULL && (img_offset + subcluster_pos + size * nmemb) > (uint64_t) qcow_metadata.backing_file_size) {
				mem_set(ptr, 0, size * nmemb);
				return QCOW_NO_ERROR;
			}
			
			if ((err = read_from_backing_file(ptr, size, nmemb, img_offset, offset, qcow_metadata)) < 0) {
				WARNING_LOG("Failed to read from the backing file at img_offset: 0x%lX\n", img_offset);
				return err;
			}
			
			return QCOW_NO_ERROR;
		}
	}
	
	if (IS_COMPRESSED_CLUSTER(img_offset)) {
		unsigned char* cluster = NULL;
		unsigned int cluster_data_size = 0;
		unsigned int compressed_cluster_size = 0;
		if ((err = read_compressed_cluster(qcow_metadata, qcow_metadata.clusters_file, &img_offset, &cluster, &cluster_data_size, &compressed_cluster_size)) < 0) {
			WARNING_LOG("Failed to read compressed cluster at img_offset: 0x%lX\n", img_offset);
			return err;
		}
		
		mem_cpy(ptr, cluster + (offset % qcow_metadata.cluster_size), MIN(cluster_data_size, size * nmemb));
		SAFE_FREE(cluster);
			
		return QCOW_NO_ERROR;
	} 

	if (((img_offset & MASK_BITS_INTERVAL(62, 56)) != 0) || ((img_offset & MASK_BITS_INTERVAL(9, 0)) != 0)) {
		WARNING_LOG("Use of reserved field in l2 entry.\n");
		return -QCOW_USE_OF_RESERVED_FIELD;
	} else if ((img_offset & MASK_BITS_INTERVAL(56, 9)) == 0 && ((img_offset >> 63) & 1) == 0 && !qcow_metadata.use_erdf) {
		mem_set(ptr, 0, size * nmemb);
		return QCOW_NO_ERROR;
	}

	img_offset = (img_offset & MASK_BITS_INTERVAL(56, 9)) + (offset % qcow_metadata.cluster_size);
	if ((err = read_at(qcow_metadata.clusters_file, img_offset, ptr, size, nmemb)) < 0) {
		WARNING_LOG("Failed to read from the qcow image.\n");
		return err;
	}
	
	return QCOW_NO_ERROR; 
}

#endif // _QCOW_PARSER_H_

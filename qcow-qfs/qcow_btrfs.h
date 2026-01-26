#ifndef _QCOW_BTRFS_H_
#define _QCOW_BTRFS_H_

typedef enum {
	BTRFS_MAX_NAME                  = 256,
	BTRFS_SUPERBLOCK_LOGICAL_OFFSET = 0x10000,
	BTRFS_SUPERBLOCK_OFFSET         = 128,
	BTRFS_NUM_BACKUP_ROOTS          = 4,
	BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE  = 0x800,
	BTRFS_MAX_LEVEL                 = 8,
	BTRFS_OLD                       = 0,
	BTRFS_MIXED                     = 1,
	BTRFS_LEAF_NODE                 = 0,
	BTRFS_TIME_SIZE                 = 0x0C,
	BTRFS_KEY_SIZE                  = 0x11,
	BTRFS_DEV_ITEM_SIZE             = 0x62,
	BTRFS_CHUNK_ITEM_SIZE           = 0x30,
	BTRFS_STRIPE_SIZE               = 0x20,
	BTRFS_ROOT_BACKUP_SIZE          = 0xA8,
	BTRFS_SUPERBLOCK_SIZE           = 0x1000,
	BTRFS_NODE_HEADER_SIZE          = 0x65,
	BTRFS_INTERNAL_NODE_SIZE        = 0x21,
	BTRFS_LEAF_NODE_SIZE            = 0x19,
	BTRFS_INODE_REF_SIZE            = 0x0A,
	BTRFS_ROOT_REF_SIZE             = 0x12,
	BTRFS_MAX_NODE_SIZE             = 65536
} QCowBtrfsConstants;

typedef enum {
	// Compatible Features (cannot be ignored for writing into the fs)
	BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE       = (1ULL << 0), // If this bit is clear, then the free space tree cannot be trusted.
	BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID = (1ULL << 1),
	BTRFS_FEATURE_COMPAT_RO_VERITY                = (1ULL << 2),
	BTRFS_FEATURE_COMPAT_RO_BLOCK_GROUP_TREE      = (1ULL << 3),

	// Incompatible Features (must never be ignored)
	BTRFS_FEATURE_INCOMPAT_MIXED_BACKREF          = (1ULL << 0),
	BTRFS_FEATURE_INCOMPAT_DEFAULT_SUBVOL         = (1ULL << 1),
	BTRFS_FEATURE_INCOMPAT_MIXED_GROUPS           = (1ULL << 2),
	BTRFS_FEATURE_INCOMPAT_COMPRESS_LZO           = (1ULL << 3),
	BTRFS_FEATURE_INCOMPAT_COMPRESS_ZSTD          = (1ULL << 4),
	BTRFS_FEATURE_INCOMPAT_BIG_METADATA           = (1ULL << 5),
	BTRFS_FEATURE_INCOMPAT_EXTENDED_IREF          = (1ULL << 6),
	BTRFS_FEATURE_INCOMPAT_RAID56                 = (1ULL << 7),
	BTRFS_FEATURE_INCOMPAT_SKINNY_METADATA        = (1ULL << 8),
	BTRFS_FEATURE_INCOMPAT_NO_HOLES               = (1ULL << 9),
	BTRFS_FEATURE_INCOMPAT_METADATA_UUID          = (1ULL << 10),
	BTRFS_FEATURE_INCOMPAT_RAID1C34               = (1ULL << 11),
	BTRFS_FEATURE_INCOMPAT_ZONED                  = (1ULL << 12),
	BTRFS_FEATURE_INCOMPAT_EXTENT_TREE_V2         = (1ULL << 13),
	BTRFS_FEATURE_INCOMPAT_RAID_STRIPE_TREE       = (1ULL << 14),
	BTRFS_FEATURE_INCOMPAT_SIMPLE_QUOTA           = (1ULL << 16)
} BtrfsFlags;
	
typedef enum {
	BTRFS_FIRST_FREE_OBJECTID       = 256,
	BTRFS_LAST_FREE_OBJECTID        = -256,
	BTRFS_FIRST_CHUNK_TREE_OBJECTID = 256,
	BTRFS_EXTENT_TREE_OBJECTID      = 2,
	BTRFS_DEV_TREE_OBJECTID         = 4,
	BTRFS_FS_TREE_OBJECTID          = 5,
	BTRFS_CSUM_TREE_OBJECTID        = 7,
	BTRFS_UUID_TREE_OBJECTID        = 9,
	BTRFS_FREE_SPACE_TREE_OBJECTID  = 10,
	BTRFS_DATA_RELOC_TREE_OBJECTID  = -9
} BtrfsObjectIds;

typedef enum {
	BTRFS_INODE_ITEM_KEY        = 0x01,
	BTRFS_INODE_REF_KEY         = 0x0C,
	BTRFS_XATTR_ITEM_KEY        = 0x18,
	BTRFS_DIR_ITEM_KEY          = 0x54,
	BTRFS_DIR_INDEX_KEY         = 0x60,
	BTRFS_EXTENT_DATA_ITEM_KEY  = 0x6C,
	BTRFS_CSUM_ITEM_KEY         = 0x80,
	BTRFS_ROOT_ITEM_KEY         = 0x84,
	BTRFS_ROOT_BACKREF_KEY      = 0x90,
	BTRFS_ROOT_REF_KEY          = 0x9C,
	BTRFS_EXTENT_ITEM_KEY       = 0xA8,
	BTRFS_METADATA_ITEM_KEY     = 0xA9,
	BTRFS_BLOCK_GROUP_KEY       = 0xC0,
	BTRFS_FREE_SPACE_INFO_KEY   = 0xC6,
	BTRFS_FREE_SPACE_EXTENT_KEY = 0xC7,
	BTRFS_FREE_SPACE_BITMAP_KEY = 0xC8,
	BTRFS_DEV_EXTENT_KEY        = 0xCC,
	BTRFS_DEV_ITEM_KEY          = 0xD8,
	BTRFS_CHUNK_ITEM_KEY        = 0xE4,
	BTRFS_RAID_STRIPE_KEY       = 0xE6,
	BTRFS_DEV_STATS_KEY         = 0xF9,
	BTRFS_UUID_KEY              = 0xFB
} BtrfsItemTypes;

typedef enum {
	BTRFS_FT_UNKNOWN   = 0,
	BTRFS_FT_REG_FILE  = 1,
	BTRFS_FT_DIR       = 2,
	BTRFS_FT_CHRDEV    = 3,
	BTRFS_FT_BLKDEV    = 4,
	BTRFS_FT_FIFO      = 5,
	BTRFS_FT_SOCK      = 6,
	BTRFS_FT_SYMLINK   = 7,
	BTRFS_FT_XATTR     = 8,
	BTRFS_FT_MAX       = 9,
	BTRFS_FT_ENCRYPTED = 0x80 /* Directory contains encrypted data */
} BtrfsDirTypes;

typedef enum {
	/* disk I/O failure stats */
	BTRFS_DEV_STAT_WRITE_ERRS = 0, /* EIO or EREMOTEIO from lower layers */
	BTRFS_DEV_STAT_READ_ERRS,      /* EIO or EREMOTEIO from lower layers */
	BTRFS_DEV_STAT_FLUSH_ERRS,     /* EIO or EREMOTEIO from lower layers */
	/* stats for indirect indications for I/O failures */
	BTRFS_DEV_STAT_CORRUPTION_ERRS, /* checksum error, bytenr error or
									 * contents is illegal: this is an
									 * indication that the block was damaged
									 * during read or write, or written to
									 * wrong location or read from wrong
									 * location */
	BTRFS_DEV_STAT_GENERATION_ERRS, /* an indication that blocks have not
									 * been written */
	BTRFS_DEV_STAT_VALUES_MAX
} BtrfsDevStatValues;

static const char* dev_stat_value_str[] = {
	"BTRFS_DEV_STAT_WRITE_ERRS",
	"BTRFS_DEV_STAT_READ_ERRS",
	"BTRFS_DEV_STAT_FLUSH_ERRS",
	"BTRFS_DEV_STAT_CORRUPTION_ERRS",
	"BTRFS_DEV_STAT_GENERATION_ERRS",
};

#define BTRFS_AVAIL_ALLOC_BIT_SINGLE (1ULL << 48)
#define BTRFS_SPACE_INFO_GLOBAL_RSV	 (1ULL << 49)
#define BTRFS_BLOCK_GROUP_RESERVED (BTRFS_AVAIL_ALLOC_BIT_SINGLE | BTRFS_SPACE_INFO_GLOBAL_RSV)
typedef enum {
	BTRFS_BLOCK_GROUP_DATA         = (1 << 0),
	BTRFS_BLOCK_GROUP_SYSTEM       = (1 << 1),
	BTRFS_BLOCK_GROUP_METADATA     = (1 << 2),
	BTRFS_BLOCK_GROUP_RAID0        = (1 << 3),
	BTRFS_BLOCK_GROUP_RAID1        = (1 << 4),
	BTRFS_BLOCK_GROUP_DUP          = (1 << 5),
	BTRFS_BLOCK_GROUP_RAID10       = (1 << 6),
	BTRFS_BLOCK_GROUP_RAID5        = (1 << 7),
	BTRFS_BLOCK_GROUP_RAID6        = (1 << 8),
	BTRFS_BLOCK_GROUP_RAID1C3      = (1 << 9),
	BTRFS_BLOCK_GROUP_RAID1C4      = (1 << 10),
	BTRFS_BLOCK_GROUP_TYPE_MASK    = BTRFS_BLOCK_GROUP_DATA | BTRFS_BLOCK_GROUP_SYSTEM | BTRFS_BLOCK_GROUP_METADATA,
	BTRFS_BLOCK_GROUP_RAID56_MASK  = BTRFS_BLOCK_GROUP_RAID5 | BTRFS_BLOCK_GROUP_RAID6,
	BTRFS_BLOCK_GROUP_RAID1_MASK   = BTRFS_BLOCK_GROUP_RAID1 | BTRFS_BLOCK_GROUP_RAID1C3 | BTRFS_BLOCK_GROUP_RAID1C4,
	BTRFS_BLOCK_GROUP_PROFILE_MASK = BTRFS_BLOCK_GROUP_RAID0 | BTRFS_BLOCK_GROUP_RAID1 | BTRFS_BLOCK_GROUP_RAID1C3 |
									 BTRFS_BLOCK_GROUP_RAID1C4 | BTRFS_BLOCK_GROUP_RAID5 | BTRFS_BLOCK_GROUP_RAID6 | 
									 BTRFS_BLOCK_GROUP_DUP | BTRFS_BLOCK_GROUP_RAID10,
} BtrfsBlockGroups;

typedef enum { BTRFS_CRC32C = 0x00 } BtrfsCRCAlgorithms;

static const char* compatible_ro_features_str[] = {
	"BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE",
	"BTRFS_FEATURE_COMPAT_RO_FREE_SPACE_TREE_VALID",
	"BTRFS_FEATURE_COMPAT_RO_VERITY",
	"BTRFS_FEATURE_COMPAT_RO_BLOCK_GROUP_TREE",
};

static const char* incompatible_features_str[] = {
	"BTRFS_FEATURE_INCOMPAT_MIXED_BACKREF",
	"BTRFS_FEATURE_INCOMPAT_DEFAULT_SUBVOL",
	"BTRFS_FEATURE_INCOMPAT_MIXED_GROUPS",
	"BTRFS_FEATURE_INCOMPAT_COMPRESS_LZO",
	"BTRFS_FEATURE_INCOMPAT_COMPRESS_ZSTD",
	"BTRFS_FEATURE_INCOMPAT_BIG_METADATA",
	"BTRFS_FEATURE_INCOMPAT_EXTENDED_IREF",
	"BTRFS_FEATURE_INCOMPAT_RAID56",
	"BTRFS_FEATURE_INCOMPAT_SKINNY_METADATA",
	"BTRFS_FEATURE_INCOMPAT_NO_HOLES",
	"BTRFS_FEATURE_INCOMPAT_METADATA_UUID",
	"BTRFS_FEATURE_INCOMPAT_RAID1C34",
	"BTRFS_FEATURE_INCOMPAT_ZONED",
	"BTRFS_FEATURE_INCOMPAT_EXTENT_TREE_V2",
	"BTRFS_FEATURE_INCOMPAT_RAID_STRIPE_TREE",
	"BTRFS_INVALID_FEATURE_INCOMPAT",
	"BTRFS_FEATURE_INCOMPAT_SIMPLE_QUOTA",
};

static const char* block_groups_str[] = {
	"BTRFS_BLOCK_GROUP_DATA",
	"BTRFS_BLOCK_GROUP_SYSTEM",
	"BTRFS_BLOCK_GROUP_METADATA",
	"BTRFS_BLOCK_GROUP_RAID0",
	"BTRFS_BLOCK_GROUP_RAID1",
	"BTRFS_BLOCK_GROUP_DUP",
	"BTRFS_BLOCK_GROUP_RAID10",
	"BTRFS_BLOCK_GROUP_RAID5",
	"BTRFS_BLOCK_GROUP_RAID6",
	"BTRFS_BLOCK_GROUP_RAID1C3",
	"BTRFS_BLOCK_GROUP_RAID1C4",
};

static const char btrfs_magic[9] = "_BHRfS_M";

typedef u8 btrfs_csum_t[32];
typedef guid_t btrfs_uuid_t;

typedef struct {
	u64 low_range;
	u64 up_range;
	u64 phys_offset;
	u64 dev_id;
} qfs_btrfs_chunks;

typedef struct {
	u64 size;
	u64 start_lba;
	qfs_btrfs_chunks* chunks;
	u64 chunks_cnt;
	u64 node_size;
} qfs_btrfs_t;

typedef struct PACKED_STRUCT {
	s64 secs;     // Number of seconds since 1970-01-01T00:00:00Z.
	u32 nanosecs; // Number of nanoseconds since the beginning of the second.
} btrfs_time_t;

typedef btrfs_time_t btrfs_timespec_t;

typedef struct PACKED_STRUCT {
	u64 obj_id;   // Object ID. Each tree has its own set of Object IDs.
	u8 item_type;
	u64 offset;   // Offset. The meaning depends on the item type.
} btrfs_key_t;

// NOTE: that the disk key should be in little endian, and key_t in cpu native
// endianess (but we only work with little endian so they are always the same).
typedef btrfs_key_t btrfs_disk_key_t;

typedef struct PACKED_STRUCT {
	u64 dir_idx; // index in the directory
	u16 dir_name_size;
	// Follows ASCII dir_name for dir_name_size bytes
} btrfs_inode_ref_t;

typedef struct PACKED_STRUCT {
	u64 dir_idx;       // ID of directory in [tree id] that contains the subtree
	u64 sequence;      // Sequence (index in tree) (even, starting at 2?)
	u16 dir_name_size;
	// Follows ASCII dir_name for dir_name_size bytes
} btrfs_root_ref_t;

typedef struct PACKED_STRUCT {
	btrfs_disk_key_t location;
	u64 transid;
	u16 data_len;
	u16 name_len;
	u8 type;
} btrfs_dir_item_t;

typedef struct PACKED_STRUCT {
	u64 device_id;
	u64 bytes_cnt;       // number of bytes
	u64 used_bytes_cnt;  // number of bytes used
	u32 optimal_io_align;
	u32 optimal_io_width;
	u32 minimal_io_size; // aka sector size
	u64 type;
	u64 generation;
	u64 start_offset;
	u32 dev_group;
	u8 seek_speed;
	u8 bandwidth;
	btrfs_uuid_t device_uuid;
	btrfs_uuid_t fs_uuid;
} btrfs_dev_item_t;

typedef struct PACKED_STRUCT {
    u64 length;      // Size of chunk in bytes
    u64 owner;       // Root objectid that owns this chunk
    u64 stripe_len;  // Stripe length in bytes
    u64 type;        // RAID profile / block group flags
    u32 io_align;    // Optimal I/O alignment
    u32 io_width;    // Optimal I/O width
    u32 sector_size; // Minimal I/O size (usually FS sector size)
    u16 num_stripes; // Number of stripes
    u16 sub_stripes; // RAID10 / RAID56 sub-stripes
    // Followed by variable size array of struct btrfs_stripe stripes[num_stripes];
} btrfs_chunk_item_t;

typedef struct PACKED_STRUCT {
    u64 dev_id;             // Device ID
    u64 offset;             // Physical offset on that device
    btrfs_uuid_t dev_uuid;
} btrfs_stripe_t;

typedef struct PACKED_STRUCT {
	u64 tree_root;
	u64 tree_root_gen;
	u64 chunk_root;
	u64 chunk_root_gen;
	u64 extent_root;
	u64 extent_root_gen;
	u64 fs_root;
	u64 fs_root_gen;
	u64 dev_root;
	u64 dev_root_gen;
	u64 csum_root;
	u64 csum_root_gen;
	u64 total_bytes;
	u64 bytes_used;
	u64 num_devices;
	/* future */
	u64 unused_64[4];
	u8 tree_root_level;
	u8 chunk_root_level;
	u8 extent_root_level;
	u8 fs_root_level;
	u8 dev_root_level;
	u8 csum_root_level;
	/* future and to align */
	u8 unused_8[10];
} btrfs_root_backup_t;

typedef struct PACKED_STRUCT {
	btrfs_csum_t csum;
	btrfs_uuid_t fs_uuid;	
	u64 physical_address;
	u64 flags;
	u8 magic[8];
	u64 generation;
	u64 root_tree_root_lba;
	u64 chunk_tree_root_lba;
	u64 log_tree_root_lba;
	u64 log_root_transid;
	u64 total_bytes;
	u64 bytes_used;
	u64 root_dir_objectid;     // (usually 6)
	u64 num_devices;
	u32 sector_size;
	u32 node_size;
	u32 leaf_size;
	u32 stripe_size;
	u32 sys_chunk_array_size;
	u64 chunk_root_generation;
	u64 compat_flags;
	u64 compat_ro_flags;       // - only implementations that support the flags can write to the filesystem
	u64 incompat_flags;        // - only implementations that support the flags can use the filesystem
	u16 csum_type;             // - Btrfs currently uses the CRC32c little-endian hash function with seed -1.
	u8 root_tree_level;
	u8 chunk_root_level;
	u8 log_root_level;
	btrfs_dev_item_t dev_item;       // DEV_ITEM data for this device
	u8 label[0x100];           // (may not contain ‘/’ or ‘\\’)
	u64 cache_generation;
	u64 uuid_tree_generation;
	u8 reserved[0xf0];         // future expansion
	u8 sys_chunk_array[BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE]; // (n bytes valid) Contains (KEY, CHUNK_ITEM) pairs for all SYSTEM chunks. This is needed
							                            // to bootstrap the mapping from logical addresses to physical.
	btrfs_root_backup_t super_roots[BTRFS_NUM_BACKUP_ROOTS]; // Contain super_roots (4 btrfs_root_backup)
	u8 unused[0x235];
} btrfs_superblock_t;

typedef struct PACKED_STRUCT {
	btrfs_csum_t csum;            // Checksum of everything after this field (from 20 to the end of the node)
	btrfs_uuid_t fs_uuid;
	u64 node_lba;                 // Logical address of this node
	u8 flags[7];
	u8 backref;                   // Rev.: always 1 (MIXED) for new filesystems; 0 (OLD) indicates an old filesystem.
	btrfs_uuid_t chunk_tree_uuid;
	u64 generation;
	u64 tree_id;                  // The ID of the tree that contains this node
	u32 items_cnt;                // Number of items
	u8 level;                     // (0 for leaf nodes)
} btrfs_node_header_t;

typedef struct PACKED_STRUCT {
	btrfs_key_t key;
	u64 block_idx; // block_number
	u64 generation;
} btrfs_internal_node_t;

typedef struct PACKED_STRUCT {
	btrfs_key_t key;
	u32 offset;      // data offset relative to end of header (65)
	u32 size;        // data size;
} btrfs_leaf_node_t;

typedef struct PACKED_STRUCT {
	/* nfs style generation number */
	u64 generation;
	/* transid that last touched this inode */
	u64 transid;
	u64 size;
	u64 nbytes;
	u64 block_group;
	u32 nlink;
	u32 uid;
	u32 gid;
	u32 mode;
	u64 rdev;
	u64 flags;

	/* modification sequence number for NFS */
	u64 sequence;

	/*
	 * a little future expansion, for more than this we can
	 * just grow the inode item and version it
	 */
	u64 reserved[4];
	btrfs_timespec_t atime;
	btrfs_timespec_t ctime;
	btrfs_timespec_t mtime;
	btrfs_timespec_t otime;
} btrfs_inode_item_t;

typedef struct PACKED_STRUCT {
	btrfs_inode_item_t inode;
	u64 generation;
	u64 root_dirid;
	u64 bytenr;
	u64 byte_limit;
	u64 bytes_used;
	u64 last_snapshot;
	u64 flags;
	u32 refs;
	btrfs_disk_key_t drop_progress;
	u8 drop_level;
	u8 level;

	/*
	 * The following fields appear after subvol_uuids+subvol_times
	 * were introduced.
	 */

	/*
	 * This generation number is used to test if the new fields are valid
	 * and up to date while reading the root item. Every time the root item
	 * is written out, the "generation" field is copied into this field. If
	 * anyone ever mounted the fs with an older kernel, we will have
	 * mismatching generation values here and thus must invalidate the
	 * new fields. See btrfs_update_root and btrfs_find_last_root for
	 * details.
	 * The offset of generation_v2 is also used as the start for the memset
	 * when invalidating the fields.
	 */
	u64 generation_v2;
	btrfs_uuid_t uuid;
	btrfs_uuid_t parent_uuid;
	btrfs_uuid_t received_uuid;
	u64 ctransid; /* updated when an inode changes */
	u64 otransid; /* trans when created */
	u64 stransid; /* trans when sent. non-zero for received subvol */
	u64 rtransid; /* trans when received. non-zero for received subvol */
	btrfs_timespec_t ctime;
	btrfs_timespec_t otime;
	btrfs_timespec_t stime;
	btrfs_timespec_t rtime;
	u64 reserved[8]; /* for future */
} btrfs_root_item_t;

typedef enum ExtentDataField {
	NONE     = 0,
	INLINE   = 0,
	REGULAR  = 1,
	PREALLOC = 2
} ExtentDataField;

typedef enum BtrfsCompression {
	BTRFS_COMPRESSION_NONE    = 0,
	/* Data is compressed as a single zlib stream. */
	BTRFS_COMPRESSION_ZLIB    = 1,
	/*
	 * Data is compressed as a single zstd frame with the windowLog compression
	 * parameter set to no more than 17.
	 */
	BTRFS_COMPRESSION_ZSTD    = 2,
	/*
	 * Data is compressed sector by sector (using the sector size indicated by the
	 * name of the constant) with LZO1X and wrapped in the format documented in
	 * fs/btrfs/lzo.c. For writes, the compression sector size must match the
	 * filesystem sector size.
	 */
	BTRFS_COMPRESSION_LZO_4K  = 3,
	BTRFS_COMPRESSION_LZO_8K  = 4,
	BTRFS_COMPRESSION_LZO_16K = 5,
	BTRFS_COMPRESSION_LZO_32K = 6,
	BTRFS_COMPRESSION_LZO_64K = 7,
	BTRFS_COMPRESSION_TYPES   = 8
} BtrfsCompression;

static const char* extent_encryption_str[] = {
	"NONE",
	"UNKNOWN"
};

static const char* extent_other_encoding_str[] = {
	"NONE",
	"UNKNOWN"
};

static const char* extent_compression_str[] = {
	"BTRFS_COMPRESSION_NONE",
	"BTRFS_COMPRESSION_ZLIB",
	"BTRFS_COMPRESSION_ZSTD",
	"BTRFS_COMPRESSION_LZO_4K",
	"BTRFS_COMPRESSION_LZO_8K",
	"BTRFS_COMPRESSION_LZO_16K",
	"BTRFS_COMPRESSION_LZO_32K",
	"BTRFS_COMPRESSION_LZO_64K",
	"BTRFS_COMPRESSION_TYPES",
	"BTRFS_COMPRESSION_UNKNOWN"
};

static const char* extent_type_str[] = {
	"INLINE",
	"REGULAR",
	"PREALLOC",
	"UNKNOWN"
};

typedef struct PACKED_STRUCT {
	u64 generation;
	u64 size;           // (n) size of decoded extent
	u8 compression;     // (0=none, 1=zlib, 2=LZO)
	u8 encryption;      // (0=none)
	u16 other_encoding; // (0=none)
	u8 type;            // (0=inline, 1=regular, 2=prealloc)
} btrfs_extent_data_t;

typedef struct PACKED_STRUCT {
	u64 address;      // (ea) logical address of extent. If this is zero, the extent is sparse and consists of all zeroes.
	u64 size;         // (es) size of extent
	u64 offset;       // (o) offset within the extent
	u64 logical_size; // (s) logical number of bytes in file
} btrfs_extended_extent_data_t;

typedef struct PACKED_STRUCT {
	u64 refs;
	u64 generation;
	u64 flags;
} btrfs_extent_item_t;

typedef struct PACKED_STRUCT {
	u64 used;
	u64 chunk_obj_id;
	u64 flags;
} btrfs_block_group_item_t;

typedef struct PACKED_STRUCT {
	u64 chunk_tree;
	u64 chunk_objectid;
	u64 chunk_offset;
	u64 length;
	guid_t chunk_tree_uuid;
} btrfs_dev_extent_t;

typedef struct PACKED_STRUCT {
	u32 extent_count;
	u32 flags;
} btrfs_free_space_info_t;

// Validate Structs Sizes
STATIC_ASSERT(sizeof(btrfs_time_t)          == BTRFS_TIME_SIZE,          "BTRFS time size mismatch");
STATIC_ASSERT(sizeof(btrfs_key_t)           == BTRFS_KEY_SIZE,           "BTRFS key size mismatch");
STATIC_ASSERT(sizeof(btrfs_dev_item_t)      == BTRFS_DEV_ITEM_SIZE,      "BTRFS DEV ITEM size mismatch");
STATIC_ASSERT(sizeof(btrfs_chunk_item_t)    == BTRFS_CHUNK_ITEM_SIZE,    "BTRFS Chunk Item size mismatch");
STATIC_ASSERT(sizeof(btrfs_stripe_t)        == BTRFS_STRIPE_SIZE,        "BTRFS Stripe size mismatch");
STATIC_ASSERT(sizeof(btrfs_root_backup_t)   == BTRFS_ROOT_BACKUP_SIZE,   "BTRFS Root Backup size mismatch");
STATIC_ASSERT(sizeof(btrfs_superblock_t)    == BTRFS_SUPERBLOCK_SIZE,    "BTRFS SuperBlock size mismatch");
STATIC_ASSERT(sizeof(btrfs_node_header_t)   == BTRFS_NODE_HEADER_SIZE,   "BTRFS Node Header size mismatch");
STATIC_ASSERT(sizeof(btrfs_internal_node_t) == BTRFS_INTERNAL_NODE_SIZE, "BTRFS Internal Node size mismatch");
STATIC_ASSERT(sizeof(btrfs_leaf_node_t)     == BTRFS_LEAF_NODE_SIZE,     "BTRFS Leaf Node size mismatch");
STATIC_ASSERT(sizeof(btrfs_inode_ref_t)     == BTRFS_INODE_REF_SIZE,     "BTRFS Inode Ref size mismatch");
STATIC_ASSERT(sizeof(btrfs_root_ref_t)      == BTRFS_ROOT_REF_SIZE,      "BTRFS Root Ref size mismatch");

// Utility Functions
static void print_compat_ro_feature(BtrfsFlags btrfs_flag) {
	for (u8 i = 0; i < QCOW_ARR_SIZE(compatible_ro_features_str); ++i) {
		if ((btrfs_flag >> i) & 1) printf("%s | ", compatible_ro_features_str[i]);
	}
	return;
}

static void print_incompat_feature(BtrfsFlags btrfs_flag) {
	for (u8 i = 0; i < QCOW_ARR_SIZE(incompatible_features_str); ++i) {
		if ((btrfs_flag >> i) & 1) printf("%s | ", incompatible_features_str[i]);
	}
	return;
}

static void print_block_group(BtrfsBlockGroups btrfs_block_group) {
	for (u8 i = 0; i < QCOW_ARR_SIZE(block_groups_str); ++i) {
		if ((btrfs_block_group >> i) & 1) printf("%s | ", block_groups_str[i]);
	}
	return;
}

static void print_btrfs_dev_stats(const u64* dev_stats, const char* prefix, const u64 size) {
	printf(" -- Dev Stats --\n");
	for (u64 i = 0, j = 0; i < size && j < BTRFS_DEV_STAT_VALUES_MAX; i += sizeof(u64), ++j) {
		printf("%s  -> %s: 0x%llX\n", prefix, dev_stat_value_str[j], dev_stats[j]);
	}
	printf("---------------------------\n");
	return;
}

static void print_btrfs_free_space_info(const btrfs_key_t* key, const btrfs_free_space_info_t* free_space_info, const char* prefix) {
	printf(" -- Free Space Info --\n");
	printf("%s  -> size:         0x%llX\n", prefix, key -> offset);
	printf("%s  -> offset:       0x%llX\n", prefix, key -> obj_id);
	printf("%s  -> extent_count: 0x%X\n", prefix, free_space_info -> extent_count);
	printf("%s  -> flags:        0x%X\n", prefix, free_space_info -> flags);
	printf("---------------------------\n");
	return;
} 

static void print_btrfs_free_space_bitmap(const btrfs_key_t* key, const u8* free_space_bitmap, const char* prefix, const u64 size) {
	printf(" -- Free Space Bitmap from %llX to %llX --\n", key -> obj_id, key -> obj_id + key -> offset);
	for (u64 i = 0, t = 0; i < size; i += 8, ++t) {
		printf("%s\t", prefix);
		for (u8 j = 0; j < 8; ++j) {
			printf("%01u ", (free_space_bitmap[t] >> j) & 0x01);
		}
		printf("\n");
	}
	printf("---------------------------\n");
	return;
} 

static void print_btrfs_dev_extent(const btrfs_dev_extent_t* dev_extent, const char* prefix) {
	printf(" -- Dev Extent --\n");
	printf("%s  -> chunk_tree: 0x%llX\n", prefix, dev_extent -> chunk_tree);
	printf("%s  -> chunk_objectid: 0x%llX\n", prefix, dev_extent -> chunk_objectid);
	printf("%s  -> chunk_offset: 0x%llX\n", prefix, dev_extent -> chunk_offset);
	printf("%s  -> length: 0x%llX\n", prefix, dev_extent -> length);
	printf("%s  -> chunk_tree_uuid: ", prefix);
	print_guid(dev_extent -> chunk_tree_uuid);
	printf("\n");
	printf("---------------------------\n");
	return;
} 

static void print_btrfs_block_group_item(const btrfs_block_group_item_t* block_group_item, const char* prefix) {
	printf(" -- Block Group Item --\n");
	printf("%s  -> used:         0x%llX\n", prefix, block_group_item -> used);
	printf("%s  -> chunk_obj_id: 0x%llX\n", prefix, block_group_item -> chunk_obj_id);
	printf("%s  -> flags:        0x%llX\n", prefix, block_group_item -> flags);
	printf("---------------------------\n");
	return;
} 

static void print_btrfs_extent_item(const btrfs_extent_item_t* extent_item, const char* prefix, const u64 size) {
	printf(" -- Extent Item (%llu size) --\n", size);
	printf("%s  -> refs:       0x%llX\n", prefix, extent_item -> refs);
	printf("%s  -> generation: 0x%llX\n", prefix, extent_item -> generation);
	printf("%s  -> flags:      0x%llX\n", prefix, extent_item -> flags);
	printf("---------------------------\n");
	return;
} 

static void print_btrfs_extent_data(const btrfs_extent_data_t* extent_data, const char* prefix) {
	printf(" -- Extent Data --\n");
	printf("%s  -> generation:         0x%llX\n", prefix, extent_data -> generation);
	printf("%s  -> size:               0x%llX\n", prefix, extent_data -> size);
	printf("%s  -> compression(%02X):    %s\n", prefix, extent_data -> compression, extent_compression_str[MIN(extent_data -> compression, QCOW_ARR_SIZE(extent_compression_str))]);
	printf("%s  -> encryption(%02X):     %s\n", prefix, extent_data -> encryption, extent_encryption_str[MIN(extent_data -> encryption, QCOW_ARR_SIZE(extent_encryption_str))]);
	printf("%s  -> other_encoding(%02X): %s\n", prefix, extent_data -> other_encoding, extent_other_encoding_str[MIN(extent_data -> other_encoding, QCOW_ARR_SIZE(extent_other_encoding_str))]);
	printf("%s  -> type(%02X):           %s\n", prefix, extent_data -> type, extent_type_str[MIN(extent_data -> type, QCOW_ARR_SIZE(extent_type_str))]);
	printf("---------------------------\n");
	return;
}

static void print_btrfs_extended_extent_data(const btrfs_extended_extent_data_t* extended_extent_data, const char* prefix) {
	printf(" -- Extended Extent Data --\n");
	printf("%s  -> address:      0x%llX\n", prefix, extended_extent_data -> address);
	printf("%s  -> size:         0x%llX\n", prefix, extended_extent_data -> size);
	printf("%s  -> offset:       0x%llX\n", prefix, extended_extent_data -> offset);
	printf("%s  -> logical_size: 0x%llX\n", prefix, extended_extent_data -> logical_size);
	printf("---------------------------\n");
	return;
}

static void print_btrfs_inode_ref(const btrfs_inode_ref_t* inode_ref) {
	printf(" -- Inode Ref --\n");
	printf("  -> dir_idx: 0x%llX\n", inode_ref -> dir_idx);
	printf("  -> name:    '%.*s'\n", (int) inode_ref -> dir_name_size, QCOW_CAST_PTR(inode_ref, u8) + sizeof(btrfs_inode_ref_t));
	printf("---------------------------\n");
	return;
}

static void print_btrfs_root_ref(const btrfs_root_ref_t* root_ref) {
	printf(" -- Root Ref --\n");
	printf("  -> dir_idx:   0x%llX\n", root_ref -> dir_idx);
	printf("  -> sequence:  0x%llX\n", root_ref -> sequence);
	printf("  -> name:      '%.*s'\n", (int) root_ref -> dir_name_size, QCOW_CAST_PTR(root_ref, u8) + sizeof(btrfs_root_ref_t));
	printf("---------------------------\n");
	return;
}

static void print_dev_item(const btrfs_dev_item_t* dev_item) {
	printf("  -> dev_item:\n");
	printf("     -> device_id:         0x%llX\n", dev_item -> device_id);
	printf("     -> bytes_cnt:         0x%llX\n", dev_item -> bytes_cnt);
	printf("     -> used_bytes_cnt:    0x%llX\n", dev_item -> used_bytes_cnt);
	printf("     -> optimal_io_align:  0x%X\n", dev_item -> optimal_io_align);
	printf("     -> optimal_io_width:  0x%X\n", dev_item -> optimal_io_width);
	printf("     -> minimal_io_size:   0x%X\n", dev_item -> minimal_io_size);
	printf("     -> type:              0x%llX\n", dev_item -> type);
	printf("     -> generation:        0x%llX\n", dev_item -> generation);
	printf("     -> start_offset:      0x%llX\n", dev_item -> start_offset);
	printf("     -> dev_group:         0x%X\n", dev_item -> dev_group);
	printf("     -> seek_speed:        0x%X\n", dev_item -> seek_speed);
	printf("     -> bandwidth:         0x%X\n", dev_item -> bandwidth);
	printf("     -> device_uuid:       ");
	print_guid(dev_item -> device_uuid);
	printf("\n");
	printf("     -> fs_uuid:           ");
	print_guid(dev_item -> fs_uuid);
	printf("\n");
	return;
}

const u8 day_in_month[] = {
	31, /* January   */
    28, /* February  */
    31, /* March     */
    30, /* April     */
    31, /* May       */
    30, /* June      */
    31, /* July      */
    31, /* August    */
    30, /* September */
    31, /* October   */
    30, /* November  */
    31  /* December  */
};

static void print_btrfs_time(const btrfs_time_t* time) {
	u64 secs = time -> secs;
	const u32 seconds = secs % 60;
	secs -= seconds;
	
	u32 minutes = secs % 3600;
	secs -= minutes;
   	minutes /= 60;

	u32 hours = secs % (3600 * 24);
	secs -= hours;
	hours /= 3600;

	secs += 3600 * 24;
	u32 year = 1970;
	while (secs > (365 + (year % 4 == 0)) * (3600 * 24)) {
		secs -= (365 + (year % 4 == 0)) * (3600 * 24);	
		year++;
	}

	u32 month = 0;
	while (secs > day_in_month[month % 12] * (3600 * 24)) {
		secs -= day_in_month[month % 12] * (3600 * 24);	
		month++;
	}
	month++;
	
	const u32 day = secs / (3600 * 24);
	
	printf("%02u:%02u:%02u:%u %02u/%02u/%04u", hours, minutes, seconds, time -> nanosecs, day, month, year);
	
	return;
}

static void print_btrfs_key(const btrfs_key_t* key, const char* prefix) {
	printf("%s  -> key:\n", prefix);
	printf("%s      -> obj_id:    ", prefix);
	if ((s64) key -> obj_id < 0) printf("%lld\n", key -> obj_id);
	else printf("0x%llX\n", key -> obj_id);
	printf("%s      -> item_type: 0x%X\n",   prefix, key -> item_type);
	printf("%s      -> offset:    0x%llX\n", prefix, key -> offset);
	return;
}

static void print_btrfs_dir_item(const btrfs_dir_item_t* dir_items, const u64 dir_items_size) {
	printf(" -- Dir Items --\n");
	for (u64 i = 0; i < dir_items_size;) {
		const btrfs_dir_item_t* dir_item = QCOW_CAST_PTR(QCOW_CAST_PTR(dir_items, u8) + i, btrfs_dir_item_t);
		printf("  -> dir_item:\n");
		print_btrfs_key(&(dir_item -> location), "    ");
		printf("      -> transid:  0x%llX\n", dir_item -> transid);
		printf("      -> data_len: 0x%X\n", dir_item -> data_len);
		printf("      -> name_len: 0x%X\n", dir_item -> name_len);
		printf("      -> type:     0x%X\n", dir_item -> type);
		printf("      -> name:     '%.*s'\n", (int) dir_item -> name_len, QCOW_CAST_PTR(dir_item, u8) + sizeof(btrfs_dir_item_t));
		i += sizeof(btrfs_dir_item_t) + dir_item -> name_len;
		if (dir_item -> type == BTRFS_FT_XATTR) i += dir_item -> data_len;
	}
	printf("----------------------------------------\n");
	return;
}

static void print_btrfs_stripe(const btrfs_stripe_t* stripe, const char* prefix) {
    printf("%s      -> dev_id: 0x%llX\n", prefix, stripe -> dev_id);
    printf("%s      -> offset: 0x%llX\n", prefix, stripe -> offset);
    printf("%s      -> dev_uuid: ", prefix);
    print_guid(stripe -> dev_uuid);
    printf("\n");
	return;
}

static void print_btrfs_chunk_item(const btrfs_chunk_item_t* chunk_item, const char* prefix) {
	printf("%s  -> chunk_item:\n", prefix);
    printf("%s      -> length:      0x%llX\n", prefix, chunk_item -> length);
    printf("%s      -> owner:       0x%llX\n", prefix, chunk_item -> owner);
    printf("%s      -> stripe_len:  0x%llX\n", prefix, chunk_item -> stripe_len);
    printf("%s      -> type:        0x%llX\n", prefix, chunk_item -> type);
    printf("%s      -> io_align:    0x%X\n", prefix, chunk_item -> io_align);
    printf("%s      -> io_width:    0x%X\n", prefix, chunk_item -> io_width);
    printf("%s      -> sector_size: 0x%X\n", prefix, chunk_item -> sector_size);
    printf("%s      -> num_stripes: 0x%X\n", prefix, chunk_item -> num_stripes);
    printf("%s      -> sub_stripes: 0x%X\n", prefix, chunk_item -> sub_stripes);
	
	const btrfs_stripe_t* stripes = QCOW_CAST_PTR(QCOW_CAST_PTR(chunk_item, u8) + sizeof(btrfs_chunk_item_t), btrfs_stripe_t); 
	for (u16 i = 0; i < chunk_item -> num_stripes; ++i) {
		printf("%s      -> stripe %u:\n", prefix, i);
		char new_prefix[128] = {0};
		mem_cpy(new_prefix, prefix, str_len(prefix));
		mem_set(new_prefix + str_len(prefix), ' ', 4);
		print_btrfs_stripe(stripes + i, new_prefix);
	}

	return;
}

static void print_sys_chunk_array(const u8* sys_chunk_array, const u32 sys_chunk_array_size) {
	printf("  -> sys_chunk_array(%u bytes):\n", sys_chunk_array_size);
	u32 bytes_cnt = 0;
	for (u32 j = 0; bytes_cnt < sys_chunk_array_size && bytes_cnt < BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE; ++j) {
		printf("      -> chunk %u:\n", j);
		const btrfs_key_t* key = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_key_t);
		print_btrfs_key(key, "          ");
		bytes_cnt += sizeof(btrfs_key_t);
		btrfs_chunk_item_t* chunk_item = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_chunk_item_t);
		bytes_cnt += sizeof(btrfs_chunk_item_t) + sizeof(btrfs_stripe_t) * chunk_item -> num_stripes;
		if (bytes_cnt > sys_chunk_array_size) WARNING_LOG("bytes_cnt exceeds the sys_chunk_array: 0x%X > 0x%X\n", bytes_cnt, sys_chunk_array_size);
		print_btrfs_chunk_item(chunk_item, "          ");
	}
	if (bytes_cnt != sys_chunk_array_size) WARNING_LOG("bytes_cnt mismatch the sys_chunk_array: 0x%X != 0x%X\n", bytes_cnt, sys_chunk_array_size);
	return;
}

static void print_root_backup(const btrfs_root_backup_t* root_backups, const u32 root_backups_cnt) {
	for (u32 i = 0; i < root_backups_cnt; ++i) {
		printf("  -> root_backup %u:\n", i);
		printf("    -> tree_root:          0x%llX\n", (root_backups + i) -> tree_root);
		printf("    -> tree_root_gen:      0x%llX\n", (root_backups + i) -> tree_root_gen);
		printf("    -> chunk_root:         0x%llX\n", (root_backups + i) -> chunk_root);
		printf("    -> chunk_root_gen:     0x%llX\n", (root_backups + i) -> chunk_root_gen);
		printf("    -> extent_root:        0x%llX\n", (root_backups + i) -> extent_root);
		printf("    -> extent_root_gen:    0x%llX\n", (root_backups + i) -> extent_root_gen);
		printf("    -> fs_root:            0x%llX\n", (root_backups + i) -> fs_root);
		printf("    -> fs_root_gen:        0x%llX\n", (root_backups + i) -> fs_root_gen);
		printf("    -> dev_root:           0x%llX\n", (root_backups + i) -> dev_root);
		printf("    -> dev_root_gen:       0x%llX\n", (root_backups + i) -> dev_root_gen);
		printf("    -> csum_root:          0x%llX\n", (root_backups + i) -> csum_root);
		printf("    -> csum_root_gen:      0x%llX\n", (root_backups + i) -> csum_root_gen);
		printf("    -> total_bytes:        0x%llX\n", (root_backups + i) -> total_bytes);
		printf("    -> bytes_used:         0x%llX\n", (root_backups + i) -> bytes_used);
		printf("    -> num_devices:        0x%llX\n", (root_backups + i) -> num_devices);
		printf("    -> tree_root_level:    0x%X\n", (root_backups + i) -> tree_root_level);
		printf("    -> chunk_root_level:   0x%X\n", (root_backups + i) -> chunk_root_level);
		printf("    -> extent_root_level:  0x%X\n", (root_backups + i) -> extent_root_level);
		printf("    -> fs_root_level:      0x%X\n", (root_backups + i) -> fs_root_level);
		printf("    -> dev_root_level:     0x%X\n", (root_backups + i) -> dev_root_level);
		printf("    -> csum_root_level:    0x%X\n", (root_backups + i) -> csum_root_level);
	}
	return;
}

static void print_btrfs_superblock(const btrfs_superblock_t* superblock) {
	printf(" -- Superblock --\n");
	printf("  -> csum:                   ");
	for (int i = 31; i >= 0; --i) printf("%02X", (superblock -> csum)[i]);
	printf("\n");
	printf("  -> fs_uuid:                ");
	print_guid(superblock -> fs_uuid);
	printf("\n");
	printf("  -> physical_address:       0x%llX\n", superblock -> physical_address);
	printf("  -> flags:                  0x%llX\n", superblock -> flags);
	printf("  -> magic:                  '%.*s'\n", 8, superblock -> magic);
	printf("  -> generation:             0x%llX\n", superblock -> generation);
	printf("  -> root_tree_root_lba:     0x%llX\n", superblock -> root_tree_root_lba);
	printf("  -> chunk_tree_root_lba:    0x%llX\n", superblock -> chunk_tree_root_lba);
	printf("  -> log_tree_root_lba:      0x%llX\n", superblock -> log_tree_root_lba);
	printf("  -> log_root_transid:       0x%llX\n", superblock -> log_root_transid);
	printf("  -> total_bytes:            0x%llX\n", superblock -> total_bytes);
	printf("  -> bytes_used:             0x%llX\n", superblock -> bytes_used);
	printf("  -> root_dir_objectid:      0x%llX\n", superblock -> root_dir_objectid);
	printf("  -> num_devices:            0x%llX\n", superblock -> num_devices);
	printf("  -> sector_size:            0x%X\n", superblock -> sector_size);
	printf("  -> node_size:              0x%X\n", superblock -> node_size);
	printf("  -> leaf_size:              0x%X\n", superblock -> leaf_size);
	printf("  -> stripe_size:            0x%X\n", superblock -> stripe_size);
	printf("  -> sys_chunk_array_size:   0x%X\n", superblock -> sys_chunk_array_size);
	printf("  -> chunk_root_generation:  0x%llX\n", superblock -> chunk_root_generation);
	printf("  -> compat_flags:           0x%llX\n", superblock -> compat_flags);
	printf("  -> compat_ro_flags (%08llX): ", superblock -> compat_ro_flags);
	print_compat_ro_feature(superblock -> compat_ro_flags);
	printf("\n");
	printf("  -> incompat_flags  (%08llX): ", superblock -> incompat_flags);
	print_incompat_feature(superblock -> incompat_flags);
	printf("\n");
	printf("  -> csum_type:              0x%X\n", superblock -> csum_type);
	printf("  -> root_tree_level:             0x%X\n", superblock -> root_tree_level);
	printf("  -> chunk_root_level:       0x%X\n", superblock -> chunk_root_level);
	printf("  -> log_root_level:         0x%X\n", superblock -> log_root_level);
	print_dev_item(&(superblock -> dev_item));
	const int label_size = (int) MIN(str_len((const char*) superblock -> label), 0x100);
	printf("  -> label[%03d]:             '%.*s'\n", label_size, label_size, superblock -> label);
	printf("  -> cache_generation:       0x%llX\n", superblock -> cache_generation);
	printf("  -> uuid_tree_generation:   0x%llX\n", superblock -> uuid_tree_generation);
	print_sys_chunk_array(superblock -> sys_chunk_array, superblock -> sys_chunk_array_size);
	print_root_backup(superblock -> super_roots, BTRFS_NUM_BACKUP_ROOTS);
	printf("-----------------------------------\n");
	return;
}

static void print_btrfs_leaf_node_items(const btrfs_leaf_node_t* leaf_nodes, const u64 leaf_nodes_cnt) {
	printf(" -- Leaf Nodes --\n");
	for (u64 i = 0; i < leaf_nodes_cnt; ++i) {
		printf("  -> leaf_node %llu:\n", i);
		print_btrfs_key(&((leaf_nodes + i) -> key), "    ");
		printf("      -> offset: 0x%X\n", (leaf_nodes + i) -> offset);
		printf("      -> size:   %u\n", (leaf_nodes + i) -> size);
	}
	printf("-----------------------\n");
	return;
}

static void print_btrfs_node_header(const btrfs_node_header_t* header) {
	printf(" -- Node Header --\n");
	printf("  -> csum:            %X\n", *QCOW_CAST_PTR(header -> csum, u32));
	printf("  -> fs_uuid:         ");
	print_guid(header -> fs_uuid);
	printf("\n");
	printf("  -> node_lba:        %llX\n", header -> node_lba);
	printf("  -> flags:           ");
	for (unsigned char i = 0; i < 7; ++i) printf("%02X%s", (header -> flags)[i], i == 6 ? "\n" : " | ");
	printf("  -> backref:         %02X\n", header -> backref);                   
	printf("  -> chunk_tree_uuid: ");
	print_guid(header -> chunk_tree_uuid);
	printf("\n");
	printf("  -> generation:      %llX\n", header -> generation);
	printf("  -> tree_id:         %llX\n", header -> tree_id);
	printf("  -> items_cnt:       %u\n", header -> items_cnt);
	printf("  -> level:           %u\n", header -> level);
	printf("-----------------------------------\n");
	return;
}

static void print_btrfs_inode_item(const btrfs_inode_item_t* inode_item, const char* prefix) {
	printf("%s  -> generation:  0x%llX\n", prefix, inode_item -> generation);
	printf("%s  -> transid:     0x%llX\n", prefix, inode_item -> transid);
	printf("%s  -> size:        0x%llX\n", prefix, inode_item -> size);
	printf("%s  -> nbytes:      0x%llX\n", prefix, inode_item -> nbytes);
	printf("%s  -> block_group: 0x%llX\n", prefix, inode_item -> block_group);
	printf("%s  -> nlink:       0x%X\n", prefix, inode_item -> nlink);
	printf("%s  -> uid:         0x%X\n", prefix, inode_item -> uid);
	printf("%s  -> gid:         0x%X\n", prefix, inode_item -> gid);
	printf("%s  -> mode:        0x%X\n", prefix, inode_item -> mode);
	printf("%s  -> rdev:        0x%llX\n", prefix, inode_item -> rdev);
	printf("%s  -> flags:       0x%llX\n", prefix, inode_item -> flags);
	printf("%s  -> sequence:    0x%llX\n", prefix, inode_item -> sequence);

	printf("%s  -> atime:       ", prefix);
	print_btrfs_time(&(inode_item -> atime));
	printf("\n");
	printf("%s  -> ctime:       ", prefix);
	print_btrfs_time(&(inode_item -> ctime));
	printf("\n");
	printf("%s  -> mtime:       ", prefix);
	print_btrfs_time(&(inode_item -> mtime));
	printf("\n");
	printf("%s  -> otime:       ", prefix);
	print_btrfs_time(&(inode_item -> otime));
	printf("\n");
	
	return;
}

static void print_btrfs_root_item(const btrfs_root_item_t* root_item) {
	printf(" -- Root Item --\n");	
	printf("  -> inode:\n");
	print_btrfs_inode_item(&(root_item -> inode), "    ");
	printf("  -> generation:    0x%llX\n", root_item -> generation);
	printf("  -> root_dirid:    0x%llX\n", root_item -> root_dirid);
	printf("  -> bytenr:        0x%llX\n", root_item -> bytenr);
	printf("  -> byte_limit:    0x%llX\n", root_item -> byte_limit);
	printf("  -> bytes_used:    0x%llX\n", root_item -> bytes_used);
	printf("  -> last_snapshot: 0x%llX\n", root_item -> last_snapshot);
	printf("  -> flags:         0x%llX\n", root_item -> flags);
	printf("  -> refs:          0x%X\n", root_item -> refs);
	printf("  -> drop_progress: \n");
	print_btrfs_key(&(root_item -> drop_progress), "    ");
	printf("  -> drop_level:    0x%X\n", root_item -> drop_level);
	printf("  -> level:         %u\n", root_item -> level);

	/*
	 * The following fields appear after subvol_uuids+subvol_times
	 * were introduced.
	 */

	printf("  -> generation_v2: 0x%llX\n", root_item -> generation_v2);
	printf("  -> uuid:          ");
	print_guid(root_item -> uuid);
	printf("\n");
	printf("  -> parent_uuid:   ");
	print_guid(root_item -> parent_uuid);
	printf("\n");
	printf("  -> received_uuid: ");
	print_guid(root_item -> received_uuid);
	printf("\n");
	printf("  -> ctransid: 0x%llX\n", root_item -> ctransid);
	printf("  -> otransid: 0x%llX\n", root_item -> otransid);
	printf("  -> stransid: 0x%llX\n", root_item -> stransid);
	printf("  -> rtransid: 0x%llX\n", root_item -> rtransid);
	printf("  -> ctime:    ");
	print_btrfs_time(&(root_item -> ctime));
	printf("\n");
	printf("  -> otime:    ");
	print_btrfs_time(&(root_item -> otime));
	printf("\n");
	printf("  -> stime:    ");
	print_btrfs_time(&(root_item -> stime));
	printf("\n");
	printf("  -> rtime:    ");
	print_btrfs_time(&(root_item -> rtime));
	printf("\n");
	
	printf("-----------------------------------\n");

	return;
}

static void deallocate_qfs_btrfs(qfs_btrfs_t* qfs_btrfs) {
	QCOW_SAFE_FREE(qfs_btrfs -> chunks);
	return;
}

static int validate_btrfs_key(const btrfs_key_t* key, const u32 sector_size) {
	if (key -> offset % sector_size) {
		WARNING_LOG("Offset is unaligned with sector size.\n");
		return -QCOW_UNALIGNED_OFFSET;
	}
	return QCOW_NO_ERROR;
}

static int extend_chunk_mapping(qfs_btrfs_t* qfs_btrfs, const btrfs_key_t* key, const btrfs_chunk_item_t* chunk_item, const u32 sector_size) {
	if (chunk_item -> length % chunk_item -> stripe_len) {
		WARNING_LOG("Chunk item length must be aligned with stripe length.\n");
		return -QCOW_UNALIGNED_LENGTH;
	} else if (chunk_item -> num_stripes == 0) {
		WARNING_LOG("Chunk item cannot have zero stripes.\n");
		return -QCOW_CHUNK_ITEM_WITH_NO_STRIPES;
	} else if (sector_size != chunk_item -> sector_size) {
		WARNING_LOG("Mismatch between superblock's sector size and chunk item sector size.\n");
		return -QCOW_FIELD_MISMATCH;
	} else if (chunk_item -> stripe_len < chunk_item -> sector_size) {
		WARNING_LOG("Stripe length cannot be less than sector size.\n");
		return -QCOW_INVALID_STRIPE_LENGTH;
	}

	DEBUG_LOG("chunk type: ");
	/* print_block_group(chunk_item -> type & BTRFS_BLOCK_GROUP_PROFILE_MASK); */
	print_block_group(chunk_item -> type);
	printf("\n");
	
	qfs_btrfs -> chunks = qcow_realloc(qfs_btrfs -> chunks, sizeof(qfs_btrfs_chunks) * (qfs_btrfs -> chunks_cnt + chunk_item -> num_stripes));
	if (qfs_btrfs -> chunks == NULL) {
		WARNING_LOG("Failed to reallocate the chunks.\n");
		return -QCOW_IO_ERROR;
	}

	DEBUG_LOG("local_range: [%llX, %llX]\n", key -> offset, key -> offset + chunk_item -> length);

	const btrfs_stripe_t* stripes = QCOW_CAST_PTR(QCOW_CAST_PTR(chunk_item, u8) + sizeof(btrfs_chunk_item_t), btrfs_stripe_t); 
	for (u16 i = 0; i < chunk_item -> num_stripes; ++i) {
		if ((stripes + i) -> offset % chunk_item -> stripe_len) {
			WARNING_LOG("Stripe offset unaligned with stripe length.\n");
			return -QCOW_UNALIGNED_OFFSET;
		}
		const qfs_btrfs_chunks chunk = { .low_range = key -> offset, .up_range = key -> offset + chunk_item -> length, .phys_offset = (stripes + i) -> offset, .dev_id = stripes -> dev_id };
		DEBUG_LOG("mapped to: [%llX, %llX]\n", (stripes + i) -> dev_id, (stripes + i) -> offset);
		(qfs_btrfs -> chunks)[(qfs_btrfs -> chunks_cnt)++] = chunk;
	}

	return QCOW_NO_ERROR;
}

static int check_sys_chunk_array_size(qfs_btrfs_t* qfs_btrfs, const u32 sector_size, const u8* sys_chunk_array, const u32 sys_chunk_array_size) {
	if (sys_chunk_array_size > BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE) {
		WARNING_LOG("The sys chunk array size exceeds the max sys chunk array size: 0x%X > 0x%X\n", sys_chunk_array_size, BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE);
		return -QCOW_INVALID_SYS_CHUNK_ARRAY_SIZE;
	}

	int err = 0;
	u32 bytes_cnt = 0;
	for (u32 j = 0; bytes_cnt < sys_chunk_array_size && bytes_cnt < BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE; ++j) {
		const btrfs_key_t* key = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_key_t);
		bytes_cnt += sizeof(btrfs_key_t);
		if ((err = validate_btrfs_key(key, sector_size)) < 0) {
			WARNING_LOG("Failed to validate key.\n");
			return err;
		} else if (key -> obj_id != BTRFS_FIRST_CHUNK_TREE_OBJECTID) {
			WARNING_LOG("Expected First Chunk Tree Object ID, but found: %llX\n", key -> obj_id);
			return -QCOW_INVALID_OBJECT_ID;
		} else if (key -> item_type != BTRFS_CHUNK_ITEM_KEY) {
			WARNING_LOG("Expected Chunk Item Type, but found: %X\n", key -> item_type);
			return -QCOW_INVALID_ITEM_TYPE;
		}
		
		const btrfs_chunk_item_t* chunk_item = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_chunk_item_t);
		bytes_cnt += sizeof(btrfs_chunk_item_t);
	   	
		if ((err = extend_chunk_mapping(qfs_btrfs, key, chunk_item, sector_size)) < 0) {
			WARNING_LOG("Failed to extend chunk mapping.\n");
			return err;
		}

		bytes_cnt += sizeof(btrfs_stripe_t) * chunk_item -> num_stripes;

		if (bytes_cnt > sys_chunk_array_size) {
			WARNING_LOG("bytes_cnt exceeds the sys chunk array size: 0x%X > 0x%X\n", bytes_cnt, sys_chunk_array_size);
			return -QCOW_INVALID_SYS_CHUNK_ARRAY_SIZE;
		}
	}
	
	if (bytes_cnt != sys_chunk_array_size) {
		WARNING_LOG("bytes_cnt mismatch the sys chunk array size: 0x%X != 0x%X\n", bytes_cnt, sys_chunk_array_size);
		return -QCOW_INVALID_SYS_CHUNK_ARRAY_SIZE;
	}

	return QCOW_NO_ERROR;
}

static int validate_superblock(qfs_btrfs_t* qfs_btrfs, btrfs_superblock_t* superblock) {
	// Check the CSUM of the superblock
	if (superblock -> csum_type != BTRFS_CRC32C) {
		WARNING_LOG("Unknown CRC algorithm type: %X\n", superblock -> csum_type);
		return -QCOW_UNKNOWN_ALGORITHM;
	}
	
	// NOTE: the current size is u32, but in case of future introduction of
	// larger/different crc algorithms this should be updated
	const u8* buf = QCOW_CAST_PTR(superblock, u8) + sizeof(btrfs_csum_t);
	const u32 csum_check = crc32c(buf, BTRFS_SUPERBLOCK_SIZE - sizeof(btrfs_csum_t));
	if (mem_n_cmp(&csum_check, superblock -> csum, sizeof(u32))) {
		WARNING_LOG("CRC mismatch %X != %X\n", csum_check, *((u32*) superblock -> csum));
		return -QCOW_INVALID_CRC_CHECKSUM;
	}
	
	// Ensure that the magic is correct
	if (mem_n_cmp(superblock -> magic, btrfs_magic, sizeof(superblock -> magic))) {
		WARNING_LOG("Invalid magic: '%.*s'\n", (int) sizeof(superblock -> magic), superblock -> magic);
		return -QCOW_INVALID_BTRFS_MAGIC;
	}

	if (superblock -> physical_address != BTRFS_SUPERBLOCK_LOGICAL_OFFSET) {
		WARNING_LOG("Mismatch physical address: %llX != %X\n", superblock -> physical_address, BTRFS_SUPERBLOCK_LOGICAL_OFFSET);
		return -QCOW_INVALID_SUPERBLOCK_PHYSICAL_ADDRESS;
	}

	if (superblock -> sector_size % 2 || superblock -> sector_size > superblock -> node_size || superblock -> sector_size > superblock -> stripe_size) {
		WARNING_LOG("Invalid sector size: 0x%X\n", superblock -> sector_size);
		return -QCOW_INVALID_SIZE;
	} else if (superblock -> node_size % 2 || superblock -> node_size > BTRFS_MAX_NODE_SIZE) {
		WARNING_LOG("Invalid node size: 0x%X\n", superblock -> node_size);
		return -QCOW_INVALID_SIZE;
	} else if (superblock -> stripe_size % 2) {
		WARNING_LOG("Invalid stripe size: 0x%X\n", superblock -> stripe_size);
		return -QCOW_INVALID_SIZE;
	}

	if (superblock -> root_tree_level > BTRFS_MAX_LEVEL) {
		WARNING_LOG("Invalid root level: %X\n", superblock -> root_tree_level);
		return -QCOW_INVALID_LEVEL;
	} else if (superblock -> chunk_root_level > BTRFS_MAX_LEVEL) {
		WARNING_LOG("Invalid chunk root level: %X\n", superblock -> chunk_root_level);
		return -QCOW_INVALID_LEVEL;
	} else if (superblock -> log_root_level > BTRFS_MAX_LEVEL) {
		WARNING_LOG("Invalid log root level: %X\n", superblock -> log_root_level);
		return -QCOW_INVALID_LEVEL;
	}

	if (superblock -> root_tree_root_lba % superblock -> sector_size) {
		WARNING_LOG("Root Tree Root LBA must be aligned to sector.\n");
		return -QCOW_UNALIGNED_SECTOR;
	} else if (superblock -> chunk_tree_root_lba % superblock -> sector_size) {
		WARNING_LOG("Chunk Tree Root LBA must be aligned to sector.\n");
		return -QCOW_UNALIGNED_SECTOR;
	} else if (superblock -> log_tree_root_lba % superblock -> sector_size) {
		WARNING_LOG("Log Tree Root LBA must be aligned to sector.\n");
		return -QCOW_UNALIGNED_SECTOR;
	}

	if (superblock -> generation == 0x00) {
		WARNING_LOG("Invalid generation: %llX\n", superblock -> generation);
		return -QCOW_INVALID_GENERATION;
	} else if (superblock -> chunk_root_generation == 0x00) {
		WARNING_LOG("Invalid chunk root generation: %llX\n", superblock -> chunk_root_generation);
		return -QCOW_INVALID_GENERATION;
	}

	if (superblock -> num_devices == 0x00) {
		WARNING_LOG("Invalid num devices: 0x%llX\n", superblock -> num_devices);
		return -QCOW_INVALID_NUM_DEVICES;
	}

	if (check_sys_chunk_array_size(qfs_btrfs, superblock -> sector_size, superblock -> sys_chunk_array, superblock -> sys_chunk_array_size)) {
		WARNING_LOG("Invalid sys chunk array size: 0x%X\n", superblock -> sys_chunk_array_size);
		return -QCOW_INVALID_SYS_CHUNK_ARRAY_SIZE;
	}

	// TODO: Validate the btrfs root backups ?? 

	return QCOW_NO_ERROR;
}

static int validate_chunk_tree(const u8* chunk_tree, const u64 size, const guid_t fs_uuid, const u32 chunk_lba, const u8 chunk_level) {
	const btrfs_node_header_t zero_header = {0};
	const btrfs_node_header_t* header = QCOW_CAST_PTR(chunk_tree, btrfs_node_header_t);
	if (mem_n_cmp(header, &zero_header, sizeof(btrfs_node_header_t)) == 0) {
		DEBUG_LOG("Found empty header.\n");
		return -QCOW_EMPTY_TREE;
	}

	// NOTE: the current size is u32, but in case of future introduction of
	// larger/different crc algorithms this should be updated
	const u8* buf = QCOW_CAST_PTR(chunk_tree, u8) + sizeof(btrfs_csum_t); //BTRFS_SUPERBLOCK_CSUM_OFFSET;
	const u32 csum_check = crc32c(buf, size - sizeof(btrfs_csum_t));
	if (mem_n_cmp(&csum_check, header -> csum, sizeof(u32))) {
		printf("\n");
		WARNING_LOG("CRC mismatch %X != %X\n", csum_check, *((u32*) header -> csum));
		print_btrfs_node_header(header);
		return -QCOW_INVALID_CRC_CHECKSUM;
	}

	if (fs_uuid != NULL && mem_n_cmp(header -> fs_uuid, fs_uuid, sizeof(guid_t))) {
		WARNING_LOG("Mismatch in FS UUID, expected: ");
		print_guid(fs_uuid);
		printf(", but found: ");
		print_guid(header -> fs_uuid);
		printf("\n");
		return -QCOW_GUID_MISMATCH;
	} else if (header -> node_lba != chunk_lba) {
		WARNING_LOG("Mismatch node_lba and chunk_lba.\n");
		return -QCOW_FIELD_MISMATCH;
	} else if (header -> level != chunk_level) {
		WARNING_LOG("Mismatch node level and chunk level.\n");
		return -QCOW_FIELD_MISMATCH;
	}

	return QCOW_NO_ERROR;
}

static inline u64 btrfs_lba_to_physical_lba(const qfs_btrfs_t* qfs_btrfs, u64 btrfs_lba) {
	for (u64 i = 0; i < qfs_btrfs -> chunks_cnt; ++i) {
		if (btrfs_lba >= (qfs_btrfs -> chunks + i) -> low_range && btrfs_lba <= (qfs_btrfs -> chunks + i) -> up_range) {
			return (qfs_btrfs -> chunks + i) -> phys_offset + (btrfs_lba - (qfs_btrfs -> chunks + i) -> low_range);
		}
	}
	return -1ULL;
}

static int parse_btree(qfs_btrfs_t* qfs_btrfs, const btrfs_superblock_t superblock, const char* btree_name, const u64 btree_lba, const guid_t fs_uuid, const u8 level);

static int parse_node_header(qfs_btrfs_t* qfs_btrfs, const btrfs_superblock_t superblock, const btrfs_node_header_t* header, const u32 sector_size) {
	print_btrfs_node_header(header);
	
	if (header -> level == 0) {
		const btrfs_leaf_node_t* leaf_nodes = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t), btrfs_leaf_node_t);
		
		int err = 0;
		for (u64 i = 0; i < header -> items_cnt; ++i) {
			const btrfs_key_t key = (leaf_nodes + i) -> key;
			if (key.item_type == BTRFS_DEV_ITEM_KEY) {
				// TODO: Validate dev item
				print_dev_item(QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_dev_item_t));
			} else if (key.item_type == BTRFS_CHUNK_ITEM_KEY) {
				const btrfs_chunk_item_t* chunk_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_chunk_item_t);
				print_btrfs_chunk_item(chunk_item, "");
				if ((err = extend_chunk_mapping(qfs_btrfs, &key, chunk_item, sector_size)) < 0) {
					WARNING_LOG("Failed to extend the chunk mapping.\n");
					return err;
				}
			} else if (key.item_type == BTRFS_ROOT_ITEM_KEY) {
				printf("Obj ID: %lld\n", key.obj_id);
				const btrfs_root_item_t* root_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_root_item_t);
				print_btrfs_root_item(root_item);
				if (key.obj_id == BTRFS_FS_TREE_OBJECTID) {
					DEBUG_LOG("Found FS Root Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "fs", root_item -> bytenr, NULL, root_item -> level); */
				} else if (key.obj_id == BTRFS_EXTENT_TREE_OBJECTID) {
					DEBUG_LOG("Found Extent Root Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "extent", root_item -> bytenr, NULL, root_item -> level); */
				} else if (key.obj_id == BTRFS_DEV_TREE_OBJECTID) {
					DEBUG_LOG("Found Dev Root Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "dev", root_item -> bytenr, NULL, root_item -> level); */
				} else if (key.obj_id == BTRFS_CSUM_TREE_OBJECTID) {
					DEBUG_LOG("Found CSum Root Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "csum", root_item -> bytenr, NULL, root_item -> level); */
				} else if (key.obj_id == BTRFS_UUID_TREE_OBJECTID) {
					DEBUG_LOG("Found UUID Root Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "uuid", root_item -> bytenr, NULL, root_item -> level); */
				} else if (key.obj_id == BTRFS_FREE_SPACE_TREE_OBJECTID) {
					DEBUG_LOG("Found Free Space Root Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "free space", root_item -> bytenr, NULL, root_item -> level); */
				} else if (((s64) key.obj_id) >= BTRFS_FIRST_FREE_OBJECTID || ((s64) key.obj_id) <= BTRFS_LAST_FREE_OBJECTID) {
					DEBUG_LOG("Found File Tree %lld.\n", key.obj_id);
					/* err = parse_btree(qfs_btrfs, superblock, "file", root_item -> bytenr, NULL, root_item -> level); */
				} else if (key.obj_id == (u64) BTRFS_DATA_RELOC_TREE_OBJECTID) {
					DEBUG_LOG("Found Data Reloc Tree.\n");
					/* err = parse_btree(qfs_btrfs, superblock, "data reloc", root_item -> bytenr, NULL, root_item -> level); */
				}

				if (err < 0) {
					WARNING_LOG("Failed to perform parsing of tree.\n");
					return err;
				}
			} else if (key.item_type == BTRFS_INODE_REF_KEY) {
				const btrfs_inode_ref_t* inode_ref = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_inode_ref_t);
				print_btrfs_inode_ref(inode_ref);
			} else if (key.item_type == BTRFS_DIR_ITEM_KEY || key.item_type == BTRFS_DIR_INDEX_KEY) {
				const btrfs_dir_item_t* dir_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_dir_item_t);
				print_btrfs_dir_item(dir_item, (leaf_nodes + i) -> size);
			} else if (key.item_type == BTRFS_XATTR_ITEM_KEY) {
				printf(" -- XAttr Item --\n");
				const btrfs_dir_item_t* dir_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_dir_item_t);
				print_btrfs_dir_item(dir_item, (leaf_nodes + i) -> size);
				printf("--------------------------------\n");
			} else if (key.item_type == BTRFS_ROOT_REF_KEY || key.item_type == BTRFS_ROOT_BACKREF_KEY) {
				const btrfs_root_ref_t* root_ref = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_root_ref_t);
				print_btrfs_root_ref(root_ref);
			} else if (key.item_type == BTRFS_INODE_ITEM_KEY) {
				const btrfs_inode_item_t* inode_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_inode_item_t);
				printf(" -- Inode Item --\n");
				print_btrfs_inode_item(inode_item, "    ");
				printf("--------------------------------\n");
			} else if (key.item_type == BTRFS_EXTENT_DATA_ITEM_KEY) {
				const btrfs_extent_data_t* extent_data = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_extent_data_t);
				print_btrfs_extent_data(extent_data, "    ");
				if (extent_data -> type != INLINE) {
					const btrfs_extended_extent_data_t* extended_extent_data = QCOW_CAST_PTR(QCOW_CAST_PTR(extent_data, u8) + sizeof(btrfs_extent_data_t), btrfs_extended_extent_data_t);
					print_btrfs_extended_extent_data(extended_extent_data, "    ");
				}
			} else if (key.item_type == BTRFS_EXTENT_ITEM_KEY) {
				const btrfs_extent_item_t* extent_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_extent_item_t);
				print_btrfs_extent_item(extent_item, "    ", (leaf_nodes + i) -> size);
			} else if (key.item_type == BTRFS_METADATA_ITEM_KEY) {
				printf(" -- Metadata Item --\n");
				const btrfs_extent_item_t* extent_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_extent_item_t);
				print_btrfs_extent_item(extent_item, "    ", (leaf_nodes + i) -> size);
				printf("--------------------------------\n");
			} else if (key.item_type == BTRFS_BLOCK_GROUP_KEY) {
				const btrfs_block_group_item_t* block_group_item = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_block_group_item_t);
				print_btrfs_block_group_item(block_group_item, "    ");
			} else if (key.item_type == BTRFS_DEV_EXTENT_KEY) {
				const btrfs_dev_extent_t* dev_extent = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_dev_extent_t);
				print_btrfs_dev_extent(dev_extent, "    ");
			} else if (key.item_type == BTRFS_DEV_STATS_KEY) {
				const u64* dev_stats = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, u64);
				print_btrfs_dev_stats(dev_stats, "    ", (leaf_nodes + i) -> size);
			} else if (key.item_type == BTRFS_CSUM_ITEM_KEY) {
				printf("-- CSum Item --\n");
				const u32* csums = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, u32);
				for (u64 i = 0, j = 0; i < (leaf_nodes + i) -> size; ++j, i += sizeof(u32)) {
					printf("  -> csum %llu: 0x%X\n", j, csums[j]);
				}
				printf("--------------------------------\n");
			} else if (key.item_type == BTRFS_UUID_KEY) {
				printf("-- UUID Item --\n");
				const u8* uuid = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, u8);
				printf("  -> uuid: ");
			   	print_guid(uuid);
				printf("\n");
				printf("--------------------------------\n");
			} else if (key.item_type == BTRFS_FREE_SPACE_INFO_KEY) {
				const btrfs_free_space_info_t* free_space_info = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset, btrfs_free_space_info_t);
				print_btrfs_free_space_info(&key, free_space_info, "    ");
			} else if (key.item_type == BTRFS_FREE_SPACE_BITMAP_KEY) {
				const u8* free_space_info = QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t) + (leaf_nodes + i) -> offset;
				print_btrfs_free_space_bitmap(&key, free_space_info, "    ", (leaf_nodes + i) -> size);
			} else if (key.item_type == BTRFS_FREE_SPACE_EXTENT_KEY) {
				printf(" -- Free Space Extent --\n");
				printf("  -> size: %llX\n", key.offset);
				printf("  -> offset: %llX\n", key.obj_id);
				printf("--------------------------------\n");
			} else {
				DEBUG_LOG("  -> leaf_node %llu:\n", i);
				print_btrfs_key(&key, "    ");
				printf("      -> offset: 0x%X\n", (leaf_nodes + i) -> offset);
				printf("      -> size:   %u\n", (leaf_nodes + i) -> size);
				return -QCOW_UNKNOWN_ITEM_TYPE;
			}
		}

		return QCOW_NO_ERROR;
	}
	
	const btrfs_internal_node_t* internal_nodes = QCOW_CAST_PTR(QCOW_CAST_PTR(header, u8) + sizeof(btrfs_node_header_t), btrfs_internal_node_t);
	for (u64 i = 0; i < header -> items_cnt; ++i) {
		int err = parse_btree(qfs_btrfs, superblock, "internal node", (internal_nodes + i) -> block_idx, NULL, header -> level - 1);
		if (err < 0) {
			const btrfs_key_t key = (internal_nodes + i) -> key;
			WARNING_LOG("An error occurred while parsing the item.\n");
			DEBUG_LOG("item_type: %u, blockidx: %llu, generation: %llu\n", key.item_type, (internal_nodes + i) -> block_idx, (internal_nodes + i) -> generation);
			return err;
		}
	}

	return QCOW_NO_ERROR;
}

static int parse_btree(qfs_btrfs_t* qfs_btrfs, const btrfs_superblock_t superblock, const char* btree_name, const u64 btree_lba, const guid_t fs_uuid, const u8 level) {
	int err = 0;
	
	DEBUG_LOG("Parsing '%s' btree...\n", btree_name);
	u8* btree = qcow_calloc(superblock.node_size, sizeof(u8));
	if (btree == NULL) {
		WARNING_LOG("Failed to allocate btree buffer.\n");
		return -QCOW_IO_ERROR;
	}

	u64 btree_btrfs_lba = btrfs_lba_to_physical_lba(qfs_btrfs, btree_lba);
	// TODO: Should use the correct sector_size
	/* if (btree_btrfs_lba % superblock.sector_size) { */
	if (btree_btrfs_lba % SECTOR_SIZE) {
		WARNING_LOG("Trying to perform an unaligned sector read.");
		return -QCOW_UNALIGNED_SECTOR;
	}

	btree_btrfs_lba = qfs_btrfs -> start_lba + btree_btrfs_lba / SECTOR_SIZE;
	if (get_n_sector_at(btree_btrfs_lba, superblock.node_size / SECTOR_SIZE, btree)) {
		QCOW_SAFE_FREE(btree);
		WARNING_LOG("Failed to get btree sectors.\n");
		return -QCOW_IO_ERROR;
	}

	err = validate_chunk_tree(btree, superblock.node_size, fs_uuid, btree_lba, level);
	if (err == -QCOW_EMPTY_TREE) {
		QCOW_SAFE_FREE(btree);
		return QCOW_NO_ERROR;
	} else if (err < 0) {
		WARNING_LOG("Failed to validate '%s' btree.\n", btree_name);
		return err;
	}

	const btrfs_node_header_t* btree_header = QCOW_CAST_PTR(btree, btrfs_node_header_t);
	if ((err = parse_node_header(qfs_btrfs, superblock, btree_header, superblock.sector_size)) < 0) {
		WARNING_LOG("Failed to parse node header.\n");
		return err;
	}
	
	QCOW_SAFE_FREE(btree);
	
	DEBUG_LOG("'%s' btree successfully parsed.\n", btree_name);

	return QCOW_NO_ERROR;
}

static int parse_superblock(qfs_btrfs_t* qfs_btrfs) {
	btrfs_superblock_t superblock = {0};
	if (get_n_sector_at(qfs_btrfs -> start_lba + BTRFS_SUPERBLOCK_OFFSET, sizeof(btrfs_superblock_t) / SECTOR_SIZE, &superblock)) {
		WARNING_LOG("Failed to get superblock sectors.\n");
		return -QCOW_IO_ERROR;
	}

	int err = 0;
	if ((err = validate_superblock(qfs_btrfs, &superblock)) < 0) {
		WARNING_LOG("Failed to validate the superblock.\n");
		return err;
	}

	print_btrfs_superblock(&superblock);
	
	if ((err = parse_btree(qfs_btrfs, superblock, "chunk", superblock.chunk_tree_root_lba, superblock.fs_uuid, superblock.chunk_root_level)) < 0) {
		WARNING_LOG("Failed to parse the chunk root.\n");
		return err;
	}

	if ((err = parse_btree(qfs_btrfs, superblock, "root", superblock.root_tree_root_lba, superblock.fs_uuid, superblock.root_tree_level)) < 0) {
		WARNING_LOG("Failed to parse the chunk root.\n");
		return err;
	}

	TODO("Implement me!");
	return -QCOW_TODO;
}

static int parse_btrfs_fs(qfs_btrfs_t* qfs_btrfs) {
	int err = 0;
	if ((err = parse_superblock(qfs_btrfs)) < 0) {
		deallocate_qfs_btrfs(qfs_btrfs);
		WARNING_LOG("Failed to parse the superblock.\n");
		return err;
	}
	
	deallocate_qfs_btrfs(qfs_btrfs);

	TODO("Implement me!");
	return -QCOW_TODO;
}

static int btrfs_lookup(qfs_btrfs_t* qfs_btrfs, char name[BTRFS_MAX_NAME], const char* s_name) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

static int btrfs_readdir(qfs_btrfs_t* qfs_btrfs, u32* entry_idx, char name[BTRFS_MAX_NAME]) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

static int btrfs_read(qfs_btrfs_t* qfs_btrfs, u64* offset, void* buf, u64 size) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

#endif //_QCOW_BTRFS_H_


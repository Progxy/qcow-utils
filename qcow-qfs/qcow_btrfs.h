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
	DEV_ITEM_SIZE                   = 0x62,
	BTRFS_CHUNK_ITEM_SIZE           = 0x30,
	BTRFS_STRIPE_SIZE               = 0x20,
	BTRFS_ROOT_BACKUP_SIZE          = 0xA8,
	BTRFS_SUPERBLOCK_SIZE           = 0x1000,
	BTRFS_NODE_HEADER_SIZE          = 0x65,
	BTRFS_INTERNAL_NODE_SIZE        = 0x21,
	BTRFS_LEAF_NODE_SIZE            = 0x19,
	BTRFS_SUPERBLOCK_CSUM_OFFSET    = 0x20,
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
} BtrfsObjectIds;

typedef enum {
	BTRFS_FREE_SPACE_BITMAP_KEY = 200,
	BTRFS_DEV_EXTENT_KEY        = 204,
	BTRFS_DEV_ITEM_KEY          = 216,
	BTRFS_CHUNK_ITEM_KEY        = 228,
	BTRFS_RAID_STRIPE_KEY       = 230,
} BtrfsItemTypes;

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
	u64 size;
	u64 start_lba;
} qfs_btrfs_t;

typedef struct PACKED_STRUCT {
	s64 secs;     // Number of seconds since 1970-01-01T00:00:00Z.
	u32 nanosecs; // Number of nanoseconds since the beginning of the second.
} btrfs_time_t;

typedef struct PACKED_STRUCT {
	u64 obj_id;   // Object ID. Each tree has its own set of Object IDs.
	u8 item_type;
	u64 offset;   // Offset. The meaning depends on the item type.
} btrfs_key_t;

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
} dev_item_t;

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
    u64 offset;            // Physical offset on that device
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
	u8 root_level;
	u8 chunk_root_level;
	u8 log_root_level;
	dev_item_t dev_item;       // DEV_ITEM data for this device
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
	u32 offset;      // data offset relative to end of header (65);
	u32 size;        // data size;
} btrfs_leaf_node_t;

// Validate Structs Sizes
STATIC_ASSERT(sizeof(btrfs_time_t)          == BTRFS_TIME_SIZE,          "BTRFS time size mismatch");
STATIC_ASSERT(sizeof(btrfs_key_t)           == BTRFS_KEY_SIZE,           "BTRFS key size mismatch");
STATIC_ASSERT(sizeof(dev_item_t)            == DEV_ITEM_SIZE,            "DEV_ITEM size mismatch");
STATIC_ASSERT(sizeof(btrfs_chunk_item_t)    == BTRFS_CHUNK_ITEM_SIZE,    "BTRFS Chunk Item size mismatch");
STATIC_ASSERT(sizeof(btrfs_stripe_t)        == BTRFS_STRIPE_SIZE,        "BTRFS Stripe size mismatch");
STATIC_ASSERT(sizeof(btrfs_root_backup_t)   == BTRFS_ROOT_BACKUP_SIZE,   "BTRFS Root Backup size mismatch");
STATIC_ASSERT(sizeof(btrfs_superblock_t)    == BTRFS_SUPERBLOCK_SIZE,    "BTRFS SuperBlock size mismatch");
STATIC_ASSERT(sizeof(btrfs_node_header_t)   == BTRFS_NODE_HEADER_SIZE,   "BTRFS Node Header size mismatch");
STATIC_ASSERT(sizeof(btrfs_internal_node_t) == BTRFS_INTERNAL_NODE_SIZE, "BTRFS Internal Node size mismatch");
STATIC_ASSERT(sizeof(btrfs_leaf_node_t)     == BTRFS_LEAF_NODE_SIZE,     "BTRFS Leaf Node size mismatch");

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

static void print_dev_item(const dev_item_t* dev_item) {
	printf("dev_item:\n");
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

static void print_btrfs_key(const btrfs_key_t* key, const char* prefix) {
	printf("%s  -> key:\n", prefix);
	printf("%s      -> obj_id:    0x%llX\n", prefix, key -> obj_id);
	printf("%s      -> item_type: 0x%X\n",   prefix, key -> item_type);
	printf("%s      -> offset:    0x%llX\n", prefix, key -> offset);
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
	printf("  -> root_level:             0x%X\n", superblock -> root_level);
	printf("  -> chunk_root_level:       0x%X\n", superblock -> chunk_root_level);
	printf("  -> log_root_level:         0x%X\n", superblock -> log_root_level);
	printf("  -> ");
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

static int check_sys_chunk_array_size(const u32 sector_size, const u8* sys_chunk_array, const u32 sys_chunk_array_size) {
	if (sys_chunk_array_size > BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE) {
		WARNING_LOG("The sys chunk array size exceeds the max sys chunk array size: 0x%X > 0x%X\n", sys_chunk_array_size, BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE);
		return -QCOW_INVALID_SYS_CHUNK_ARRAY_SIZE;
	}

	u32 bytes_cnt = 0;
	for (u32 j = 0; bytes_cnt < sys_chunk_array_size && bytes_cnt < BTRFS_MAX_SYS_CHUNK_ARRAY_SIZE; ++j) {
		const btrfs_key_t* key = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_key_t);
		bytes_cnt += sizeof(btrfs_key_t);
		if (key -> offset % sector_size) {
			WARNING_LOG("Offset is unaligned with sector size.\n");
			return -QCOW_UNALIGNED_OFFSET;
		} else if (key -> obj_id != BTRFS_FIRST_CHUNK_TREE_OBJECTID) {
			WARNING_LOG("Expected First Chunk Tree Object ID, but found: %llX\n", key -> obj_id);
			return -QCOW_INVALID_OBJECT_ID;
		} else if (key -> item_type != BTRFS_CHUNK_ITEM_KEY) {
			WARNING_LOG("Expected Chunk Item Type, but found: %X\n", key -> item_type);
			return -QCOW_INVALID_ITEM_TYPE;
		}
		
		const btrfs_chunk_item_t* chunk_item = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_chunk_item_t);
		bytes_cnt += sizeof(btrfs_chunk_item_t);
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
		print_block_group(chunk_item -> type & BTRFS_BLOCK_GROUP_PROFILE_MASK);
		printf("\n");
		DEBUG_LOG("local_range: [%llX, %llX]\n", key -> offset, key -> offset + chunk_item -> length);

		const btrfs_stripe_t* stripes = QCOW_CAST_PTR(sys_chunk_array + bytes_cnt, btrfs_stripe_t); 
	   	bytes_cnt += sizeof(btrfs_stripe_t) * chunk_item -> num_stripes;
		for (u16 i = 0; i < chunk_item -> num_stripes; ++i) {
			if ((stripes + i) -> offset % chunk_item -> stripe_len) {
				WARNING_LOG("Stripe offset unaligned with stripe length.\n");
				return -QCOW_UNALIGNED_OFFSET;
			}
			DEBUG_LOG("mapped to: [%llX, %llX]\n", (stripes + i) -> dev_id, (stripes + i) -> offset);
		}

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

static int validate_superblock(btrfs_superblock_t* superblock) {
	// Check the CSUM of the superblock
	if (superblock -> csum_type != BTRFS_CRC32C) {
		WARNING_LOG("Unknown CRC algorithm type: %X\n", superblock -> csum_type);
		return -QCOW_UNKNOWN_ALGORITHM;
	}
	
	// NOTE: the current size is u32, but in case of future introduction of
	// larger/different crc algorithms this should be updated
	const u8* buf = QCOW_CAST_PTR(superblock, u8) + BTRFS_SUPERBLOCK_CSUM_OFFSET;
	const u32 csum_check = crc32c(buf, BTRFS_SUPERBLOCK_SIZE - BTRFS_SUPERBLOCK_CSUM_OFFSET);
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

	if (superblock -> root_level > BTRFS_MAX_LEVEL) {
		WARNING_LOG("Invalid root level: %X\n", superblock -> root_level);
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

	if (check_sys_chunk_array_size(superblock -> sector_size, superblock -> sys_chunk_array, superblock -> sys_chunk_array_size)) {
		WARNING_LOG("Invalid sys chunk array size: 0x%X\n", superblock -> sys_chunk_array_size);
		return -QCOW_INVALID_SYS_CHUNK_ARRAY_SIZE;
	}

	// TODO: Validate the btrfs root backups ?? 

	return QCOW_NO_ERROR;
}

static int parse_superblock(qfs_btrfs_t* qfs_btrfs) {
	btrfs_superblock_t superblock = {0};
	if (get_n_sector_at(qfs_btrfs -> start_lba + BTRFS_SUPERBLOCK_OFFSET, sizeof(btrfs_superblock_t) / SECTOR_SIZE, &superblock)) {
		return -QCOW_IO_ERROR;
	}

	int err = 0;
	if ((err = validate_superblock(&superblock)) < 0) {
		WARNING_LOG("Failed to validate the superblock.\n");
		return err;
	}

	print_btrfs_superblock(&superblock);
	
	TODO("Implement me!");
	return -QCOW_TODO;
}

static int parse_btrfs_fs(qfs_btrfs_t* qfs_btrfs) {
	int err = 0;
	if ((err = parse_superblock(qfs_btrfs)) < 0) {
		WARNING_LOG("Failed to parse the superblock.\n");
		return err;
	}

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


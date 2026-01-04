#ifndef _QCOW_BTRFS_H_
#define _QCOW_BTRFS_H_

typedef enum {
	BTRFS_SUPERBLOCK_OFFSET = 128,
	BTRFS_NUM_BACKUP_ROOTS  = 4,
	BTRFS_OLD               = 0,
	BTRFS_MIXED             = 1,
	BTRFS_LEAF_NODE         = 0
} QCowBtrfsConstants;

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
    u64 devid;             // Device ID
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
	u32 sectorsize;
	u32 nodesize;
	u32 leafsize;
	u32 stripesize;
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
	u8 sys_chunk_array[0x800]; // (n bytes valid) Contains (KEY, CHUNK_ITEM) pairs for all SYSTEM chunks. This is needed
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
STATIC_ASSERT(sizeof(btrfs_time_t)          == 0x0C,   "BTRFS time size mismatch");
STATIC_ASSERT(sizeof(btrfs_key_t)           == 0x11,   "BTRFS key size mismatch");
STATIC_ASSERT(sizeof(dev_item_t)            == 0x62,   "DEV_ITEM size mismatch");
STATIC_ASSERT(sizeof(btrfs_chunk_item_t)    == 0x30,   "BTRFS Chunk Item size mismatch");
STATIC_ASSERT(sizeof(btrfs_stripe_t)        == 0x20,   "BTRFS Stripe size mismatch");
STATIC_ASSERT(sizeof(btrfs_root_backup_t)   == 0xA8,   "BTRFS Root Backup size mismatch");
STATIC_ASSERT(sizeof(btrfs_superblock_t)    == 0x1000, "BTRFS SuperBlock size mismatch");
STATIC_ASSERT(sizeof(btrfs_node_header_t)   == 0x65,   "BTRFS Node Header size mismatch");
STATIC_ASSERT(sizeof(btrfs_internal_node_t) == 0x21,   "BTRFS Internal Node size mismatch");
STATIC_ASSERT(sizeof(btrfs_leaf_node_t)     == 0x19,   "BTRFS Leaf Node size mismatch");

// Utility Functions
static void print_dev_item(const dev_item_t * dev_item) {
	TODO("Implement me!");
	return;
}

static void print_sys_chunk_array(const u8* sys_chunk_array, const u32 sys_chunk_array_size) {
	TODO("Implement me!");
	return;
}

static void print_root_backup(const btrfs_root_backup_t* root_backups, const u32 root_backups_cnt) {
	TODO("Implement me!");
	return;
}

static void print_btrfs_superblock(const btrfs_superblock_t* superblock) {
	printf("csum:                   ");
	for (int i = 31; i >= 0; --i) printf("%02X", (superblock -> csum)[i]);
	printf("\n");
	printf("fs_uuid:                ");
	print_guid(superblock -> fs_uuid);
	printf("\n");
	printf("physical_address:       0x%llX\n", superblock -> physical_address);
	printf("flags:                  0x%llX\n", superblock -> flags);
	printf("magic:                  '%.*s'\n", 8, superblock -> magic);
	printf("generation:             0x%llX\n", superblock -> generation);
	printf("root_tree_root_lba:     0x%llX\n", superblock -> root_tree_root_lba);
	printf("chunk_tree_root_lba:    0x%llX\n", superblock -> chunk_tree_root_lba);
	printf("log_tree_root_lba:      0x%llX\n", superblock -> log_tree_root_lba);
	printf("log_root_transid:       0x%llX\n", superblock -> log_root_transid);
	printf("total_bytes:            0x%llX\n", superblock -> total_bytes);
	printf("bytes_used:             0x%llX\n", superblock -> bytes_used);
	printf("root_dir_objectid:      0x%llX\n", superblock -> root_dir_objectid);
	printf("num_devices:            0x%llX\n", superblock -> num_devices);
	printf("sectorsize:             0x%X\n", superblock -> sectorsize);
	printf("nodesize:               0x%X\n", superblock -> nodesize);
	printf("leafsize:               0x%X\n", superblock -> leafsize);
	printf("stripesize:             0x%X\n", superblock -> stripesize);
	printf("sys_chunk_array_size:   0x%X\n", superblock -> sys_chunk_array_size);
	printf("chunk_root_generation:  0x%llX\n", superblock -> chunk_root_generation);
	printf("compat_flags:           0x%llX\n", superblock -> compat_flags);
	printf("compat_ro_flags:        0x%llX\n", superblock -> compat_ro_flags);
	printf("incompat_flags:         0x%llX\n", superblock -> incompat_flags);
	printf("csum_type:              0x%X\n", superblock -> csum_type);
	printf("root_level:             0x%X\n", superblock -> root_level);
	printf("chunk_root_level:       0x%X\n", superblock -> chunk_root_level);
	printf("log_root_level:         0x%X\n", superblock -> log_root_level);
	print_dev_item(&(superblock -> dev_item));
	const int label_size = (int) MIN(str_len((const char*) superblock -> label), 0x100);
	printf("label[%03d]:             '%.*s'\n", label_size, label_size, superblock -> label);
	printf("cache_generation:       0x%llX\n", superblock -> cache_generation);
	printf("uuid_tree_generation:   0x%llX\n", superblock -> uuid_tree_generation);
	print_root_backup(superblock -> super_roots, BTRFS_NUM_BACKUP_ROOTS);
	return;
}

static int parse_btrfs_fs(qfs_btrfs_t* qfs_btrfs) {
	btrfs_superblock_t superblock = {0};
	if (get_n_sector_at(qfs_btrfs -> start_lba + BTRFS_SUPERBLOCK_OFFSET, sizeof(btrfs_superblock_t) / SECTOR_SIZE, &superblock)) {
		return -QCOW_IO_ERROR;
	}

	print_btrfs_superblock(&superblock);

	TODO("Implement me!");
	return -QCOW_TODO;
}

#endif //_QCOW_BTRFS_H_


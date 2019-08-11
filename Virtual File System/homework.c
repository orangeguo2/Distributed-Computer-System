/*
 * file:        homework.c
 * description: skeleton file for CS 5600/7600 file system
 *
 * CS 5600, Computer Systems, Northeastern CCIS
 * Peter Desnoyers, November 2016
 * Philip Gust, March 2019
 */

#define FUSE_USE_VERSION 27

#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fuse.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "fsx600.h"
#include "blkdev.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//extern int homework_part;       /* set by '-part n' command-line option */

/*
 * disk access - the global variable 'disk' points to a blkdev
 * structure which has been initialized to access the image file.
 *
 * NOTE - blkdev access is in terms of 1024-byte blocks
 */
extern struct blkdev *disk;

/* by defining bitmaps as 'fd_set' pointers, you can use existing
 * macros to handle them.
 *   FD_ISSET(##, inode_map);
 *   FD_CLR(##, block_map);
 *   FD_SET(##, block_map);
 */

/** pointer to inode bitmap to determine free inodes */
static fd_set *inode_map;
static int     inode_map_base;
static int inode_map_sz;

/** pointer to inode blocks */
static struct fs_inode *inodes;
/** number of inodes from superblock */
static int   n_inodes;
/** number of first inode block */
static int   inode_base;

/** pointer to block bitmap to determine free blocks */
fd_set *block_map;
/** number of first data block bitmap */
static int     block_map_base;
static int block_map_sz;

/** number of available blocks from superblock */
static int   n_blocks;

/** number of data blocks */
static int n_data_blocks;

/** number of first data block */
static int data_block_base;

/** pointer to data blocks */
static void *data_blocks;

/** number of root inode from superblock */
static int   root_inode;

/** array of dirty metadata blocks to write  -- optional */
static void **dirty;

/** length of dirty array -- optional */
static int    dirty_len;

static int get_used_blocks_num();
static int find_in_dir(struct fs_dirent *de, char *name);
static struct fs_inode *get_inode(int inode);
static int get_data_blocks_num(struct fs_inode *inode, int data_block_nums[]);
static void get_inode_stat(struct fs_inode *inode, struct stat *st);
static void *get_data_block(int number);
static int translate_dirent(char *path, char *name);
static void write_inode_to_disk(int num);
static void write_data_block_to_disk(int num);
static int get_free_inode(void);

/* Suggested functions to implement -- you are free to ignore these
 * and implement your own instead
 */

static void write_inode_to_disk(int inum) {
    int inode_offset = inum / INODES_PER_BLK;
    int offset = inode_base + inode_offset;
    if (disk->ops->write(disk, offset, 1, (void*) inodes + inode_offset * FS_BLOCK_SIZE) < 0) {
        exit(1);
    }
}

static void write_data_block_to_disk(int num) {
    if (disk->ops->write(disk, num, 1, data_blocks + (num - data_block_base) * FS_BLOCK_SIZE) < 0) {
        exit(1);
    }
}

static void write_inode_map_to_disk() {
    if (disk->ops->write(disk, inode_map_base, inode_map_sz, inode_map) < 0) {
        exit(1);
    }
}

static void write_block_map_to_disk() {
    if (disk->ops->write(disk, block_map_base, block_map_sz, block_map) < 0) {
        exit(1);
    }
}
/**
 * Look up a single directory entry in a directory.
 *
 * Errors
 *   -EIO     - error reading block
 *   -ENOENT  - a component of the path is not present.
 *   -ENOTDIR - intermediate component of path not a directory
 *
 */
static int lookup(int inum, char *name)
{
    struct fs_inode *inode_ptr = get_inode(inum);

    if (S_ISDIR(inode_ptr->mode)) {
        struct fs_dirent *de = get_data_block(inode_ptr->direct[0]);

        int inode = find_in_dir(de, name);
        if (inode == 0) {
            return -ENOENT;
        } else {
            return inode;
        }
    } else {
        return -ENOTDIR;
    }
}

static struct fs_inode *get_inode(int inum) {
    return (void*)inodes + inum * sizeof(struct fs_inode);
}

static void *get_data_block(int number) {
    return data_blocks + (number - data_block_base) * FS_BLOCK_SIZE;
}

static void get_inode_stat(struct fs_inode *inode, struct stat *st) {
    st->st_blocks = S_ISDIR(inode->mode) ? 0 : get_data_blocks_num(inode, NULL);
    st->st_size = inode->size;
    st->st_mode = inode->mode;
    st->st_nlink = 1;
    st->st_uid = inode->uid;
    st->st_gid = inode->gid;
    st->st_ctime = inode->ctime;
    st->st_mtime = inode->mtime;
}

static int get_data_blocks_num(struct fs_inode *inode, int data_block_nums[]) {
    int count = 0;

    // report on direct blocks
    for (int i = 0; i < N_DIRECT; i++) {
        if (inode->direct[i] != 0) {
            if (data_block_nums != NULL) {
                data_block_nums[count] = inode->direct[i];
            }
            count++;
        }
    }

    // report on single indirect blocks
    if (inode->indir_1 != 0) {
        int *buf = get_data_block(inode->indir_1);
        for (int i = 0; i < PTRS_PER_BLK; i++) {
            if (buf[i] != 0) {
                if (data_block_nums != NULL) {
                    data_block_nums[count] = buf[i];
                }
                count++;
            }
        }
    }

    // report on double indirect blocks
    if (inode->indir_2 != 0) {
        int *buf2 = get_data_block(inode->indir_2);
        // scan indirect block
        for (int i = 0; i < PTRS_PER_BLK; i++) {
            if (buf2[i] != 0) {
                // scan double-indirect block
                int *buf = get_data_block(buf2[i]);
                for (int j = 0; j < PTRS_PER_BLK; j++) {
                    if (buf[j] != 0) {
                        if (data_block_nums != NULL) {
                            data_block_nums[count] = buf[j];
                        }
                        count++;
                    }
                }
            }
        }
    }

    return count;
}
/**
 * Parse path name into tokens at most nnames tokens after
 * normalizing paths by removing '.' and '..' elements.
 *
 * If names is NULL,path is not altered and function  returns
 * the path count. Otherwise, path is altered by strtok() and
 * function returns names in the names array, which point to
 * elements of path string.
 *
 * @param path the directory path
 * @param names the argument token array or NULL
 * @param nnames the maximum number of names, 0 = unlimited
 * @return the number of path name tokens
 */
static int parse(char *path, char *names[], int nnames)
{
    char *path_copy = malloc(sizeof(char) * (strlen(path) + 1));
    strcpy(path_copy, path);
    char *use_path = path;

    if (names == NULL) {
        use_path = path_copy;
    }

    char *delim = "/";
    char *ptr = strtok(use_path, delim);
    int count = 0;
    int i = 0;

    while (ptr != NULL)
    {
        if (*ptr == '.') {
            ptr = strtok(NULL, delim);
        } else if (nnames == 0 || count < nnames) {
            count++;

            if (names != NULL) {
                names[i] = ptr;
                i++;
            }
            ptr = strtok(NULL, delim);
        } else {
            break;
        }
    }

    return count;
}

/* Return inode number for specified file or
 * directory.
 *
 * Errors
 *   -ENOENT  - a component of the path is not present.
 *   -ENOTDIR - an intermediate component of path not a directory
 *
 * @param path the file path
 * @return inode of path node or error
 */
static int translate(char *path)
{
    int count = parse(path, NULL, 0);
    char *names[count];
    parse(path, names, 0);

    int inum = root_inode;
    for (int i = 0; i < count; i++) {
        inum = lookup(inum, names[i]);
        if (inum < 0) {
            return inum;
        }
    }

    return inum;
}

static int translate_dirent(char *path, char *name) {
    int count = parse(path, NULL, 0);
    char *names[count];
    parse(path, names, 0);

    int inum = root_inode;
    for (int i = 0; i < count - 1; i++) {
        inum = lookup(inum, names[i]);
        if (inum < 0) {
            return inum;
        }
    }

    strcpy(name, names[count - 1]);
    return inum;
}


/**
 *  Return inode number for path to specified file
 *  or directory, and a leaf name that may not yet
 *  exist.
 *
 * Errors
 *   -ENOENT  - a component of the path is not present.
 *   -ENOTDIR - an intermediate component of path not a directory
 *
 * @param path the file path
 * @param leaf pointer to space for FS_FILENAME_SIZE leaf name
 * @return inode of path node or error
 */
static int translate_1(char *path, char *leaf)
{
    return -EOPNOTSUPP;
}

/**
 * Mark a inode as dirty.
 *
 * @param in pointer to an inode
 */
static void mark_inode(struct fs_inode *in)
{
    int inum = in - inodes;
    int blk = inum / INODES_PER_BLK;
    dirty[inode_base + blk] = (void*)inodes + blk * FS_BLOCK_SIZE;
}

/**
 * Flush dirty metadata blocks to disk.
 */
void flush_metadata(void)
{
    int i;
    for (i = 0; i < dirty_len; i++) {
        if (dirty[i]) {
            disk->ops->write(disk, i, 1, dirty[i]);
            dirty[i] = NULL;
        }
    }
}

/**
 * Returns a free block number or 0 if none available.
 *
 * @return free block number or 0 if none available
 */
static int get_free_blk(void)
{
    for (int i = 1; i < n_data_blocks; i++) {
        if (!FD_ISSET(i, block_map)) {
            return i;
        }
    }
    return 0;
}

/**
 * Return a block to the free list
 *
 * @param  blkno the block number
 */
static void return_blk(int blkno)
{
    FD_CLR(blkno, block_map);
    write_block_map_to_disk();
}

/**
 * Returns a free inode number
 *
 * @return a free inode number or 0 if none available
 */
static int get_free_inode(void)
{
    for (int i = 1; i < n_inodes; i++) {
        if (!FD_ISSET(i, inode_map)) {
            return i;
        }
    }
    return 0;
}

/**
 * Return a inode to the free list.
 *
 * @param  inum the inode number
 */
static void return_inode(int inum)
{
    FD_CLR(inum, inode_map);
    write_block_map_to_disk();
}

/**
 * Find inode for existing directory entry.
 *
 * @param fs_dirent ptr to first dirent in directory
 * @param name the name of the directory entry
 * @return the entry inode, or 0 if not found.
 */
static int find_in_dir(struct fs_dirent *de, char *name)
{
    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (de[i].valid && strcmp(de[i].name, name) == 0) {
            return de[i].inode;
        }
    }
    return 0;
}

/**
 * Find free directory entry.
 *
 * @return index of directory free entry or -ENOSPC
 *   if no space for new entry in directory
 */
static int find_free_dir(struct fs_dirent *de)
{
    return -ENOSPC;
}

/**
 * Determines whether directory is empty.
 *
 * @param de ptr to first entry in directory
 * @return 1 if empty 0 if has entries
 */
static int is_empty_dir(struct fs_dirent *de)
{
    return -ENOSPC;
}

/**
 * Returns the n-th block of the file, or allocates
 * it if it does not exist and alloc == 1.
 *
 * @param in the file inode
 * @param n the 0-based block index in file
 * @param alloc 1=allocate block if does not exist 0 = fail
 *   if does not exist
 * @return block number of the n-th block or 0 if available
 */
static int get_blk(struct fs_inode *in, int n, int alloc)
{
    int idx = 0;
    // report on direct blocks
    for (int i = 0; i < N_DIRECT; i++) {
        if (in->direct[i] != 0) {
            if (idx == n) {
                return in->direct[i];
            }
        } else if (alloc == 1) {
            int free_data_block = get_free_blk();
            if (free_data_block == 0) {
                return 0;
            }
            in->direct[i] = free_data_block;
            FD_SET(free_data_block, block_map);
            write_block_map_to_disk();

            if (idx == n) {
                return in->direct[i];
            }
        }
        idx++;
    }

    if (in->indir_1 == 0 && alloc == 1) {
        int free_data_block = get_free_blk();
        if (free_data_block == 0) {
            return 0;
        }
        in->indir_1 = free_data_block;
        FD_SET(free_data_block, block_map);
        write_block_map_to_disk();

        void *data_block = get_data_block(in->indir_1);
        memset(data_block, 0, FS_BLOCK_SIZE);
        write_data_block_to_disk(in->indir_1);
    }

    // report on single indirect blocks
    if (in->indir_1 != 0) {
        int *buf = get_data_block(in->indir_1);
        for (int i = 0; i < PTRS_PER_BLK; i++) {
            if (buf[i] != 0) {
                if (idx == n) {
                    return buf[i];
                }
            } else if (alloc == 1) {
                int free_data_block = get_free_blk();
                if (free_data_block == 0) {
                    return 0;
                }
                buf[i] = free_data_block;
                write_data_block_to_disk(in->indir_1);
                FD_SET(free_data_block, block_map);
                write_block_map_to_disk();

                if (idx == n) {
                    return buf[i];
                }
            }
            idx++;
        }
    }

    if (in->indir_2 == 0 && alloc == 1) {
        int free_data_block = get_free_blk();
        if (free_data_block == 0) {
            return 0;
        }
        in->indir_2 = free_data_block;
        FD_SET(free_data_block, block_map);
        write_block_map_to_disk();

        void *data_block = get_data_block(in->indir_2);
        memset(data_block, 0, FS_BLOCK_SIZE);
        write_data_block_to_disk(in->indir_2);
    }

    // report on double indirect blocks
    if (in->indir_2 != 0) {
        int *buf2 = get_data_block(in->indir_2);
        // scan indirect block
        for (int i = 0; i < PTRS_PER_BLK; i++) {
            if (buf2[i] == 0 && alloc == 1) {
                int free_data_block = get_free_blk();
                if (free_data_block == 0) {
                    return 0;
                }
                buf2[i] = free_data_block;
                write_data_block_to_disk(in->indir_2);
                FD_SET(free_data_block, block_map);
                write_block_map_to_disk();

                void *data_block = get_data_block(buf2[i]);
                memset(data_block, 0, FS_BLOCK_SIZE);
                write_data_block_to_disk(buf2[i]);
            }

            if (buf2[i] != 0) {
                // scan double-indirect block
                int *buf = get_data_block(buf2[i]);
                for (int j = 0; j < PTRS_PER_BLK; j++) {
                    if (buf[j] != 0) {
                        if (idx == n) {
                            return buf[j];
                        }
                    } else if (alloc == 1) {
                        int free_data_block = get_free_blk();
                        if (free_data_block == 0) {
                            return 0;
                        }
                        buf[j] = free_data_block;
                        write_data_block_to_disk(buf2[i]);
                        FD_SET(free_data_block, block_map);
                        write_block_map_to_disk();

                        if (idx == n) {
                            return buf[j];
                        }
                    }
                    idx++;
                }
            }
        }
    }
    return 0;
}

/* Fuse functions
 */

/**
 * init - this is called once by the FUSE framework at startup.
 *
 * This is a good place to read in the super-block and set up any
 * global variables you need. You don't need to worry about the
 * argument or the return value.
 *
 * @param conn fuse connection information - unused
 * @return unused - returns NULL
 */
void* fs_init(struct fuse_conn_info *conn)
{
    // read the superblock
    struct fs_super sb;
    if (disk->ops->read(disk, 0, 1, &sb) < 0) {
        exit(1);
    }

    root_inode = sb.root_inode;

    /* The inode map and block map are written directly to the disk after the superblock */

    // read inode map
    inode_map_base = 1;
    inode_map = malloc(sb.inode_map_sz * FS_BLOCK_SIZE);
    inode_map_sz = sb.inode_map_sz;
    if (disk->ops->read(disk, inode_map_base, sb.inode_map_sz, inode_map) < 0) {
        exit(1);
    }

    // read block map
    block_map_base = inode_map_base + sb.inode_map_sz;
    block_map = malloc(sb.block_map_sz * FS_BLOCK_SIZE);
    block_map_sz = sb.block_map_sz;
    if (disk->ops->read(disk, block_map_base, sb.block_map_sz, block_map) < 0) {
        exit(1);
    }

    /* The inode data is written to the next set of blocks */
    inode_base = block_map_base + sb.block_map_sz;
    n_inodes = sb.inode_region_sz * INODES_PER_BLK;
    inodes = malloc(sb.inode_region_sz * FS_BLOCK_SIZE);
    if (disk->ops->read(disk, inode_base, sb.inode_region_sz, inodes) < 0) {
        exit(1);
    }

    // number of blocks on device
    n_blocks = sb.num_blocks;

    // dirty metadata blocks
    dirty_len = inode_base + sb.inode_region_sz;
    dirty = calloc(dirty_len*sizeof(void*), 1);

    /* The data blocks are written to the next set of blocks */
    data_block_base = inode_base + sb.inode_region_sz;
    n_data_blocks = n_blocks - 1 - sb.inode_map_sz - sb.block_map_sz - sb.inode_region_sz;
    data_blocks = malloc(n_data_blocks * FS_BLOCK_SIZE);
    if (disk->ops->read(disk, data_block_base, n_data_blocks, data_blocks) < 0) {
        exit(1);
    }

    return NULL;
}

/* Note on path translation errors:
 * In addition to the method-specific errors listed below, almost
 * every method can return one of the following errors if it fails to
 * locate a file or directory corresponding to a specified path.
 *
 * ENOENT - a component of the path is not present.
 * ENOTDIR - an intermediate component of the path (e.g. 'b' in
 *           /a/b/c) is not a directory
 */

/* note on splitting the 'path' variable:
 * the value passed in by the FUSE framework is declared as 'const',
 * which means you can't modify it. The standard mechanisms for
 * splitting strings in C (strtok, strsep) modify the string in place,
 * so you have to copy the string and then free the copy when you're
 * done. One way of doing this:
 *
 *    char *_path = strdup(path);
 *    int inum = translate(_path);
 *    free(_path);
 */

/**
 * getattr - get file or directory attributes. For a description of
 * the fields in 'struct stat', see 'man lstat'.
 *
 * Note - fields not provided in CS5600fs are:
 *    st_nlink - always set to 1
 *    st_atime, st_ctime - set to same value as st_mtime
 *
 * Errors
 *   -ENOENT  - a component of the path is not present.
 *   -ENOTDIR - an intermediate component of path not a directory
 *
 * @param path the file path
 * @param sb pointer to stat struct
 * @return 0 if successful, or -error number
 */
static int fs_getattr(const char *path, struct stat *sb)
{
    char *_path = strdup(path);
    int inum = translate(_path);

    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);
    get_inode_stat(inode, sb);

    free(_path);
    return 0;
}

/**
 * readdir - get directory contents.
 *
 * For each entry in the directory, invoke the 'filler' function,
 * which is passed as a function pointer, as follows:
 *     filler(buf, <name>, <statbuf>, 0)
 * where <statbuf> is a struct stat, just like in getattr.
 *
 * Errors
 *   -ENOENT  - a component of the path is not present.
 *   -ENOTDIR - an intermediate component of path not a directory
 *
 * @param path the directory path
 * @param ptr  filler buf pointer
 * @param filler filler function to call for each entry
 * @param offset the file offset -- unused
 * @param fi the fuse file information
 * @return 0 if successful, or -error number
 */
static int fs_readdir(const char *path, void *ptr, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi)
{
    char *_path = strdup(path);
    int inum = (fi == NULL) ? translate(_path) : fi->fh;

    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);

    if (!S_ISDIR(inode->mode)) {
        free(_path);
        return -ENOTDIR;
    }

    struct fs_dirent *de = get_data_block(inode->direct[0]);

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (!de[i].valid) {
            continue;
        }

        struct fs_inode *entry_inode = get_inode(de[i].inode);
        struct stat st;
        get_inode_stat(entry_inode, &st);
        filler(NULL, de[i].name, &st, 0);
    }

    free(_path);
    return 0;
}

/**
 * open - open file directory.
 *
 * You can save information about the open directory in
 * fi->fh. If you allocate memory, free it in fs_releasedir.
 *
 * Errors
 *   -ENOENT  - a component of the path is not present.
 *   -ENOTDIR - an intermediate component of path not a directory
 *
 * @param path the file path
 * @param fi fuse file system information
 * @return 0 if successful, or -error number
 */
static int fs_opendir(const char *path, struct fuse_file_info *fi)
{
    if (fi == NULL) {
        return 0;
    }

    char *_path = strdup(path);
    int inum = translate(_path);

    if (inum < 0) {
        return inum;
    }

    fi->fh = inum;

    struct fs_inode *inode = get_inode(inum);

    if (!S_ISDIR(inode->mode)) {
        free(_path);
        return -ENOTDIR;
    }

    free(_path);
    return 0;
}

/**
 * Release resources when directory is closed.
 * If you allocate memory in fs_opendir, free it here.
 *
 * @param path the directory path
 * @param fi fuse file system information
 * @return 0 if successful, or -error number
 */
static int fs_releasedir(const char *path, struct fuse_file_info *fi)
{
    if (fi == NULL) {
        return 0;
    }

    int inum = fi->fh;

    if (inum < 0) {
        return inum;
    }

    fi->fh = 0;

    struct fs_inode *inode = get_inode(inum);

    if (!S_ISDIR(inode->mode)) {
        return -ENOTDIR;
    }

    return 0;
}

/**
 * mknod - create a new file with permissions (mode & 01777)
 * minor device numbers extracted from mode. Behavior undefined
 * when mode bits other than the low 9 bits are used.
 *
 * The access permissions of path are constrained by the
 * umask(2) of the parent process.
 *
 * Errors
 *   -ENOTDIR  - component of path not a directory
 *   -EEXIST   - file already exists
 *   -ENOSPC   - free inode not available
 *   -ENOSPC   - results in >32 entries in directory
 *
 * @param path the file path
 * @param mode the mode, indicating block or character-special file
 * @param dev the character or block I/O device specification
 * @return 0 if successful, or -error number
 */
static int fs_mknod(const char *path, mode_t mode, dev_t dev)
{
    char *_path = strdup(path);
    char name[FS_FILENAME_SIZE];
    int dirent_inum = translate_dirent(_path, &name);

    if (dirent_inum < 0) {
        free(_path);
        return dirent_inum;
    }

    struct fs_inode *dirent_inode = get_inode(dirent_inum);
    struct fs_dirent *de = get_data_block(dirent_inode->direct[0]);

    bool isFull = true;
    int available_idx = -1;

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (!de[i].valid) {
            isFull = false;

            if (available_idx == -1) {
                available_idx = i;
            }
            continue;
        }

        if (strcmp(de[i].name, name) == 0) {
            free(_path);
            return -EEXIST;
        }
    }

    if (isFull) {
        free(_path);
        return -ENOSPC;
    }

    int free_inum = get_free_inode();
    if (free_inum == 0) {
        free(_path);
        return -ENOSPC;
    }

    struct fs_dirent new_de;
    strcpy(new_de.name, name);
    new_de.inode = free_inum;
    new_de.valid = true;
    new_de.isDir = false;

    de[available_idx] = new_de;
    write_data_block_to_disk(dirent_inode->direct[0]);

    struct fs_inode inode;
    inode.mode = mode | S_IFREG;
    struct fuse_context *ctx = fuse_get_context();
    inode.uid = (ctx->pid == 0) ? getuid() : ctx->uid;
    inode.gid = (ctx->pid == 0) ? getgid() : ctx->gid;
    inode.mtime = time(NULL);
    inode.ctime = time(NULL);
    inode.size = 0;
    memset(inode.direct, 0, N_DIRECT * sizeof(uint32_t));
    inode.indir_1 = 0;
    inode.indir_2 = 0;

    memcpy((void*)inodes + free_inum * sizeof(struct fs_inode), &inode, sizeof(struct fs_inode));
    write_inode_to_disk(free_inum);

    FD_SET(free_inum, inode_map);
    write_inode_map_to_disk();

    free(_path);
    return 0;
}

/**
 *  mkdir - create a directory with the given mode. Behavior
 *  undefined when mode bits other than the low 9 bits are used.
 *
 * Errors
 *   -ENOTDIR  - component of path not a directory
 *   -EEXIST   - directory already exists
 *   -ENOSPC   - free inode not available
 *   -ENOSPC   - results in >32 entries in directory
 *
 * @param path path to file
 * @param mode the mode for the new directory
 * @return 0 if successful, or -error number
 */
static int fs_mkdir(const char *path, mode_t mode)
{
    char *_path = strdup(path);
    char name[FS_FILENAME_SIZE];
    int dirent_inum = translate_dirent(_path, &name);

    if (dirent_inum < 0) {
        free(_path);
        return dirent_inum;
    }

    struct fs_inode *dirent_inode = get_inode(dirent_inum);
    struct fs_dirent *de = get_data_block(dirent_inode->direct[0]);

    bool isFull = true;
    int available_idx = -1;

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (!de[i].valid) {
            isFull = false;

            if (available_idx == -1) {
                available_idx = i;
            }
            continue;
        }

        if (strcmp(de[i].name, name) == 0) {
            free(_path);
            return -EEXIST;
        }
    }

    if (isFull) {
        free(_path);
        return -ENOSPC;
    }

    int free_inum = get_free_inode();
    if (free_inum == 0) {
        free(_path);
        return -ENOSPC;
    }

    int free_data_block = get_free_blk();
    if (free_data_block == 0) {
        free(_path);
        return -ENOSPC;
    }

    struct fs_dirent new_de;
    strcpy(new_de.name, name);
    new_de.inode = free_inum;
    new_de.valid = true;
    new_de.isDir = true;

    de[available_idx] = new_de;
    write_data_block_to_disk(dirent_inode->direct[0]);

    struct fs_inode inode;
    inode.mode = mode | S_IFDIR;
    struct fuse_context *ctx = fuse_get_context();
    inode.uid = (ctx->pid == 0) ? getuid() : ctx->uid;
    inode.gid = (ctx->pid == 0) ? getgid() : ctx->gid;
    inode.mtime = time(NULL);
    inode.ctime = time(NULL);
    inode.size = 0;
    memset(inode.direct, 0, N_DIRECT * sizeof(uint32_t));
    inode.direct[0] = free_data_block;
    inode.indir_1 = 0;
    inode.indir_2 = 0;

    memcpy((void*)inodes + free_inum * sizeof(struct fs_inode), &inode, sizeof(struct fs_inode));
    write_inode_to_disk(free_inum);

    FD_SET(free_inum, inode_map);
    FD_SET(free_data_block, block_map);
    write_inode_map_to_disk();
    write_block_map_to_disk();

    free(_path);
    return 0;
}

/**
 * truncate - truncate file to exactly 'len' bytes.
 *
 * Errors:
 *   ENOENT  - file does not exist
 *   ENOTDIR - component of path not a directory
 *   EINVAL  - length invalid (only supports 0)
 *   EISDIR	 - path is a directory (only files)
 *
 * @param path the file path
 * @param len the length
 * @return 0 if successful, or -error number
 */
static int fs_truncate(const char *path, off_t len)
{
    /* you can cheat by only implementing this for the case of len==0,
     * and an error otherwise.
     */
    if (len != 0) {
        return -EINVAL;		/* invalid argument */
    }

    char *_path = strdup(path);

    int inum = translate(_path);
    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);

    if (S_ISDIR(inode->mode)) {
        free(_path);
        return -EISDIR;
    }

    int total_blocks = get_data_blocks_num(inode, NULL);
    int data_block_nums[total_blocks];
    get_data_blocks_num(inode, data_block_nums);

    for (int i = 0; i < total_blocks; i++) {
        return_blk(data_block_nums[i]);
    }

    inode->size = 0;
    inode->indir_1 = 0;
    inode->indir_2 = 0;
    memset(inode->direct, 0, N_DIRECT * sizeof(uint32_t));

    write_inode_to_disk(inum);
    write_block_map_to_disk();

    free(_path);
    return 0;
}

/**
 * unlink - delete a file.
 *
 * Errors
 *   -ENOENT   - file does not exist
 *   -ENOTDIR  - component of path not a directory
 *   -EISDIR   - cannot unlink a directory
 *
 * @param path path to file
 * @return 0 if successful, or -error number
 */
static int fs_unlink(const char *path)
{
    char *_dirent_path = strdup(path);
    char *_path = strdup(path);
    char name[FS_FILENAME_SIZE];

    int dirent_inum = translate_dirent(_dirent_path, &name);
    if (dirent_inum < 0) {
        free(_dirent_path);
        free(_path);
        return dirent_inum;
    }

    int inum = translate(_path);
    if (inum < 0) {
        free(_dirent_path);
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);

    if (S_ISDIR(inode->mode)) {
        free(_dirent_path);
        free(_path);
        return -EISDIR;
    }

    int total_blocks = get_data_blocks_num(inode, NULL);
    int data_block_nums[total_blocks];
    get_data_blocks_num(inode, data_block_nums);

    for (int i = 0; i < total_blocks; i++) {
        return_blk(data_block_nums[i]);
    }

    return_inode(inum);

    struct fs_inode *dirent_inode = get_inode(dirent_inum);
    struct fs_dirent *dirent_de = get_data_block(dirent_inode->direct[0]);
    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (dirent_de[i].valid && strcmp(dirent_de[i].name, name) == 0) {
            dirent_de[i].valid = false;
        }
    }

    write_data_block_to_disk(dirent_inode->direct[0]);
    write_inode_map_to_disk();
    write_block_map_to_disk();

    free(_dirent_path);
    free(_path);
    return 0;
}

/**
 * rmdir - remove a directory.
 *
 * Errors
 *   -ENOENT   - file does not exist
 *   -ENOTDIR  - component of path not a directory
 *   -ENOTDIR  - path not a directory
 *   -ENOTEMPTY - directory not empty
 *
 * @param path the path of the directory
 * @return 0 if successful, or -error number
 */
static int fs_rmdir(const char *path)
{
    char *_dirent_path = strdup(path);
    char *_path = strdup(path);
    char name[FS_FILENAME_SIZE];

    int dirent_inum = translate_dirent(_dirent_path, &name);
    if (dirent_inum < 0) {
        free(_dirent_path);
        free(_path);
        return dirent_inum;
    }

    int inum = translate(_path);
    if (inum < 0) {
        free(_dirent_path);
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);
    struct fs_inode *dirent_inode = get_inode(dirent_inum);
    if (!S_ISDIR(inode->mode)) {
        free(_dirent_path);
        free(_path);
        return -ENOTDIR;
    }

    struct fs_dirent *de = get_data_block(inode->direct[0]);

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (de[i].valid) {
            free(_dirent_path);
            free(_path);
            return -ENOTEMPTY;
        }
    }

    return_blk(inode->direct[0]);
    return_inode(inum);

    struct fs_dirent *dirent_de = get_data_block(dirent_inode->direct[0]);

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (dirent_de[i].valid && strcmp(dirent_de[i].name, name) == 0) {
            dirent_de[i].valid = false;
        }
    }
    write_data_block_to_disk(dirent_inode->direct[0]);
    write_inode_map_to_disk();
    write_block_map_to_disk();

    free(_dirent_path);
    free(_path);
    return 0;
}

/**
 * rename - rename a file or directory.
 *
 * Note that this is a simplified version of the UNIX rename
 * functionality - see 'man 2 rename' for full semantics. In
 * particular, the full version can move across directories, replace a
 * destination file, and replace an empty directory with a full one.
 *
 * Errors:
 *   -ENOENT   - source file or directory does not exist
 *   -ENOTDIR  - component of source or target path not a directory
 *   -EEXIST   - destination already exists
 *   -EINVAL   - source and destination not in the same directory
 *
 * @param src_path the source path
 * @param dst_path the destination path.
 * @return 0 if successful, or -error number
 */
static int fs_rename(const char *src_path, const char *dst_path)
{
    char *_src_path = strdup(src_path);
    char *_dst_path = strdup(dst_path);
    char src_name[FS_FILENAME_SIZE];
    char dst_name[FS_FILENAME_SIZE];

    int dirent_inum = translate_dirent(_src_path, &src_name);
    if (dirent_inum != translate_dirent(_dst_path, &dst_name)) {
        free(_src_path);
        free(_dst_path);
        return -EINVAL;
    }

    struct fs_inode *inode = get_inode(dirent_inum);
    struct fs_dirent *de = get_data_block(inode->direct[0]);

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (!de[i].valid) {
            continue;
        }

        if (strcmp(de[i].name, dst_name) == 0) {
            free(_src_path);
            free(_dst_path);
            return -EEXIST;
        }
    }

    for (int i = 0; i < DIRENTS_PER_BLK; i++) {
        if (!de[i].valid) {
            continue;
        }

        if (strcmp(de[i].name, src_name) == 0) {
            strcpy(de[i].name, dst_name);
            write_data_block_to_disk(inode->direct[0]);
            free(_src_path);
            free(_dst_path);
            return 0;
        }
    }


    free(_src_path);
    free(_dst_path);

    return -ENOENT;
}

/**
 * chmod - change file permissions
 *
 * Errors:
 *   -ENOENT   - file does not exist
 *   -ENOTDIR  - component of path not a directory
 *
 * @param path the file or directory path
 * @param mode the mode_t mode value -- see man 'chmod'
 *   for description
 * @return 0 if successful, or -error number
 */
static int fs_chmod(const char *path, mode_t mode)
{
    char *_path = strdup(path);
    int inum = translate(_path);

    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);
    inode->mode = mode | ((S_ISDIR(inode->mode)) ? S_IFDIR : S_IFREG);

    write_inode_to_disk(inum);
    free(_path);
    return 0;
}

/**
 * utime - change access and modification times.
 *
 * Errors:
 *   -ENOENT   - file does not exist
 *   -ENOTDIR  - component of path not a directory
 *
 * @param path the file or directory path.
 * @param ut utimbuf - see man 'utime' for description.
 * @return 0 if successful, or -error number
 */
int fs_utime(const char *path, struct utimbuf *ut)
{
    char *_path = strdup(path);
    int inum = translate(_path);

    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);
    inode->mtime = ut->modtime;

    write_inode_to_disk(inum);
    free(_path);
    return 0;
}

/**
 * read - read data from an open file.
 *
 * Should return exactly the number of bytes requested, except:
 *   - if offset >= file len, return 0
 *   - if offset+len > file len, return bytes from offset to EOF
 *   - on error, return <0
 *
 * Errors:
 *   -ENOENT  - file does not exist
 *   -ENOTDIR - component of path not a directory
 *   -EISDIR  - file is a directory
 *   -EIO     - error reading block
 *
 * @param path the path to the file
 * @param buf the read buffer
 * @param len the number of bytes to read
 * @param offset to start reading at
 * @param fi fuse file info
 * @return number of bytes actually read if successful, or -error number
 */
static int fs_read(const char *path, char *buf, size_t len, off_t offset,
                   struct fuse_file_info *fi)
{
    char *_path = strdup(path);
    int inum = (fi == NULL) ? translate(_path) : fi->fh;

    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);
    if (S_ISDIR(inode->mode)) {
        free(_path);
        return -EISDIR;
    }

    if (offset > inode->size) {
        free(_path);
        return 0;
    }

    if (offset + len > inode->size) {
        len = inode->size - offset;
    }

    char *src;
    int dst_offset = 0;
    int total = 0;
    while (len > 0) {
        int block_idx = offset / FS_BLOCK_SIZE;
        int block_number = get_blk(inode, block_idx, 0);

        void* data_block = get_data_block(block_number);
        src = data_block + (offset % FS_BLOCK_SIZE);
        int copy_len = MIN(len, FS_BLOCK_SIZE - (offset % FS_BLOCK_SIZE));
        memcpy(buf + dst_offset, src, copy_len);
        total += copy_len;
        dst_offset += copy_len;
        len -= copy_len;
        offset += copy_len;
    }

    free(_path);
    return total;
}

/**
 *  write - write data to a file
 *
 * It should return exactly the number of bytes requested, except on
 * error.
 *
 * Errors:
 *   -ENOENT  - file does not exist
 *   -ENOTDIR - component of path not a directory
 *   -EISDIR  - file is a directory
 *   -EINVAL  - if 'offset' is greater than current file length.
 *  			(POSIX semantics support the creation of files with
 *  			"holes" in them, but we don't)
 *
 * @param path the file path
 * @param buf the buffer to write
 * @param len the number of bytes to write
 * @param offset the offset to starting writing at
 * @param fi the Fuse file info for writing
 * @return number of bytes actually written if successful, or -error number
 *
 */
static int fs_write(const char *path, const char *buf, size_t len,
                    off_t offset, struct fuse_file_info *fi)
{
    char *_path = strdup(path);
    int inum = (fi == NULL) ? translate(_path) : fi->fh;

    if (inum < 0) {
        free(_path);
        return inum;
    }

    struct fs_inode *inode = get_inode(inum);
    if (S_ISDIR(inode->mode)) {
        free(_path);
        return -EISDIR;
    }

    if (offset > inode->size) {
        free(_path);
        return -EINVAL;
    }

    char *dst;
    int src_offset = 0;
    int total = 0;

    while (len > 0) {
        int block_idx = offset / FS_BLOCK_SIZE;
        int block_number = get_blk(inode, block_idx, 1);

        void* data_block = get_data_block(block_number);
        dst = data_block + (offset % FS_BLOCK_SIZE);
        int copy_len = MIN(len, FS_BLOCK_SIZE - (offset % FS_BLOCK_SIZE));
        memcpy(dst, buf + src_offset, copy_len);
        src_offset += copy_len;
        len -= copy_len;
        offset += copy_len;
        total += copy_len;
        inode->size += copy_len;
        write_data_block_to_disk(block_number);
    }

    write_inode_to_disk(inum);
    write_block_map_to_disk();
    free(_path);
    return total;
}

/**
 * Open a filesystem file or directory path.
 *
 * Errors:
 *   -ENOENT  - file does not exist
 *   -ENOTDIR - component of path not a directory
 *   -EISDIR  - file is a directory
 *
 * @param path the path
 * @param fuse file info data
 * @return 0 if successful, or -error number
 */
static int fs_open(const char *path, struct fuse_file_info *fi)
{
    if (fi == NULL) {
        return 0;
    }

    char *_path = strdup(path);
    int inum = translate(_path);

    if (inum < 0) {
        free(_path);
        return inum;
    }

    fi->fh = inum;

    struct fs_inode *inode = get_inode(inum);

    if (S_ISDIR(inode->mode)) {
        free(_path);
        return -EISDIR;
    }

    free(_path);
    return 0;
}

/**
 * Release resources created by pending open call.
 *
 * Errors:
 *   -ENOENT  - file does not exist
 *   -ENOTDIR - component of path not a directory
 *
 * @param path the file name
 * @param fi the fuse file info
 * @return 0 if successful, or -error number
 */
static int fs_release(const char *path, struct fuse_file_info *fi)
{
    if (fi == NULL) {
        return 0;
    }

    int inum = fi->fh;

    if (inum < 0) {
        return inum;
    }

    fi->fh = 0;

    struct fs_inode *inode = get_inode(inum);

    if (S_ISDIR(inode->mode)) {
        return -EISDIR;
    }

    return 0;
}

/**
 * statfs - get file system statistics.
 * See 'man 2 statfs' for description of 'struct statvfs'.
 *
 * Errors
 *   none -  Needs to work
 *
 * @param path the path to the file
 * @param st the statvfs struct
 * @return 0 for successful
 */
static int fs_statfs(const char *path, struct statvfs *st)
{
    /* needs to return the following fields (set others to zero):
     *   f_bsize = BLOCK_SIZE
     *   f_blocks = total image - metadata
     *   f_bfree = f_blocks - blocks used
     *   f_bavail = f_bfree
     *   f_namelen = <whatever your max namelength is>
     *
     * this should work fine, but you may want to add code to
     * calculate the correct values later.
     */
    st->f_bsize = FS_BLOCK_SIZE;
    st->f_blocks = n_blocks;
    st->f_bfree = n_blocks - get_used_blocks_num();
    st->f_bavail = n_blocks - get_used_blocks_num();
    st->f_namemax = FS_FILENAME_SIZE - 1;

    return 0;
}

static int get_used_blocks_num()
{
    int count = 0;
    for (int i = 0; i < n_blocks; i++) {
        if (FD_ISSET(i, block_map)) {
            count++;
        }
    }
    return count;
}

/**
 * Operations vector. Please don't rename it, as the
 * skeleton code in misc.c assumes it is named 'fs_ops'.
 */
struct fuse_operations fs_ops = {
        .init = fs_init,
        .getattr = fs_getattr,
        .opendir = fs_opendir,
        .readdir = fs_readdir,
        .releasedir = fs_releasedir,
        .mknod = fs_mknod,
        .mkdir = fs_mkdir,
        .unlink = fs_unlink,
        .rmdir = fs_rmdir,
        .rename = fs_rename,
        .chmod = fs_chmod,
        .utime = fs_utime,
        .truncate = fs_truncate,
        .open = fs_open,
        .read = fs_read,
        .write = fs_write,
        .release = fs_release,
        .statfs = fs_statfs,
};
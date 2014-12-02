#ifndef I_FUSELIB_TRACE_H
#define I_FUSELIB_TRACE_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

// trace wrappers around the irods Fuse operations
#include <sys/statvfs.h>

// don't include if all we're doing is testing the trace functionality
#ifndef _TEST_TRACE
#include "rodsClient.h"
#include "rodsPath.h"
#include "iFuseLib.h"
#endif

#include "iFuseLib.Logging.h"
#include "iFuseLib.Http.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define TRACE_GETATTR   0x1
#define TRACE_READLINK  0x2
#define TRACE_MKNOD     0x4
#define TRACE_MKDIR     0x8
#define TRACE_UNLINK    0x10
#define TRACE_RMDIR     0x20
#define TRACE_SYMLINK   0x40
#define TRACE_RENAME    0x80
#define TRACE_LINK      0x100
#define TRACE_CHMOD     0x200
#define TRACE_CHOWN     0x400
#define TRACE_TRUNCATE  0x800
#define TRACE_FLUSH     0x1000
#define TRACE_UTIMENS   0x2000
#define TRACE_OPEN      0x4000
#define TRACE_READ      0x8000
#define TRACE_WRITE     0x10000
#define TRACE_STATFS    0x20000
#define TRACE_RELEASE   0x40000
#define TRACE_FSYNC     0x80000
#define TRACE_READDIR   0x100000
   
#define TRACE_DEBUG     0x200000
   
#define TRACE_ALL       0x3FFFFF

// don't compile these if all we're doing is testing the trace functionality
#ifndef _TEST_TRACE

int traced_irodsGetattr(const char *path, struct stat *stbuf);
int traced_irodsReadlink(const char *path, char *buf, size_t size);
int traced_irodsMknod(const char *path, mode_t mode, dev_t rdev);
int traced_irodsMkdir(const char *path, mode_t mode);
int traced_irodsUnlink(const char *path);
int traced_irodsRmdir(const char *path);
int traced_irodsSymlink(const char *from, const char *to);
int traced_irodsRename(const char *from, const char *to);
int traced_irodsLink(const char *from, const char *to);
int traced_irodsChmod(const char *path, mode_t mode);
int traced_irodsChown(const char *path, uid_t uid, gid_t gid);
int traced_irodsTruncate(const char *path, off_t size);
int traced_irodsFlush(const char *path, struct fuse_file_info *fi);
int traced_irodsUtimens (const char *path, const struct timespec ts[]);
int traced_irodsOpen(const char *path, struct fuse_file_info *fi);
int traced_irodsRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int traced_irodsWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int traced_irodsStatfs(const char *path, struct statvfs *stbuf);
int traced_irodsRelease(const char *path, struct fuse_file_info *fi);
int traced_irodsFsync(const char *path, int isdatasync, struct fuse_file_info *fi);
int traced_irodsReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);

#endif

int trace_get_environment_variables( char** http_host, int* portnum, int* sync_delay, int* max_lines, uint64_t* methods, char** log_path_salt );
int trace_read_arg( int argc, char** argv, int i );
int trace_begin( struct log_context** ctx );
int trace_end( struct log_context** ctx );

void trace_usage(void);

extern log_context* LOGCTX;

// helpful macros
#define strdup_or_default( s, d ) ((s) != NULL ? strdup(s) : ((d) != NULL ? strdup(d) : NULL))

#ifdef  __cplusplus
}
#endif


#endif

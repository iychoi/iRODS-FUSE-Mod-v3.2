#ifndef I_FUSE_LIB_LAZYUPLOAD_H
#define I_FUSE_LIB_LAZYUPLOAD_H

#ifdef ENABLE_LAZY_UPLOAD

#include "rodsClient.h"
#include "rodsPath.h"
#include "iFuseLib.h"

#define FUSE_LAZY_UPLOAD_BUFFER_DIR  "/tmp/fuseLazyUploadBuffer"
#define FUSE_LAZY_UPLOAD_MEM_BUFFER_SIZE    (500*1024)

#ifdef USE_BOOST
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#else
#include <pthread.h>
#endif

typedef struct LazyUploadConfig {
    int lazyUpload;
    char *bufferPath;
} lazyUploadConfig_t;

#define NUM_LAZYUPLOAD_FILE_HASH_SLOT   201

typedef struct LazyUploadFileInfo {
    char *path;
    int accmode;
    int localHandle;
    int commitCount;
    off_t curLocalOffsetStart;
    off_t curOffset;
#ifdef USE_BOOST
    boost::mutex* mutex;
#else
    pthread_mutex_t lock;
#endif
} lazyUploadFileInfo_t;

#ifdef  __cplusplus
extern "C" {
#endif

// lazy-upload
int
initLazyUpload (lazyUploadConfig_t *lazyUploadConfig, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs);
int
uninitLazyUpload (lazyUploadConfig_t *lazyUploadConfig);
int
isLazyUploadEnabled ();
int
isFileLazyUploading (const char *path);
int
mknodLazyUploadBufferedFile (const char *path);
int
openLazyUploadBufferedFile (const char *path, int accmode);
int
writeLazyUploadBufferedFile (const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int
syncLazyUploadBufferedFile (const char *path, struct fuse_file_info *fi);
int
closeLazyUploadBufferedFile (const char *path);
#ifdef  __cplusplus
}
#endif

#endif

#endif	/* I_FUSE_LIB_LAZYUPLOAD_H */

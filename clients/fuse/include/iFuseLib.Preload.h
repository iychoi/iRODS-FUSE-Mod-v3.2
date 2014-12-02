#ifndef I_FUSE_LIB_PRELOAD_H
#define I_FUSE_LIB_PRELOAD_H

#ifdef ENABLE_PRELOAD

#include "rodsClient.h"
#include "rodsPath.h"
#include "iFuseLib.h"

#define FUSE_PRELOAD_CACHE_DIR  "/tmp/fusePreloadCache"

#ifdef USE_BOOST
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#else
#include <pthread.h>
#endif

typedef struct PreloadConfig {
    int preload;
    int clearCache;
    char *cachePath;
    rodsLong_t cacheMaxSize; /* 0 means unlimited */
    rodsLong_t preloadMinSize; /* 0 means "use default" */ 
} preloadConfig_t;

#define PRELOAD_FILES_IN_DOWNLOADING_EXT    ".part"
#define NUM_PRELOAD_THREAD_HASH_SLOT	201

typedef struct PreloadThreadInfo {
#ifdef USE_BOOST
    boost::thread* thread;
#else
    pthread_t thread;
#endif
    char *path;
    int running;
#ifdef USE_BOOST
    boost::mutex* mutex;
#else
    pthread_mutex_t lock;
#endif
} preloadThreadInfo_t;

#define PRELOAD_THREAD_RUNNING    1
#define PRELOAD_THREAD_IDLE    0

typedef struct PreloadThreadData {
    char *path;
    struct stat stbuf;
    preloadThreadInfo_t *threadInfo;
} preloadThreadData_t;

#define NUM_PRELOAD_FILEHANDLE_HASH_SLOT    201

typedef struct PreloadFileHandleInfo {
    char *path;
    int handle;
#ifdef USE_BOOST
    boost::mutex* mutex;
#else
    pthread_mutex_t lock;
#endif
} preloadFileHandleInfo_t;

#ifdef  __cplusplus
extern "C" {
#endif

int
initPreload (preloadConfig_t *preloadConfig, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs);
int
waitPreloadJobs ();
int
uninitPreload (preloadConfig_t *preloadConfig);
int
isPreloadEnabled ();
int
preloadFile (const char *path, struct stat *stbuf);
int
invalidatePreloadedFile (const char *path);
int
renamePreloadedFile (const char *fromPath, const char *toPath);
int
truncatePreloadedFile (const char *path, off_t size);
int
isPreloadedFile (const char *path);
int
isFilePreloading (const char *path);
int
openPreloadedFile (const char *path);
int
readPreloadedFile (int fileDesc, char *buf, size_t size, off_t offset);
int
closePreloadedFile (const char *path);
int
moveToPreloadedDir (const char *path, const char *iRODSPath);

#ifdef  __cplusplus
}
#endif

#endif

#endif	/* I_FUSE_LIB_PRELOAD_H */

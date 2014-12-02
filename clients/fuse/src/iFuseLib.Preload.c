#ifdef ENABLE_PRELOAD

#include <iostream>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "irodsFs.h"
#include "iFuseLib.h"
#include "iFuseOper.h"
#include "miscUtil.h"
#include "iFuseLib.Preload.h"
#include "iFuseLib.FSUtils.h"
#include "iFuseLib.List.h"
#include "iFuseLib.Hashtable.h"
#include "getUtil.h"

/**************************************************************************
 * global variables
 **************************************************************************/
static preloadConfig_t PreloadConfig;
static rodsEnv *PreloadRodsEnv;
static rodsArguments_t *PreloadRodsArgs;

#ifdef USE_BOOST
static boost::mutex*             PreloadLock;
#else
static pthread_mutex_t           PreloadLock;
static pthread_mutexattr_t       PreloadLockAttr;
#endif

static fl_concurrentList_t *PreloadThreadList;
static fl_concurrentList_t *PreloadFileHandleList;
static fl_Hashtable *PreloadThreadTable;
static fl_Hashtable *PreloadFileHandleTable;

/**************************************************************************
 * function definitions
 **************************************************************************/
static void _initlockPreload();
static void _uninitlockPreload();
static void _lockPreload();
static void _unlockPreload();
static void *_preloadThread(void *arg);
static int _download(const char *path, struct stat *stbufIn);
static int _completeDownload(const char *workPath, const char *cachePath, struct stat *stbuf);
static int _hasValidCache(const char *path, struct stat *stbuf);
static int _getCachePath(const char *path, char *cachePath);
static int _hasCache(const char *path);
static int _invalidateCache(const char *path);
static int _evictOldCache(off_t sizeNeeded);
static int _findOldestCache(const char *path, char *oldCachePath, struct stat *oldStatbuf);
static int _getCacheWorkPath(const char *path, char *cachePath);
static int _preparePreloadCacheDir(const char *path);
static int _removeAllCaches();
static int _removeAllIncompleteCaches();
static int _removeIncompleteCaches(const char *path);
static int _renameCache(const char *fromPath, const char *toPath);
static int _truncateCache(const char *path, off_t size);
static int _getiRODSPath(const char *path, char *iRODSPath);

/**************************************************************************
 * public functions
 **************************************************************************/
int
initPreload (preloadConfig_t *preloadConfig, rodsEnv *myPreloadRodsEnv, rodsArguments_t *myPreloadRodsArgs) {
    rodsLog (LOG_DEBUG, "initPreload: MyPreloadConfig.preload = %d", preloadConfig->preload);
    rodsLog (LOG_DEBUG, "initPreload: MyPreloadConfig.clearCache = %d", preloadConfig->clearCache);
    rodsLog (LOG_DEBUG, "initPreload: MyPreloadConfig.cachePath = %s", preloadConfig->cachePath);
    rodsLog (LOG_DEBUG, "initPreload: MyPreloadConfig.cacheMaxSize = %lld", preloadConfig->cacheMaxSize);
    rodsLog (LOG_DEBUG, "initPreload: MyPreloadConfig.preloadMinSize = %lld", preloadConfig->preloadMinSize);
    rodsLog (LOG_DEBUG, "initPreload: empty space = %lld", getEmptySpace(preloadConfig->cachePath));

    // copy given configuration
    memcpy(&PreloadConfig, preloadConfig, sizeof(preloadConfig_t));
    PreloadRodsEnv = myPreloadRodsEnv;
    PreloadRodsArgs = myPreloadRodsArgs;

    // init hashtables
    PreloadThreadTable = fl_newHashTable(NUM_PRELOAD_THREAD_HASH_SLOT);
    PreloadFileHandleTable = fl_newHashTable(NUM_PRELOAD_FILEHANDLE_HASH_SLOT);

    // init lists
    PreloadThreadList = fl_newConcurrentList();
    PreloadFileHandleList = fl_newConcurrentList();

    // init lock
    _initlockPreload();

    _preparePreloadCacheDir(preloadConfig->cachePath);

    if(preloadConfig->clearCache) {
        // clear all cache
        _removeAllCaches();
    } else {
        // remove incomplete preload caches
        _removeAllIncompleteCaches();
    }

    return (0);
}

int
waitPreloadJobs () {
    int i;
    int size;

    // destroy preload thread list
    size = fl_listSize(PreloadThreadList);
    for(i=0;i<size;i++) {
        _lockPreload();
        preloadThreadInfo_t *threadInfo = (preloadThreadInfo_t *)fl_removeFirstElementOfConcurrentList(PreloadThreadList);
        if(threadInfo != NULL) {
            rodsLog (LOG_DEBUG, "waitPreloadJobs: Waiting for a preload job - %s", threadInfo->path);
            _unlockPreload();
#ifdef USE_BOOST
            threadInfo->thread->join();
#else
            pthread_join(threadInfo->thread, NULL);
#endif
            if(threadInfo->path != NULL) {
                free(threadInfo->path);
                threadInfo->path = NULL;
            }
            FREE_STRUCT_LOCK((*threadInfo));
#ifdef USE_BOOST
            if(threadInfo->thread != NULL) {
                delete threadInfo->thread;
                threadInfo->thread = NULL;
            }
#endif
            free(threadInfo);
        } else {
            _unlockPreload();
        }
    }
    return 0;
}

int
uninitPreload (preloadConfig_t *preloadConfig) {
    int size;
    int i;

    if(preloadConfig->clearCache) {
        // clear all cache
        _removeAllCaches();
    } else {
        // remove incomplete preload caches
        _removeAllIncompleteCaches();
    }

    // destroy file handle list
    size = fl_listSize(PreloadFileHandleList);
    for(i=0;i<size;i++) {
        preloadFileHandleInfo_t *preloadFileHandleInfo = (preloadFileHandleInfo_t *)fl_removeFirstElementOfConcurrentList(PreloadFileHandleList);
        if(preloadFileHandleInfo != NULL) {
            if(preloadFileHandleInfo->path != NULL) {
                free(preloadFileHandleInfo->path);
                preloadFileHandleInfo->path = NULL;
            }
            FREE_STRUCT_LOCK((*preloadFileHandleInfo));

            free(preloadFileHandleInfo);
        }
    }

    fl_deleteConcurrentList(PreloadThreadList);
    fl_deleteConcurrentList(PreloadFileHandleList);
    fl_deleteHashTable(PreloadThreadTable, NULL);
    fl_deleteHashTable(PreloadFileHandleTable, NULL);

    // uninit lock
    _uninitlockPreload();

    return (0);
}

int
isPreloadEnabled() {
    // check whether preload is enabled
    if(PreloadConfig.preload == 0) {
        return -1;
    }
    return 0;
}

int
preloadFile (const char *path, struct stat *stbuf) {
    int status;
    preloadThreadInfo_t *existingThreadInfo = NULL;
    preloadThreadInfo_t *threadInfo = NULL;
    preloadThreadData_t *threadData = NULL;
    char iRODSPath[MAX_NAME_LEN];
    off_t cacheSize;
    off_t freeSize;

    // convert input path to iRODSPath
    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "preloadFile: failed to get iRODS path - %s", path);
        return status;
    }

    _lockPreload();

    // check the given file is preloading
    existingThreadInfo = (preloadThreadInfo_t *)fl_lookupFromHashTable(PreloadThreadTable, iRODSPath);
    if(existingThreadInfo != NULL) {
        rodsLog (LOG_DEBUG, "preloadFile: preloading is already running - %s", iRODSPath);
        _unlockPreload();
        return 0;
    }

    if(_hasValidCache(iRODSPath, stbuf) != 0) {
        // invalidate cache - this may fail if cache file does not exists
        // if old cache exists in local, invalidate it
        _invalidateCache(iRODSPath);

        if(stbuf->st_size < PreloadConfig.preloadMinSize) {
            rodsLog (LOG_DEBUG, "preloadFile: given file is smaller than preloadMinSize, canceling preloading - %s", iRODSPath);
            _unlockPreload();
            return (0);
        }

        // check whether preload cache exceeds limit
        if(PreloadConfig.cacheMaxSize > 0) {
            // cache max size is set
            if(stbuf->st_size > (off_t)PreloadConfig.cacheMaxSize) {
                rodsLog (LOG_DEBUG, "preloadFile: given file is bigger than cacheMaxSize, canceling preloading - %s", iRODSPath);
                _unlockPreload();
                return (0);
            }

            cacheSize = getFileSizeRecursive(PreloadConfig.cachePath);
            if((cacheSize + stbuf->st_size) > (off_t)PreloadConfig.cacheMaxSize) {
                // evict?
                status = _evictOldCache((cacheSize + stbuf->st_size) - (off_t)PreloadConfig.cacheMaxSize);
                if(status < 0) {
                    rodsLog (LOG_DEBUG, "preloadFile: failed to evict old cache");
                    _unlockPreload();
                    return status;
                }
            }
        }

        freeSize = getEmptySpace(PreloadConfig.cachePath);
        if(freeSize < stbuf->st_size) {
            // evict?
            status = _evictOldCache(stbuf->st_size - freeSize);
            if(status < 0) {
                rodsLog (LOG_DEBUG, "preloadFile: failed to evict old cache");
                _unlockPreload();
                return status;
            }
        }

        // does not have valid cache. now, start a new preloading

        // create a new thread to preload
        threadInfo = (preloadThreadInfo_t *)malloc(sizeof(preloadThreadInfo_t));
        threadInfo->path = strdup(iRODSPath);
        threadInfo->running = PRELOAD_THREAD_RUNNING;
        INIT_STRUCT_LOCK((*threadInfo));

        fl_addToConcurrentList(PreloadThreadList, threadInfo);
        fl_insertIntoHashTable(PreloadThreadTable, iRODSPath, threadInfo);

        // prepare thread argument
        threadData = (preloadThreadData_t *)malloc(sizeof(preloadThreadData_t));
        threadData->path = strdup(iRODSPath);
        memcpy(&threadData->stbuf, stbuf, sizeof(struct stat));
        threadData->threadInfo = threadInfo;

        rodsLog (LOG_DEBUG, "preloadFile: start preloading - %s", iRODSPath);
#ifdef USE_BOOST
        status = threadInfo->thread = new boost::thread(_preloadThread, (void *)threadData);
#else
        status = pthread_create(&threadInfo->thread, NULL, _preloadThread, (void *)threadData);
#endif
    } else {
        rodsLog (LOG_DEBUG, "preloadFile: given file is already preloaded - %s", iRODSPath);
        _unlockPreload();
        status = 0;
    }

    _unlockPreload();
    return status;
}

int
invalidatePreloadedFile (const char *path) {
    int status;
    char iRODSPath[MAX_NAME_LEN];

    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "invalidatePreloadedFile: failed to get iRODS path - %s", path);
        return status;
    }

    _lockPreload();

    status = _invalidateCache(iRODSPath);    

    _unlockPreload();

    return status;
}

int
renamePreloadedFile (const char *fromPath, const char *toPath) {
    int status;
    char fromiRODSPath[MAX_NAME_LEN];
    char toiRODSPath[MAX_NAME_LEN];

    status = _getiRODSPath(fromPath, fromiRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "renamePreloadedFile: failed to get iRODS path - %s", fromPath);
        return status;
    }

    status = _getiRODSPath(toPath, toiRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "renamePreloadedFile: failed to get iRODS path - %s", toPath);
        return status;
    }

    _lockPreload();

    status = _renameCache(fromiRODSPath, toiRODSPath);

    _unlockPreload();

    return status;
}

int
truncatePreloadedFile (const char *path, off_t size) {
    int status;
    char iRODSPath[MAX_NAME_LEN];

    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "truncatePreloadedFile: failed to get iRODS path - %s", path);
        return status;
    }

    _lockPreload();

    status = _truncateCache(iRODSPath, size);
    
    _unlockPreload();

    return status;
}

int
isPreloadedFile (const char *path) {
    int status;
    char iRODSPath[MAX_NAME_LEN];

    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "isPreloadedFile: failed to get iRODS path - %s", path);
        return status;
    }

    _lockPreload();

    status = _hasCache(iRODSPath);

    _unlockPreload();

    return status;
}

int
isFilePreloading (const char *path) {
    int status;
    preloadThreadInfo_t *threadInfo = NULL;
    char iRODSPath[MAX_NAME_LEN];

    // convert input path to iRODSPath
    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "isFilePreloading: failed to get iRODS path - %s", path);
        return status;
    }

    // check the given file is already preloaded or preloading
    _lockPreload();

    // check the given file is preloading
    threadInfo = (preloadThreadInfo_t *)fl_lookupFromHashTable(PreloadThreadTable, iRODSPath);
    if(threadInfo != NULL) {
        _unlockPreload();
        return 0;
    }

    _unlockPreload();
    return -1;
}

int
openPreloadedFile (const char *path) {
    int status;
    char iRODSPath[MAX_NAME_LEN];
    char preloadCachePath[MAX_NAME_LEN];
    preloadFileHandleInfo_t *preloadFileHandleInfo = NULL;
    int desc;

    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "openPreloadedFile: failed to get iRODS path - %s", path);
        return status;
    }

    status = _getCachePath(iRODSPath, preloadCachePath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "openPreloadedFile: failed to get cache path - %s", path);
        return status;
    }

    _lockPreload();

    desc = -1;

    preloadFileHandleInfo = (preloadFileHandleInfo_t *)fl_lookupFromHashTable(PreloadFileHandleTable, iRODSPath);
    if(preloadFileHandleInfo != NULL) {
        // has preload file handle opened
        if(preloadFileHandleInfo->handle > 0) {
            // reuse handle
            desc = preloadFileHandleInfo->handle;
            rodsLog (LOG_DEBUG, "openPreloadedFile: file is already opened - %s", iRODSPath);
        } else {
            // reuse handleinfo
            if(_hasCache(iRODSPath) >= 0) {
                desc = open (preloadCachePath, O_RDONLY);

                if(desc > 0) {
                    preloadFileHandleInfo->handle = desc;
                    rodsLog (LOG_DEBUG, "openPreloadedFile: opens a file handle - %s", iRODSPath);
                }
            }
        }
    } else {
        // if preloaded cache file is not opened
        // open new
        if(_hasCache(iRODSPath) >= 0) {
            desc = open (preloadCachePath, O_RDONLY);
            rodsLog (LOG_DEBUG, "openPreloadedFile: open a preloaded cache path - %s", iRODSPath);

            if(desc > 0) {
                preloadFileHandleInfo = (preloadFileHandleInfo_t *)malloc(sizeof(preloadFileHandleInfo_t));
                preloadFileHandleInfo->path = strdup(iRODSPath);
                preloadFileHandleInfo->handle = desc;
                INIT_STRUCT_LOCK((*preloadFileHandleInfo));

                fl_addToConcurrentList(PreloadFileHandleList, preloadFileHandleInfo);
                fl_insertIntoHashTable(PreloadFileHandleTable, iRODSPath, preloadFileHandleInfo);
            }
        }
    }

    _unlockPreload();

    return desc;
}

int
readPreloadedFile (int fileDesc, char *buf, size_t size, off_t offset) {
    int status;
    off_t seek_status;

    _lockPreload();

    seek_status = lseek (fileDesc, offset, SEEK_SET);
    if (seek_status != offset) {
        status = (int)seek_status;
        rodsLog (LOG_DEBUG, "readPreloadedFile: failed to seek file desc - %d, %ld -> %ld", fileDesc, offset, seek_status);

        _unlockPreload();
        return status;
    }

    status = read (fileDesc, buf, size);
    rodsLog (LOG_DEBUG, "readPreloadedFile: read from opened preloaded file - %d", fileDesc);

    _unlockPreload();

    return status;
}

int
closePreloadedFile (const char *path) {
    int status;
    char iRODSPath[MAX_NAME_LEN];
    preloadFileHandleInfo_t *preloadFileHandleInfo = NULL;

    status = _getiRODSPath(path, iRODSPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "closePreloadedFile: failed to get iRODS path - %s", path);
        return status;
    }

    _lockPreload();

    preloadFileHandleInfo = (preloadFileHandleInfo_t *)fl_lookupFromHashTable(PreloadFileHandleTable, iRODSPath);
    if(preloadFileHandleInfo != NULL) {
        // has preload file handle opened
        if(preloadFileHandleInfo->handle > 0) {
            close(preloadFileHandleInfo->handle);
            preloadFileHandleInfo->handle = -1;
            rodsLog (LOG_DEBUG, "closePreloadedFile: close preloaded cache handle - %s", iRODSPath);
        }

        // remove from hash table
        fl_removeFromConcurrentList2(PreloadFileHandleList, preloadFileHandleInfo);
        fl_deleteFromHashTable(PreloadFileHandleTable, iRODSPath);

        if(preloadFileHandleInfo->path != NULL) {
            free(preloadFileHandleInfo->path);
            preloadFileHandleInfo->path = NULL;
        }
    
        free(preloadFileHandleInfo);
    }
    
    _unlockPreload();

    return status;
}

int
moveToPreloadedDir (const char *path, const char *iRODSPath) {
    int status;
    char preloadCachePath[MAX_NAME_LEN];
    off_t cacheSize;

    if (path == NULL || iRODSPath == NULL) {
        rodsLog (LOG_DEBUG, "moveToPreloadedDir: input path or iRODSPath is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    status = _getCachePath(iRODSPath, preloadCachePath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "moveToPreloadedDir: failed to get cache path - %s", path);
        return status;
    }

    // make dir
    makeParentDirs(preloadCachePath);
    
    // move the file
    status = rename(path, preloadCachePath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "moveToPreloadedDir: rename error : %d", status);
        return status;
    }

    // check whether preload cache exceeds limit
    if(PreloadConfig.cacheMaxSize > 0) {
        cacheSize = getFileSizeRecursive(PreloadConfig.cachePath);
        if(cacheSize > (off_t)PreloadConfig.cacheMaxSize) {
            // evict?
            status = _evictOldCache(cacheSize - (off_t)PreloadConfig.cacheMaxSize);
            if(status < 0) {
                rodsLog (LOG_DEBUG, "moveToPreloadedDir: failed to evict old cache");
                return status;
            }
        }
    }

    return (0);
}

/**************************************************************************
 * private functions
 **************************************************************************/
static void _initlockPreload() {
#ifdef USE_BOOST
    PreloadLock = new boost::mutex();
#else
    pthread_mutexattr_init (&PreloadLockAttr);
    pthread_mutexattr_settype (&PreloadLockAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&PreloadLock, &PreloadLockAttr);
#endif
}

static void _uninitlockPreload() {
#ifdef USE_BOOST
    delete PreloadLock;
    PreloadLock = NULL;
#else
    pthread_mutex_destroy (&PreloadLock);
    pthread_mutexattr_destroy (&PreloadLockAttr);
#endif
}

static void _lockPreload() {
#ifdef USE_BOOST
    PreloadLock->lock();
#else
    pthread_mutex_lock (&PreloadLock);
#endif
}

static void _unlockPreload() {
#ifdef USE_BOOST
    PreloadLock->unlock();
#else
    pthread_mutex_unlock (&PreloadLock);
#endif
}

static void *_preloadThread(void *arg) {
    int status;
    preloadThreadData_t *threadData = (preloadThreadData_t *)arg;
    preloadThreadInfo_t *threadInfo = NULL;

    if(threadData == NULL) {
        rodsLog (LOG_DEBUG, "_preloadThread: given thread argument is null");
        pthread_exit(NULL);
    }

    threadInfo = threadData->threadInfo;

    rodsLog (LOG_DEBUG, "_preloadThread: preload - %s", threadData->path);
    
    status = _download(threadData->path, &threadData->stbuf);
    if(status != 0) {
        rodsLog (LOG_DEBUG, "_preloadThread: download error - %d", status);
    }

    // downloading is done
    _lockPreload();

    // change thread status
    LOCK_STRUCT(*threadInfo);
    threadInfo->running = PRELOAD_THREAD_IDLE;
    UNLOCK_STRUCT(*threadInfo);

    // release threadData
    rodsLog (LOG_DEBUG, "_preloadThread: thread finished - %s", threadData->path);
    if(threadData->path != NULL) {
        free(threadData->path);
        threadData->path = NULL;
    }

    free(threadData);

    // remove from hash table
    fl_removeFromConcurrentList2(PreloadThreadList, threadInfo);
    fl_deleteFromHashTable(PreloadThreadTable, threadInfo->path);

    if(threadInfo != NULL) {
        if(threadInfo->path != NULL) {
            free(threadInfo->path);
            threadInfo->path = NULL;
        }
        free(threadInfo);
    }

    _unlockPreload();
    pthread_exit(NULL);
}

static int
_download(const char *path, struct stat *stbufIn) {
    int status;
    rcComm_t *conn;
    rodsPathInp_t rodsPathInp;
    rErrMsg_t errMsg;
    char preloadCachePath[MAX_NAME_LEN];
    char preloadCacheWorkPath[MAX_NAME_LEN];

    // set path for getUtil
    status = _getCachePath(path, preloadCachePath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_download: failed to get cache path - %s", path);
        return status;
    }

    status = _getCacheWorkPath(path, preloadCacheWorkPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_download: failed to get cache work path - %s", path);
        return status;
    }

    rodsLog (LOG_DEBUG, "_download: download %s to %s", path, preloadCachePath);

    // set src path
    memset( &rodsPathInp, 0, sizeof( rodsPathInp_t ) );
    addSrcInPath( &rodsPathInp, (char*)path );
    status = parseRodsPath (&rodsPathInp.srcPath[0], PreloadRodsEnv);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_download: parseRodsPath error : %d", status);
        return status;
    }

    // set dest path
    rodsPathInp.destPath = ( rodsPath_t* )malloc( sizeof( rodsPath_t ) );
    memset( rodsPathInp.destPath, 0, sizeof( rodsPath_t ) );
    rstrcpy( rodsPathInp.destPath->inPath, preloadCacheWorkPath, MAX_NAME_LEN );
    status = parseLocalPath (rodsPathInp.destPath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_download: parseLocalPath error : %d", status);
        return status;
    }

    // Connect
    conn = rcConnect (PreloadRodsEnv->rodsHost, PreloadRodsEnv->rodsPort, PreloadRodsEnv->rodsUserName, PreloadRodsEnv->rodsZone, RECONN_TIMEOUT, &errMsg);
    if (conn == NULL) {
        rodsLog (LOG_DEBUG, "_download: error occurred while connecting to irods");
        return -EPIPE;
    }

    // Login
    if (strcmp (PreloadRodsEnv->rodsUserName, PUBLIC_USER_NAME) != 0) { 
        status = clientLogin(conn);
        if (status != 0) {
            rodsLog (LOG_DEBUG, "_download: ClientLogin error : %d", status);
            rcDisconnect(conn);
            return status;
        }
    }

    // make dir
    makeParentDirs(preloadCachePath);

    // Preload
    rodsLog (LOG_DEBUG, "_download: download %s", path);
    status = getUtil (&conn, PreloadRodsEnv, PreloadRodsArgs, &rodsPathInp);
    rodsLog (LOG_DEBUG, "_download: complete downloading %s", path);

    // Disconnect 
    rcDisconnect(conn);

    if(status < 0) {
        rodsLog (LOG_DEBUG, "_download: getUtil error : %d", status);
        return status;
    }

    // be careful when using Lock
    _lockPreload();
    status = _completeDownload(preloadCacheWorkPath, preloadCachePath, stbufIn);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_download: _completeDownload error : %d", status);
        _unlockPreload();
        return status;
    }

    _unlockPreload();
    return 0;
}

static int
_completeDownload(const char *workPath, const char *cachePath, struct stat *stbuf) {
    int status;
    struct utimbuf amtime;

    if (workPath == NULL || cachePath == NULL || stbuf == NULL) {
        rodsLog (LOG_DEBUG, "_completeDownload: input workPath or cachePath or stbuf is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    //amtime.actime = stbuf->st_atime;
    amtime.actime = convTime(getCurrentTime());
    amtime.modtime = stbuf->st_mtime;

    // set last access time and modified time the same as the original file
    status = utime(workPath, &amtime);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_completeDownload: utime error : %d", status);
        return status;
    }

    // change the name
    status = rename(workPath, cachePath);
    if(status < 0) {
        rodsLog (LOG_DEBUG, "_completeDownload: rename error : %d", status);
        return status;
    }

    return (0);
}

static int
_hasCache(const char *path) {
    int status;
    char cachePath[MAX_NAME_LEN];
    struct stat stbufCache;

    if (path == NULL) {
        rodsLog (LOG_DEBUG, "_hasCache: input inPath is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

	if ((status = _getCachePath(path, cachePath)) < 0) {
        rodsLog (LOG_DEBUG, "_hasCache: _getCachePath error : %d", status);
        return status;
    }

    if ((status = stat(cachePath, &stbufCache)) < 0) {
        //rodsLog (LOG_DEBUG, "_hasCache: stat error for %s", cachePath);
        return status;
    }

    return (0);
}

static int
_hasValidCache(const char *path, struct stat *stbuf) {
    int status;
    char cachePath[MAX_NAME_LEN];
    struct stat stbufCache;

    if (path == NULL || stbuf == NULL) {
        rodsLog (LOG_DEBUG, "_hasValidCache: input path or stbuf is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

	if ((status = _getCachePath(path, cachePath)) < 0) {
        return status;
    }

    if ((status = stat(cachePath, &stbufCache)) < 0) {
        return status;
    }

    // compare stbufs
    if (stbufCache.st_size != stbuf->st_size) {
        return -1; // invalid size
    }
    if (stbufCache.st_mtime != stbuf->st_mtime) {
        return -1; // invalid mod time
    }

    return (0);
}

static int
_invalidateCache(const char *path) {
    int status;
    char cachePath[MAX_NAME_LEN];
    char cacheWorkPath[MAX_NAME_LEN];

    if (path == NULL) {
        rodsLog (LOG_DEBUG, "_invalidateCache: input path is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if ((status = _getCacheWorkPath(path, cacheWorkPath)) < 0) {
        return status;
    }

    if ((status = _getCachePath(path, cachePath)) < 0) {
        return status;
    }

    if (isDirectory(cachePath) == 0) {
        // directory
        status = removeDirRecursive(cachePath);
    } else {
        // file
        // remove incomplete preload cache if exists
        unlink(cacheWorkPath);
        status = unlink(cachePath);
    }

    return status;
}

static int
_findOldestCache(const char *path, char *oldCachePath, struct stat *oldStatbuf) {
    int status;    
    DIR *dir = NULL;
    char filepath[MAX_NAME_LEN];
    char tempPath[MAX_NAME_LEN];
    struct stat tempStatBuf;
    char oldestCachePath[MAX_NAME_LEN];
    struct stat oldestStatBuf;
    struct dirent *entry;
    struct stat statbuf;

    memset(oldestCachePath, 0, MAX_NAME_LEN);
    memset(&oldestStatBuf, 0, sizeof(struct stat));

    dir = opendir(path);
    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            snprintf(filepath, MAX_NAME_LEN, "%s/%s", path, entry->d_name);
            
            if (!stat(filepath, &statbuf)) {
                // has entry
                if (S_ISDIR(statbuf.st_mode)) {
                    // directory
                    status = _findOldestCache(filepath, tempPath, &tempStatBuf);
                    if (status == 0) {
                        if (strlen(oldestCachePath) == 0) {
                            // just set
                            rstrcpy (oldestCachePath, tempPath, MAX_NAME_LEN);
                            memcpy (&oldestStatBuf, &tempStatBuf, sizeof(struct stat));
                        } else {
                            // compare
                            if(oldestStatBuf.st_atime > tempStatBuf.st_atime) {
                                rstrcpy (oldestCachePath, tempPath, MAX_NAME_LEN);
                                memcpy (&oldestStatBuf, &tempStatBuf, sizeof(struct stat));                                
                            }
                        }           
                    }
                } else {
                    // file
                    if (strlen(oldestCachePath) == 0) {
                        // just set
                        rstrcpy (oldestCachePath, filepath, MAX_NAME_LEN);
                        memcpy (&oldestStatBuf, &statbuf, sizeof(struct stat));
                    } else {
                        // compare
                        if(oldestStatBuf.st_atime > statbuf.st_atime) {
                            rstrcpy (oldestCachePath, filepath, MAX_NAME_LEN);
                            memcpy (&oldestStatBuf, &statbuf, sizeof(struct stat));                                
                        }
                    }
                }
            }
        }
        closedir(dir);
    }

    if (strlen(oldestCachePath) == 0) {
        return -1;
    }

    rstrcpy (oldCachePath, oldestCachePath, MAX_NAME_LEN);
    memcpy (oldStatbuf, &oldestStatBuf, sizeof(struct stat));

    return (0);
}

static int
_evictOldCache(off_t sizeNeeded) {
    int status;
    char oldCachePath[MAX_NAME_LEN];
    struct stat statbuf;
    off_t cacheSize = 0;
    off_t removedCacheSize = 0;

    if(sizeNeeded <= 0) {
        return 0;
    }

    while(sizeNeeded > removedCacheSize) {
        status = _findOldestCache(PreloadConfig.cachePath, oldCachePath, &statbuf);
        if(status < 0) {
            rodsLog (LOG_DEBUG, "_evictOldCache: findOldestCache failed");
            return status;
        }

        if(statbuf.st_size > 0) {
            cacheSize = statbuf.st_size;
        } else {
            cacheSize = 0;
        }
    
        // remove
        status = unlink(oldCachePath);
        if(status < 0) {
            rodsLog (LOG_DEBUG, "_evictOldCache: unlink failed - %s", oldCachePath);
            return status;
        }

        removedCacheSize += cacheSize;
    }
    
    return (0);
}

static int
_getCachePath(const char *path, char *cachePath) {
    if (path == NULL || cachePath == NULL) {
        rodsLog (LOG_DEBUG, "_getCachePath: given path or cachePath is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if(strlen(path) > 0 && path[0] == '/') {
        snprintf(cachePath, MAX_NAME_LEN, "%s%s", PreloadConfig.cachePath, path);
    } else {
        snprintf(cachePath, MAX_NAME_LEN, "%s/%s", PreloadConfig.cachePath, path);
    }
    return (0);
}

static int
_getCacheWorkPath(const char *path, char *cachePath) {
    if (path == NULL || cachePath == NULL) {
        rodsLog (LOG_DEBUG, "_getCacheWorkPath: given path or cachePath is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if(strlen(path) > 0 && path[0] == '/') {
        snprintf(cachePath, MAX_NAME_LEN, "%s%s%s", PreloadConfig.cachePath, path, PRELOAD_FILES_IN_DOWNLOADING_EXT);
    } else {
        snprintf(cachePath, MAX_NAME_LEN, "%s/%s%s", PreloadConfig.cachePath, path, PRELOAD_FILES_IN_DOWNLOADING_EXT);
    }
    return (0);
}

static int
_preparePreloadCacheDir(const char *path) {
    int status;

    status = makeDirs(path);

    return status; 
}

static int
_renameCache(const char *fromPath, const char *toPath) {
    int status;
    char fromCachePath[MAX_NAME_LEN];
    char toCachePath[MAX_NAME_LEN];

    rodsLog (LOG_DEBUG, "_renameCache: %s -> %s", fromPath, toPath);

    if ((status = _getCachePath(fromPath, fromCachePath)) < 0) {
        return status;
    }

    if ((status = _getCachePath(toPath, toCachePath)) < 0) {
        return status;
    }

    rodsLog (LOG_DEBUG, "_renameCache (local): %s -> %s", fromCachePath, toCachePath);
    status = rename(fromCachePath, toCachePath);

    return status;
}

static int
_truncateCache(const char *path, off_t size) {
    int status;
    char cachePath[MAX_NAME_LEN];

    rodsLog (LOG_DEBUG, "_truncateCache: %s, %ld", path, size);

    if ((status = _getCachePath(path, cachePath)) < 0) {
        return status;
    }

    status = truncate(cachePath, size);

    return status;
}

static int
_removeAllCaches() {
    int status;
    
    if((status = emptyDir(PreloadConfig.cachePath)) < 0) {
        return status;
    }

    return 0;
}

static int
_removeAllIncompleteCaches() {
    int status;

    if((status = _removeIncompleteCaches(PreloadConfig.cachePath)) < 0) {
        return status;
    }

    return 0;
}

static int
_removeIncompleteCaches(const char *path) {
    DIR *dir = opendir(path);
    char filepath[MAX_NAME_LEN];
    struct dirent *entry;
    struct stat statbuf;
    int filenameLen;
    int extLen;
    int status;
    int statusFailed = 0;

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            snprintf(filepath, MAX_NAME_LEN, "%s/%s", path, entry->d_name);

            if (!stat(filepath, &statbuf)) {
                // has entry
                if (S_ISDIR(statbuf.st_mode)) {
                    // directory
                    status = _removeIncompleteCaches(filepath);
                    if (status < 0) {
                        statusFailed = status;
                    }

                    if (isEmptyDir(filepath) == 0) {
                        rodsLog (LOG_DEBUG, "_removeIncompleteCaches: removing empty dir : %s", filepath);
                        status = rmdir(filepath);
                        if (status < 0) {
                            statusFailed = status;
                        }
                    }
                } else {
                    // file
                    filenameLen = strlen(entry->d_name);
                    extLen = strlen(PRELOAD_FILES_IN_DOWNLOADING_EXT);

                    if (filenameLen > extLen && !strcmp(entry->d_name + filenameLen - extLen, PRELOAD_FILES_IN_DOWNLOADING_EXT)) {
                        // found incomplete cache
                        rodsLog (LOG_DEBUG, "_removeIncompleteCaches: removing incomplete cache : %s", filepath);

                        status = unlink(filepath);
                        if (status < 0) {
                            statusFailed = status;
                        }
                    }
                }
            }
        }
        closedir(dir);
    }

    return statusFailed;
}

static int
_getiRODSPath(const char *path, char *iRODSPath) {
    return getiRODSPath(path, iRODSPath, PreloadRodsEnv->rodsHome, PreloadRodsEnv->rodsCwd);
}

#endif 

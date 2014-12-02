/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* irodsFs.c - The main program of the iRODS/Fuse server. It is to be run to
 * serve a single client 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include "irodsFs.h"
#include "iFuseOper.h"
#include "iFuseLib.h"

#ifdef ENABLE_PRELOAD
#include "iFuseLib.Preload.h"
#endif
#ifdef ENABLE_LAZY_UPLOAD
#include "iFuseLib.LazyUpload.h"
#endif

#ifdef ENABLE_TRACE
#include "iFuseLib.Trace.h"
#endif

/* some global variables */

extern rodsEnv MyRodsEnv;
#ifdef ENABLE_PRELOAD
preloadConfig_t MyPreloadConfig;
#endif
#ifdef ENABLE_LAZY_UPLOAD
lazyUploadConfig_t MyLazyUploadConfig;
#endif


#ifdef ENABLE_TRACE
#ifdef  __cplusplus
struct fuse_operations irodsOper; 
#else
static struct fuse_operations irodsOper =
{
  .getattr = traced_irodsGetattr,
  .readlink = traced_irodsReadlink,
  .readdir = traced_irodsReaddir,
  .mknod = traced_irodsMknod,
  .mkdir = traced_irodsMkdir,
  .symlink = traced_irodsSymlink,
  .unlink = traced_irodsUnlink,
  .rmdir = traced_irodsRmdir,
  .rename = traced_irodsRename,
  .link = traced_irodsLink,
  .chmod = traced_irodsChmod,
  .chown = traced_irodsChown,
  .truncate = traced_irodsTruncate,
  .utimens = traced_irodsUtimens,
  .open = traced_irodsOpen,
  .read = traced_irodsRead,
  .write = traced_irodsWrite,
  .statfs = traced_irodsStatfs,
  .release = traced_irodsRelease,
  .fsync = traced_irodsFsync,
  .flush = traced_irodsFlush,
};
#endif

#else   // no ENABLE_TRACE

#ifdef  __cplusplus
struct fuse_operations irodsOper; 
#else
static struct fuse_operations irodsOper =
{
  .getattr = irodsGetattr,
  .readlink = irodsReadlink,
  .readdir = irodsReaddir,
  .mknod = irodsMknod,
  .mkdir = irodsMkdir,
  .symlink = irodsSymlink,
  .unlink = irodsUnlink,
  .rmdir = irodsRmdir,
  .rename = irodsRename,
  .link = irodsLink,
  .chmod = irodsChmod,
  .chown = irodsChown,
  .truncate = irodsTruncate,
  .utimens = irodsUtimens,
  .open = irodsOpen,
  .read = irodsRead,
  .write = irodsWrite,
  .statfs = irodsStatfs,
  .release = irodsRelease,
  .fsync = irodsFsync,
  .flush = irodsFlush,
};
#endif

#endif // ENABLE_TRACE

int parseFuseSpecificCmdLineOpt(int argc, char **argv);
int makeCleanCmdLineOpt (int argc, char **argv, int *argc2, char ***argv2);
int releaseCmdLineOpt (int argc, char **argv);

void usage ();

/* Note - fuse_main parses command line options 
 * static const struct fuse_opt fuse_helper_opts[] = {
    FUSE_HELPER_OPT("-d",          foreground),
    FUSE_HELPER_OPT("debug",       foreground),
    FUSE_HELPER_OPT("-f",          foreground),
    FUSE_HELPER_OPT("-s",          singlethread),
    FUSE_HELPER_OPT("fsname=",     nodefault_subtype),
    FUSE_HELPER_OPT("subtype=",    nodefault_subtype),

    FUSE_OPT_KEY("-h",          KEY_HELP),
    FUSE_OPT_KEY("--help",      KEY_HELP),
    FUSE_OPT_KEY("-ho",         KEY_HELP_NOHEADER),
    FUSE_OPT_KEY("-V",          KEY_VERSION),
    FUSE_OPT_KEY("--version",   KEY_VERSION),
    FUSE_OPT_KEY("-d",          FUSE_OPT_KEY_KEEP),
    FUSE_OPT_KEY("debug",       FUSE_OPT_KEY_KEEP),
    FUSE_OPT_KEY("fsname=",     FUSE_OPT_KEY_KEEP),
    FUSE_OPT_KEY("subtype=",    FUSE_OPT_KEY_KEEP),
    FUSE_OPT_END
};

static void usage(const char *progname)
{
    fprintf(stderr,
            "usage: %s mountpoint [options]\n\n", progname);
    fprintf(stderr,
            "general options:\n"
            "    -o opt,[opt...]        mount options\n"
            "    -h   --help            print help\n"
            "    -V   --version         print version\n"
            "\n");
}

*/

int 
main (int argc, char **argv)
{

#ifdef ENABLE_TRACE
   
irodsOper.getattr = traced_irodsGetattr;
irodsOper.readlink = traced_irodsReadlink;
irodsOper.readdir = traced_irodsReaddir;
irodsOper.mknod = traced_irodsMknod;
irodsOper.mkdir = traced_irodsMkdir;
irodsOper.symlink = traced_irodsSymlink;
irodsOper.unlink = traced_irodsUnlink;
irodsOper.rmdir = traced_irodsRmdir;
irodsOper.rename = traced_irodsRename;
irodsOper.link = traced_irodsLink;
irodsOper.chmod = traced_irodsChmod;
irodsOper.chown = traced_irodsChown;
irodsOper.truncate = traced_irodsTruncate;
irodsOper.utimens = traced_irodsUtimens;
irodsOper.open = traced_irodsOpen;
irodsOper.read = traced_irodsRead;
irodsOper.write = traced_irodsWrite;
irodsOper.statfs = traced_irodsStatfs;
irodsOper.release = traced_irodsRelease;
irodsOper.fsync = traced_irodsFsync;
irodsOper.flush = traced_irodsFlush;

#else
irodsOper.getattr = irodsGetattr;
irodsOper.readlink = irodsReadlink;
irodsOper.readdir = irodsReaddir;
irodsOper.mknod = irodsMknod;
irodsOper.mkdir = irodsMkdir;
irodsOper.symlink = irodsSymlink;
irodsOper.unlink = irodsUnlink;
irodsOper.rmdir = irodsRmdir;
irodsOper.rename = irodsRename;
irodsOper.link = irodsLink;
irodsOper.chmod = irodsChmod;
irodsOper.chown = irodsChown;
irodsOper.truncate = irodsTruncate;
irodsOper.utimens = irodsUtimens;
irodsOper.open = irodsOpen;
irodsOper.read = irodsRead;
irodsOper.write = irodsWrite;
irodsOper.statfs = irodsStatfs;
irodsOper.release = irodsRelease;
irodsOper.fsync = irodsFsync;
irodsOper.flush = irodsFlush;

#endif  // ENABLE_TRACE

    int status;
    rodsArguments_t myRodsArgs;
    char *optStr;

    int new_argc;
    char** new_argv;

#ifdef  __cplusplus
#ifdef ENABLE_TRACE
    bzero (&irodsOper, sizeof (irodsOper));
    irodsOper.getattr = traced_irodsGetattr;
    irodsOper.readlink = traced_irodsReadlink;
    irodsOper.readdir = traced_irodsReaddir;
    irodsOper.mknod = traced_irodsMknod;
    irodsOper.mkdir = traced_irodsMkdir;
    irodsOper.symlink = traced_irodsSymlink;
    irodsOper.unlink = traced_irodsUnlink;
    irodsOper.rmdir = traced_irodsRmdir;
    irodsOper.rename = traced_irodsRename;
    irodsOper.link = traced_irodsLink;
    irodsOper.chmod = traced_irodsChmod;
    irodsOper.chown = traced_irodsChown;
    irodsOper.truncate = traced_irodsTruncate;
    irodsOper.utimens = traced_irodsUtimens;
    irodsOper.open = traced_irodsOpen;
    irodsOper.read = traced_irodsRead;
    irodsOper.write = traced_irodsWrite;
    irodsOper.statfs = traced_irodsStatfs;
    irodsOper.release = traced_irodsRelease;
    irodsOper.fsync = traced_irodsFsync;
    irodsOper.flush = traced_irodsFlush;
#else // no ENABLE_TRACE
    bzero (&irodsOper, sizeof (irodsOper));
    irodsOper.getattr = irodsGetattr;
    irodsOper.readlink = irodsReadlink;
    irodsOper.readdir = irodsReaddir;
    irodsOper.mknod = irodsMknod;
    irodsOper.mkdir = irodsMkdir;
    irodsOper.symlink = irodsSymlink;
    irodsOper.unlink = irodsUnlink;
    irodsOper.rmdir = irodsRmdir;
    irodsOper.rename = irodsRename;
    irodsOper.link = irodsLink;
    irodsOper.chmod = irodsChmod;
    irodsOper.chown = irodsChown;
    irodsOper.truncate = irodsTruncate;
    irodsOper.utimens = irodsUtimens;
    irodsOper.open = irodsOpen;
    irodsOper.read = irodsRead;
    irodsOper.write = irodsWrite;
    irodsOper.statfs = irodsStatfs;
    irodsOper.release = irodsRelease;
    irodsOper.fsync = irodsFsync;
    irodsOper.flush = irodsFlush;
#endif // ENABLE_TRACE
#endif

    status = getRodsEnv (&MyRodsEnv);

    if (status < 0) {
        rodsLogError(LOG_ERROR, status, "main: getRodsEnv error. ");
        exit (1);
    }

    /* handle iRODS-FUSE specific command line options*/
    status = parseFuseSpecificCmdLineOpt (argc, argv);

    if (status < 0) {
        printf("Use -h for help.\n");
        exit (1);
    }

    status = makeCleanCmdLineOpt (argc, argv, &new_argc, &new_argv);

    argc = new_argc;
    argv = new_argv;

    optStr = "hdo:";

    status = parseCmdLineOpt (argc, argv, optStr, 0, &myRodsArgs);

    if (status < 0) {
        printf("Use -h for help.\n");
        exit (1);
    }
    if (myRodsArgs.help==True) {
       usage();
       exit(0);
    }

    srandom((unsigned int) time(0) % getpid());

#ifdef CACHE_FILE_FOR_READ
    if (setAndMkFileCacheDir () < 0) exit (1);
#endif

    initPathCache ();
    initIFuseDesc ();

#ifdef ENABLE_PRELOAD
    // initialize preload
    initPreload (&MyPreloadConfig, &MyRodsEnv, &myRodsArgs);
#endif
#ifdef ENABLE_LAZY_UPLOAD
    // initialize Lazy Upload
    initLazyUpload (&MyLazyUploadConfig, &MyRodsEnv, &myRodsArgs);
#endif    
#ifdef ENABLE_TRACE

    // start tracing
    status = trace_begin( NULL );
    if( status != 0 ) {
        rodsLogError(LOG_ERROR, status, "main: trace_begin failed. ");
        exit(1);
    }

#endif

    status = fuse_main (argc, argv, &irodsOper, NULL);

#ifdef ENABLE_TRACE
    // stop tracing 
    trace_end( NULL );
#endif

    /* release the preload command line options */
    releaseCmdLineOpt (argc, argv);

#ifdef ENABLE_PRELOAD
    // wait preload jobs
    waitPreloadJobs();
#endif

#ifdef ENABLE_PRELOAD
    // uninitialize preload
    uninitPreload (&MyPreloadConfig);
    if (MyPreloadConfig.cachePath != NULL) {
        free(MyPreloadConfig.cachePath);
    }
#endif

#ifdef ENABLE_LAZY_UPLOAD
    // uninitialize lazy upload
    uninitLazyUpload (&MyLazyUploadConfig);
    if (MyLazyUploadConfig.bufferPath!=NULL) {
        free(MyLazyUploadConfig.bufferPath);
    }
#endif

    disconnectAll ();

    if (status < 0) {
        exit (3);
    } else {
        exit(0);
    }
}

int 
parseFuseSpecificCmdLineOpt (int argc, char **argv) {
    int i;
#ifdef ENABLE_PRELOAD
    preloadConfig_t* preloadConfig = &MyPreloadConfig;
#endif
#ifdef ENABLE_LAZY_UPLOAD
    lazyUploadConfig_t* lazyUploadConfig = &MyLazyUploadConfig;
#endif

#ifdef ENABLE_PRELOAD
    memset(&MyPreloadConfig, 0, sizeof(preloadConfig_t));
#endif
#ifdef ENABLE_LAZY_UPLOAD
    memset(&MyLazyUploadConfig, 0, sizeof(lazyUploadConfig_t));
#endif

    for (i=0;i<argc;i++) {
#ifdef ENABLE_PRELOAD
        if (strcmp("--preload", argv[i])==0) {
            preloadConfig->preload=True;
            argv[i]="-Z";
        }
        if (strcmp("--preload-clear-cache", argv[i])==0) {
            preloadConfig->preload=True;
            preloadConfig->clearCache=True;
            argv[i]="-Z";
        }
        if (strcmp("--preload-cache-dir", argv[i])==0) {
            argv[i]="-Z";
            if (i + 2 < argc) {
                if (*argv[i+1] == '-') {
                    rodsLog (LOG_ERROR,
                    "--preload-cache-dir option takes a directory argument");
                    return USER_INPUT_OPTION_ERR;
                }
                preloadConfig->preload=True;
                preloadConfig->cachePath=strdup(argv[i+1]);
                argv[i+1]="-Z";
            }
        }
        if (strcmp("--preload-cache-max", argv[i])==0) {
            argv[i]="-Z";
            if (i + 2 < argc) {
                if (*argv[i+1] == '-') {
                    rodsLog (LOG_ERROR,
                    "--preload-cache-max option takes a size argument");
                    return USER_INPUT_OPTION_ERR;
                }
                preloadConfig->preload=True;
                preloadConfig->cacheMaxSize=strtoll(argv[i+1], 0, 0);
                argv[i+1]="-Z";
            }
        }
        if (strcmp("--preload-file-min", argv[i])==0) {
            argv[i]="-Z";
            if (i + 2 < argc) {
                if (*argv[i+1] == '-') {
                    rodsLog (LOG_ERROR,
                    "--preload-file-min option takes a size argument");
                    return USER_INPUT_OPTION_ERR;
                }
                preloadConfig->preload=True;
                preloadConfig->preloadMinSize=strtoll(argv[i+1], 0, 0);
                argv[i+1]="-Z";
            }
        }
#endif
#ifdef ENABLE_LAZY_UPLOAD
        if (strcmp("--lazyupload", argv[i])==0) {
            lazyUploadConfig->lazyUpload=True;
            argv[i]="-Z";
        }
        if (strcmp("--lazyupload-buffer-dir", argv[i])==0) {
            argv[i]="-Z";
            if (i + 2 < argc) {
                if (*argv[i+1] == '-') {
                    rodsLog (LOG_ERROR,
                    "--lazyupload-cache-dir option takes a directory argument");
                    return USER_INPUT_OPTION_ERR;
                }
                lazyUploadConfig->lazyUpload=True;
                lazyUploadConfig->bufferPath=strdup(argv[i+1]);
                argv[i+1]="-Z";
            }
        }
#endif
#ifdef ENABLE_TRACE

        int rc = trace_read_arg( argc, argv, i );
        if( rc != 0 ) {
           return rc;
        }
#endif
    }

    // set default
#ifdef ENABLE_PRELOAD
    if(preloadConfig->cachePath == NULL) {
        rodsLog (LOG_DEBUG, "parseFuseSpecificCmdLineOpt: uses default preload cache dir - %s", FUSE_PRELOAD_CACHE_DIR);
        preloadConfig->cachePath=strdup(FUSE_PRELOAD_CACHE_DIR);
    }

    if(preloadConfig->preloadMinSize < MAX_READ_CACHE_SIZE) {
        // in this case, given iRODS file is not cached by preload but cached by file cache.
        rodsLog (LOG_DEBUG, "parseFuseSpecificCmdLineOpt: uses default min size %lld - (given %lld)", MAX_READ_CACHE_SIZE, preloadConfig->preloadMinSize);
        preloadConfig->preloadMinSize = MAX_READ_CACHE_SIZE;
    }
#endif
#ifdef ENABLE_LAZY_UPLOAD
    if(lazyUploadConfig->bufferPath == NULL) {
        rodsLog (LOG_DEBUG, "parseFuseSpecificCmdLineOpt: uses default lazy-upload buffer dir - %s", FUSE_LAZY_UPLOAD_BUFFER_DIR);
        lazyUploadConfig->bufferPath=strdup(FUSE_LAZY_UPLOAD_BUFFER_DIR);
    }
#endif

    return(0);
}

int
makeCleanCmdLineOpt (int argc, char **argv, int *argc2, char ***argv2) {
    int i;
    int actual_argc = 0;
    int j;

    if (argc2 == NULL) {
        return -1;
    }
    
    if (argv2 == NULL) {
        return -1;
    }

    for (i=0;i<argc;i++) {
        if (strcmp("-Z", argv[i])!=0) {
            actual_argc++;
        }
    }

    *argv2 = (char**)malloc(sizeof(char*) * actual_argc);

    j = 0;
    for (i=0;i<argc;i++) {
        if (strcmp("-Z", argv[i])!=0) {
            (*argv2)[j] = strdup(argv[i]);
            j++;
        }
    }

    *argc2 = actual_argc;
    return(0);
}

int
releaseCmdLineOpt (int argc, char **argv) {
    int i;
    
    if (argv != NULL) {
        for (i=0;i<argc;i++) {
            if (argv[i] != NULL) {
                free(argv[i]);
            }
        }
        free(argv);
    }
    
    return(0);
}

void
usage ()
{
   char *msgs[]={
   "Usage : irodsFs [-hd] [-o opt,[opt...]]",
"Single user iRODS/Fuse server",
"Options are:",
" -h  this help",
" -d  FUSE debug mode",
" -o  opt,[opt...]  FUSE mount options",

#ifdef ENABLE_PRELOAD
" ",
"Extended Options for Preload",
" --preload                use preload",
" --preload-clear-cache    clear preload caches",
" --preload-cache-dir      specify preload cache directory",
" --preload-cache-max      specify preload cache max limit (in bytes)", 
" --preload-file-min       specify minimum file size that will be preloaded (in bytes)",
#endif
#ifdef ENABLE_LAZY_UPLOAD
" ",
"Extended Options for LazyUpload",
" --lazyupload             use lazy-upload",
" --lazyupload-buffer-dir  specify lazy-upload buffer directory",
#endif
#ifdef ENABLE_TRACE
" ",
"Extended Options for Tracing",
" --trace [on|off]         Enable/disable access tracing",
" --trace-path-salt SALT   Salt to add to the paths when hashing them in the trace log",
" --trace-host HOSTNAME    Send traces to HOSTNAME",
" --trace-port PORTNUM     Connect to the trace host on port PORTNUM",
" --trace-timeout SECONDS  Number of seconds to wait before giving up connecting to the trace host",
" --trace-sync-delay SECONDS   Number of seconds between sending trace snapshots",
#endif
""};
    int i;
    for (i=0;;i++) {
        if (strlen(msgs[i])==0) 
            break;
         printf("%s\n",msgs[i]);
    }
    
#ifdef ENABLE_TRACE
    trace_usage();
#endif // ENABLE_TRACE
}



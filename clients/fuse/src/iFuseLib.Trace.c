#ifndef _TEST_TRACE
#include "irodsFs.h"
#include "iFuseOper.h"
#include "miscUtil.h"

#else

#define USER_INPUT_OPTION_ERR  -1

#endif 

#include "iFuseLib.Trace.h"

struct log_context* LOGCTX = NULL;


#ifndef _TEST_TRACE
int traced_irodsGetattr(const char *path, struct stat *stbuf) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_GETATTR, "irodsGetattr(%s, %p)\n", path_hash, stbuf );

   int rc = irodsGetattr( path, stbuf );
   
   logmsg( LOGCTX, TRACE_GETATTR, "irodsGetattr(%s, %p) rc = %d\n", path_hash, stbuf, rc );
   
   return rc;
}

int traced_irodsReadlink(const char *path, char *buf, size_t size) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_READLINK, "irodsReadlink(%s, %p, %zu)\n", path_hash, buf, size );
   
   int rc = irodsReadlink( path, buf, size );
   
   logmsg( LOGCTX, TRACE_READLINK, "irodsReadlink(%s, %p, %zu) rc = %d\n", path_hash, buf, size, rc );
   
   return rc;
}

int traced_irodsMknod(const char *path, mode_t mode, dev_t rdev) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );
   logmsg( LOGCTX, TRACE_MKNOD, "irodsMknod(%s, %o, %lX)\n", path_hash, mode, rdev );
   
   int rc = irodsMknod( path, mode, rdev );

   logmsg( LOGCTX, TRACE_MKNOD, "irodsMknod(%s, %o, %lX) rc = %d\n", path_hash, mode, rdev, rc );
   
   return rc;
}

int traced_irodsMkdir(const char *path, mode_t mode) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_MKDIR, "irodsMkdir(%s, %o)\n", path_hash, mode );
   
   int rc = irodsMkdir( path, mode );
   
   logmsg( LOGCTX, TRACE_MKDIR, "irodsMkdir(%s, %o) rc = %d\n", path_hash, mode, rc );
   
   return rc;
}

int traced_irodsUnlink(const char *path) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_UNLINK, "irodsUnlink(%s)\n", path_hash );
   
   int rc = irodsUnlink( path );

   logmsg( LOGCTX, TRACE_UNLINK, "irodsUnlink(%s) rc = %d\n", path_hash, rc );
   
   return rc;
}

int traced_irodsRmdir(const char *path) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_RMDIR, "irodsRmdir(%s)\n", path_hash );
   
   int rc = irodsRmdir( path );

   logmsg( LOGCTX, TRACE_RMDIR, "irodsRmdir(%s) rc = %d\n", path_hash, rc );
   
   return rc;
}

int traced_irodsSymlink(const char *from, const char *to) {
   
   char from_hash[LOG_PATH_HASH_LEN];
   char to_hash[LOG_PATH_HASH_LEN];
   
   log_hash_path( LOGCTX, from, from_hash );    
   log_hash_path( LOGCTX, to, to_hash );
   
   logmsg( LOGCTX, TRACE_SYMLINK, "irodsSymlink(%s, %s)\n", from_hash, to_hash );
   
   int rc = irodsSymlink( from, to );

   logmsg( LOGCTX, TRACE_SYMLINK, "irodsSymlink(%s, %s) rc = %d\n", from_hash, to_hash, rc );
   
   return rc;
}

int traced_irodsRename(const char *from, const char *to) {

   char from_hash[LOG_PATH_HASH_LEN];
   char to_hash[LOG_PATH_HASH_LEN];
   
   log_hash_path( LOGCTX, from, from_hash );    
   log_hash_path( LOGCTX, to, to_hash );
   
   logmsg( LOGCTX, TRACE_RENAME, "irodsRename(%s, %s)\n", from_hash, to_hash );
   
   int rc = irodsRename( from, to );

   logmsg( LOGCTX, TRACE_RENAME, "irodsRename(%s, %s) rc = %d\n", from_hash, to_hash, rc );
   
   return rc;
}

int traced_irodsLink(const char *from, const char *to) {

   char from_hash[LOG_PATH_HASH_LEN];
   char to_hash[LOG_PATH_HASH_LEN];
   
   log_hash_path( LOGCTX, from, from_hash );    
   log_hash_path( LOGCTX, to, to_hash );
   
   logmsg( LOGCTX, TRACE_LINK, "irodsLink(%s, %s)", from_hash, to_hash );
   
   int rc = irodsLink( from, to );

   logmsg( LOGCTX, TRACE_LINK, "irodsLink(%s, %s) rc = %d", from_hash, to_hash, rc );
   
   return rc;
}

int traced_irodsChmod(const char *path, mode_t mode) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_CHMOD, "irodsChmod(%s, %o)\n", path_hash, mode );
   
   int rc = irodsChmod( path, mode );

   logmsg( LOGCTX, TRACE_CHMOD, "irodsChmod(%s, %o) rc = %d\n", path_hash, mode, rc );
   
   return rc;
}

int traced_irodsChown(const char *path, uid_t uid, gid_t gid) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_CHOWN, "irodsChown(%s, %d, %d)\n", path_hash, uid, gid);
   
   int rc = irodsChown( path, uid, gid );

   logmsg( LOGCTX, TRACE_CHOWN, "irodsChown(%s, %d, %d) rc = %d\n", path_hash, uid, gid, rc);
   
   return rc;
}

int traced_irodsTruncate(const char *path, off_t size) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_TRUNCATE, "irodsTruncate(%s, %zu)\n", path_hash, size);
   
   int rc = irodsTruncate( path, size );

   logmsg( LOGCTX, TRACE_TRUNCATE, "irodsTruncate(%s, %zu) rc = %d\n", path_hash, size, rc);
   
   return rc;
}

int traced_irodsFlush(const char *path, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_FLUSH, "irodsFlush(%s, %p)\n", path_hash, fi);
   
   int rc = irodsFlush( path, fi );

   logmsg( LOGCTX, TRACE_FLUSH, "irodsFlush(%s, %p) rc = %d\n", path_hash, fi, rc);
   
   return rc;
}

int traced_irodsUtimens (const char *path, const struct timespec ts[]) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_UTIMENS, "irodsUtimens(%s, %ld, %ld, %ld, %ld)\n", path_hash, ts[0].tv_sec, ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec );
   
   int rc = irodsUtimens( path, ts );

   logmsg( LOGCTX, TRACE_UTIMENS, "irodsUtimens(%s, %ld, %ld, %ld, %ld) rc = %d\n", path_hash, ts[0].tv_sec, ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rc );
   
   return rc;
}

int traced_irodsOpen(const char *path, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_OPEN, "irodsOpen(%s, %p, flags=%X)\n", path_hash, fi, fi->flags );
   
   int rc = irodsOpen( path, fi );

   logmsg( LOGCTX, TRACE_OPEN, "irodsOpen(%s, %p, flags=%X) rc = %d\n", path_hash, fi, fi->flags, rc );
   
   return rc;
}

int traced_irodsRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_READ, "irodsRead(%s, %p, %zu, %jd, %p)\n", path_hash, buf, size, offset, fi);
   
   int rc = irodsRead( path, buf, size, offset, fi );

   logmsg( LOGCTX, TRACE_READ, "irodsRead(%s, %p, %zu, %jd, %p) rc = %d\n", path_hash, buf, size, offset, fi, rc);
   
   return rc;
}

int traced_irodsWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_WRITE, "irodsWrite(%s, %p, %zu, %jd, %p)\n", path_hash, buf, size, offset, fi );
   
   int rc = irodsWrite( path, buf, size, offset, fi );

   logmsg( LOGCTX, TRACE_WRITE, "irodsWrite(%s, %p, %zu, %jd, %p) rc = %d\n", path_hash, buf, size, offset, fi, rc );
   
   return rc;
}

int traced_irodsStatfs(const char *path, struct statvfs *stbuf) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_STATFS, "irodsStatfs(%s, %p)\n", path_hash, stbuf );
   
   int rc = irodsStatfs( path, stbuf );

   logmsg( LOGCTX, TRACE_STATFS, "irodsStatfs(%s, %p) rc = %d\n", path_hash, stbuf, rc );
   
   return rc;
}

int traced_irodsRelease(const char *path, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_RELEASE, "irodsRelease(%s, %p)\n", path_hash, fi );
   
   int rc = irodsRelease( path, fi );

   logmsg( LOGCTX, TRACE_RELEASE, "irodsRelease(%s, %p) rc = %d\n", path_hash, fi, rc );
   
   return rc;
}

int traced_irodsFsync (const char *path, int isdatasync, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_FSYNC, "irodsFsync(%s, %d, %p)\n", path_hash, isdatasync, fi );
   
   int rc = irodsFsync( path, isdatasync, fi );
   
   logmsg( LOGCTX, TRACE_FSYNC, "irodsFsync(%s, %d, %p) rc = %d\n", path_hash, isdatasync, fi, rc );
   
   return rc;
}

int traced_irodsReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
   
   char path_hash[LOG_PATH_HASH_LEN];
   log_hash_path( LOGCTX, path, path_hash );    
   logmsg( LOGCTX, TRACE_READDIR, "irodsReaddir(%s, %p, %p, %jd, %p)\n", path_hash, buf, filler, offset, fi );
   
   int rc = irodsReaddir( path, buf, filler, offset, fi );

   logmsg( LOGCTX, TRACE_READDIR, "irodsReaddir(%s, %p, %p, %jd, %p) rc = %d\n", path_hash, buf, filler, offset, fi, rc );
   
   return rc;
}
#endif


struct trace_method_to_bit {
   char const* method_name;
   uint64_t bit;
};

int trace_get_methods_mask( char const* methods_csv_const, uint64_t* ret_mask ) {
   
   static const struct trace_method_to_bit trace_bits[] = {
      {"getattr",               TRACE_GETATTR},
      {"readlink",              TRACE_READLINK},
      {"mknod",                 TRACE_MKNOD},
      {"mkdir",                 TRACE_MKDIR},
      {"unlink",                TRACE_UNLINK},
      {"rmdir",                 TRACE_RMDIR},
      {"symlink",               TRACE_SYMLINK},
      {"rename",                TRACE_RENAME},
      {"link",                  TRACE_LINK},
      {"chmod",                 TRACE_CHMOD},
      {"chown",                 TRACE_CHOWN},
      {"truncate",              TRACE_TRUNCATE},
      {"flush",                 TRACE_FLUSH},
      {"utimens",               TRACE_UTIMENS},
      {"open",                  TRACE_OPEN},
      {"read",                  TRACE_READ},
      {"write",                 TRACE_WRITE},
      {"statfs",                TRACE_STATFS},
      {"release",               TRACE_RELEASE},
      {"fsync",                 TRACE_FSYNC},
      {"readdir",               TRACE_READDIR},
      {"all",                   TRACE_ALL},
      {NULL,                    0}
   };
   
   char* tmp = NULL;
   char* methods_csv = strdup( methods_csv_const );
   char* method_tok = NULL;
   char* methods_csv_strtok = methods_csv;
   uint64_t mask = 0;
   
   if( methods_csv == NULL ) {
      return -ENOMEM;
   }
   
   // tokenize by ',' and ' '
   while( 1 ) {
      
      method_tok = strtok_r( methods_csv_strtok, ", ", &tmp );
      methods_csv_strtok = NULL;
      
      if( method_tok == NULL ) {
         // end 
         break;
      }
      
      // find method
      for( int i = 0; trace_bits[i].method_name != NULL; i++ ) {
         
         if( strcmp( trace_bits[i].method_name, method_tok ) == 0 ) {
            mask |= trace_bits[i].bit;
         }
      }
   }
   
   free( methods_csv );
   *ret_mask = mask;
   
   return 0;
}


int trace_get_environment_variables( char** http_host, int* portnum, int* sync_delay, int* timeout, int* max_lines, uint64_t* methods, char** path_salt, char** log_dir ) {
   
   // get the environment 
   *http_host = strdup_or_default( getenv("IRODSFS_LOG_SERVER_HOSTNAME"), HTTP_LOG_SERVER_HOSTNAME );
   *path_salt = strdup_or_default( getenv("IRODSFS_LOG_PATH_SALT"), LOG_FILENAME_SALT );
   *log_dir = strdup_or_default( getenv("IRODSFS_LOG_DIR"), LOG_PATH_DIR );
   
   char* http_port_str = strdup_or_default( getenv("IRODSFS_LOG_SERVER_PORTNUM"), HTTP_LOG_SERVER_PORTNUM_STR );
   char* sync_delay_str = strdup_or_default( getenv("IRODSFS_LOG_SERVER_SYNC_DELAY"), HTTP_LOG_SYNC_TIMEOUT_STR );
   char* timeout_str = strdup_or_default( getenv("IRODSFS_LOG_SERVER_TIMEOUT"), HTTP_LOG_SERVER_TIMEOUT_STR );
   char* max_lines_str = strdup_or_default( getenv("IRODSFS_LOG_MAX_LINES"), HTTP_LOG_MAX_LINES_STR );
   char* methods_csv = strdup_or_default( getenv("IRODSFS_LOG_METHODS"), "all" );
   int rc = 0;
   
   // parse!
   char* tmp = NULL;
   
   *portnum = (int)strtoll( http_port_str, &tmp, 10 );
   if( tmp == NULL || *portnum < 0 || *portnum >= 65535 ) {
      
      fprintf(stderr, "WARN: invalid port number %d.  Using default %d\n", *portnum, HTTP_LOG_SERVER_PORTNUM );
      *portnum = HTTP_LOG_SERVER_PORTNUM;
   }
   
   *sync_delay = (int)strtoll( sync_delay_str, &tmp, 10 );
   if( tmp == NULL || *sync_delay <= 0 ) {
      
      fprintf(stderr, "WARN: invalid sync delay of %d seconds.  Using default of %d seconds\n", *sync_delay, HTTP_LOG_SYNC_TIMEOUT );
      *sync_delay = HTTP_LOG_SYNC_TIMEOUT;
   }
   
   *timeout = (int)strtoll( timeout_str, &tmp, 10 );
   if( tmp == NULL || *timeout <= 0 ) {
      
      fprintf(stderr, "WARN: invalid timeout delay of %d seconds.  Using default of %d seconds\n", *timeout, HTTP_LOG_SERVER_TIMEOUT );
      *timeout = HTTP_LOG_SERVER_TIMEOUT;
   }
   
   *max_lines = (int)strtoll( max_lines_str, &tmp, 10 );
   if( tmp == NULL || *max_lines <= 0 ) {
      
      fprintf(stderr, "WARN: invalid maximum line count %d.  Using default of %d lines\n", *max_lines, HTTP_LOG_MAX_LINES );
      *max_lines = HTTP_LOG_MAX_LINES;
   }
   
   rc = trace_get_methods_mask( methods_csv, methods );
   if( rc != 0 ) {
      
      fprintf(stderr, "WARN: invalid methods list '%s'.  Using default of '%s'\n", methods_csv, "all" );
      *methods = TRACE_ALL;
   }
   
   free( http_port_str );
   free( sync_delay_str );
   free( timeout_str );
   
#ifdef _TEST_TRACE
   printf("ENV: IRODSFS_LOG_SERVER_HOSTNAME   = %s\n", *http_host );
   printf("ENV: IRODSFS_LOG_SERVER_PORTNUM    = %d\n", *portnum );
   printf("ENV: IRODSFS_LOG_PATH_SALT         = %s\n", *path_salt );
   printf("ENV: IRODSFS_LOG_DIR               = %s\n", *log_dir );
   printf("ENV: IRODSFS_LOG_SERVER_SYNC_DELAY = %d\n", *sync_delay );
   printf("ENV: IRODSFS_LOG_SERVER_TIMEOUT    = %d\n", *timeout );
   printf("ENV: IRODSFS_LOG_MAX_LINES         = %d\n", *max_lines );
   printf("ENV: IRODSFS_LOG_METHODS           = %" PRIX64 "\n", *methods );
#endif
   
   return 0;
}


int trace_begin( struct log_context** ret ) {

   // are we even going to trace?
   char* trace_status = strdup_or_default( getenv("IRODSFS_LOG_TRACE"), "1" );
   
   if( strcmp(trace_status, "0") == 0 ||
       strcasecmp(trace_status, "false") == 0 ||
       strcasecmp(trace_status, "off") == 0 ||
       strcasecmp(trace_status, "disabled") == 0 ||
       strcasecmp(trace_status, "disable") == 0 ) {
      
      // user probably wants this disabled 
      free( trace_status );
      return 0;
   }
   
   free( trace_status );
   
   // get the environment 
   char* http_host = NULL;
   char* log_path_salt = NULL;
   char* log_dir = NULL;
   int portnum = 0;
   int sync_delay = 0;
   int timeout = 0;
   int max_lines = 0;
   uint64_t methods = 0;
   
   trace_get_environment_variables( &http_host, &portnum, &sync_delay, &timeout, &max_lines, &methods, &log_path_salt, &log_dir );
   
   // set up logging 
   struct log_context* ctx = log_init( http_host, portnum, sync_delay, timeout, max_lines, methods, log_path_salt, log_dir );
   
   if( ctx == NULL ) {
      // OOM
      fprintf(stderr, "FATAL: out of memory\n");
      
      free( http_host );
      free( log_path_salt );
      
      return -1;
   }
   
   // start up logging 
   int rc = log_start_threads( ctx );
   if( rc != 0 ) {
      fprintf(stderr, "FATAL: log_start_threads rc = %d\n", rc );
      
      log_free( ctx );
      
      free( http_host );
      free( log_path_salt );
      
      return rc;
   }
   
   logmsg( ctx, TRACE_ALL, "%s", "trace_begin\n");
   
   free( http_host );
   free( log_path_salt );
   
   if( ret != NULL ) {
      *ret = ctx;
   }
   else {
      // global init 
      LOGCTX = ctx;
   }
   
   return 0;
}


int trace_end( struct log_context** ctx ) {
   
   int rc = 0;
   
   if( ctx == NULL ) {
      // global shutdown 
      ctx = &LOGCTX;
   }

   logmsg( *ctx, TRACE_ALL, "%s", "trace_end\n");
   
   // stop the threads 
   rc = log_stop_threads( *ctx );
   if( rc != 0 ) {
      fprintf(stderr, "ERR: log_stop_threads rc = %d\n", rc );
      return rc;
   }
   
   // sync the last of the logs 
   rc = log_rollover( *ctx );
   if( rc != 0 ) {
      fprintf(stderr, "WARN: log_rollover rc = %d\n", rc );
   }
   
   // upload the last of the logs 
   rc = http_sync_all_logs( *ctx );
   if( rc != 0 ) {
      fprintf(stderr, "WARN: http_sync_all_logs rc = %d\n", rc );
   }
   
   log_free( *ctx );
   *ctx = NULL;
   
   return rc;
}

static int trace_setenv_from_argv( char const* varname, int argc, char** argv, int i ) {
   if( i + 2 < argc ) {
      if( argv[i+1][0] == '-' ) {
         fprintf(stderr, "Required argument for option %s", argv[i] );
         return USER_INPUT_OPTION_ERR;
      }
      else {
         // argv overrides environment
         setenv( varname, argv[i+1], 1 );
      }
   }
   else {
      fprintf(stderr, "Required argument for option %s", argv[i] );
      return USER_INPUT_OPTION_ERR;
   }

   return 0;
}

static char* zeroed_argv = (char*)"-Z";

int trace_read_arg( int argc, char** argv, int i ) {

   // see if the ith argument in argv is a trace option, and enable it.
   
   static char const* argv_to_envar[][2] = {
      {"--trace",            "IRODSFS_LOG_TRACE"},
      {"--trace-host",       "IRODSFS_LOG_SERVER_HOSTNAME"},
      {"--trace-port",       "IRODSFS_LOG_SERVER_PORTNUM"},
      {"--trace-salt",       "IRODSFS_LOG_PATH_SALT"},
      {"--trace-timeout",    "IRODSFS_LOG_SERVER_TIMEOUT"},
      {"--trace-sync-delay", "IRODSFS_LOG_SERVER_SYNC_DELAY"},
      {"--trace-max-lines",  "IRODSFS_LOG_MAX_LINES"},
      {"--trace-methods",    "IRODSFS_LOG_METHODS"},
      {"--trace-log-dir",    "IRODSFS_LOG_DIR"},
      {NULL, NULL}
   };

   int rc = 0;

   for( int j = 0; argv_to_envar[j][0] != NULL; j++ ) {
      
      if( strcmp(argv_to_envar[j][0], argv[i]) == 0 ) {
         
         rc = trace_setenv_from_argv( argv_to_envar[j][1], argc, argv, i );
         if( rc != 0 ) {
            return rc;
         }

         // consumed!
         argv[i] = zeroed_argv;
         argv[i+1] = zeroed_argv;
         break;
      }
   }

   return 0;
}


void trace_usage(void) {
   char const* msgs[] = {
" ",
"Special environment variables that control tracing:",
" IRODSFS_LOG_TRACE                Activate/disable tracing by setting this to 'on' or 'off'",
" ",
" IRODSFS_LOG_PATH_SALT            A string to be used to salt path hashes when logging.",
"                                  It is best to make this at least 256 random characters.",
" ",
" IRODSFS_LOG_SERVER_HOSTNAME      The hostname of the log server that will receive access",
"                                  traces from this filesystem.  The built-in default is",
"                                  " HTTP_LOG_SERVER_HOSTNAME,
" ",
" IRODSFS_LOG_SERVER_PORTNUM       The port number of said log server.  The built-in",
"                                  default is " HTTP_LOG_SERVER_PORTNUM_STR,
" ",
" IRODSFS_LOG_SERVER_TIMEOUT       The number of seconds to wait before giving up on sending",
"                                  a compressed trace to the log server.  The built-in default",
"                                  is " HTTP_LOG_SERVER_TIMEOUT_STR,
" ",
" IRODSFS_LOG_SERVER_SYNC_DELAY    The number of seconds to wait between uploading snapshots",
"                                  of access traces to the log server.  The default is " HTTP_LOG_SYNC_TIMEOUT_STR,
" ",
" IRODSFS_LOG_MAX_LINES            The number of operations to log before trying to upload them to the",
"                                  log server.  The default is " HTTP_LOG_MAX_LINES_STR,
" ",
" IRODSFS_LOG_DIR                  Directory in which to store log files.  The default is " LOG_PATH_DIR,
" "
" IRODSFS_LOG_METHODS              Comma-separated list of which methods to log.  Valid options are "
"                                  getattr, readlink, mknod, mkdir, unlink, rmdir, symlink, reaname, link,"
"                                  chmod, chown, truncate, flush, utimens, oen, read, write, statfs, fsync,"
"                                  readdir, release.  Pass \"all\" for every method (this is the default)."
""};
   int i;
   for (i=0;;i++) {
      if (strlen(msgs[i])==0) return;
      printf("%s\n",msgs[i]);
   }
}


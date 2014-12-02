#include "iFuseLib.Logging.h" 
#include "iFuseLib.Http.h"

// get task ID (no glibc wrapper around this...)
pid_t gettid(void) {
   return syscall( __NR_gettid );
}

// concatenate two paths
static char* log_fullpath( char const* root, char const* path, char* dest ) {
   char delim = 0;
   int path_off = 0;
   
   int len = strlen(path) + strlen(root) + 2;
   
   if( strlen(root) > 0 ) {
      size_t root_delim_off = strlen(root) - 1;
      if( root[root_delim_off] != '/' && path[0] != '/' ) {
         len++;
         delim = '/';
      }
      else if( root[root_delim_off] == '/' && path[0] == '/' ) {
         path_off = 1;
      }
   }

   if( dest == NULL ) {
      dest = (char*)calloc( len, 1 );
   }
   
   memset(dest, 0, len);
   
   strcpy( dest, root );
   if( delim != 0 ) {
      dest[strlen(dest)] = '/';
   }
   strcat( dest, path + path_off );
   
   return dest;
}

// hash a path 
void log_hash_path( struct log_context* ctx, char const* path, char hash_buf[LOG_PATH_HASH_LEN] ) {
   
   unsigned char digest[SHA256_DIGEST_LENGTH];

   // omit the last / in the path 
   size_t path_len = strlen(path);
   if( path[path_len - 1] == '/' )
      path_len --;
   
   SHA256_CTX context;
   SHA256_Init( &context );

   SHA256_Update( &context, ctx->log_path_salt, strlen(ctx->log_path_salt) );
   SHA256_Update( &context, path, path_len );

   SHA256_Final( digest, &context );

   char buf[3];
   for( int i = 0; i < SHA256_DIGEST_LENGTH; i++ ) {
      sprintf(buf, "%02x", digest[i] );
      hash_buf[2*i] = buf[0];
      hash_buf[2*i + 1] = buf[1];
   }
   hash_buf[ 2 * SHA256_DIGEST_LENGTH ] = 0;
}


// open up a log as a temporary file, piped through gzip
// log_path will be modified by this method.
static FILE* log_open( char* log_path, int* ret_tmp_fd ) {
   
   int rc = 0;
   char const* gzip_cmd_fmt = "/bin/gzip >&%d";
   int tmpfd = 0;
   
   // open temporary file
   tmpfd = mkstemp( log_path );
   if( tmpfd < 0 ) {
      
      rc = -errno;
      fprintf(stderr, "log_open: mkstemp(%s) errno = %d\n", log_path, rc );
      
      return NULL;
   }
   
   // pipe log data through gzip 
   size_t gzip_cmd_len = strlen(gzip_cmd_fmt) + 10;
   char* gzip_cmd = (char*)calloc( gzip_cmd_len + 1, 1 );
   
   if( gzip_cmd == NULL ) {
      close( tmpfd );
      return NULL;
   }
   
   snprintf( gzip_cmd, gzip_cmd_len, gzip_cmd_fmt, tmpfd );
   
   // begin piping the data into gzip
   FILE* gzip_pipe = popen( gzip_cmd, "w" );
   
   if( gzip_pipe == NULL ) {
      rc = -errno;
      fprintf(stderr, "log_open: popen(%s) errno = %d\n", gzip_cmd, rc );
      
      close( tmpfd );
      return NULL;
   }
   
   *ret_tmp_fd = tmpfd;
   
   return gzip_pipe;
}


// set up a log context 
struct log_context* log_init( char const* http_server, int http_port, int sync_delay, int timeout, int max_lines, uint64_t methods, char const* log_path_salt, char const* log_path_dir ) {
   
   struct log_context* logctx = (struct log_context*)calloc( sizeof(struct log_context), 1 );
   if( logctx == NULL ) {
      return NULL;
   }
   
   logctx->hostname = strdup( http_server );
   logctx->log_path_salt = strdup( log_path_salt );
   logctx->sync_buf = new log_sync_buf_t();
   
   if( logctx->hostname == NULL || logctx->sync_buf == NULL || logctx->log_path_salt == NULL ) {
      
      // OOM
      log_free( logctx );
      return NULL;
   }
   
   char* log_path = log_fullpath( log_path_dir, LOG_PATH_FMT, NULL );
   if( log_path == NULL ) {
      
      // OOM
      log_free( logctx );
      return NULL;
   }
   
   int tmpfd = -1;
   FILE* logfile_pipe = log_open( log_path, &tmpfd );
   if( logfile_pipe == NULL ) {
      
      fprintf(stderr, "log_open(%s) failed\n", log_path );
      
      free( log_path );
      log_free( logctx );
      return NULL;
   }
   
   logctx->logfile_pipe = logfile_pipe;
   logctx->logfile_path = log_path;
   
   logctx->portnum = http_port;
   logctx->sync_delay = sync_delay;
   logctx->running = 0;
   logctx->timeout = timeout;
   logctx->max_lines = max_lines;
   logctx->num_lines = 0;
   logctx->methods = methods;
   logctx->tmpfd = tmpfd;
   logctx->logfile_dir = strdup( log_path_dir );
   logctx->logfile_path = log_path;
   
   pthread_rwlock_init( &logctx->lock, NULL );
   sem_init( &logctx->sync_sem, 0, 0 );
   sem_init( &logctx->rollover_sem, 0, 0 );
   
   return logctx;
}


// free a log context 
int log_free( struct log_context* logctx ) {
   
   if( logctx->running ) {
      return -EINVAL;
   }
   
   if( logctx->logfile_path ) {
      free( logctx->logfile_path );
      logctx->logfile_path = NULL;
   }
   
   if( logctx->logfile_dir ) {
      free( logctx->logfile_dir );
      logctx->logfile_dir = NULL;
   }
   
   if( logctx->hostname ) {
      free( logctx->hostname );
      logctx->hostname = NULL;
   }
   
   if( logctx->log_path_salt ) {
      free( logctx->log_path_salt );
      logctx->log_path_salt = NULL;
   }
   
   if( logctx->sync_buf ) {
      
      for( unsigned int i = 0; i < logctx->sync_buf->size(); i++ ) {
         
         if( logctx->sync_buf->at(i) != NULL ) {
            
            free( logctx->sync_buf->at(i) );
         }
      }
      
      logctx->sync_buf->clear();
      
      delete logctx->sync_buf;
      logctx->sync_buf = NULL;
   }
   
   pthread_rwlock_destroy( &logctx->lock );
   sem_destroy( &logctx->sync_sem );
   sem_destroy( &logctx->rollover_sem );
   
   pclose( logctx->logfile_pipe );
   close( logctx->tmpfd );
   
   memset( logctx, 0, sizeof(struct log_context) );
   free( logctx );
   
   return 0;
}


// start up the logging thread
int log_start_threads( struct log_context* logctx ) {
   
   pthread_attr_t attrs;
   int rc;
   
   rc = pthread_attr_init( &attrs );
   if( rc != 0 ) {
      fprintf(stderr, "pthread_attr_init rc = %d\n", rc);
      return rc;
   }
   
   logctx->running = 1;
   
   // start the rollover thread 
   rc = pthread_create( &logctx->rollover_thread, &attrs, log_rollover_thread, logctx );
   if( rc != 0 ) {
      
      fprintf( stderr, "pthread_create(rollover) rc = %d\n", rc );
      logctx->running = 0;
      return rc;
   }
   
   // start the sync thread 
   rc = pthread_create( &logctx->sync_thread, &attrs, http_sync_log_thread, logctx );
   if( rc != 0 ) {
      
      fprintf(stderr, "pthread_create(sync) rc = %d\n", rc );
      logctx->running = 0;
      
      // join the rollover thread 
      pthread_cancel( logctx->rollover_thread );
      pthread_join( logctx->rollover_thread, NULL );
      return rc;
   }
   
   return 0;
}


// stop the logging thread 
int log_stop_threads( struct log_context* logctx ) {
   
   logctx->running = 0;
   
   pthread_cancel( logctx->rollover_thread );
   pthread_join( logctx->rollover_thread, NULL );
   
   pthread_cancel( logctx->sync_thread );
   pthread_join( logctx->sync_thread, NULL );
   
   return 0;
}


// swap logs atomically, cleaning up the old log
// save the path to the previous log, so it can be queued for synchronization
int log_swap( struct log_context* logctx, char** ret_compressed_logfile_path ) {
   
   if( logctx == NULL ) {
      return -EINVAL;
   }
   
   pthread_rwlock_wrlock( &logctx->lock );
   
   // new path 
   char* new_logpath = log_fullpath( logctx->logfile_dir, LOG_PATH_FMT, NULL );
   
   if( new_logpath == NULL ) {
      
      pthread_rwlock_unlock( &logctx->lock );
      
      fprintf(stderr, "log_swap: log_fullpath(%s) failed\n", logctx->logfile_dir);
      return -EPERM;
   }
   
   // new logfile 
   int new_tmpfd = -1;
   FILE* new_logfile_pipe = log_open( new_logpath, &new_tmpfd );
   if( new_logfile_pipe == NULL ) {
      
      pthread_rwlock_unlock( &logctx->lock );
      
      fprintf(stderr, "log_swap: log_open(%s) failed\n", new_logpath );
      
      free( new_logpath );
      return -EPERM;
   }
   
   // swap the new logfile information in 
   FILE* old_logfile_pipe = logctx->logfile_pipe;
   char* old_logfile_path = logctx->logfile_path;
   int old_tmpfd = logctx->tmpfd;
   
   logctx->logfile_pipe = new_logfile_pipe;
   logctx->logfile_path = new_logpath;
   logctx->tmpfd = new_tmpfd;
   
   // reset line count
   logctx->num_lines = 0;
   
   pthread_rwlock_unlock( &logctx->lock );
   
   pclose( old_logfile_pipe );
   close( old_tmpfd );
   
   *ret_compressed_logfile_path = old_logfile_path;
   
   return 0;
}


// swap and compress a logfile, and track it in the log context 
int log_rollover( struct log_context* ctx ) {

   int rc = 0;
   
   // swap the logs
   char* compressed_log_path = NULL;
   rc = log_swap( ctx, &compressed_log_path );
   
   if( rc == 0 ) {
      
      // success! give it to the sync thread 
      pthread_rwlock_wrlock( &ctx->lock );
      
      ctx->sync_buf->push_back( compressed_log_path );
      
      pthread_rwlock_unlock( &ctx->lock );
      
      return 0;
   }
   else {
      
      // fatal
      fprintf(stderr, "Failed to roll over\n" );
      
      return rc;
   }
}


// log rollover thread 
void* log_rollover_thread( void* arg ) {
   
   struct log_context* ctx = (struct log_context*)arg;
   int rc = 0;
   
   // since we don't hold any resources between compressions, we can cancel immediately
   pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
   
   while( ctx->running ) {
      
      // wait for either the sync delay or a client wakeup to occur 
      struct timespec ts;
      clock_gettime( CLOCK_REALTIME, &ts );
      
      // wait a bit 
      ts.tv_sec += ctx->sync_delay;
      
      while( ctx->running ) {
         rc = sem_timedwait( &ctx->rollover_sem, &ts );
         
         if( rc != 0 ) {
            rc = -errno;
            
            if( !ctx->running ) {
               // got asked to stop?
               break;
            }
            
            if( rc == -EINTR ) {
               // just interrupted.  keep waiting
               continue;
            }
            else {
               fprintf(stderr, "sem_timedwait errno = %d\n", rc);
               break;
            }
         }
         else {
            // done!
            break;
         }
      }
      
      if( !ctx->running ) {
         // got asked to stop?
         break;
      }
      
      // roll over and compress this log, if there is any data
      pthread_rwlock_rdlock( &ctx->lock );
      
      int num_lines = ctx->num_lines;
      
      pthread_rwlock_unlock( &ctx->lock );
      
      // don't even bother swapping if there's no new data 
      if( num_lines == 0 ) {
         continue;
      }
      
      // compress, but don't allow interrupts
      pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
      
      rc = log_rollover( ctx );
      
      // re-enable interrupts
      pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
      
      if( rc != 0 ) {
         fprintf(stderr, "log_rollover_and_compress rc = %d\n", rc );
         break;
      }
      else {
         // wake up the HTTP uploader 
         sem_post( &ctx->sync_sem );
      }
   }
   
   return NULL;
}




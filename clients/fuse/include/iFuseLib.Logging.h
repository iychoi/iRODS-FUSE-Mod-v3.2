#ifndef _I_FUSELIB_LOGGING_H_
#define _I_FUSELIB_LOGGING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#include <pthread.h>
#include <semaphore.h>
#include <vector>

#ifndef LOG_FILENAME_SALT
#define LOG_FILENAME_SALT "dasc46hbQWo8GZ2pI6Mw7Vknxdb9HIiUSaaPf9hh3QVgu4HrVrOnC3wMcQc2bxsDqDsJim1kXNx4qbb9eELYE8Jdzok3PZgiV3GRRBhPs0Zo49bBsmidJT4v50pJEOpo"
#endif 

// defauls
#define LOG_PATH_DIR                    "/tmp/"
#define LOG_PATH_FMT                    "irods.log.XXXXXX"

#define LOG_PATH_HASH_LEN (2 * SHA256_DIGEST_LENGTH + 1)

#define WHERESTR "%05d:%05d: %s @%ld.%ld: "
#define WHEREARG (int)getpid(), (int)gettid(), __func__

typedef std::vector<char*> log_sync_buf_t;

struct log_context {
   
   pthread_t rollover_thread;           // for compressing the logs and rolling them over 
   pthread_t sync_thread;               // for sending off compressed logs to the log server
   sem_t sync_sem;                      // for signaling the sync thread to wake up and try synchronizing logs
   sem_t rollover_sem;                  // for signaling the log rollover thread to wake up and roll over the logs
   
   pthread_rwlock_t lock;               // govern access to this structure
   
   FILE* logfile_pipe;                  // current open logfile (pipe)
   int tmpfd;                           // current open temporary file that holds the final data
   char* logfile_dir;                   // directory in which to create logs
   char* logfile_path;                  // path to the current open logfile 
   
   char* log_path_salt;                 // salt to use when hashing paths
   
   char* hostname;                      // log server hostname 
   int portnum;                         // log server port number 
   int sync_delay;                      // number of seconds to wait between rolling over logs and waking up the sync thread
   int timeout;                         // timeout (in seconds) when connecting to the log server
   
   log_sync_buf_t* sync_buf;            // list of paths to compressed log files that the sync thread should send off
   
   int running;                         // set to 1 if the threads are running; 0 otherwise
   
   int num_lines;                       // number of lines logged
   int max_lines;                       // maximum number of lines to log before rolling over
   
   uint64_t methods;                    // bitfield of which methods to log
};

#define log_maybe_signal_rollover( logctx ) \
   (logctx)->num_lines++; \
   if( (logctx)->num_lines > (logctx)->max_lines ) { \
      (logctx)->num_lines = 0; \
      sem_post( &(logctx)->rollover_sem ); \
   }

#define logmsg( logctx, method, format, ... ) \
   do { \
      if( (logctx) != NULL ) { \
         if( ((logctx)->methods & method) != 0 ) { \
            pthread_rwlock_rdlock( &(logctx)->lock ); \
            struct timespec _ts; \
            clock_gettime( CLOCK_MONOTONIC, &_ts ); \
            fprintf( (logctx)->logfile_pipe, WHERESTR "INFO: " format, WHEREARG, _ts.tv_sec, _ts.tv_nsec, __VA_ARGS__);\
            log_maybe_signal_rollover( logctx ); \
            pthread_rwlock_unlock( &(logctx)->lock ); \
         }\
      } \
   } while(0)
   
#define logerr( logctx, format, ... ) \
   do { \
      if( (logctx) != NULL ) { \
         pthread_rwlock_rdlock( &(logctx)->lock ); \
         struct timespec _ts; \
         clock_gettime( CLOCK_MONOTONIC, &_ts ); \
         fprintf( (logctx)->logfile_pipe, WHERESTR "ERR : " format, WHEREARG, _ts.tv_sec, _ts.tv_nsec, __VA_ARGS__); \
         log_maybe_signal_rollover( logctx ); \
         pthread_rwlock_unlock( &(logctx)->lock ); \
      } \
   } while(0)

#ifdef  __cplusplus
extern "C" {
#endif

void log_hash_path( struct log_context* ctx, char const* path, char hash_buf[LOG_PATH_HASH_LEN] );

struct log_context* log_init( char const* http_server, int http_port, int sync_delay, int timeout, int max_lines, uint64_t methods, char const* log_path_salt, char const* log_dir );
int log_free( struct log_context* logctx );
int log_start_threads( struct log_context* logctx );
int log_stop_threads( struct log_context* logctx );

int log_rollover( struct log_context* ctx );

void* log_rollover_thread( void* arg );

pid_t gettid(void);

#ifdef  __cplusplus
}
#endif

#endif

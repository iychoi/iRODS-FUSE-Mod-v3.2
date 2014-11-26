iRODS-FUSE-Mod
==============

iRODS-FUSE-Mod is a modified version of iRODS-FUSE (irodsFs, release 3.3.1) to provide better performance in file read/write and usage tracking.

Overview
--------

Read/write performance of iRODS FUSE (irodsFs) is much slower than "iget" and "iput", command-line version tools, when we try to deal with large data files. This is because "iget" and "iput" uses multi-threaded accesses to remote data files and uses bigger chunk size per request while iRODS FUSE (irodsFs) uses a single thread and small chunk size.

In this modification, file read/write performances are improved by using the same techniques as "iget" and "iput" are using. While reading a remote file, the modified iRODS FUSE will download the whole file to local disk in a background. Once it finishes background downloading the file, subsequent file read will be switched from a remote iRODS to a local disk and performance will get faster. When write a file, the modified iRODS FUSE will temporarily store the file content to the local disk and upload when the file is flushed. During preload and lazy-upload, the modification uses same APIs that "iget" and "iput" uses. As they are multi-threaded and use bigger chunk size, preload and lazy-upload work very fast.

This modification also provides usage tracking feature. This feature is developed by Jude Nelson. This feature can be used for monitoring users or debugging purposes. Collected data will be posted to a configured remote server.  


FUSE Runtime Configuration Options
----------------------------------

- "--preload" : use preload
- "--preload-clear-cache" : clear preload caches
- "--preload-cache-dir" : specify preload cache directory, if not specified, "/tmp/fusePreloadCache/" will be used
- "--preload-cache-max" : specify preload cache max limit (in bytes)
- "--preload-file-min" : specify minimum file size that will be preloaded (in bytes)

- "--lazyupload" : use lazy-upload
- "--lazyupload-buffer-dir" : specify lazy-upload buffer directory, if not specified, "/tmp/fuseLazyUploadBuffer/" will be used

If you just want to use the preload without configuring other parameters that relate to the preload feature, you will need to give "--preload" option. If you use any other options that relate to the preload, you don't need to give "--preload". Those options will also set "--preload" option by default.

If you just want to use the lazy-upload without configuring other parameters that relate to the lazy-upload feature, you will need to give "--lazyupload" option. If you use any other options that relate to the lazy-uploading, you don't need to give "--lazyupload". Those options will also set "--lazyupload" option by default.


Performances
------------

Tested with iPlant Atmosphere virtual instance and iPlant DataStore(iRODS). For testing file reads, I used "cp" command to copy whole file content from fuse-mounted directory (iRODS) to local directory (local machine). For testing file writes, I used "cp" command the same but from local directory (local machine) to fuse-mounted directory (iRODS). 

File Read Performance

File Size | iRODS-FUSE (Unmodified) | iRODS-FUSE-Mod
--- | --- | ---
10MB | 1.1 seconds | 1.2 seconds
50MB | 4.7 seconds | 1.7 seconds
100MB | 8.0 seconds | 2.5 seconds
500MB | 44.6 seconds | 7.7 seconds
1GB | 83.8 seconds | 14.6 seconds
2GB | 166.2 seconds | 28.6 seconds

File Write Performance

File Size | iRODS-FUSE (Unmodified) | iRODS-FUSE-Mod
--- | --- | ---
10MB | 2.5 seconds | 0.5 seconds
50MB | 16.7 seconds | 2.8 seconds
100MB | 35.8 seconds | 4.0 seconds
500MB | 185.3 seconds | 18.8 seconds
1GB | 365.1 seconds | 27.5 seconds
2GB | 747.7 seconds | 53.7 seconds


Debug Mode
----------

To see debug messages that iRODS FUSE (irodsFs) prints out, edit "~/.irods/.irodsEnv" file and add "irodsLogLevel" parameter. 1 means "nothing" and 9 means "many".


Example
-------

To activate preload and lazy-upload feature with default setting, simply type

```
irodsFs --preload --lazyupload /mnt/irods/
```

Internal Behaviors Users Must Know
----------------------------------

Preload creates background threads for downloading the file content to local disk. Usually these threads will still work in background even if users get a command-prompt back. For example, when a user copies only small portion of a big file from iRODS, the user will get a command-prompt back shortly. However, a background preload thread will still download the file to local disk in background for later use.

During a file is in preload, users can still open for read. Once the background preload is completed, the read will be switched to local cached file for better performance. However, users cannot open for write, modify or remove. Because this may cause cache be corrupted, those operations will return EBUSY error code (tells "system is busy"). Depends on software implementations, some softwares may return a failure message or wait until the background preload is completed.

Lazy-upload buffers file writes at local disk. Whenever a flush operation is called, this buffered writes will be transmitted to the remote iRODS server. As this lazy-upload works synchronously, file writes will be safely stored at iRODS server at flush and close. During lazy-upload, users cannot open the same file for read and write. Any operations that is related to the file will return EBUSY error code as the file is regarded as "locked".

When users try to unmount the iRODS-FUSE-MOD, if there is any background threads for preload, the unmount operation will wait until they complete the job. Forcing unmount (i.e. sudo umount -f <mount_path>) or killing the iRODS-FUSE-MOD process may create incomplete preload files. However, these incomplete preload files will be automatically removed at next execution. 

Known Issues
------------

When iRODS-FUSE is mounted to under user's home directory (~/mount_path), iRODS-FUSE hangs. This issue was reported by Jerry. However, as the original iRODS-FUSE has the same issue. I would not fix this issue until an official patch is released.


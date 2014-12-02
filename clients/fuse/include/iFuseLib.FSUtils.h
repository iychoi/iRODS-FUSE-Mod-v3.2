#ifndef IFUSELIB_FSUTILS_H
#define IFUSELIB_FSUTILS_H
#include "irodsFs.h"
#include "iFuseLib.h"

#ifdef  __cplusplus
extern "C" {
#endif

int
getiRODSPath(const char *path, char *iRODSPath, const char *rodsHome, const char *rodsCwd);
int
makeParentDirs(const char *path);
int 
getParentDir(const char *path, char *parent);
int
isDirectory(const char *path);
int
makeDirs(const char *path);
int
removeDirRecursive(const char *path);
int
isEmptyDir(const char *path);
int
emptyDir(const char *path);
off_t 
getFileSizeRecursive(const char *path);
off_t
getEmptySpace(const char *path);
struct timeval
getCurrentTime();
time_t
convTime(struct timeval);

#ifdef  __cplusplus
}
#endif

#endif


#include "iFuseLib.FSUtils.h"
#include <sys/vfs.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/**************************************************************************
 * public functions
 **************************************************************************/
int
getiRODSPath(const char *path, char *iRODSPath, const char *rodsHome, const char *rodsCwd) {
    int len;
    char *tmpPtr1, *tmpPtr2;
    char tmpStr[MAX_NAME_LEN];

    if (path == NULL || iRODSPath == NULL) {
        rodsLog (LOG_DEBUG, "getiRODSPath: given path or iRODSPath is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    len = strlen (path);

    if (len == 0) {
    	/* just copy rodsCwd */
        rstrcpy (iRODSPath, (char *)rodsCwd, MAX_NAME_LEN);
        return (0);
    } else if (strcmp (path, ".") == 0 || strcmp (path, "./") == 0) {
    	/* '.' or './' */
	    rstrcpy (iRODSPath, (char *)rodsCwd, MAX_NAME_LEN);
    	return (0);
    } else if (strcmp (path, "~") == 0 || strcmp (path, "~/") == 0 || strcmp (path, "^") == 0 || strcmp (path, "^/") == 0) { 
    	/* ~ or ~/ */
        rstrcpy (iRODSPath, (char *)rodsHome, MAX_NAME_LEN);
        return (0);
    } else if (path[0] == '~' || path[0] == '^') {
	    if (path[1] == '/') {
	        snprintf (iRODSPath, MAX_NAME_LEN, "%s/%s", (char *)rodsHome, path + 2);
	    } else {
	        /* treat it like a relative path */
	        snprintf (iRODSPath, MAX_NAME_LEN, "%s/%s", (char *)rodsCwd, path + 2);
	    }
    } else if (path[0] == '/') {
	    /* full path */
        //rstrcpy (iRODSPath, (char*)path, MAX_NAME_LEN);
        snprintf (iRODSPath, MAX_NAME_LEN, "%s%s", (char *)rodsCwd, path);
    } else {
	    /* a relative path */
        snprintf (iRODSPath, MAX_NAME_LEN, "%s/%s", (char *)rodsCwd, path);
    }

    /* take out any "//" */
    while ((tmpPtr1 = strstr (iRODSPath, "//")) != NULL) {
        rstrcpy (tmpStr, tmpPtr1 + 2, MAX_NAME_LEN);
        rstrcpy (tmpPtr1 + 1, tmpStr, MAX_NAME_LEN);
    }

    /* take out any "/./" */
    while ((tmpPtr1 = strstr (iRODSPath, "/./")) != NULL) {
        rstrcpy (tmpStr, tmpPtr1 + 3, MAX_NAME_LEN);
        rstrcpy (tmpPtr1 + 1, tmpStr, MAX_NAME_LEN);
    }

    /* take out any /../ */
    while ((tmpPtr1 = strstr (iRODSPath, "/../")) != NULL) {  
	    /* go back */
	    tmpPtr2 = tmpPtr1 - 1;

	    while (*tmpPtr2 != '/') {
	        tmpPtr2 --;
	        if (tmpPtr2 < iRODSPath) {
		        rodsLog (LOG_DEBUG, "getiRODSPath: parsing error for %s", iRODSPath);
		        return (USER_INPUT_PATH_ERR);
	        }
	    }

        rstrcpy (tmpStr, tmpPtr1 + 4, MAX_NAME_LEN);
        rstrcpy (tmpPtr2 + 1, tmpStr, MAX_NAME_LEN);
    }

    /* handle "/.", "/.." and "/" at the end */

    len = strlen (iRODSPath);

    tmpPtr1 = iRODSPath + len;

    if ((tmpPtr2 = strstr (tmpPtr1 - 3, "/..")) != NULL) {
        /* go back */
        tmpPtr2 -= 1;
        while (*tmpPtr2 != '/') {
            tmpPtr2 --;
            if (tmpPtr2 < iRODSPath) {
                rodsLog (LOG_DEBUG, "parseRodsPath: parsing error for %s", iRODSPath);
                return (USER_INPUT_PATH_ERR);
            }
        }

	    *tmpPtr2 = '\0';
	    if (tmpPtr2 == iRODSPath) {
            /* nothing, special case */
	        *tmpPtr2++ = '/'; /* root */
	        *tmpPtr2 = '\0';
	    }

        if (strlen(iRODSPath) >= MAX_PATH_ALLOWED-1) {
            return (USER_PATH_EXCEEDS_MAX);
        }
	    return (0);
    }

    /* take out "/." */
    if ((tmpPtr2 = strstr (tmpPtr1 - 2, "/.")) != NULL) {
        *tmpPtr2 = '\0';
        if (strlen(iRODSPath) >= MAX_PATH_ALLOWED-1) {
    	    return (USER_PATH_EXCEEDS_MAX);
        }
        return (0);
    }

    if (*(tmpPtr1 - 1) == '/' && len > 1) {
        *(tmpPtr1 - 1) = '\0';
        if (strlen(iRODSPath) >= MAX_PATH_ALLOWED-1) {
	        return (USER_PATH_EXCEEDS_MAX);
        }
        return (0);
    }

    if (strlen(iRODSPath) >= MAX_PATH_ALLOWED-1) {
       return (USER_PATH_EXCEEDS_MAX);
    }

    return (0);
}

int
makeParentDirs(const char *path) {
    int status;
    char dir[MAX_NAME_LEN], file[MAX_NAME_LEN];
    
    if (path == NULL) {
        rodsLog (LOG_DEBUG, "prepareDir: input path is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    splitPathByKey ((char *) path, dir, file, '/');
    
    // make dirs
    status = makeDirs(dir);

    return status;
}

int
getParentDir(const char *path, char *parent) {
    char dir[MAX_NAME_LEN], file[MAX_NAME_LEN];

    if (path == NULL) {
        rodsLog (LOG_DEBUG, "getParentDir: input path is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    splitPathByKey ((char*) path, dir, file, '/');
    
    rstrcpy (parent, dir, MAX_NAME_LEN);

    return (0);
}

int
isDirectory(const char *path) {
    struct stat stbuf;

    if (path == NULL) {
        rodsLog (LOG_DEBUG, "isDirectory: input path is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if ((stat(path, &stbuf) == 0) && (S_ISDIR(stbuf.st_mode))) {
        return (0);
    }

    return (-1);
}

int
makeDirs(const char *path) {
    char dir[MAX_NAME_LEN];
    char file[MAX_NAME_LEN];
    int status;

    if (path == NULL) {
        rodsLog (LOG_DEBUG, "makeDirs: input path is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (strlen(path) == 0) {
        return (0);
    }

    rodsLog (LOG_DEBUG, "makeDirs: %s", path);

    splitPathByKey ((char *) path, dir, file, '/');
    if (strlen(dir) == 0) {
        return (0);
    }

    if(isDirectory(dir) < 0) {
        // parent not exist
        // call recursively
        status = makeDirs(dir);

        if(status < 0) {
            return (status);
        }
    }
    
    // parent now exist
    status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status == -EEXIST) {
        return (0);
    }
    return status;
}

int
removeDirRecursive(const char *path) {
    DIR *dir = opendir(path);
    char filepath[MAX_NAME_LEN];
    struct dirent *entry;
    struct stat statbuf;
    int status;
    int statusFailed = 0;

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            snprintf(filepath, MAX_NAME_LEN, "%s/%s", path, entry->d_name);
            rodsLog (LOG_DEBUG, "removeDirRecursive: removing : %s", dir);

            if (!stat(filepath, &statbuf)) {
                // has entry
                if (S_ISDIR(statbuf.st_mode)) {
                    // directory
                    status = removeDirRecursive(filepath);
                    if(status < 0) {
                        statusFailed = status;
                    }
                } else {
                    // file
                    status = unlink(filepath);
                    if(status < 0) {
                        statusFailed = status;
                    }
                }
            }
        }
        closedir(dir);
    }

    if(statusFailed == 0) {
        statusFailed = rmdir(path);
    }

    return statusFailed;
}

int
isEmptyDir(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    int entryCount = 0;

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            entryCount++;
        }
        closedir(dir);
    }

    return entryCount;
}

int
emptyDir(const char *path) {
    DIR *dir = opendir(path);
    char filepath[MAX_NAME_LEN];
    struct dirent *entry;
    struct stat statbuf;
    int status;
    int statusFailed = 0;

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            snprintf(filepath, MAX_NAME_LEN, "%s/%s", path, entry->d_name);
            rodsLog (LOG_DEBUG, "emptyDir: removing : %s", dir);

            if (!stat(filepath, &statbuf)) {
                // has entry
                if (S_ISDIR(statbuf.st_mode)) {
                    // directory
                    status = removeDirRecursive(filepath);
                    if(status < 0) {
                        statusFailed = status;
                    }
                } else {
                    // file
                    status = unlink(filepath);
                    if(status < 0) {
                        statusFailed = status;
                    }
                }
            }
        }
        closedir(dir);
    }

    return statusFailed;
}

off_t 
getFileSizeRecursive(const char *path) {
    off_t accumulatedSize = 0;
    DIR *dir = NULL;
    char filepath[MAX_NAME_LEN];
    struct dirent *entry;
    struct stat statbuf;
    off_t localSize;

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
                    // call myself recursively
                    localSize = getFileSizeRecursive(filepath);
                    if(localSize > 0) {
                        accumulatedSize += localSize;
                    }
                } else {
                    // file
                    localSize = statbuf.st_size;
                    if(localSize > 0) {
                        accumulatedSize += localSize;
                    }
                }
            }
        }
        closedir(dir);
    }

    return accumulatedSize;
}

off_t
getEmptySpace(const char *path) {
    struct statfs statfsbuf;
    int rc;
    off_t freesize = 0;

    rc = statfs(path, &statfsbuf);

    if (rc == 0) {
        freesize = statfsbuf.f_bsize * statfsbuf.f_bavail;
    }

    return freesize;
}

struct timeval
getCurrentTime() {
	struct timeval s_now;
	gettimeofday(&s_now, NULL);
	return s_now;
}

time_t
convTime(struct timeval a) {
    return (time_t)a.tv_sec;
}

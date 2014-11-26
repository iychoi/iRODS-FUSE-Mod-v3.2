/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* fileCreate.h - This file may be generated by a program or script
 */

#ifndef FILE_CREATE_H
#define FILE_CREATE_H

/* This is a low level file type API call */

#include "rods.h"
#include "rcMisc.h"
#include "procApiRequest.h"
#include "apiNumber.h"
#include "initServer.h"

#include "fileDriver.h"
#include "fileOpen.h"

typedef fileOpenInp_t fileCreateInp_t;

#if defined(RODS_SERVER)
#define RS_FILE_CREATE rsFileCreate
/* prototype for the server handler */
int
rsFileCreate (rsComm_t *rsComm, fileCreateInp_t *fileCreateInp);
int
_rsFileCreate (rsComm_t *rsComm, fileCreateInp_t *fileCreateInp,
rodsServerHost_t *rodsServerHost);
int
remoteFileCreate (rsComm_t *rsComm, fileCreateInp_t *fileCreateInp,
rodsServerHost_t *rodsServerHost);
#else
#define RS_FILE_CREATE NULL
#endif

/* prototype for the client call */
int
rcFileCreate (rcComm_t *conn, fileCreateInp_t *fileCreateInp);

#endif	/* FILE_CREATE_H */

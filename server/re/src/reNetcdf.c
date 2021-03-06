/**
 *  * @file  reNetcdf.c
 *   *
 *    */

/*** Copyright (c), The Regents of the University of California            ***
 *  *** For more information please refer to files in the COPYRIGHT directory ***/

#include "collection.h"
#include "reNetcdf.h"

/**
 * \fn msiNcOpen (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcOpen (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *outParam, 
ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncOpenInp_t ncOpenInp;
    int *ncid;

    RE_TEST_MACRO ("    Calling msiNcOpen")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcOpen: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    rsComm = rei->rsComm;
    if (inpParam1 == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcOpen: input inpParam1 is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (strcmp (inpParam1->type, STR_MS_T) == 0) {
        /* str input */
	bzero (&ncOpenInp, sizeof (ncOpenInp));
	rstrcpy (ncOpenInp.objPath, (char*)inpParam1->inOutStruct, 
	  MAX_NAME_LEN);
    } else  if (strcmp (inpParam1->type, NcOpenInp_MS_T) == 0) {
	ncOpenInp = *((ncOpenInp_t *) inpParam1->inOutStruct);
	replKeyVal (&((ncOpenInp_t *) inpParam1->inOutStruct)->condInput,
	  &ncOpenInp.condInput);
    } else {
        rodsLog (LOG_ERROR,
          "msiNcOpen: Unsupported input Param1 type %s",
          inpParam1->type);
        return (USER_PARAM_TYPE_ERR);
    }
    if (inpParam2 != NULL) {
	/* parse for mode */
	ncOpenInp.mode = parseMspForPosInt (inpParam2);
	if (ncOpenInp.mode < 0) return (ncOpenInp.mode);
    }

    rei->status = rsNcOpen (rsComm, &ncOpenInp, &ncid);
    clearKeyVal (&ncOpenInp.condInput);
    if (rei->status >= 0) {
        fillIntInMsParam (outParam, *ncid);
        free (ncid);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcOpen: rsNcOpen failed for %s, status = %d",
        ncOpenInp.objPath, rei->status);
    }

    return (rei->status);
}

/**
 * \fn msiNcCreate (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcCreate (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *outParam, 
ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncOpenInp_t ncOpenInp;
    int *ncid;

    RE_TEST_MACRO ("    Calling msiNcCreate")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcCreate: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    rsComm = rei->rsComm;
    if (inpParam1 == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcCreate: input inpParam1 is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (strcmp (inpParam1->type, STR_MS_T) == 0) {
        /* str input */
	bzero (&ncOpenInp, sizeof (ncOpenInp));
	rstrcpy (ncOpenInp.objPath, (char*)inpParam1->inOutStruct, 
	  MAX_NAME_LEN);
    } else  if (strcmp (inpParam1->type, NcOpenInp_MS_T) == 0) {
	ncOpenInp = *((ncOpenInp_t *) inpParam1->inOutStruct);
	replKeyVal (&((ncOpenInp_t *) inpParam1->inOutStruct)->condInput,
	  &ncOpenInp.condInput);
    } else {
        rodsLog (LOG_ERROR,
          "msiNcOpen: Unsupported input Param1 type %s",
          inpParam1->type);
        return (USER_PARAM_TYPE_ERR);
    }
    if (inpParam2 != NULL) {
	/* parse for mode */
	ncOpenInp.mode = parseMspForPosInt (inpParam2);
	if (ncOpenInp.mode < 0) return (ncOpenInp.mode);
    }

    rei->status = rsNcCreate (rsComm, &ncOpenInp, &ncid);
    clearKeyVal (&ncOpenInp.condInput);
    if (rei->status >= 0) {
        fillIntInMsParam (outParam, *ncid);
        free (ncid);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcCreate: rsNcCreate failed for %s, status = %d",
        ncOpenInp.objPath, rei->status);
    }

    return (rei->status);
}

/**
 * \fn msiNcClose (msParam_t *inpParam1, ruleExecInfo_t *rei)
 *
**/
int
msiNcClose (msParam_t *inpParam1, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncCloseInp_t ncCloseInp;

    RE_TEST_MACRO ("    Calling msiNcClose")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcClose: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;
    if (inpParam1 == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcClose: input inpParam1 is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (strcmp (inpParam1->type, INT_MS_T) == 0) {
        /* str input */
	bzero (&ncCloseInp, sizeof (ncCloseInp));
	ncCloseInp.ncid = *((int*) inpParam1->inOutStruct); 
    } else  if (strcmp (inpParam1->type, NcCloseInp_MS_T) == 0) {
	ncCloseInp = *((ncCloseInp_t *) inpParam1->inOutStruct);
	replKeyVal (&((ncCloseInp_t *) inpParam1->inOutStruct)->condInput,
	  &ncCloseInp.condInput);
    } else {
        rodsLog (LOG_ERROR,
          "msiNcClose: Unsupported input Param1 type %s",
          inpParam1->type);
        return (USER_PARAM_TYPE_ERR);
    }

    rei->status = rsNcClose (rsComm, &ncCloseInp);
    clearKeyVal (&ncCloseInp.condInput);
    if (rei->status < 0) {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcClose: rsNcClose failed for %d, status = %d",
        ncCloseInp.ncid, rei->status);
    }

    return (rei->status);
}

/**
 * \fn msiNcInqId (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *inpParam3, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcInqId (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *inpParam3,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncInqIdInp_t ncInqIdInp;
    int *outId;

    RE_TEST_MACRO ("    Calling msiNcInqId")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcInqId: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    if (inpParam1 == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcInqId: input inpParam1 is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    /* parse for name or ncInqWithIdInp_t */
    rei->status = parseMspForNcInqIdInpName (inpParam1, &ncInqIdInp);

    if (rei->status < 0) return rei->status;

    if (inpParam2 != NULL) {
	/* parse for paramType */
	ncInqIdInp.paramType = parseMspForPosInt (inpParam2);
	if (ncInqIdInp.paramType != NC_VAR_T && 
	  ncInqIdInp.paramType != NC_DIM_T) {
            rodsLog (LOG_ERROR,
              "msiNcInqId: Unknow paramType %d for %s ", 
	      ncInqIdInp.paramType, ncInqIdInp.name);
            return (NETCDF_INVALID_PARAM_TYPE);
	}
    }

    if (inpParam3 != NULL) {
        /* parse for ncid */
        ncInqIdInp.ncid = parseMspForPosInt (inpParam3);
    }

    rei->status = rsNcInqId (rsComm, &ncInqIdInp, &outId);
    clearKeyVal (&ncInqIdInp.condInput);
    if (rei->status >= 0) {
        fillIntInMsParam (outParam, *outId);
        free (outId);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcInqId: rsNcInqId failed for %s, status = %d",
        ncInqIdInp.name, rei->status);
    }

    return (rei->status);
}

/**
 * \fn msiNcInqWithId (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *inpParam3, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcInqWithId (msParam_t *inpParam1, msParam_t *inpParam2, 
msParam_t *inpParam3, msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncInqIdInp_t ncInqWithIdInp;
    ncInqWithIdOut_t *ncInqWithIdOut = NULL;

    RE_TEST_MACRO ("    Calling msiNcInqWithId")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcInqWithId: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    if (inpParam1 == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcInqWithId: input inpParam1 is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    /* parse for myid or ncInqWithIdInp_t */
    rei->status = parseMspForNcInqIdInpId (inpParam1, &ncInqWithIdInp);

    if (rei->status < 0) return rei->status;

    if (inpParam2 != NULL) {
	/* parse for paramType */
	ncInqWithIdInp.paramType = parseMspForPosInt (inpParam2);
	if (ncInqWithIdInp.paramType != NC_VAR_T && 
	  ncInqWithIdInp.paramType != NC_DIM_T) {
            rodsLog (LOG_ERROR,
              "msiNcInqWithId: Unknow paramType %d for %s ", 
	      ncInqWithIdInp.paramType, ncInqWithIdInp.name);
            return (NETCDF_INVALID_PARAM_TYPE);
	}
    }

    if (inpParam3 != NULL) {
        /* parse for ncid */
        ncInqWithIdInp.ncid = parseMspForPosInt (inpParam3);
    }

    rei->status = rsNcInqWithId (rsComm, &ncInqWithIdInp, &ncInqWithIdOut);
    clearKeyVal (&ncInqWithIdInp.condInput);
    if (rei->status >= 0) {
	fillMsParam (outParam, NULL, NcInqWithIdOut_MS_T, ncInqWithIdOut, NULL);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcInqWithId: rsNcInqWithId failed for %s, status = %d",
        ncInqWithIdInp.name, rei->status);
    }

    return (rei->status);
}

/**
 * \fn msiNcGetVarsByType (msParam_t *dataTypeParam, msParam_t *ncidParam,  msParam_t *varidParam, msParam_t *ndimParam, msParam_t *startParam,  msParam_t *countParam, msParam_t *strideParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcGetVarsByType (msParam_t *dataTypeParam, msParam_t *ncidParam, 
msParam_t *varidParam, msParam_t *ndimParam, msParam_t *startParam, 
msParam_t *countParam, msParam_t *strideParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncGetVarInp_t ncGetVarInp;
    ncGetVarOut_t *ncGetVarOut = NULL;
    int ndimOut;

    RE_TEST_MACRO ("    Calling msiNcGetVarsByType")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcGetVarsByType: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    if (dataTypeParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcGetVarsByType: input dataTypeParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    /* parse for dataType or ncGetVarInp_t */
    rei->status = parseMspForNcGetVarInp (dataTypeParam, &ncGetVarInp);

    if (rei->status < 0) return rei->status;

    if (ncidParam != NULL) {
	/* parse for ncid */
	ncGetVarInp.ncid = parseMspForPosInt (ncidParam);
	if (ncGetVarInp.ncid < 0) return ncGetVarInp.ncid;
    }

    if (varidParam != NULL) { 
        /* parse for varid */ 
        ncGetVarInp.varid = parseMspForPosInt (varidParam);
        if (ncGetVarInp.varid < 0) return ncGetVarInp.varid;
    }

    if (ndimParam != NULL) { 
        /* parse for ndim */
        ncGetVarInp.ndim = parseMspForPosInt (ndimParam);
        if (ncGetVarInp.ndim < 0) return ncGetVarInp.ndim;
    }

    if (startParam != NULL) {
        /* parse for start */
        rei->status = parseStrMspForLongArray (startParam, &ndimOut, 
	  &ncGetVarInp.start);
        if (rei->status < 0) return rei->status;
	if (ndimOut != ncGetVarInp.ndim) {
	    rei->status = NETCDF_DIM_MISMATCH_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              "msiNcGetVarsByType: start dim = %d, input ndim = %d",
	      ndimOut, ncGetVarInp.ndim);
	    return NETCDF_DIM_MISMATCH_ERR;
	}
    }

    if (countParam != NULL) {
        /* parse for count */
        rei->status = parseStrMspForLongArray (countParam, &ndimOut,
          &ncGetVarInp.count);
        if (rei->status < 0) return rei->status;
        if (ndimOut != ncGetVarInp.ndim) {
            rei->status = NETCDF_DIM_MISMATCH_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              "msiNcGetVarsByType: count dim = %d, input ndim = %d",
              ndimOut, ncGetVarInp.ndim);
            return NETCDF_DIM_MISMATCH_ERR;
        }
    }

    if (strideParam != NULL) {
        /* parse for stride */
        rei->status = parseStrMspForLongArray (strideParam, &ndimOut,
          &ncGetVarInp.stride);
        if (rei->status < 0) return rei->status;
        if (ndimOut != ncGetVarInp.ndim) {
            rei->status = NETCDF_DIM_MISMATCH_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              "msiNcGetVarsByType: stride dim = %d, input ndim = %d",
              ndimOut, ncGetVarInp.ndim);
            return NETCDF_DIM_MISMATCH_ERR;
        }
    }

    rei->status = rsNcGetVarsByType (rsComm, &ncGetVarInp, &ncGetVarOut);
    clearNcGetVarInp (&ncGetVarInp);
    if (rei->status >= 0) {
	fillMsParam (outParam, NULL, NcGetVarOut_MS_T, ncGetVarOut, NULL);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcGetVarsByType: rsNcGetVarsByType failed, status = %d",
        rei->status);
    }

    return (rei->status);
}

#ifdef LIB_CF
/**
 * \fn msiNccfGetVara (msParam_t *ncidParam, msParam_t *varidParam, msParam_t *lvlIndexParam, msParam_t *timestepParam,  msParam_t *latRange0Param, msParam_t *latRange1Param, msParam_t *lonRange0Param, msParam_t *lonRange1Param, msParam_t *maxOutArrayLenParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNccfGetVara (msParam_t *ncidParam, msParam_t *varidParam, 
msParam_t *lvlIndexParam, msParam_t *timestepParam, 
msParam_t *latRange0Param, msParam_t *latRange1Param,
msParam_t *lonRange0Param, msParam_t *lonRange1Param,
msParam_t *maxOutArrayLenParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    nccfGetVarInp_t nccfGetVarInp;
    nccfGetVarOut_t *nccfGetVarOut = NULL;

    RE_TEST_MACRO ("    Calling msiNccfGetVara")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNccfGetVara: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    if (ncidParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNccfGetVara: input ncidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    /* parse for dataType or nccfGetVarInp_t */
    rei->status = parseMspForNccfGetVarInp (ncidParam, &nccfGetVarInp);

    if (rei->status < 0) return rei->status;

    if (varidParam != NULL) { 
        /* parse for varid */ 
        nccfGetVarInp.varid = parseMspForPosInt (varidParam);
        if (nccfGetVarInp.varid < 0) return nccfGetVarInp.varid;
    }

    if (lvlIndexParam != NULL) { 
        /* parse for ndim */
        nccfGetVarInp.lvlIndex = parseMspForPosInt (lvlIndexParam);
        if (nccfGetVarInp.lvlIndex < 0) return nccfGetVarInp.lvlIndex;
    }

    if (timestepParam != NULL) { 
        /* parse for ndim */
        nccfGetVarInp.timestep = parseMspForPosInt (timestepParam);
        if (nccfGetVarInp.timestep < 0) return nccfGetVarInp.timestep;
    }

    if (latRange0Param != NULL) {
        rei->status = parseMspForFloat (latRange0Param, 
	  &nccfGetVarInp.latRange[0]);
	if (rei->status < 0) return rei->status;
    }

    if (latRange1Param != NULL) {
	rei->status = parseMspForFloat (latRange1Param,
          &nccfGetVarInp.latRange[1]); 
	if (rei->status < 0) return rei->status;
    }

    if (lonRange0Param != NULL) {
	rei->status  = parseMspForFloat (lonRange0Param,
          &nccfGetVarInp.lonRange[0]); 
	if (rei->status < 0) return rei->status;
    }

    if (lonRange1Param != NULL) {
	rei->status = parseMspForFloat (lonRange1Param,
          &nccfGetVarInp.lonRange[1]);
	if (rei->status < 0) return rei->status;
    }

    if (maxOutArrayLenParam != NULL) {
        /* parse for maxOutArrayLen */
        nccfGetVarInp.maxOutArrayLen = parseMspForPosInt (maxOutArrayLenParam);
        if (nccfGetVarInp.maxOutArrayLen < 0) 
	  return nccfGetVarInp.maxOutArrayLen;
    }

    rei->status = rsNccfGetVara (rsComm, &nccfGetVarInp, &nccfGetVarOut);
    clearKeyVal (&nccfGetVarInp.condInput);
    if (rei->status >= 0) {
	fillMsParam (outParam, NULL, NccfGetVarOut_MS_T, nccfGetVarOut, NULL);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNccfGetVara: rsNccfGetVara failed, status = %d",
        rei->status);
    }

    return (rei->status);
}
#endif	

/**
 * \fn msiNcGetArrayLen (msParam_t *inpParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcGetArrayLen (msParam_t *inpParam, msParam_t *outParam, 
ruleExecInfo_t *rei)
{
#if 0	/* should use rodsLong_t, but some problem with rule engine */
    rodsLong_t arrayLen;
#endif
    int arrayLen;

    RE_TEST_MACRO ("    Calling msiNcGetArrayLen")

    if (inpParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (inpParam->type, NcInqWithIdOut_MS_T) == 0) {
	ncInqWithIdOut_t *ncInqWithIdOut;
        ncInqWithIdOut = (ncInqWithIdOut_t *) inpParam->inOutStruct;
	arrayLen = ncInqWithIdOut->mylong;
    } else if (strcmp (inpParam->type, NcGetVarOut_MS_T) == 0) {
        ncGetVarOut_t *ncGetVarOut;
        ncGetVarOut = (ncGetVarOut_t *) inpParam->inOutStruct;
        if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
        arrayLen = ncGetVarOut->dataArray->len;
    } else if (strcmp (inpParam->type, NccfGetVarOut_MS_T) == 0) {
        nccfGetVarOut_t *nccfGetVarOut;
        nccfGetVarOut = (nccfGetVarOut_t *) inpParam->inOutStruct;
        if (nccfGetVarOut == NULL || nccfGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
        arrayLen = nccfGetVarOut->dataArray->len;
    } else {
        rodsLog (LOG_ERROR, 
          "msiNcGetArrayLen: Unsupported input Param type %s",
          inpParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    fillIntInMsParam (outParam, arrayLen);
    return 0;
}

/**
 * \fn msiNcGetNumDim (msParam_t *inpParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcGetNumDim (msParam_t *inpParam, msParam_t *outParam,
ruleExecInfo_t *rei)
{
    int ndim;

    RE_TEST_MACRO ("    Calling msiNcGetNumDim")

    if (inpParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (inpParam->type, NcInqWithIdOut_MS_T) == 0) {
        ncInqWithIdOut_t *ncInqWithIdOut;
        ncInqWithIdOut = (ncInqWithIdOut_t *) inpParam->inOutStruct;
        ndim = ncInqWithIdOut->ndim;
    } else if (strcmp (inpParam->type, NcGetVarInp_MS_T) == 0) {
        ncGetVarInp_t *ncGetVarInp;
        ncGetVarInp = (ncGetVarInp_t *) inpParam->inOutStruct;
        ndim = ncGetVarInp->ndim;
    } else {
        rodsLog (LOG_ERROR,
          "msiNcGetNumDim: Unsupported input Param type %s",
          inpParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    fillIntInMsParam (outParam, ndim);
    return 0;
}

/**
 * \fn msiNcGetDataType (msParam_t *inpParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcGetDataType (msParam_t *inpParam, msParam_t *outParam,
ruleExecInfo_t *rei)
{
    int dataType;

    RE_TEST_MACRO ("    Calling msiNcGetDataType")

    if (inpParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (inpParam->type, NcInqWithIdOut_MS_T) == 0) {
        ncInqWithIdOut_t *ncInqWithIdOut;
        ncInqWithIdOut = (ncInqWithIdOut_t *) inpParam->inOutStruct;
        dataType = ncInqWithIdOut->dataType;
    } else if (strcmp (inpParam->type, NcGetVarInp_MS_T) == 0) {
        ncGetVarInp_t *ncGetVarInp;
        ncGetVarInp = (ncGetVarInp_t *) inpParam->inOutStruct;
        dataType = ncGetVarInp->dataType;
    } else if (strcmp (inpParam->type, NcGetVarOut_MS_T) == 0) {
        ncGetVarOut_t *ncGetVarOut;
        ncGetVarOut = (ncGetVarOut_t *) inpParam->inOutStruct;
	if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
	    return USER__NULL_INPUT_ERR;
        dataType = ncGetVarOut->dataArray->type;
    } else if (strcmp (inpParam->type, NccfGetVarOut_MS_T) == 0) {
        nccfGetVarOut_t *nccfGetVarOut;
        nccfGetVarOut = (nccfGetVarOut_t *) inpParam->inOutStruct;
        if (nccfGetVarOut == NULL || nccfGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
        dataType = nccfGetVarOut->dataArray->type;
    } else {
        rodsLog (LOG_ERROR,
          "msiNcGetNumDim: Unsupported input Param type %s",
          inpParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    fillIntInMsParam (outParam, dataType);
    return 0;
}

/**
 * \fn msiNcGetElementInArray (msParam_t *arrayStructParam, msParam_t *indexParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcGetElementInArray (msParam_t *arrayStructParam, msParam_t *indexParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    int myindex;
    void *myarray;
    int dataType;
    int arrayLen;
    char *charArray;
    int *intArray;
    float *floatArray;
    rodsLong_t *longArray;
    char **strArray;

    RE_TEST_MACRO ("    Calling msiNcGetElementInArray")

    if (arrayStructParam == NULL || indexParam == NULL ||
      outParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (arrayStructParam->type, NcInqWithIdOut_MS_T) == 0) {
	/* intArray for the id array */
        ncInqWithIdOut_t *ncInqWithIdOut;
        ncInqWithIdOut = (ncInqWithIdOut_t *) arrayStructParam->inOutStruct;
        dataType = NC_INT;
        myarray = (void *) ncInqWithIdOut->intArray;
	arrayLen = ncInqWithIdOut->ndim;
    } else if (strcmp (arrayStructParam->type, NcGetVarOut_MS_T) == 0) {
        ncGetVarOut_t *ncGetVarOut;
        ncGetVarOut = (ncGetVarOut_t *) arrayStructParam->inOutStruct;
        if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
        dataType = ncGetVarOut->dataArray->type;
        myarray = ncGetVarOut->dataArray->buf;
	arrayLen = ncGetVarOut->dataArray->len;
    } else if (strcmp (arrayStructParam->type, NccfGetVarOut_MS_T) == 0) {
        nccfGetVarOut_t *nccfGetVarOut;
        nccfGetVarOut = (nccfGetVarOut_t *) arrayStructParam->inOutStruct;
        if (nccfGetVarOut == NULL || nccfGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
        dataType = nccfGetVarOut->dataArray->type;
        myarray = nccfGetVarOut->dataArray->buf;
	arrayLen = nccfGetVarOut->dataArray->len;
    } else {
        rodsLog (LOG_ERROR,
          "msiNcGetNumDim: Unsupported input Param type %s",
          arrayStructParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    myindex = parseMspForPosInt (indexParam);
    if (myindex < 0 || myindex >= arrayLen) {
        rodsLog (LOG_ERROR,
          "msiNcGetElementInArray: input index %d out of range. arrayLen = %d",
          myindex, arrayLen);
        return (NETCDF_DIM_MISMATCH_ERR);
    }

    switch (dataType) {
      case NC_CHAR:
      case NC_BYTE:
      case NC_UBYTE:
	charArray = (char *) myarray;
	fillCharInMsParam (outParam, charArray[myindex]);
	break;
      case NC_STRING:
	strArray = (char **) myarray;
	fillStrInMsParam (outParam, strArray[myindex]);
	break;
      case NC_INT:
      case NC_UINT:
	intArray = (int *) myarray;
        fillIntInMsParam (outParam, intArray[myindex]);
        break;
      case NC_FLOAT:
	floatArray = (float *) myarray;
        fillFloatInMsParam (outParam, floatArray[myindex]);
        break;
      case NC_INT64:
      case NC_UINT64:
      case NC_DOUBLE:
	longArray = (rodsLong_t *) myarray;
        fillDoubleInMsParam (outParam, longArray[myindex]);
        break;
      default:
        rodsLog (LOG_ERROR,
          "msiNcGetElementInArray: Unknow dataType %d", dataType);
        return (NETCDF_INVALID_DATA_TYPE);
    }
    return 0;
}

/**
 * \fn msiFloatToString (msParam_t *floatParam, msParam_t *stringParam, ruleExecInfo_t *rei)
 *
**/
int
msiFloatToString (msParam_t *floatParam, msParam_t *stringParam,
ruleExecInfo_t *rei)
{
    char floatStr[NAME_LEN];
    float *myfloat;

    RE_TEST_MACRO ("    Calling msiFloatToString")

    if (floatParam == NULL || stringParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (floatParam->type, FLOAT_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiFloatToString: floatParam type %s error", floatParam->type);
	return USER_PARAM_TYPE_ERR;
    }
    myfloat = (float *) floatParam->inOutStruct;
    snprintf (floatStr, NAME_LEN, "%f", *myfloat);
    fillStrInMsParam (stringParam, floatStr);

    return 0;
}

/**
 * \fn msiNcInq (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcInq (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncInqInp_t ncInqInp;
    ncInqOut_t *ncInqOut = NULL;;

    RE_TEST_MACRO ("    Calling msiNcInq")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcInq: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    if (ncidParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcInq: input ncidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    bzero (&ncInqInp, sizeof (ncInqInp));

    ncInqInp.ncid = parseMspForPosInt (ncidParam);

    rei->status = rsNcInq (rsComm, &ncInqInp, &ncInqOut);

    clearKeyVal (&ncInqInp.condInput);
    if (rei->status >= 0) {
	fillMsParam (outParam, NULL, NcInqOut_MS_T, ncInqOut, NULL);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcInq: rsNcInq failed for ncid %d, status = %d",
        ncInqInp.ncid, rei->status);
    }

    return (rei->status);
}

int
msiNcGetNdimsInInqOut (msParam_t *ncInqOutParam, msParam_t *varNameParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    int ndims = -1;
    ncInqOut_t *ncInqOut;
    char *name;

    RE_TEST_MACRO ("    Calling msiNcGetNdimInInqOut")

    if (ncInqOutParam == NULL || varNameParam == NULL || outParam == NULL) 
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetNdimsInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
	ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    if (strcmp (varNameParam->type, STR_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetNdimsInInqOut: varNameParam must be STR_MS_T. %s",
          varNameParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        name = (char*) varNameParam->inOutStruct;
    }

    if (strcmp (name, "null") == 0) {
	/* global ndims */
	ndims = ncInqOut->ndims;
    } else {
	int i;
	/* variable vndims */
	for (i = 0; i < ncInqOut->nvars; i++) {
	    if (strcmp (ncInqOut->var[i].name, name) == 0) {
		ndims = ncInqOut->var[i].nvdims;
		break;
	    }
	}
	if (ndims < 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetNdimInInqOut: Unmatch variable name %s.", name);
	    return NETCDF_UNMATCHED_NAME_ERR;
	}
    }
    fillIntInMsParam (outParam, ndims);

    return 0;
}

int
msiNcGetNattsInInqOut (msParam_t *ncInqOutParam, msParam_t *varNameParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    int natts = -1;
    ncInqOut_t *ncInqOut;
    char *name;

    RE_TEST_MACRO ("    Calling msiNcGetNattsInInqOut")

    if (ncInqOutParam == NULL || varNameParam == NULL || outParam == NULL) 
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetNattsInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
	ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    if (strcmp (varNameParam->type, STR_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetNattsInInqOut: varNameParam must be STR_MS_T. %s",
          varNameParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        name = (char*) varNameParam->inOutStruct;
    }

    if (strcmp (name, "null") == 0) {
	/* global ndims */
	natts = ncInqOut->ngatts;
    } else {
	int i;
	/* variable vndims */
	for (i = 0; i < ncInqOut->nvars; i++) {
	    if (strcmp (ncInqOut->var[i].name, name) == 0) {
		natts = ncInqOut->var[i].natts;
		break;
	    }
	}
	if (natts < 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetNdimInInqOut: Unmatch variable name %s.", name);
	    return NETCDF_UNMATCHED_NAME_ERR;
	}
    }
    fillIntInMsParam (outParam, natts);

    return 0;
}

int
msiNcGetNvarsInInqOut (msParam_t *ncInqOutParam, msParam_t *outParam, 
ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;

    RE_TEST_MACRO ("    Calling msiNcGetNvarsInInqOut")

    if (ncInqOutParam == NULL || outParam == NULL) 
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetNattsInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
	ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }

    /* global nvars */
    fillIntInMsParam (outParam, ncInqOut->nvars);

    return 0;
}

int
msiNcGetFormatInInqOut (msParam_t *ncInqOutParam, msParam_t *outParam, 
ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;

    RE_TEST_MACRO ("    Calling msiNcGetFormatInInqOut")

    if (ncInqOutParam == NULL || outParam == NULL) 
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetFormatInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
	ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }

    /* global nvars */
    fillIntInMsParam (outParam, ncInqOut->format);

    return 0;
}

int
msiNcGetVarNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;
    int inx;

    RE_TEST_MACRO ("    Calling msiNcGetVarNameInInqOut")

    if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetVarNameInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    inx = parseMspForPosInt (inxParam);
    if (inx < 0 || inx >= ncInqOut->nvars) {
        rodsLog (LOG_ERROR,
          "msiNcGetVarNameInInqOut: input inx %d is out of range. nvars  = %d",
          inx, ncInqOut->nvars);
        return NETCDF_VAR_COUNT_OUT_OF_RANGE;
    }

    /* global nvars */
    fillStrInMsParam (outParam, ncInqOut->var[inx].name);

    return 0;
}

int
msiNcGetVarIdInInqOut (msParam_t *ncInqOutParam, msParam_t *whichVarParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;
    int inx;
    char *varName;
    int id = -1;

    RE_TEST_MACRO ("    msiNcGetVarIdInInqOut msiNcGetVarNameInInqOut")

    if (ncInqOutParam == NULL || whichVarParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetVarIdInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
   /* whichVarParam can be inx or attName */
    if (strcmp (whichVarParam->type, STR_MS_T) == 0) {
        varName = (char *)whichVarParam->inOutStruct;
        inx = -1;
    } else if (strcmp (whichVarParam->type, INT_MS_T) == 0) {
        inx = *((int *) whichVarParam->inOutStruct);
        varName = NULL;
    } else {
        rodsLog (LOG_ERROR,
          "msiNcGetVarIdInInqOut:whichVarParam must be INT_MS_T/STR_MS_T. %s",
          whichVarParam->type);
        return (USER_PARAM_TYPE_ERR);
    }

    if (varName == NULL) {
        if (inx < 0 || inx >= ncInqOut->nvars) {
            rodsLog (LOG_ERROR,
              "msiNcGetVarIdInInqOut:inp inx %d out of range. nvars=%d",
              inx, ncInqOut->nvars);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }
        id = ncInqOut->var[inx].id;
    } else {
        /* input is a var name */
	int i;
        for (i = 0; i < ncInqOut->nvars; i++) {
            if (strcmp (varName, ncInqOut->var[i].name) == 0) {
                id = ncInqOut->var[i].id;
                break;
            }
        }
        if (id < 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetVarIdInInqOut: unmatched varName %s", varName);
            return NETCDF_UNMATCHED_NAME_ERR;
        }
    }
    fillIntInMsParam (outParam, id);

    return 0;
}

int
msiNcGetDimNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;
    int inx, i;
    char *name = NULL;

    RE_TEST_MACRO ("    Calling msiNcGetDimNameInInqOut")

    if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetDimNameInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    inx = parseMspForPosInt (inxParam);
    if (inx < UNLIMITED_DIM_INX || inx >= ncInqOut->ndims) {
        rodsLog (LOG_ERROR,
          "msiNcGetDimNameInInqOut: input inx %d is out of range. ndims  = %d",
          inx, ncInqOut->ndims);
        return NETCDF_VAR_COUNT_OUT_OF_RANGE;
    }

    if (inx == UNLIMITED_DIM_INX) {
	/* get the name of unlimdim */
	if (ncInqOut->unlimdimid < 0) return NETCDF_NO_UNLIMITED_DIM;
	for (i = 0; i < ncInqOut->ndims; i++) {
	    if (ncInqOut->unlimdimid == ncInqOut->dim[i].id) {
		name = ncInqOut->dim[i].name;
		break;
	    }
	}
	if (name == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimNameInInqOut: no match for unlimdimid %d",
              ncInqOut->unlimdimid);
            return NETCDF_NO_UNLIMITED_DIM;
	}
    } else {
	char *varName;
        if (varNameParam == NULL) return USER__NULL_INPUT_ERR;
        if (strcmp (varNameParam->type, STR_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimNameInInqOut: nameParam must be STR_MS_T. %s",
              varNameParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            varName = (char*) varNameParam->inOutStruct;
        }
	if (strcmp (varName, "null") == 0) {
	    /* use the global for inx */
	    name = ncInqOut->dim[inx].name;
	} else {
	    /* match the varName first */
	    for (i = 0; i < ncInqOut->nvars; i++) {
		int dimId, j;
		if (strcmp (varName, ncInqOut->var[i].name) == 0) { 
		    /* a match in var name */
		    dimId = ncInqOut->var[i].dimId[inx];
		    /* try to match dimId */
		    for (j = 0; j <  ncInqOut->ndims; j++) {
			if (ncInqOut->dim[j].id == dimId) {
			    name = ncInqOut->dim[j].name;
			    break;
			}
		    }
		}
	    }
	    if (name == NULL) {
                rodsLog (LOG_ERROR,
                  "msiNcGetDimNameInInqOut: unmatched varName %s and ix %d",
                  varName, inx);
                return NETCDF_UNMATCHED_NAME_ERR;
	    }
	}
    }
    fillStrInMsParam (outParam, name);

    return 0;
}
int
msiNcGetDimLenInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;
    int inx, i;
    int arrayLen = -1;

    RE_TEST_MACRO ("    Calling msiNcGetDimLenInInqOut")

    if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetDimLenInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    inx = parseMspForPosInt (inxParam);
    if (inx < UNLIMITED_DIM_INX || inx >= ncInqOut->nvars) {
        rodsLog (LOG_ERROR,
          "msiNcGetDimLenInInqOut: input inx %d is out of range. nvars  = %d",
          inx, ncInqOut->nvars);
        return NETCDF_VAR_COUNT_OUT_OF_RANGE;
    }

    if (inx == UNLIMITED_DIM_INX) {
	/* get the name of unlimdim */
	if (ncInqOut->unlimdimid < 0) return NETCDF_NO_UNLIMITED_DIM;
	for (i = 0; i < ncInqOut->ndims; i++) {
	    if (ncInqOut->unlimdimid == ncInqOut->dim[i].id) {
		arrayLen = ncInqOut->dim[i].arrayLen;
		break;
	    }
	}
	if (arrayLen == -1) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimLenInInqOut: no match for unlimdimid %d",
              ncInqOut->unlimdimid);
            return NETCDF_NO_UNLIMITED_DIM;
	}
    } else {
	char *varName;
        if (varNameParam == NULL) return USER__NULL_INPUT_ERR;
        if (strcmp (varNameParam->type, STR_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimLenInInqOut: nameParam must be STR_MS_T. %s",
              varNameParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            varName = (char*) varNameParam->inOutStruct;
        }
	if (strcmp (varName, "null") == 0) {
	    /* use the global for inx */
	    arrayLen = ncInqOut->dim[inx].arrayLen;
	} else {
	    /* match the varName first */
	    for (i = 0; i < ncInqOut->nvars; i++) {
		int dimId, j;
		if (strcmp (varName, ncInqOut->var[i].name) == 0) { 
		    /* a match in var name */
		    dimId = ncInqOut->var[i].dimId[inx];
		    /* try to match dimId */
		    for (j = 0; j <  ncInqOut->ndims; j++) {
			if (ncInqOut->dim[j].id == dimId) {
			    arrayLen = ncInqOut->dim[j].arrayLen;
			    break;
			}
		    }
		}
	    }
	    if (arrayLen == -1) {
                rodsLog (LOG_ERROR,
                  "msiNcGetDimLenInInqOut: unmatched varName %s and ix %d",
                  varName, inx);
                return NETCDF_UNMATCHED_NAME_ERR;
	    }
	}
    }
    fillIntInMsParam (outParam, arrayLen);

    return 0;
}

int
msiNcGetAttNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;
    int inx, i;
    char *varName;
    char *name = NULL;

    RE_TEST_MACRO ("    Calling msiNcGetAttNameInInqOut")

    if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetAttNameInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    inx = parseMspForPosInt (inxParam);

    if (varNameParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (varNameParam->type, STR_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetAttNameInInqOut: nameParam must be STR_MS_T. %s",
          varNameParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        varName = (char*) varNameParam->inOutStruct;
    }

    if (strcmp (varName, "null") == 0) {
	/* use the global att */
	if (inx < 0 || inx >= ncInqOut->ngatts) {
            rodsLog (LOG_ERROR,
              "msiNcGetAttNameInInqOut: input inx %d out of range. ngatts = %d",
              inx, ncInqOut->ngatts);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }
	name = ncInqOut->gatt[inx].name;
    } else {
	/* match the varName first */
	for (i = 0; i < ncInqOut->nvars; i++) {
	    if (strcmp (varName, ncInqOut->var[i].name) == 0) { 
		/* a match in var name */
		break;
	    }
        }
	if (i >= ncInqOut->nvars) {
            rodsLog (LOG_ERROR,
              "msiNcGetAttNameInInqOut: unmatched varName %s", varName);
            return NETCDF_UNMATCHED_NAME_ERR;
        }
        if (inx < 0 || inx >= ncInqOut->var[i].natts) {
            rodsLog (LOG_ERROR,
              "msiNcGetAttNameInInqOut: input inx %d out of range. natts = %d",
              inx, ncInqOut->var[i].natts);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }
        name = ncInqOut->var[i].att[inx].name;
    }
    fillStrInMsParam (outParam, name);

    return 0;
}

int
msiNcGetAttValStrInInqOut (msParam_t *ncInqOutParam, msParam_t *whichAttParam,
msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    int status;
    ncGetVarOut_t *value = NULL;
    char tempStr[NAME_LEN];
    void *bufPtr;

    RE_TEST_MACRO ("    Calling msiNcGetAttValStrInInqOut")
    status = _msiNcGetAttValInInqOut (ncInqOutParam, whichAttParam, 
      varNameParam, &value);

    if (status < 0) return status;

    bufPtr = value->dataArray->buf;
    if (value->dataArray->type == NC_CHAR && value->dataArray->len > 0) {
        /* must be a string */
        status = ncValueToStr (NC_STRING, &bufPtr, tempStr);
    } else {
        status = ncValueToStr (value->dataArray->type, &bufPtr, tempStr);
    }
    if (status < 0) return status;

    fillStrInMsParam (outParam, tempStr);

    return status;
}

int
_msiNcGetAttValInInqOut (msParam_t *ncInqOutParam, msParam_t *whichAttParam,
msParam_t *varNameParam, ncGetVarOut_t **ncGetVarOut)
{
    ncInqOut_t *ncInqOut;
    int i;
    int inx;
    char *varName, *attName;
    ncGetVarOut_t *value = NULL;

    if (ncInqOutParam == NULL || whichAttParam == NULL || ncGetVarOut == NULL)
        return USER__NULL_INPUT_ERR;

    *ncGetVarOut = NULL;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "_msiNcGetAttValInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }
    /* whichAttParam can be inx or attName */
    if (strcmp (whichAttParam->type, STR_MS_T) == 0) {
	attName = (char *)whichAttParam->inOutStruct;
	inx = -1;
    } else if (strcmp (whichAttParam->type, INT_MS_T) == 0) {
	inx = *((int *) whichAttParam->inOutStruct);
	attName = NULL;
    } else {
        rodsLog (LOG_ERROR,
          "_msiNcGetAttValInInqOut:whichAttParam must be INT_MS_T/STR_MS_T. %s",
          whichAttParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    if (varNameParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (varNameParam->type, STR_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "_msiNcGetAttValInInqOut: varNameParam must be STR_MS_T. %s",
          varNameParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        varName = (char*) varNameParam->inOutStruct;
    }

    if (strcmp (varName, "null") == 0) {
	/* use the global att */
	if (attName == NULL) {
	    if (inx < 0 || inx >= ncInqOut->ngatts) {
                rodsLog (LOG_ERROR,
                  "_msiNcGetAttValInInqOut:inp inx %d out of range. ngatts=%d",
                  inx, ncInqOut->ngatts);
                return NETCDF_VAR_COUNT_OUT_OF_RANGE;
            }
	    value = &ncInqOut->gatt[inx].value;
	} else {
	    /* input is a att name */
	    for (i = 0; i < ncInqOut->ngatts; i++) {
		if (strcmp (attName, ncInqOut->gatt[i].name) == 0) {
		    value = &ncInqOut->gatt[i].value;
		    break;
		}
	    }
	    if (value == NULL) {
                rodsLog (LOG_ERROR,
                  "_msiNcGetAttValInInqOut: unmatched attName %s", attName);
                return NETCDF_UNMATCHED_NAME_ERR;
            }
	}
    } else {
	/* match the varName first */
	for (i = 0; i < ncInqOut->nvars; i++) {
	    if (strcmp (varName, ncInqOut->var[i].name) == 0) { 
		/* a match in var name */
		break;
	    }
        }
	if (i >= ncInqOut->nvars) {
            rodsLog (LOG_ERROR,
              "_msiNcGetAttValInInqOut: unmatched varName %s", varName);
            return NETCDF_UNMATCHED_NAME_ERR;
        }
        if (attName == NULL) {
            if (inx < 0 || inx >= ncInqOut->var[i].natts) {
                rodsLog (LOG_ERROR,
                  "_msiNcGetAttNameInInqOut:inp inx %d out of range. natts=%d",
                  inx, ncInqOut->var[i].natts);
                return NETCDF_VAR_COUNT_OUT_OF_RANGE;
            }
            value = &ncInqOut->var[i].att[inx].value;
        } else {
            /* input is a att name */
	    int j;
            for (j = 0; j < ncInqOut->ngatts; j++) {
                if (strcmp (attName, ncInqOut->var[i].att[j].name) == 0) {
                    value = &ncInqOut->var[i].att[j].value;
                    break;
                }
            }
            if (value == NULL) {
                rodsLog (LOG_ERROR,
                  "_msiNcGetAttValInInqOut: unmatched attName %s", attName);
                return NETCDF_UNMATCHED_NAME_ERR;
            }
	}
    }
    *ncGetVarOut = value;

    return 0;
}

int
msiNcGetVarTypeInInqOut (msParam_t *ncInqOutParam, msParam_t *varNameParam, 
msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqOut_t *ncInqOut;
    int i;
    char *varName;

    RE_TEST_MACRO ("    Calling msiNcGetVarTypeInInqOut")

    if (ncInqOutParam == NULL || outParam == NULL || varNameParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetVarTypeInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
          ncInqOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
    }

    if (strcmp (varNameParam->type, STR_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetAttNameInInqOut: nameParam must be STR_MS_T. %s",
          varNameParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        varName = (char*) varNameParam->inOutStruct;
    }

    /* match the varName */
    for (i = 0; i < ncInqOut->nvars; i++) {
        if (strcmp (varName, ncInqOut->var[i].name) == 0) { 
            /* a match in var name */
	    break;
	}
    }
    if (i >= ncInqOut->nvars) {
        rodsLog (LOG_ERROR,
          "msiNcGetAttNameInInqOut: unmatched varName %s", varName);
        return NETCDF_UNMATCHED_NAME_ERR;
    }
    fillIntInMsParam (outParam, ncInqOut->var[i].dataType);

    return 0;
}

int
msiNcIntDataTypeToStr (msParam_t *dataTypeParam, msParam_t *outParam, 
ruleExecInfo_t *rei)
{
    int dataType;
    char dataTypeStr[NAME_LEN];
    int status;

    RE_TEST_MACRO ("    Calling msiNcIntDataTypeToStr")

    if (dataTypeParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (dataTypeParam->type, INT_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcIntDataTypeToStr: Unsupported input dataTypeParam type %s",
          dataTypeParam->type);
        return (USER_PARAM_TYPE_ERR);
    }

    dataType = *((int *) dataTypeParam->inOutStruct);

    if ((status = getNcTypeStr (dataType, dataTypeStr)) < 0) return status; 

    fillStrInMsParam (outParam, dataTypeStr);

    return 0;
}

int
msiAddToNcArray (msParam_t *elementParam, msParam_t *inxParam, 
msParam_t *ncArrayParam, ruleExecInfo_t *rei)
{
    ncGetVarOut_t *ncArray;
    int inx;

    RE_TEST_MACRO ("    Calling msiAddToNcArray")

    if (elementParam == NULL || ncArrayParam == NULL) 
        return USER__NULL_INPUT_ERR;

    inx = parseMspForPosInt (inxParam);
    if (inx < 0 || inx >= NC_MAX_DIMS) {
        rodsLog (LOG_ERROR,
          "msiAddToNcArray: input inx %d is out of range. max  = %d",
          inx, NC_MAX_DIMS);
        return NETCDF_VAR_COUNT_OUT_OF_RANGE;
    }

    if (strcmp (elementParam->type, INT_MS_T) == 0) {
	int *intArray;

	if (ncArrayParam->inOutStruct == NULL) {
	    /* first time */
	    ncArray = (ncGetVarOut_t *) calloc (1, sizeof (ncGetVarOut_t));
	    ncArray->dataArray = (dataArray_t *) 
	        calloc (1, sizeof (dataArray_t));
	    ncArray->dataArray->type = NC_INT;
	    rstrcpy (ncArray->dataType_PI, "intDataArray_PI", NAME_LEN);
	    ncArray->dataArray->buf  = calloc (1, sizeof (int) * NC_MAX_DIMS);
	    fillMsParam (ncArrayParam, NULL, NcGetVarOut_MS_T, ncArray, NULL);
	} else {
	    ncArray = (ncGetVarOut_t *) ncArrayParam->inOutStruct;
	    if (strcmp (ncArray->dataType_PI, "intDataArray_PI") != 0) {
                rodsLog (LOG_ERROR, 
                  "msiAddToNcArray: wrong dataType_PI for INT_MS_T %s",
                  ncArray->dataType_PI);
                return (USER_PARAM_TYPE_ERR);
	    }
        }
	intArray = (int *) ncArray->dataArray->buf;
	intArray[inx] = *((int *) elementParam->inOutStruct);
	if (ncArray->dataArray->len < inx + 1)
	    ncArray->dataArray->len = inx + 1;
    } else {
	/* only do INT_MS_T for now */
        rodsLog (LOG_ERROR,
          "msiAddToNcArray: Unsupported input dataTypeParam type %s",
          elementParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    return 0;
}

int
msiFreeNcStruct (msParam_t *inpParam, ruleExecInfo_t *rei)
{
    ncGetVarOut_t *ncArray;

    RE_TEST_MACRO ("    Calling msiFreeNcStruct")

    if (inpParam == NULL) return USER__NULL_INPUT_ERR;

    if (strcmp (inpParam->type, NcGetVarOut_MS_T) == 0) {
	ncArray = (ncGetVarOut_t *) inpParam->inOutStruct;
	if (ncArray != NULL) {
	    freeNcGetVarOut (&ncArray);
	    inpParam->inOutStruct = NULL;
	}
    } else {
        rodsLog (LOG_ERROR,
          "msiFreeNcStruct: inpParam must be NcGetVarOut_MS_T. %s",
          inpParam->type);
        return (USER_PARAM_TYPE_ERR);
    }

    return 0;
}

#ifdef NETCDF4_API
/**
 * \fn msiNcInqGrps (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcInqGrps (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncInqGrpsInp_t ncInqGrpsInp;
    ncInqGrpsOut_t *ncInqGrpsOut = NULL;

    RE_TEST_MACRO ("    Calling msiNcInqGrps")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcInqGrps: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    if (ncidParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcInqGrps: input ncidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    bzero (&ncInqGrpsInp, sizeof (ncInqGrpsInp));

    ncInqGrpsInp.ncid = parseMspForPosInt (ncidParam);

    rei->status = rsNcInqGrps (rsComm, &ncInqGrpsInp, &ncInqGrpsOut);

    clearKeyVal (&ncInqGrpsInp.condInput);
    if (rei->status >= 0) {
        fillMsParam (outParam, NULL, NcInqGrpsOut_MS_T, ncInqGrpsOut, NULL);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcInqGrps: rsNcInqGrps failed for ncid %d, status = %d",
        ncInqGrpsInp.ncid, rei->status);
    }
    return (rei->status);
}

/**
 * \fn msiNcOpenGroup (msParam_t *rootNcidParam, msParam_t *fullGrpNameParam,
msParam_t *outParam, ruleExecInfo_t *rei)
 *
**/
int
msiNcOpenGroup (msParam_t *rootNcidParam, msParam_t *fullGrpNameParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncOpenInp_t ncOpenInp;
    int *grpNcid = NULL;

    RE_TEST_MACRO ("    Calling msiNcOpenGroup")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcOpenGroup: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;

    bzero (&ncOpenInp, sizeof (ncOpenInp));
    if (rootNcidParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcOpenGroup: input rootNcidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    ncOpenInp.rootNcid = parseMspForPosInt (rootNcidParam);
    if (strcmp (fullGrpNameParam->type, STR_MS_T) == 0) {
        rstrcpy (ncOpenInp.objPath, (char*)fullGrpNameParam->inOutStruct,
          MAX_NAME_LEN);
    } else {
        rodsLog (LOG_ERROR,
          "msiNcOpenGroup: Unsupported input fullGrpNameParam type %s",
          fullGrpNameParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    rei->status = rsNcOpenGroup (rsComm, &ncOpenInp, &grpNcid);

    clearKeyVal (&ncOpenInp.condInput);
    if (rei->status >= 0) {
        fillIntInMsParam (outParam, *grpNcid);
        free (grpNcid);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcOpenGroup: rsNcOpenGroup failed for rootNcid %d, status = %d",
        ncOpenInp.rootNcid, rei->status);
    }
    return (rei->status);
}
#endif

int
msiNcGetNGrpsInInqOut (msParam_t *ncInqGrpsOutParam, msParam_t *outParam,
ruleExecInfo_t *rei)
{
    ncInqGrpsOut_t *ncInqGrpsOut;

    RE_TEST_MACRO ("    Calling msiNcGetNGrpsInInqOut")

    if (ncInqGrpsOutParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqGrpsOutParam->type, NcInqGrpsOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetNGrpsInInqOut: ncInqGrpsOutParam must be NcInqGrpsOut_MS_T. %s",
          ncInqGrpsOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqGrpsOut = (ncInqGrpsOut_t *) ncInqGrpsOutParam->inOutStruct;
    }

    /* global nvars */
    fillIntInMsParam (outParam, ncInqGrpsOut->ngrps);

    return 0;
}

int
msiNcGetGrpInInqOut (msParam_t *ncInqGrpsOutParam, 
msParam_t *inxParam, msParam_t *outParam, ruleExecInfo_t *rei)
{
    ncInqGrpsOut_t *ncInqGrpsOut;
    int inx;

    RE_TEST_MACRO ("    Calling msiNcGetGrpInInqOut")

    if (ncInqGrpsOutParam == NULL || inxParam == NULL || outParam == NULL)
        return USER__NULL_INPUT_ERR;

    if (strcmp (ncInqGrpsOutParam->type, NcInqGrpsOut_MS_T) != 0) {
        rodsLog (LOG_ERROR,
          "msiNcGetGrpInInqOut: ncInqGrpsOutParam must be NcInqGrpsOut_MS_T. %s",
          ncInqGrpsOutParam->type);
        return (USER_PARAM_TYPE_ERR);
    } else {
        ncInqGrpsOut = (ncInqGrpsOut_t *) ncInqGrpsOutParam->inOutStruct;
    }
    inx = parseMspForPosInt (inxParam);
    if (inx < 0 || inx >= ncInqGrpsOut->ngrps) {
        rodsLog (LOG_ERROR,
          "msiNcGetGrpInInqOut: input inx %d is out of range. ngrps  = %d",
          inx, ncInqGrpsOut->ngrps);
        return NETCDF_VAR_COUNT_OUT_OF_RANGE;
    }

    /* global nvars */
    fillStrInMsParam (outParam, ncInqGrpsOut->grpName[inx]);

    return 0;
}

int
msiNcRegGlobalAttr (msParam_t *objPathParam, msParam_t *adminParam, 
msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncRegGlobalAttrInp_t ncRegGlobalAttrInp;

    RE_TEST_MACRO ("    Calling msiNcRegGlobalAttr")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcRegGlobalAttr: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    rsComm = rei->rsComm;
    if (objPathParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcRegGlobalAttr: input objPathParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (strcmp (objPathParam->type, STR_MS_T) == 0) {
        /* str input */
	bzero (&ncRegGlobalAttrInp, sizeof (ncRegGlobalAttrInp));
	rstrcpy (ncRegGlobalAttrInp.objPath, 
          (char*)objPathParam->inOutStruct, MAX_NAME_LEN);
    } else {
        rodsLog (LOG_ERROR,
          "msiNcRegGlobalAttr: Unsupported input objPathParam type %s",
          objPathParam->type);
        return (USER_PARAM_TYPE_ERR);
    }
    if (adminParam != NULL) {
	int adminFlag = parseMspForPosInt (adminParam);
        if (adminFlag > 0) {
	    addKeyVal (&ncRegGlobalAttrInp.condInput, IRODS_ADMIN_KW, "");
        }
    }
    rei->status = rsNcRegGlobalAttr (rsComm, &ncRegGlobalAttrInp);
    clearKeyVal (&ncRegGlobalAttrInp.condInput);
    fillIntInMsParam (outParam, rei->status);
    if (rei->status < 0) {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcRegGlobalAttr: rscNcRegGlobalAttr failed for %s, status = %d",
        ncRegGlobalAttrInp.objPath, rei->status);
    }

    return (rei->status);
}

int
msiNcSubsetVar (msParam_t *varNameParam, msParam_t *ncidParam, 
msParam_t *ncInqOutParam, msParam_t *subsetStrParam,
msParam_t *outParam, ruleExecInfo_t *rei)
{
    rsComm_t *rsComm;
    ncVarSubset_t ncVarSubset;
    int ncid;
    ncInqOut_t *ncInqOut;
    char *name;
    char *subsetStr;
    ncGetVarOut_t *ncGetVarOut = NULL;

    RE_TEST_MACRO ("    Calling msiNcSubsetVar")

    if (rei == NULL || rei->rsComm == NULL) {
      rodsLog (LOG_ERROR,
        "msiNcSubsetVar: input rei or rsComm is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
    rsComm = rei->rsComm;
    if (ncidParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcSubsetVar: input ncidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        ncid = parseMspForPosInt (ncidParam);
        if (ncid < 0) return ncid;
    }
    if (ncInqOutParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcSubsetVar: input ncInqOutParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;

    }

    if (varNameParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcSubsetVar: input varNameParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        name = (char*) varNameParam->inOutStruct;
    }
    if (subsetStrParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcSubsetVar: input subsetStrParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        subsetStr = (char*) subsetStrParam->inOutStruct;
    }
    bzero (&ncVarSubset, sizeof (ncVarSubset));
    ncVarSubset.numVar = 1;
    rstrcpy ((char *) ncVarSubset.varName, name, LONG_NAME_LEN);
    rei->status = parseSubsetStr (subsetStr, &ncVarSubset);
    if (rei->status < 0) return rei->status;
    rei->status = ncSubsetVar (rsComm, ncid, ncInqOut, &ncVarSubset,
      &ncGetVarOut);
    if (rei->status >= 0) {
        fillMsParam (outParam, NULL, NcGetVarOut_MS_T, ncGetVarOut, NULL);
    } else {
      rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
        "msiNcSubsetVar: ncSubsetVar failed, status = %d",
        rei->status);
    }

    return rei->status;
}

int
ncSubsetVar (rsComm_t *rsComm, int ncid, ncInqOut_t *ncInqOut, 
ncVarSubset_t *ncVarSubset, ncGetVarOut_t **ncGetVarOut)
{
    int i, j, k, status;
    rodsLong_t start[NC_MAX_DIMS], stride[NC_MAX_DIMS], count[NC_MAX_DIMS];
    ncGetVarInp_t ncGetVarInp;
    int varInx = -1;

    for (i = 0; i < ncInqOut->nvars; i++) {
        if (strcmp ((char *)ncVarSubset->varName, ncInqOut->var[i].name) == 0) {
            varInx = i;
            break;
        }
    }
    if (varInx < 0) {
        rodsLog (LOG_ERROR,
          "ncSubsetVar: unmatched input var name %s", ncVarSubset->varName);
        return NETCDF_UNMATCHED_NAME_ERR;
    }
    /* the following code is almost identical to getSingleNcVarData on 
     * the client side */
    for (j = 0; j < ncInqOut->var[varInx].nvdims; j++) {
        int dimId = ncInqOut->var[varInx].dimId[j];
        int doSubset = False;
        if (ncVarSubset != NULL && ncVarSubset->numSubset > 0) {
            for (k = 0; k < ncVarSubset->numSubset; k++) {
                if (strcmp (ncInqOut->dim[dimId].name,
                  ncVarSubset->ncSubset[k].subsetVarName) == 0) {
                    doSubset = True;
                    break;
                }
            }
        }
       if (doSubset == True) {
            if (ncVarSubset->ncSubset[k].start >=
              ncInqOut->dim[dimId].arrayLen ||
              ncVarSubset->ncSubset[k].end >=
              ncInqOut->dim[dimId].arrayLen ||
              ncVarSubset->ncSubset[k].start >
              ncVarSubset->ncSubset[k].end) {
                rodsLog (LOG_ERROR,
                 "ncSubsetVar:start %d or end %d for %s outOfRange %lld",
                 ncVarSubset->ncSubset[k].start,
                 ncVarSubset->ncSubset[k].end,
                 ncVarSubset->ncSubset[k].subsetVarName,
                 ncInqOut->dim[dimId].arrayLen);
                return NETCDF_DIM_MISMATCH_ERR;
            }
            start[j] = ncVarSubset->ncSubset[k].start;
            stride[j] = ncVarSubset->ncSubset[k].stride;
            count[j] = ncVarSubset->ncSubset[k].end -
              ncVarSubset->ncSubset[k].start + 1;
        } else {
            start[j] = 0;
            count[j] = ncInqOut->dim[dimId].arrayLen;
            stride[j] = 1;
        }
    }
    bzero (&ncGetVarInp, sizeof (ncGetVarInp));
    ncGetVarInp.dataType = ncInqOut->var[varInx].dataType;
    ncGetVarInp.ncid = ncid;
    ncGetVarInp.varid =  ncInqOut->var[varInx].id;
    ncGetVarInp.ndim =  ncInqOut->var[varInx].nvdims;
    ncGetVarInp.start = start;
    ncGetVarInp.count = count;
    ncGetVarInp.stride = stride;

    status = rsNcGetVarsByType (rsComm, &ncGetVarInp, ncGetVarOut);

    if (status < 0) {
        rodsLogError (LOG_ERROR, status,
          "ncSubsetVar: rcNcGetVarsByType error for %s",
          ncInqOut->var[varInx].name);
    }
    return status;
}

int
msiNcVarStat (msParam_t *ncGetVarOutParam, msParam_t *statOutStr,
ruleExecInfo_t *rei)
{
    ncGetVarOut_t *ncGetVarOut;
    char outStr[MAX_NAME_LEN];
    int i;
    float mymax = 0;
    float mymin = 0;
    float mytotal = 0;
    short *myshortArray;
    unsigned short *myushortArray;
    int *myintArray;
    unsigned int *myuintArray;
    long long *mylongArray;
    unsigned long long *myulongArray;
    float myfloat, *myfloatArray;
    double *mydoubleArray; 
    

    RE_TEST_MACRO ("    Calling msiNcVarStat")

    if (rei == NULL) {
        rodsLog (LOG_ERROR, "msiNcVarStat: input rei is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (ncGetVarOutParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiNcVarStat: input ncidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        ncGetVarOut = (ncGetVarOut_t *) ncGetVarOutParam->inOutStruct;
        if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
    }

    if (ncGetVarOut->dataArray->type == NC_CHAR || 
      ncGetVarOut->dataArray->type == NC_STRING ||
      ncGetVarOut->dataArray->type == NC_BYTE ||  
      ncGetVarOut->dataArray->type == NC_UBYTE) {
        rodsLog (LOG_ERROR,
          "msiNcVarStat: cannot compute max/min/ave of chr or string");
        return (NETCDF_INVALID_DATA_TYPE);
    }
    switch (ncGetVarOut->dataArray->type) {
      case NC_SHORT:
        myshortArray = (short *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) myshortArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) myshortArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_USHORT:
        myushortArray = (unsigned short *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) myushortArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) myushortArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_INT:
        myintArray = (int *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) myintArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) myintArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_UINT:
        myuintArray = (unsigned int *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) myuintArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) myuintArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_INT64:
        mylongArray = (long long *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) mylongArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) mylongArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_UINT64:
        myulongArray = (unsigned long long *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) myulongArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) myulongArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_FLOAT:
        myfloatArray = (float *) ncGetVarOut->dataArray->buf;
        mymax = mymin = myfloatArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = myfloatArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      case NC_DOUBLE:
        mydoubleArray = (double *) ncGetVarOut->dataArray->buf;
        mymax = mymin = (float) mydoubleArray[0];
        for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
            myfloat = (float) mydoubleArray[i];
            procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
        }
        break;
      default:
        rodsLog (LOG_ERROR,
          "msiNcVarStat: Unknow dataType %d", ncGetVarOut->dataArray->type);
        return (NETCDF_INVALID_DATA_TYPE);
    }
    snprintf (outStr, MAX_NAME_LEN, 
      "      array length = %d max = %.4f min = %.4f ave = %.4f\n",
      ncGetVarOut->dataArray->len, mymax, mymin, 
      mytotal / ncGetVarOut->dataArray->len);
    fillMsParam (statOutStr, NULL, STR_MS_T, outStr, NULL);
    return 0;
}

int
procMaxMinAve (float myfloat, float *mymax, float *mymin, float *mytotal)
{
    *mytotal += myfloat;
    if (myfloat < *mymin) *mymin = myfloat;
    if (myfloat > *mymax) *mymax = myfloat;
    return 0;
}

#if 0	/* not done */
int
msiPrVarInNcGetVarOut (msParam_t *ncGetVarOutParam, msParam_t *prefixParam,
msParam_t *itemsPerLineParam, ruleExecInfo_t *rei)
{
    ncGetVarOut_t *ncGetVarOut;
    RE_TEST_MACRO ("    Calling msiPrVarInNcGetVarOut")
    char *prefix;
    int itemsPerLine;
    char tempStr[NAME_LEN];
    void *bufPtr;
    int outCnt = 0;
    int itemsInLine = 0;

    if (rei == NULL) {
        rodsLog (LOG_ERROR, "msiPrVarInNcGetVarOut: input rei is NULL");
      return (SYS_INTERNAL_NULL_INPUT_ERR);
    }

    if (ncGetVarOutParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiPrVarInNcGetVarOut: input ncidParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        ncGetVarOut = (ncGetVarOut_t *) ncGetVarOutParam->inOutStruct;
        if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
            return USER__NULL_INPUT_ERR;
    }
    if (prefixParam == NULL) {
        rodsLog (LOG_ERROR,
          "msiPrVarInNcGetVarOut: input prefixParam is NULL");
        return (SYS_INTERNAL_NULL_INPUT_ERR);
    } else {
        prefix = (char *) prefixParam->inOutStruct;
        if ( prefix == NULL) return USER__NULL_INPUT_ERR;
    }
    if (itemsPerLineParam == NULL) {
        itemsPerLine = 1;
    } else {
        itemsPerLine = parseMspForPosInt (itemsPerLineParam);
        if (itemsPerLine <= 0) itemsPerLine = 1;
    }
    bufPtr = ncGetVarOut->dataArray->buf;
    bzero (tempStr, sizeof (tempStr));
    if (ncInqOut->var[varInx].dataType == NC_CHAR) {
        int totalLen = 0;
        while (totalLen < ncGetVarOut->dataArray->len) {
            int len;
            char *nextBufPtr;
            /* treat it as strings */
            if (outCnt == 0) _writeString ("stdout", prefix, rei);
            len = strlen (bufPtr);
            totalLen += (len + 1);

            nextBufPtr = bufPtr + len + 1;
            while (isspace (*nextBufPtr) || *nextBufPtr = '\0') {
                bufPtr++;
                totalLen++;
                if (totalLen >= ncGetVarOut->dataArray->len) break;
            }
            if (totalLen >= ncGetVarOut->dataArray->len) {
                /* last one */
                printf ("%s ;\n", (char *) bufPtr);
                snprintf (tempStr, NAME_LEN, "%s ;\n", bufPtr);
            if (outCnt >= itemsPerLine - 1) {
                /* reset */
                snprintf (tempStr, NAME_LEN, "%s,\n", bufPtr);
                outCnt = 0;
            } else {
               snprintf (tempStr, NAME_LEN, "%s,", bufPtr);
            }
            _writeString ("stdout", tempStr, rei);
            outCnt ++;
            bufPtr = nextBufPtr;
        }
    } else {
        for (j = 0; j < ncGetVarOut->dataArray->len; j++) {
            ncValueToStr (ncInqOut->var[varInx].dataType, &bufPtr, tempStr);
            outCnt++;
            if (j >= ncGetVarOut->dataArray->len - 1) {
                strcat (tempStr, " ;\n");
                printf ("%s ;\n", tempStr);
            } else if (itemsPerLine > 0) {
                itemsInLine++;
                if (itemsInLine >= itemsPerLine) {
                    strcat (tempStr, ",\n");
                    itemsInLine = 0;
                } else {
                    strcat (tempStr, ", ");
                }
            } else if (outCnt >= lastDimLen) {
                /* reset */
                strcat (tempStr, ",\n  ");
                outCnt = 0;
            } else {
                strcat (tempStr, ", ");
            }
        }
    }
    return 0;
}
#endif


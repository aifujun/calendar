
/** Include Files */
#include "ErrorCode.h"

/** Defines/Macros/Constants/Typedefs */


/** Public Methods */
const char* getErrorName(ErrorCode error){
    const char* result = "UNKNOWN";

    switch (error){
        case ERR_OK: result = "ERR_OK"; break;
        case ERR_SYSTEM: result = "ERR_SYSTEM"; break;
        case ERR_TIMEOUT: result = "ERR_TIMEOUT"; break;
        case LOGIN_ERROR_UNKNOWN_ERROR: result = "LOGIN_ERROR_UNKNOWN_ERROR"; break;
        case LOGIN_ERROR_UNKNOWN_USER_NAME: result = "LOGIN_ERROR_UNKNOWN_USER_NAME"; break;
        case DATAMANAGER_ERROR_UNKNOWN_ERROR: result = "DATAMANAGER_ERROR_UNKNOWN_ERROR"; break;
        case DATAMANAGER_ERROR_MISSING_CONNECTION: result = "DATAMANAGER_ERROR_MISSING_CONNECTION"; break;
        default: break;
    }

    return result;
}

const char* getErrorDescription(ErrorCode error){
    const char *result = "UNKNOWN";

    switch (error){
        case ERR_OK: result = "Success."; break;
        case ERR_SYSTEM: result = "General system error."; break;
        case ERR_TIMEOUT: result = "Time out."; break;
        case LOGIN_ERROR_UNKNOWN_ERROR: result = "Unrecognized error."; break;
        case LOGIN_ERROR_UNKNOWN_USER_NAME: result = "Unrecognized user name, it is case sensitive."; break;
        case DATAMANAGER_ERROR_UNKNOWN_ERROR: result = "Unrecognized error."; break;
        case DATAMANAGER_ERROR_MISSING_CONNECTION: result = "Connection missed."; break;
        default: break;
    }

    return result;
}

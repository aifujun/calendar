/**
 * 文件头注释
 */

#ifndef PRETTYTOOLS_ERRORCODE_H
#define PRETTYTOOLS_ERRORCODE_H

/** Include Files */

/** Defines/Macros/Constants/Typedefs */


/** enum常量声明 */

/** 类型声明和定义, 包括 struct union typedef 等 */

/** 全局变量声明 */

/** 文件级变量声明 */

//basic mask code
//const unsigned int COMPONENT_MASK               = 0x8000;
//
//component code
//const unsigned int COMPONENT_LOGIN              = 0x0001;
//const unsigned int COMPONENT_DATAMANAGER        = 0x0002;
//const unsigned int COMPONENT_CALENDAR           = 0x0003;


//basic mask code
#define COMPONENT_MASK          0x8000

//component code
#define COMPONENT_LOGIN         0x0001
#define COMPONENT_DATAMANAGER   0x0002
#define COMPONENT_CALENDAR      0x0003

//
//0x0000XXXX: XXXX means component code
//0xXXXX0000: XXXX means error code of component

//Define some marcos/functions here
#define SYSTEM_ERROR_FLAG (0x4000)
#define SYSTEM_ERROR(code) ((code | SYSTEM_ERROR_FLAG) << 16)
#define ERROR_IS_SYSTEM_ERROR(err) (((err >> 16) & SYSTEM_ERROR_FLAG) != 0)

#define COMPONENT_ERROR(component, code) ((code << 16) | (component & ~(COMPONENT_MASK)))
#define CODE_FROM_ERROR(err) (static_cast<short>(err >> 16))

//All the error code is here
typedef enum {
    //   Hex        | Decimal       | Description
    //Basic error
    ERR_OK                                  = 0x00000000,                                   /*!< 0x00000000 |  0            | No error. */
    ERR_SYSTEM                              = SYSTEM_ERROR(1),                              /*!< 0x00010000 | 65536         | General system error. */
    ERR_TIMEOUT                             = SYSTEM_ERROR(2),                              /*!< 0x00020000 | 131072        | Time out. */

    //login component
    LOGIN_ERROR_UNKNOWN_ERROR               = COMPONENT_ERROR(COMPONENT_LOGIN, 1),          /*!< 0x00010001 |65537    | Unrecognized error. */
    LOGIN_ERROR_UNKNOWN_USER_NAME           = COMPONENT_ERROR(COMPONENT_LOGIN, 2),          /*!< 0x00020001 |131073   | Unrecognized user name, it is case sensitive. */

    //data manager component
    DATAMANAGER_ERROR_UNKNOWN_ERROR         = COMPONENT_ERROR(COMPONENT_DATAMANAGER, 1),    /*!< 0x00010002 |65538    | Unrecognized error. */
    DATAMANAGER_ERROR_MISSING_CONNECTION    = COMPONENT_ERROR(COMPONENT_DATAMANAGER, 2),    /*!< 0x00020002 |131074   | Connection missed. */

    // calendar component
    CALENDAR_ERROR_UNKNOWN_ERROR            = COMPONENT_ERROR(COMPONENT_CALENDAR, 1),       /*!< 0x00010003 |65539    | Unrecognized error. */
    CALENDAR_ERROR_DATE_OUT_OF_RANGE        = COMPONENT_ERROR(COMPONENT_CALENDAR, 2),       /*!< 0x00020003 |131075   | Date is outside the defined range. */
    CALENDAR_ERROR_ILLEGAL_DATE             = COMPONENT_ERROR(COMPONENT_CALENDAR, 3),       /*!< 0x00030003 |196611   | Illegal date. */
    CALENDAR_ERROR_ILLEGAL_ORDINAL_DAYS     = COMPONENT_ERROR(COMPONENT_CALENDAR, 4),       /*!< 0x00040003 |262147   | Illegal ordinal days. */
    CALENDAR_ERROR_PARAM_ERROR              = COMPONENT_ERROR(COMPONENT_CALENDAR, 5),       /*!< 0x00050003 |327683   | Param error. */

}ErrorCode;



/*!
 * ErrorHelper
 * translating error codes to useful strings.
 */
static const char* getErrorName(ErrorCode error);
static const char* getErrorDescription(ErrorCode error);


#endif //PRETTYTOOLS_ERRORCODE_H

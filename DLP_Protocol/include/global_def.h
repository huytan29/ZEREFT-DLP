/*Copyright (C) 2023, LancsNet Jsc.*/
/*Global Definitions*/
#ifndef DLP_HEADERS
#define DLP_HEADERS


#define OS_MAXSTR  65536               /* Size for logs, sockets, etc      */





/* Level of log messages */
#define LOGLEVEL_CRITICAL 4
#define LOGLEVEL_ERROR 3
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_INFO 1
#define LOGLEVEL_DEBUG 0






/* User Configuration */
#ifndef USER
#define USER            "lancsdlp"
#endif

#ifndef ROOTUSER
#define ROOTUSER        "root"
#endif

#ifndef GROUPGLOBAL
#define GROUPGLOBAL     "lancsdlp"
#endif









#endif
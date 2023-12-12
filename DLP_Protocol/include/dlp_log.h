/* Copyright (C) 2015, Wazuh Inc.
 * Copyright (C) 2009 Trend Micro Inc.
 * All right reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

/* Functions to generate debug/information/error/warning/critical reports
 *
 * We have two debug levels (1 and 2), a verbose mode and functions to catch warnings, errors, and critical situations
 *
 * To see these messages, use the "-d","-v" options (or "-d" twice to see debug2)
 * The merror is printed by default when an important error occurs
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifndef __GNUC__
#define __attribute__(x)
#endif

#include <stdarg.h>
#include "cJSON.h"
/* For internal logs */
#ifndef LOGFILE
#ifndef WIN32
#define LOGFILE   "/home/lancs/DataLossPrevention/codebase/Logging/logs/dlp.log"
#define LOGJSONFILE "/home/lancs/DataLossPrevention/codebase/Logging/logs/dlp.json"
#define _PRINTF_FORMAT printf
#else
#define LOGFILE "dlp.log"
#define LOGJSONFILE "dlp.json"
#define _PRINTF_FORMAT __MINGW_PRINTF_FORMAT
#endif
#endif
void SetNameModule(const char*);
/*Function to debug msg*/
#define ldebug(msg, ...) _mdebug1(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to debug msg with tag*/
#define ltdebug(tag, msg, ...) _mtdebug1(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to debug msg in verbose */
#define ldebug2(msg, ...) _mdebug2(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to debug msg with tag in verbose */
#define ltdebug2(tag, msg, ...) _mtdebug2(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error*/
#define lerror(msg, ...) _merror(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error with tag*/
#define lterror(tag, msg, ...) _mterror(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print warning message*/
#define lwarn(msg, ...) _mwarn(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print warning message*/
#define ltwarn(tag, msg, ...) _mtwarn(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print information message*/
#define linfo(msg, ...) _minfo(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print information message with tag*/
#define ltinfo(tag, msg, ...) _mtinfo(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error message to file, without printing to the terminal*/
#define lferror(msg, ...) _mferror(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error message to file with tag, without printing to the terminal*/
#define ltferror(tag, msg, ...) _mtferror(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error message and quit program*/
#define lerror_exit(msg, ...) _merror_exit(__FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error message with tag and quit program*/
#define lterror_exit(tag, msg, ...) _mterror_exit(tag, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)
/*Function to print error message with level and quit program*/
#define llerror_exit(level, msg, ...) _mlerror_exit(level, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)

void _mdebug1(const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull));
void _mtdebug1(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull));
void _mdebug2(const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull));
void _mtdebug2(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull));
void _merror(const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull));
void _mterror(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull));
void _mverror(const char * file, int line, const char * func, const char *msg, va_list args)  __attribute__((nonnull));
void _mwarn(const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull));
void _mtwarn(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull));
void _mvwarn(const char * file, int line, const char * func, const char *msg, va_list args)  __attribute__((nonnull));
void _minfo(const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull));
void _mtinfo(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull));
void _mvinfo(const char * file, int line, const char * func, const char *msg, va_list args)  __attribute__((nonnull));
void print_out(const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 1, 2))) __attribute__((nonnull));
void _mferror(const char * file, int line, const char * func, const char *msg, ... ) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull));
void _mtferror(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull));
void _merror_exit(const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 4, 5))) __attribute__((nonnull)) __attribute__ ((noreturn));
void _mterror_exit(const char *tag, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull)) __attribute__ ((noreturn));
void _mlerror_exit(const int level, const char * file, int line, const char * func, const char *msg, ...) __attribute__((format(_PRINTF_FORMAT, 5, 6))) __attribute__((nonnull)) __attribute__ ((noreturn));

/**
 * @brief Logging module initializer
 */
void logging_init(void);

/* Function to read the logging format configuration */
void os_logging_config(void);
cJSON *getLoggingConfig(void);

#ifdef WIN32
char * win_strerror(unsigned long error);
#endif

/* Use these three functions to set when you
 * enter in debug, chroot or daemon mode
 */
void nowDebug(void);
int isDebug(void);

void nowChroot(void);
void nowDaemon(void);

int isChroot(void);

/* Debug analysisd */
#ifdef DEBUGAD
#define DEBUG_MSG(x,y,z) minfo(x,y,z)
#else
#define DEBUG_MSG(x,y,z)
#endif /* end debug analysisd */

#endif /* DEBUG_H */

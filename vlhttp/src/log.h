/*
 * =====================================================================================
 * 
 *       Filename:  log.h
 * 
 *    Description:  logging stuff
 * 
 *        Version:  1.0
 *        Created:  13.09.2007 16:31:31 EEST
 *       Revision:  $Id: log.h 710 2008-07-31 21:32:55Z vlad $
 *       Compiler:  gcc
 * 
 *         Author:  Vladislav Moskovets, mail: devvlad dot gmail dot com
 * 
 * =====================================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LIBLOG_ENABLED
#define LIBLOG_COLORS
//#define LIBLOG_PIDCOLORS

#include <errno.h>
#include "log_colors.h"

void liblog_init(const char *filename);
void liblog_print(const char* prefix, const char* str, ...);
void liblog_fenter(const char* file, int line, const char* func);
void liblog_fleave(const char* file, int line, const char* func);
void liblog_assert(const char* file, int line, const char* func, const char *expr_str);
void liblog_verify(const char* file, int line, const char* func);
void liblog_assert_perror(const char* file, int line, const char* func, int errornumber);
void liblog_verify_perror(const char* file, int line, const char* func, int errornumber);

const char* liblog_get_debug_prefix(const char *prefix, const char* file, int line, const char* func);
void liblog_fenter_args(const char* file, int line, const char* func, const char* fmt, ...);
void liblog_fleave_args(const char* file, int line, const char* func, const char* fmt, ...);
void liblog_set_file(FILE*f);
void liblog_first_line();
int  liblog_get_level();
int  liblog_set_level(int);
void liblog_hexdump(const char* str, unsigned char *data, int bytes);
void liblog_done();

#define LOG_LEVEL_DBG       4
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_ERR       1
#define LOG_HEXDUMP_STEP    40

#define LOG_INIT(filename)		(liblog_init(filename))
#define LOG_SET_FILE(f)			(liblog_set_file(f))
#define LOG_DONE				liblog_done()
#define RESET_ERRNO				(((errno)=(0)))
#ifndef _WIN32
#define ASSERT(expr)			((void)((expr) ? (0) : (liblog_assert(__FILE__, __LINE__, __FUNCTION__, "Expression: '"#expr"'"))))
#define VERIFY(expr)			((void)((expr) ? (0) : (liblog_assert(__FILE__, __LINE__, __FUNCTION__))))
#endif
#define ASSERT_PERROR			((void)((!(errno)) ? (0) : (liblog_assert_perror(__FILE__, __LINE__, __FUNCTION__, errno))));RESET_ERRNO
#define VERIFY_PERROR			((void)((!(errno)) ? (0) : (liblog_assert_perror(__FILE__, __LINE__, __FUNCTION__, errno))))
#define FENTER					(liblog_fenter(__FILE__, __LINE__, __FUNCTION__));
#define FENTERA(fmt, ...)		(liblog_fenter_args(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__));
#define FLEAVE					(liblog_fleave(__FILE__, __LINE__, __FUNCTION__));
#define FLEAVEA(fmt, ...)		(liblog_fleave_args(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__));
#define DBG(str, ...)			{ if( LOG_GET_LEVEL >= LOG_LEVEL_DBG )  { liblog_print(((const char*) liblog_get_debug_prefix(  ANSI_BLUE"| DEBUG:"ANSI_RESET,	__FILE__, __LINE__, __FUNCTION__)), str, __VA_ARGS__);}; };
#define INFO(str, ...)			{ if( LOG_GET_LEVEL >= LOG_LEVEL_INFO ) { liblog_print(((const char*) liblog_get_debug_prefix( ANSI_GREEN"| INFO:"ANSI_RESET,		__FILE__, __LINE__, __FUNCTION__)), str, __VA_ARGS__);}; };
#define WARN(str, ...)			{ if( LOG_GET_LEVEL >= LOG_LEVEL_WARN ) { liblog_print(((const char*) liblog_get_debug_prefix(ANSI_YELLOW"| WARNING:"ANSI_RESET,	__FILE__, __LINE__, __FUNCTION__)), str, __VA_ARGS__);}; };
#define ERR(str, ...)			{ if( LOG_GET_LEVEL >= LOG_LEVEL_ERR )  { liblog_print(((const char*) liblog_get_debug_prefix(   ANSI_RED"| ERROR:"ANSI_RESET,	__FILE__, __LINE__, __FUNCTION__)), str, __VA_ARGS__);}; };
#define LOG_HEXDUMP(str, data, size) (liblog_hexdump(str, data, size))
#define LOG_GET_LEVEL			(liblog_get_level())
#define LOG_SET_LEVEL(level)	(liblog_set_level(level))



#else

void liblog_none(const char* str, ...);
#define LOG_INIT(str)				(liblog_none(str))
#define LOG_DONE				((void) 0)
#define LOG_SET_FILE(f)			((void) 0)
#define RESET_ERRNO				((void) (0))
#ifndef _WIN32
#define ASSERT(expr)			((void) (0))
#define VERIFY(expr)			((void) (expr))
#endif
#define ASSERT_PERROR			((void) (0))
#define VERIFY_PERROR			((void) (expr))
#define FENTER					((void)	(0))
#define FENTERA(fmt, ...)		((void) (0))
#define FLEAVE					((void)	(0))
#define FLEAVEA(fmt, ...)		((void)	(0))
#define DBG(str, ...)			((void) (0))
#define INFO(str, ...)			((void) (0))
#define WARN(str, ...)			((void) (0))
#define ERR(str, ...)			((void) (0))
#define LOG_HEXDUMP(str, data, size) ((void) (0))
#define LOG_GET_LEVEL			((void) (0))
#define LOG_SET_LEVEL(level)	((void) (0))
#endif

#ifdef __cplusplus
}
#endif

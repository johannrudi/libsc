/*
  This file is part of the SC Library.
  The SC library provides support for parallel scientific applications.

  Copyright (C) 2008,2009 Carsten Burstedde, Lucas Wilcox.

  The SC Library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the SC Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SC_H
#define SC_H

/* include the sc_config header first */

#include <sc_config.h>

/* include MPI before stdio.h */

#ifdef SC_MPI
#include <mpi.h>
#else
#ifdef MPI_SUCCESS
#error "mpi.h is included.  Use --enable-mpi."
#endif
#endif

/* include system headers */

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <math.h>
#include <ctype.h>
#include <float.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* provide extern C defines */

/* the hacks below enable semicolons after the SC_EXTERN_C_ macros. */
#ifdef __cplusplus
#define SC_EXTERN_C_BEGIN       extern "C" { void sc_extern_c_hack_1 (void)
#define SC_EXTERN_C_END                    } void sc_extern_c_hack_2 (void)
#else
#define SC_EXTERN_C_BEGIN                    void sc_extern_c_hack_1 (void)
#define SC_EXTERN_C_END                      void sc_extern_c_hack_1 (void)
#endif

/* these two libsc headers are always included */

#include <sc_c99_functions.h>
#include <sc_mpi.h>

SC_EXTERN_C_BEGIN;

/* extern variables */

extern const int    sc_log2_lookup_table[256];
extern int          sc_package_id;

/* control a trace file by environment variables (see sc_init) */
extern FILE        *sc_trace_file;
extern int          sc_trace_prio;

/* define math constants if necessary */
#ifndef M_E
#define M_E 2.7182818284590452354       /* e */
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634074   /* log_2 e */
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.43429448190325182765 /* log_10 e */
#endif
#ifndef M_LN2
#define M_LN2 0.69314718055994530942    /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10 2.30258509299404568402   /* log_e 10 */
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846     /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923   /* pi/2 */
#endif
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962   /* pi/4 */
#endif
#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154   /* 1/pi */
#endif
#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308   /* 2/pi */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390       /* 2/sqrt(pi) */
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880  /* sqrt(2) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440        /* 1/sqrt(2) */
#endif

#define SC_EPS               2.220446049250313e-16
#define SC_1000_EPS (1000. * 2.220446049250313e-16)

#if 0
/*@ignore@*/
#define index   DONT_USE_NAME_CONFLICT_1 ---
#define rindex  DONT_USE_NAME_CONFLICT_2 ---
#define link    DONT_USE_NAME_CONFLICT_3 ---
#define NO_DEFINE_DONT_USE_CONFLICT SPLINT_IS_STUPID_ALSO
/*@end@*/
#endif /* 0 */

/* check macros, always enabled */

#define SC_NOOP() ((void) (0))
#define SC_ABORT(s)                             \
  sc_abort_verbose (__FILE__, __LINE__, (s))
#define SC_ABORT_NOT_REACHED() SC_ABORT ("Unreachable code")
#define SC_CHECK_ABORT(c,s)                     \
  ((c) ? (void) 0 : SC_ABORT (s))
#define SC_CHECK_MPI(r) SC_CHECK_ABORT ((r) == MPI_SUCCESS, "MPI error")

/*
 * C++98 does not allow variadic macros
 * 1. Declare a default variadic function for C and C++
 * 2. Use macros in C instead of the function
 * This loses __FILE__ and __LINE__ in the C++ ..F log functions
 */
void                SC_ABORTF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_CHECK_ABORTF (int success, const char *fmt, ...)
  __attribute__ ((format (printf, 2, 3)));
#ifndef __cplusplus
#define SC_ABORTF(fmt,...)                                      \
  sc_abort_verbosef (__FILE__, __LINE__, (fmt), __VA_ARGS__)
#define SC_CHECK_ABORTF(c,fmt,...)                      \
  ((c) ? (void) 0 : SC_ABORTF (fmt, __VA_ARGS__))
#endif

/* assertions, only enabled in debug mode */

#ifdef SC_DEBUG
#define SC_ASSERT(c) SC_CHECK_ABORT ((c), "Assertion '" #c "'")
#else
#define SC_ASSERT(c) SC_NOOP ()
#endif

/* macros for memory allocation, will abort if out of memory */

#define SC_ALLOC(t,n)         (t *) sc_malloc (sc_package_id, (n) * sizeof(t))
#define SC_ALLOC_ZERO(t,n)    (t *) sc_calloc (sc_package_id, \
                                               (size_t) (n), sizeof(t))
#define SC_REALLOC(p,t,n)     (t *) sc_realloc (sc_package_id,          \
                                             (p), (n) * sizeof(t))
#define SC_STRDUP(s)                sc_strdup (sc_package_id, (s))
#define SC_FREE(p)                  sc_free (sc_package_id, (p))

/**
 * Sets n elements of a memory range to zero.
 * Assumes the pointer p is of the correct type.
 */
#define SC_BZERO(p,n) ((void) memset ((p), 0, (n) * sizeof (*(p))))

/* min, max and square helper macros */

#define SC_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define SC_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define SC_SQR(a) ((a) * (a))

/* hopefully fast binary logarithms and binary round up */

#define SC_LOG2_8(x) (sc_log2_lookup_table[(x)])
#define SC_LOG2_16(x) (((x) > 0xff) ?                                   \
                       (SC_LOG2_8 ((x) >> 8) + 8) : SC_LOG2_8 (x))
#define SC_LOG2_32(x) (((x) > 0xffff) ?                                 \
                       (SC_LOG2_16 ((x) >> 16)) + 16 : SC_LOG2_16 (x))
#define SC_LOG2_64(x) (((x) > 0xffffffffLL) ?                           \
                       (SC_LOG2_32 ((x) >> 32)) + 32 : SC_LOG2_32 (x))
#define SC_ROUNDUP2_32(x)                               \
  (((x) <= 0) ? 0 : (1 << (SC_LOG2_32 ((x) - 1) + 1)))
#define SC_ROUNDUP2_64(x)                               \
  (((x) <= 0) ? 0 : (1 << (SC_LOG2_64 ((x) - 1LL) + 1)))

/* log categories */

#define SC_LC_GLOBAL      1     /* log only for master process */
#define SC_LC_NORMAL      2     /* log for every process */

/* log priorities */

#define SC_LP_DEFAULT   (-1)    /* this selects the SC default threshold */
#define SC_LP_ALWAYS      0     /* this will log everything */
#define SC_LP_TRACE       1     /* this will prefix file and line number */
#define SC_LP_DEBUG       2     /* any information on the internal state */
#define SC_LP_VERBOSE     3     /* information on conditions, decisions */
#define SC_LP_INFO        4     /* the main things a function is doing */
#define SC_LP_STATISTICS  5     /* important for consistency or performance */
#define SC_LP_PRODUCTION  6     /* a few lines for a major api function */
#define SC_LP_SILENT      7     /* this will never log anything */
#ifdef SC_LOG_PRIORITY
#define SC_LP_THRESHOLD SC_LOG_PRIORITY
#else
#ifdef SC_DEBUG
#define SC_LP_THRESHOLD SC_LP_TRACE
#else
#define SC_LP_THRESHOLD SC_LP_INFO
#endif
#endif

/* generic log macros */
#define SC_LOG(g,c,p,s) SC_LOGF((g), (c), (p), "%s", (s))
#define SC_GLOBAL_LOG(p,s) SC_LOG (sc_package_id, SC_LC_GLOBAL, (p), (s))
#define SC_NORMAL_LOG(p,s) SC_LOG (sc_package_id, SC_LC_NORMAL, (p), (s))
void                SC_LOGF (int package, int category, int priority,
                             const char *fmt, ...)
  __attribute__ ((format (printf, 4, 5)));
void                SC_GLOBAL_LOGF (int priority, const char *fmt, ...)
  __attribute__ ((format (printf, 2, 3)));
void                SC_NORMAL_LOGF (int priority, const char *fmt, ...)
  __attribute__ ((format (printf, 2, 3)));
#ifndef __cplusplus
#define SC_LOGF(package,category,priority,fmt,...)                      \
  ((priority) < SC_LP_THRESHOLD ? (void) 0 :                            \
   sc_logf (__FILE__, __LINE__, (package), (category), (priority),      \
            (fmt), __VA_ARGS__))
#define SC_GLOBAL_LOGF(p,f,...)                                 \
  SC_LOGF (sc_package_id, SC_LC_GLOBAL, (p), (f), __VA_ARGS__)
#define SC_NORMAL_LOGF(p,f,...)                                 \
  SC_LOGF (sc_package_id, SC_LC_NORMAL, (p), (f), __VA_ARGS__)
#endif

/* convenience global log macros will only output if identifier <= 0 */
#define SC_GLOBAL_TRACE(s) SC_GLOBAL_LOG (SC_LP_TRACE, (s))
#define SC_GLOBAL_LDEBUG(s) SC_GLOBAL_LOG (SC_LP_DEBUG, (s))
#define SC_GLOBAL_VERBOSE(s) SC_GLOBAL_LOG (SC_LP_VERBOSE, (s))
#define SC_GLOBAL_INFO(s) SC_GLOBAL_LOG (SC_LP_INFO, (s))
#define SC_GLOBAL_STATISTICS(s) SC_GLOBAL_LOG (SC_LP_STATISTICS, (s))
#define SC_GLOBAL_PRODUCTION(s) SC_GLOBAL_LOG (SC_LP_PRODUCTION, (s))
void                SC_GLOBAL_TRACEF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_GLOBAL_LDEBUGF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_GLOBAL_VERBOSEF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_GLOBAL_INFOF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_GLOBAL_STATISTICSF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_GLOBAL_PRODUCTIONF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
#ifndef __cplusplus
#define SC_GLOBAL_TRACEF(f,...)                         \
  SC_GLOBAL_LOGF (SC_LP_TRACE, (f), __VA_ARGS__)
#define SC_GLOBAL_LDEBUGF(f,...)                        \
  SC_GLOBAL_LOGF (SC_LP_DEBUG, (f), __VA_ARGS__)
#define SC_GLOBAL_VERBOSEF(f,...)                       \
  SC_GLOBAL_LOGF (SC_LP_VERBOSE, (f), __VA_ARGS__)
#define SC_GLOBAL_INFOF(f,...)                          \
  SC_GLOBAL_LOGF (SC_LP_INFO, (f), __VA_ARGS__)
#define SC_GLOBAL_STATISTICSF(f,...)                    \
  SC_GLOBAL_LOGF (SC_LP_STATISTICS, (f), __VA_ARGS__)
#define SC_GLOBAL_PRODUCTIONF(f,...)                    \
  SC_GLOBAL_LOGF (SC_LP_PRODUCTION, (f), __VA_ARGS__)
#endif

/* convenience log macros that output regardless of identifier */
#define SC_TRACE(s) SC_NORMAL_LOG (SC_LP_TRACE, (s))
#define SC_LDEBUG(s) SC_NORMAL_LOG (SC_LP_DEBUG, (s))
#define SC_VERBOSE(s) SC_NORMAL_LOG (SC_LP_VERBOSE, (s))
#define SC_INFO(s) SC_NORMAL_LOG (SC_LP_INFO, (s))
#define SC_STATISTICS(s) SC_NORMAL_LOG (SC_LP_STATISTICS, (s))
#define SC_PRODUCTION(s) SC_NORMAL_LOG (SC_LP_PRODUCTION, (s))
void                SC_TRACEF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_LDEBUGF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_VERBOSEF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_INFOF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_STATISTICSF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
void                SC_PRODUCTIONF (const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
#ifndef __cplusplus
#define SC_TRACEF(f,...)                                \
  SC_NORMAL_LOGF (SC_LP_TRACE, (f), __VA_ARGS__)
#define SC_LDEBUGF(f,...)                               \
  SC_NORMAL_LOGF (SC_LP_DEBUG, (f), __VA_ARGS__)
#define SC_VERBOSEF(f,...)                              \
  SC_NORMAL_LOGF (SC_LP_VERBOSE, (f), __VA_ARGS__)
#define SC_INFOF(f,...)                                 \
  SC_NORMAL_LOGF (SC_LP_INFO, (f), __VA_ARGS__)
#define SC_STATISTICSF(f,...)                           \
  SC_NORMAL_LOGF (SC_LP_STATISTICS, (f), __VA_ARGS__)
#define SC_PRODUCTIONF(f,...)                           \
  SC_NORMAL_LOGF (SC_LP_PRODUCTION, (f), __VA_ARGS__)
#endif

/* callback typedefs */

typedef void        (*sc_handler_t) (void *data);
typedef void        (*sc_log_handler_t) (FILE * log_stream,
                                         const char *filename, int lineno,
                                         int package, int category,
                                         int priority, const char *msg);

/* memory allocation functions, handle NULL pointers by internal abort
   the sc_realloc function does not preserve alignment boundaries */

void               *sc_malloc (int package, size_t size);
void               *sc_calloc (int package, size_t nmemb, size_t size);
void               *sc_realloc (int package, void *ptr, size_t size);
char               *sc_strdup (int package, const char *s);
void                sc_free (int package, void *ptr);
void                sc_memory_check (int package);

/* comparison functions for various integer sizes */

int                 sc_int_compare (const void *v1, const void *v2);
int                 sc_int16_compare (const void *v1, const void *v2);
int                 sc_int32_compare (const void *v1, const void *v2);
int                 sc_int64_compare (const void *v1, const void *v2);
int                 sc_double_compare (const void *v1, const void *v2);

/** Controls the default SC log behavior.
 * \param [in] log_stream    Set stream to use by sc_logf (or NULL for stdout).
 * \param [in] log_handler   Set default SC log handler (NULL selects builtin).
 * \param [in] log_threshold Set default SC log threshold (or SC_LP_DEFAULT).
 *                           May be SC_LP_ALWAYS or SC_LP_SILENT.
 */
void                sc_set_log_defaults (FILE * log_stream,
                                         sc_log_handler_t log_handler,
                                         int log_thresold);

/** The central log function to be called by all packages.
 * Dispatches the log calls by package and filters by category and priority.
 * \param [in] package   Must be a registered package id or -1.
 * \param [in] category  Must be SC_LC_NORMAL or SC_LC_GLOBAL.
 * \param [in] priority  Must be > SC_LP_ALWAYS and < SC_LP_SILENT.
 */
void                sc_log (const char *filename, int lineno,
                            int package, int category, int priority,
                            const char *msg);
void                sc_logf (const char *filename, int lineno,
                             int package, int category, int priority,
                             const char *fmt, ...)
  __attribute__ ((format (printf, 6, 7)));
void                sc_logv (const char *filename, int lineno,
                             int package, int category, int priority,
                             const char *fmt, va_list ap);

/** Generic MPI abort handler. The data must point to a MPI_Comm object. */
void                sc_generic_abort_handler (void *data);

/** Installs an abort handler and its callback data. */
void                sc_set_abort_handler (sc_handler_t handler, void *data);

/** Prints a stack trace, calls the abort handler and terminates. */
void                sc_abort (void)
  __attribute__ ((noreturn));

/** Print a message to stderr and then call sc_abort (). */
void                sc_abort_verbose (const char *filename, int lineno,
                                      const char *msg)
  __attribute__ ((noreturn));

/** Print a message to stderr and then call sc_abort (). */
void                sc_abort_verbosef (const char *filename, int lineno,
                                       const char *fmt, ...)
  __attribute__ ((format (printf, 3, 4)))
  __attribute__ ((noreturn));

/** Print a message to stderr and then call sc_abort (). */
void                sc_abort_verbosev (const char *filename, int lineno,
                                       const char *fmt, va_list ap)
  __attribute__ ((noreturn));

/** Register a software package with SC.
 * The logging parameters are as in sc_set_log_defaults.
 * \return                   Returns a unique package id.
 */
int                 sc_package_register (sc_log_handler_t log_handler,
                                         int log_threshold,
                                         const char *name, const char *full);
int                 sc_package_is_registered (int package_id);
void                sc_package_unregister (int package_id);

/** Print a summary of all packages registered with SC.
 * Uses the SC_LP_GLOBAL log category which by default only prints on rank 0.
 * \param [in] log_priority     Priority passed to sc log functions.
 */
void                sc_package_print_summary (int log_priority);

/** Sets the global program identifier (e.g. the MPI rank) and some flags.
 * This function is optional.
 * If this function is not called or called with log_handler == NULL,
 * the default SC log handler will be used.
 * If this function is not called or called with log_threshold == SC_LP_DEFAULT,
 * the default SC log threshold will be used.
 * The default SC log settings can be changed with sc_set_log_defaults ().
 * \param [in] mpicomm          MPI communicator, can be MPI_COMM_NULL.
 *                              If MPI_COMM_NULL, the identifier is set to -1.
 *                              Otherwise, MPI_Init must have been called
 *                              and the communicator is stored for later
 *                              use from within sc_generic_abort_handler.
 * \param [in] catch_signals    If true, signals INT SEGV USR2 are be caught.
 * \param [in] print_backtrace  If true, sc_abort prints a backtrace.
 */
void                sc_init (MPI_Comm mpicomm,
                             int catch_signals, int print_backtrace,
                             sc_log_handler_t log_handler, int log_threshold);

/** Unregisters all packages, runs the memory check, removes the
 * abort and signal handlers and resets sc_identifier and sc_root_*.
 * This function is optional.
 * This function does not require sc_init to be called first.
 */
void                sc_finalize (void);

SC_EXTERN_C_END;

#endif /* SC_H */

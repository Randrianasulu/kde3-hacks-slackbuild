/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if you have the <baudboy.h> header file. */
/* #undef HAVE_BAUDBOY_H */

/* Define to 1 if you have the <Carbon/Carbon.h> header file. */
/* #undef HAVE_CARBON_CARBON_H */

/* Define if you have the CoreAudio API */
/* #undef HAVE_COREAUDIO */

/* Define to 1 if you have the <crt_externs.h> header file. */
/* #undef HAVE_CRT_EXTERNS_H */

/* Defines if your system has the crypt function */
#define HAVE_CRYPT 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have libjpeg */
#define HAVE_LIBJPEG 1

/* Define if you have libpng */
#define HAVE_LIBPNG 1

/* Define if you have a working libpthread (will enable threaded code) */
#define HAVE_LIBPTHREAD 1

/* Define if you have libz */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <lockdev.h> header file. */
/* #undef HAVE_LOCKDEV_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if your system needs _NSGetEnviron to set up the environment */
/* #undef HAVE_NSGETENVIRON */

/* Defined if Bluetooth is enabled. */
/* #undef HAVE_QOBEX_BLUETOOTH */

/* Defined if IrDA is enabled. */
#define HAVE_QOBEX_IRDA 1

/* Define if you have res_init */
#define HAVE_RES_INIT 1

/* Define if you have the res_init prototype */
#define HAVE_RES_INIT_PROTO 1

/* Define if you have a STL implementation by SGI */
#define HAVE_SGI_STL 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have strlcat */
/* #undef HAVE_STRLCAT */

/* Define if you have the strlcat prototype */
/* #undef HAVE_STRLCAT_PROTO */

/* Define if you have strlcpy */
/* #undef HAVE_STRLCPY */

/* Define if you have the strlcpy prototype */
/* #undef HAVE_STRLCPY_PROTO */

/* Define to 1 if you have the <sys/bitypes.h> header file. */
#define HAVE_SYS_BITYPES_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* version of the kdebluetooth package */
#define KDEBLUETOOTH_VERSION "1.00-beta8"

/* Suffix for lib directories */
#define KDELIBSUFF ""

/* Define a safe value for MAXPATHLEN */
#define KDEMAXPATHLEN 4096

/* Name of package */
#define PACKAGE "kdebluetooth-1.0~beta9~r769275"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* The size of `char *', as computed by sizeof. */
#define SIZEOF_CHAR_P 4

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* If we use arts volume */
#define USE_ARTS 1

/* Version number of package */
#define VERSION "3.5.7"

/* Defined if compiling without arts */
/* #undef WITHOUT_ARTS */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/*
 * jpeg.h needs HAVE_BOOLEAN, when the system uses boolean in system
 * headers and I'm too lazy to write a configure test as long as only
 * unixware is related
 */
#ifdef _UNIXWARE
#define HAVE_BOOLEAN
#endif



/*
 * AIX defines FD_SET in terms of bzero, but fails to include <strings.h>
 * that defines bzero.
 */

#if defined(_AIX)
#include <strings.h>
#endif



#if defined(HAVE_NSGETENVIRON) && defined(HAVE_CRT_EXTERNS_H)
# include <sys/time.h>
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#endif



#if !defined(HAVE_RES_INIT_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
int res_init(void);
#ifdef __cplusplus
}
#endif
#endif



#if !defined(HAVE_STRLCAT_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
unsigned long strlcat(char*, const char*, unsigned long);
#ifdef __cplusplus
}
#endif
#endif



#if !defined(HAVE_STRLCPY_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
unsigned long strlcpy(char*, const char*, unsigned long);
#ifdef __cplusplus
}
#endif
#endif



/*
 * On HP-UX, the declaration of vsnprintf() is needed every time !
 */

#if !defined(HAVE_VSNPRINTF) || defined(hpux)
#if __STDC__
#include <stdarg.h>
#include <stdlib.h>
#else
#include <varargs.h>
#endif
#ifdef __cplusplus
extern "C"
#endif
int vsnprintf(char *str, size_t n, char const *fmt, va_list ap);
#ifdef __cplusplus
extern "C"
#endif
int snprintf(char *str, size_t n, char const *fmt, ...);
#endif



#if defined(__SVR4) && !defined(__svr4__)
#define __svr4__ 1
#endif


/* type to use in place of socklen_t if not defined */
#define kde_socklen_t socklen_t

/* type to use in place of socklen_t if not defined (deprecated, use
   kde_socklen_t) */
#define ksize_t socklen_t

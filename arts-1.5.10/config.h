/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define if you have alsa 1.x */
#define ALSA_PCM_OLD_HW_PARAMS_API 1

/* Define if you have alsa 1.x */
#define ALSA_PCM_OLD_SW_PARAMS_API 1

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Size of pth_cond_t */
#define GLIB_SIZEOF_PTH_COND_T 48

/* Size of pth_mutex_t */
#define GLIB_SIZEOF_PTH_MUTEX_T 24

/* Size of std_intmax_t */
#define GLIB_SIZEOF_STD_INTMAX_T 8

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <alsa/asoundlib.h> header file. */
#define HAVE_ALSA_ASOUNDLIB_H 1

/* Define if you want to use glibc facilities to emulate stdio accesses in
   artsdsp */
#define HAVE_ARTSDSP_STDIO_EMU 1

/* Define to 1 if you have the `bcopy' function. */
/* #undef HAVE_BCOPY */

/* You _must_ have bool */
#define HAVE_BOOL 1

/* Define to 1 if you have the <Carbon/Carbon.h> header file. */
/* #undef HAVE_CARBON_CARBON_H */

/* Define if you have the CoreAudio API */
/* #undef HAVE_COREAUDIO */

/* Define to 1 if you have the <crt_externs.h> header file. */
/* #undef HAVE_CRT_EXTERNS_H */

/* Defines if your system has the crypt function */
#define HAVE_CRYPT 1

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define if you have the GNU dld library. */
/* #undef HAVE_DLD */

/* Define to 1 if you have the <dld.h> header file. */
/* #undef HAVE_DLD_H */

/* Define to 1 if you have the `dlerror' function. */
#define HAVE_DLERROR 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <dl.h> header file. */
/* #undef HAVE_DL_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <fnmatch.h> header file. */
#define HAVE_FNMATCH_H 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the <fstab.h> header file. */
#define HAVE_FSTAB_H 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define if you have getdomainname */
#define HAVE_GETDOMAINNAME 1

/* Define if you have the getdomainname prototype */
#define HAVE_GETDOMAINNAME_PROTO 1

/* Define if you have gethostname */
#define HAVE_GETHOSTNAME 1

/* Define if you have the gethostname prototype */
#define HAVE_GETHOSTNAME_PROTO 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `index' function. */
/* #undef HAVE_INDEX */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if ioctl is declared as int ioctl(int d, int request,...) */
/* #undef HAVE_IOCTL_INT_INT_DOTS */

/* Define if ioctl is declared as int ioctl(int d, unsigned long int
   request,...) */
#define HAVE_IOCTL_INT_ULONGINT_DOTS 3

/* Define if ioctl is declared as int ioctl(int d, unsigned long request,...)
   */
/* #undef HAVE_IOCTL_INT_ULONG_DOTS */

/* Define if you compile under IRIX */
/* #undef HAVE_IRIX */

/* Define if you have libasound.so.1 (required for ALSA 0.5.x support) */
/* #undef HAVE_LIBASOUND */

/* Define if you have libasound.so.2 (required for ALSA 0.9.x/1.x support) */
#define HAVE_LIBASOUND2 1

/* Define if you have libaudiofile (required for playing wavs with aRts) */
#define HAVE_LIBAUDIOFILE 1

/* Define if you have libaudioIO (required if you want to have libaudioio
   support) */
/* #undef HAVE_LIBAUDIOIO */

/* Define if you have libaudio (required if you want to have NAS support) */
/* #undef HAVE_LIBAUDIONAS */

/* Define if you have libcsl (required if you want to have CSL support) */
/* #undef HAVE_LIBCSL */

/* Define if you have the libdl library or equivalent. */
#define HAVE_LIBDL 1

/* Define if you have libesd (required if you want EsounD support) */
#define HAVE_LIBESD 1

/* Define if you have libjack (required if you want Jack support) */
#define HAVE_LIBJACK 1

/* Define if you have libjpeg */
#define HAVE_LIBJPEG 1

/* Define if you have libmas (required if you want MAS support) */
/* #undef HAVE_LIBMAS */

/* Define if you have libpng */
#define HAVE_LIBPNG 1

/* Define if you have a working libpthread (will enable threaded code) */
#define HAVE_LIBPTHREAD 1

/* Define if you have libz */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <linux/socket.h> header file. */
#define HAVE_LINUX_SOCKET_H 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the <mntent.h> header file. */
#define HAVE_MNTENT_H 1

/* Define to 1 if you have the <monetary.h> header file. */
#define HAVE_MONETARY_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define if your system needs _NSGetEnviron to set up the environment */
/* #undef HAVE_NSGETENVIRON */

/* Define to 1 if you have the <paths.h> header file. */
#define HAVE_PATHS_H 1

/* Define if libtool can extract symbol lists from object files. */
#define HAVE_PRELOADED_SYMBOLS 1

/* Define if you have random */
#define HAVE_RANDOM 1

/* Define if you have the random prototype */
#define HAVE_RANDOM_PROTO 1

/* Define if your system supports realtime scheduling */
#define HAVE_REALTIME_SCHED 1

/* Define if you have res_init */
#define HAVE_RES_INIT 1

/* Define if you have the res_init prototype */
#define HAVE_RES_INIT_PROTO 1

/* Define to 1 if you have the `rindex' function. */
/* #undef HAVE_RINDEX */

/* Define to 1 if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define if you have setenv */
#define HAVE_SETENV 1

/* Define if you have the setenv prototype */
#define HAVE_SETENV_PROTO 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define to 1 if you have the `setmntent' function. */
#define HAVE_SETMNTENT 1

/* Define if you have libaudio (required for sound i/o on IRIX) */
/* #undef HAVE_SGILIBAUDIO */

/* Define if you have a STL implementation by SGI */
#define HAVE_SGI_STL 1

/* Define if you have the shl_load function. */
/* #undef HAVE_SHL_LOAD */

/* Define to 1 if you have the <sigaction.h> header file. */
/* #undef HAVE_SIGACTION_H */

/* Define if libasound has snd_pcm_resume() */
#define HAVE_SND_PCM_RESUME 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <socketbits.h> header file. */
/* #undef HAVE_SOCKETBITS_H */

/* Define to 1 if you have the <soundcard.h> header file. */
/* #undef HAVE_SOUNDCARD_H */

/* Define if you have srandom */
#define HAVE_SRANDOM 1

/* Define if you have the srandom prototype */
#define HAVE_SRANDOM_PROTO 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strcmp' function. */
#define HAVE_STRCMP 1

/* Define to 1 if you have the `strfmon' function. */
#define HAVE_STRFMON 1

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

/* Define to 1 if you have the `strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the <sysent.h> header file. */
/* #undef HAVE_SYSENT_H */

/* Define to 1 if you have the <sys/asoundlib.h> header file. */
#define HAVE_SYS_ASOUNDLIB_H 1

/* Define to 1 if you have the <sys/bitypes.h> header file. */
#define HAVE_SYS_BITYPES_H 1

/* Define to 1 if you have the <sys/cdefs.h> header file. */
#define HAVE_SYS_CDEFS_H 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/mnttab.h> header file. */
/* #undef HAVE_SYS_MNTTAB_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if sys/stat.h declares S_ISSOCK. */
#define HAVE_S_ISSOCK 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have usleep */
#define HAVE_USLEEP 1

/* Define if you have the usleep prototype */
#define HAVE_USLEEP_PROTO 1

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define to 1 if you have the <X11/ICE/ICElib.h> header file. */
#define HAVE_X11_ICE_ICELIB_H 1

/* Define if you want to use optimized x86 float to int conversion code */
#define HAVE_X86_FLOAT_INT 1

/* Define if your assembler supports x86 SSE instructions */
#define HAVE_X86_SSE 1

/* The prefix to use as fallback */
#define KDEDIR "/usr"

/* Suffix for lib directories */
#define KDELIBSUFF ""

/* Define if the OS needs help to load dependent libraries for dlopen(). */
/* #undef LTDL_DLOPEN_DEPLIBS */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LTDL_OBJDIR ".libs/"

/* Define to the name of the environment variable that determines the dynamic
   library search path. */
#define LTDL_SHLIBPATH_VAR "LD_LIBRARY_PATH"

/* Define to the extension used for shared libraries, say, ".so". */
/* #undef LTDL_SHLIB_EXT */

/* Define to the system default library search path. */
#define LTDL_SYSSEARCHPATH "/lib:/usr/lib"

/* Define if dlsym() requires a leading underscode in symbol names. */
/* #undef NEED_USCORE */

/* Name of package */
#define PACKAGE "arts"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
/* #undef PACKAGE_URL */

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

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "1.5.10"

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



#if !defined(HAVE_GETDOMAINNAME_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
		int getdomainname (char *, size_t);
#ifdef __cplusplus
}
#endif
#endif



#if !defined(HAVE_GETHOSTNAME_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
int gethostname (char *, unsigned int);
#ifdef __cplusplus
}
#endif
#endif



#if !defined(HAVE_RANDOM_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
long int random(void);
#ifdef __cplusplus
}
#endif
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



#if !defined(HAVE_SETENV_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
int setenv (const char *, const char *, int);
#ifdef __cplusplus
}
#endif
#endif



#if !defined(HAVE_SRANDOM_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
void srandom(unsigned int);
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



#if !defined(HAVE_USLEEP_PROTO)
#ifdef __cplusplus
extern "C" {
#endif
int usleep (unsigned int);
#ifdef __cplusplus
}
#endif
#endif



#ifndef HAVE_S_ISSOCK
#define HAVE_S_ISSOCK
#define S_ISSOCK(mode) (1==0)
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


/* define to 1 if -fvisibility is supported */
/* #undef __KDE_HAVE_GCC_VISIBILITY */


#if defined(__SVR4) && !defined(__svr4__)
#define __svr4__ 1
#endif


/* type to use in place of socklen_t if not defined */
#define kde_socklen_t socklen_t

/* type to use in place of socklen_t if not defined (deprecated, use
   kde_socklen_t) */
#define ksize_t socklen_t

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

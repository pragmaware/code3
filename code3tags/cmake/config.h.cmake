/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define this label if your system uses case-insensitive file names */
#cmakedefine CASE_INSENSITIVE_FILENAMES

/* Define this label if you wish to check the regcomp() function at run time
   for correct behavior. This function is currently broken on Cygwin. */
#cmakedefine CHECK_REGCOMP

/* You can define this label to be a string containing the name of a
   site-specific configuration file containing site-wide default options. The
   files /etc/ctags.conf and /usr/local/etc/ctags.conf are already checked, so
   only define one here if you need a file somewhere else. */
#cmakedefine CUSTOM_CONFIGURATION_FILE


/* Define this as desired.
 * 1:  Original ctags format
 * 2:  Extended ctags format with extension flags in EX-style comment.
 */
#cmakedefine DEFAULT_FILE_FORMAT	2



/* Define this label to use the system sort utility (which is probably more
*  efficient) over the internal sorting algorithm.
*/
#ifndef INTERNAL_SORT
# undef EXTERNAL_SORT
#endif


/* Define to 1 if you have the `chmod' function. */
#cmakedefine HAVE_CHMOD

/* Define to 1 if you have the `chsize' function. */
#cmakedefine HAVE_CHSIZE

/* Define to 1 if you have the `clock' function. */
#cmakedefine HAVE_CLOCK

/* Define to 1 if you have the <dirent.h> header file. */
#cmakedefine HAVE_DIRENT_H

/* Define to 1 if you have the <direct.h> header file. */
#cmakedefine HAVE_DIRECT_H

/* Define to 1 if you have the <errno.h> header file. */
#cmakedefine HAVE_ERRNO_H

/* Define to 1 if you have the <fcntl.h> header file. */
#cmakedefine HAVE_FCNTL_H

/* Define to 1 if you have the `fgetpos' function. */
#cmakedefine HAVE_FGETPOS

/* Define to 1 if you have the `findfirst' function. */
#cmakedefine HAVE_FINDFIRST

/* Define to 1 if you have the `fnmatch' function. */
#cmakedefine HAVE_FNMATCH

/* Define to 1 if you have the <fnmatch.h> header file. */
#cmakedefine HAVE_FNMATCH_H

/* Define to 1 if you have the `ftruncate' function. */
#cmakedefine HAVE_FTRUNCATE

/* Define to 1 if you have the <iconv.h> header file. */
#cmakedefine HAVE_ICONV_H

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the <io.h> header file. */
#cmakedefine HAVE_IO_H

/* Define to 1 if you have the <limits.h> header file. */
#cmakedefine HAVE_LIMITS_H

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H

/* Define to 1 if you have the `mkstemp' function. */
#cmakedefine HAVE_MKSTEMP

/* Define to 1 if you have the `opendir' function. */
#cmakedefine HAVE_OPENDIR

/* Define to 1 if you have the `putenv' function. */
#cmakedefine HAVE_PUTENV

/* Define to 1 if you have the `regcomp' function. */
#cmakedefine HAVE_REGCOMP

/* Define to 1 if you have the `remove' function. */
#cmakedefine HAVE_REMOVE

/* Define to 1 if you have the `setenv' function. */
#cmakedefine HAVE_SETENV

/* Define to 1 if you have the <stat.h> header file. */
#cmakedefine HAVE_STAT_H

/* Define this macro if the field "st_ino" exists in struct stat in
   <sys/stat.h>. */
#cmakedefine HAVE_STAT_ST_INO

/* Define to 1 if you have the <stdbool.h> header file. */
#cmakedefine HAVE_STDBOOL_H

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H

/* Define to 1 if you have the `strcasecmp' function. */
#cmakedefine HAVE_STRCASECMP

/* Define to 1 if you have the `strerror' function. */
#cmakedefine HAVE_STRERROR

/* Define to 1 if you have the `stricmp' function. */
#cmakedefine HAVE_STRICMP

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H

/* Define to 1 if you have the `strncasecmp' function. */
#cmakedefine HAVE_STRNCASECMP

/* Define to 1 if you have the `strnicmp' function. */
#cmakedefine HAVE_STRNICMP

/* Define to 1 if you have the `strstr' function. */
#cmakedefine HAVE_STRSTR

/* Define to 1 if you have the <sys/dir.h> header file. */
#cmakedefine HAVE_SYS_DIR_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/times.h> header file. */
#cmakedefine HAVE_SYS_TIMES_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* Define to 1 if you have the `tempnam' function. */
#cmakedefine HAVE_TEMPNAM

/* Define to 1 if you have the `times' function. */
#cmakedefine HAVE_TIMES

/* Define to 1 if you have the <time.h> header file. */
#cmakedefine HAVE_TIME_H

/* Define to 1 if you have the `truncate' function. */
#cmakedefine HAVE_TRUNCATE

/* Define to 1 if you have the <types.h> header file. */
#cmakedefine HAVE_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H

/* Define to 1 if you have the `_findfirst' function. */
#cmakedefine HAVE__FINDFIRST

/* Define as the maximum integer on your system if not defined <limits.h>. */
#cmakedefine INT_MAX

/* Define to the appropriate size for tmpnam() if <stdio.h> does not define
   this. */
#cmakedefine L_tmpnam

/* Define this label if you want macro tags (defined lables) to use patterns
   in the EX command by default (original ctags behavior is to use line
   numbers). */
#cmakedefine MACROS_USE_PATTERNS

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_FGETPOS

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_FTRUNCATE

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_GETENV

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_LSTAT

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_MALLOC

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_REMOVE

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_STAT

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_TRUNCATE

/* If you receive error or warning messages indicating that you are missing a
   prototype for, or a type mismatch using, the following function, define
   this label and remake. */
#cmakedefine NEED_PROTO_UNLINK

/* Define this is you have a prototype for putenv() in <stdlib.h>, but doesn't
   declare its argument as "const char *". */
#cmakedefine NON_CONST_PUTENV_PROTOTYPE

/* Package name. */
#define PACKAGE ""

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define this label if regcomp() is broken. */
#cmakedefine REGCOMP_BROKEN

/* Define this value used by fseek() appropriately if <stdio.h> (or <unistd.h>
   on SunOS 4.1.x) does not define them. */
#cmakedefine SEEK_SET

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define this label if your system supports starting scripts with a line of
   the form "#! /bin/sh" to select the interpreter to use for the script. */
#define SYS_INTERPRETER 1

/* If you wish to change the directory in which temporary files are stored,
   define this label to the directory desired. */
#undef TMPDIR

/* Package version. */
#define VERSION "5.0.0"

/* This corrects the problem of missing prototypes for certain functions in
   some GNU installations (e.g. SunOS 4.1.x). */
#cmakedefine __USE_FIXED_PROTOTYPES__

/* Define to the appropriate type if <time.h> does not define this. */
#cmakedefine clock_t

/* Define to empty if `const' does not conform to ANSI C. */
#cmakedefine const

/* Define to long if <stdio.h> does not define this. */
#cmakedefine fpos_t

/* Define to `long int' if <sys/types.h> does not define. */
#cmakedefine off_t

/* Define remove to unlink if you have unlink(), but not remove(). */
#cmakedefine remove

/* Define to `unsigned int' if <sys/types.h> does not define. */
#cmakedefine size_t

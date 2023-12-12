#ifndef MY_HEADER_H
#define MY_HEADER_H

/*
 * Header files in C are included prior to compilation and contain declarations,
 * defines, and data needed by multiple programs. Organize them functionally,
 * separating subsystems into different header files. Declarations likely to change
 * during porting should be in a separate header.
 */

/*
 * Avoid private header filenames matching library header filenames.
 * Use #include "math.h" with a comment to clarify if the standard library header
 * is intended. Avoid absolute pathnames; use <name> or define relative paths.
 * The -I option in the C compiler is ideal for extensive private header libraries.
 */

/*
 * Include headers declaring functions or external variables in the file that
 * defines them. This ensures type checking and consistency between declaration
 * and definition.
 */

/*
 * Defining variables in header files is discouraged due to poor code partitioning.
 * It can lead to compilation issues, especially with typedefs and initialized data
 * definitions. Header files should not be nested. Prologue should mention
 * necessary #includes for functionality. In some cases, a common include file may
 * be used for multiple source files.
 */

/*
 * Use the double-inclusion guard to prevent accidental double-inclusion of
 * header files. The standard format is:
 *   #ifndef EXAMPLE_H
 *   #define EXAMPLE_H
 *   ...  body of example.h file 
 *   #endif

 * Avoid relying on this mechanism for nested includes.
 */

#endif /* MY_HEADER_H */

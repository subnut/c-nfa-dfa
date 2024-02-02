#define _POSIX_C_SOURCE 200809L
#ifndef MACROS_H
#define MACROS_H

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * errno.h	- errno
 * stdint.h	- SIZE_MAX
 * stdio.h	- stderr, fprintf
 * stdlib.h	- exit, EXIT_FAILURE, malloc, realloc
 * string.h	- strerror, memset
 */

#define ensure(cond, msg) \
	if (!(cond)) \
	exit((fprintf(stderr, "[%s:%d] %s\n", \
			__FILE__, __LINE__, msg), \
			EXIT_FAILURE))

#define perror(msg) \
	exit((fprintf(stderr, "[%s:%d] %s (%s)\n", \
			__FILE__, __LINE__, msg, \
			strerror(errno)), EXIT_FAILURE))

/* perror if */
#define perrif(cond, msg) \
	if (cond) perror(msg)



#define add(var, val) { \
	typeof(var) x = var + val; \
	ensure(val > 0 ? x > var : x < var, "Overflow"); \
	var = x; }



#define alloc(p, siz) \
	p = malloc(siz); \
	perrif(p == NULL, "Could not allocate memory")

/*
 * NOTE
 * ====
 *
 * We wont't use calloc or bzero because setting the memory to zero
 * doesn't necessarily mean that pointers shall be initialized to NULL.
 *
 * The representation of NULL may vary depending on the underlying
 * architecture, and it might not always be all-zero-bits.
 *
 * See: https://stackoverflow.com/a/25212029
 *
 */
#define new(var, T) \
	T *var; alloc(var, sizeof(T)); \
	*var = (T){0};	// Initialize everything to zero-value


#define reallocarr(ptr, n) {\
	size_t nmemb = (n); \
	ptr = reallocarray(ptr, nmemb, sizeof(typeof(*ptr))); \
	perrif(ptr == NULL, "Could not allocate memory"); }

// s1 * s2 <= SIZE_MAX iff both s1 and s2 < sqrt(SIZE_MAX+1)
#define reallocarray(ptr, nmemb, size) \
	(nmemb == 0 ? realloc(ptr, 0) : \
		((nmemb >= ((size_t)1 << (sizeof(size_t) * 4)) \
		  || size >= ((size_t)1 << (sizeof(size_t) * 4))) \
		 && nmemb > 0 && SIZE_MAX / nmemb < size) \
		 ? (errno = ENOMEM), NULL \
		 : realloc(ptr, size * nmemb))

#endif /* MACROS_H */

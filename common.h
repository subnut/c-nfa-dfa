#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * errno.h	- errno
 * stdio.h	- stderr, fprintf
 * stdlib.h	- exit, EXIT_FAILURE
 * stdint.h	- SIZE_MAX
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
	perrif(p == NULL, "Could not allocate memory");

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
	T *var; alloc(var, sizeof(T)) \
	*var = (T){0};	// Initialize everything to zero-value


void *reallocarray(void *, size_t, size_t);
#define reallocarr(p, n) \
	p = reallocarray(p, n, sizeof(typeof(*p))); \
	perrif(p == NULL, "Could not allocate memory")

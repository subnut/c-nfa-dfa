#include "common.h"
#include "dfa.h"
#include "nfa.h"

#include <limits.h>
#include <stdbool.h>
#include <string.h>

/*
 * limits.h	- CHAR_BIT
 * stdbool.h	- bool, true, false
 * string.h	- memset
 */

static void
hash(const bool *state, unsigned char *buf, Nstatenum statecount)
{
	memset(buf, 0, statecount / CHAR_BIT + 1);
	for (Nstatenum i = 0; i < statecount; i++)
		buf[i / CHAR_BIT] |= state[i]
			? 1 << (CHAR_BIT * (i / CHAR_BIT + 1) - i - 1)
			: 0;
}

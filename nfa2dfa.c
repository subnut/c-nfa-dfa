#include "common.h"
#include "dfa.h"
#include "nfa.h"

#include <limits.h>
#include <stdbool.h>
#include <string.h>

/*
 * limits.h	- CHAR_BIT
 * stdbool.h	- bool, true, false
 * string.h	- memset, memcpy
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

static void
setEclosure(NFA *nfa, bool *states)
{
	for (bool changed = true; changed; changed = false)
		for (Nstatenum i = 0; i < nfa->statecount; i++)
			if (states[i])
				for (Nstatenum j = 0; j < nfa->states[i]->etnum; j++)
					if (states[nfa->states[i]->etrans[j]] == false)
						states[nfa->states[i]->etrans[j]] = changed = true;

}

static void
move(NFA *nfa, bool *states, int input)
{
	bool buf[nfa->statecount];
	memcpy(buf, states, sizeof(buf));
	for (Nstatenum i = 0; i < nfa->statecount; i++)
		states[i] = false;
	for (Nstatenum i = 0; i < nfa->statecount; i++)
		if (buf[i])
			for (Nstatenum j = 0; j < nfa->states[i]->tnum[input]; j++)
				states[nfa->states[i]->trans[input][j]] = true;
}

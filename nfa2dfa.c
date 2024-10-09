#include "common.h"
#include "macros.h"
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

/*
 * Why?
 *
 * Because, according to C99, unsigned char is the only integer type that is
 * guaranteed to have no padding whatsoever; also, its object representation
 * is the exact same as its pure binary representation.
 */
typedef
unsigned char hash_t;

static void
hash(const bool *state, hash_t *buf, Nstatenum statecount)
{
	memset(buf, 0, statecount / CHAR_BIT + 1);
	for (Nstatenum i = 0; i < statecount; i++)
		buf[i / CHAR_BIT] |= state[i]
			? 1 << (CHAR_BIT * (i / CHAR_BIT + 1) - i - 1)
			: 0;
}

static void
unhash(const hash_t *hash, bool *statebuf, Nstatenum statecount)
{
	memset(statebuf, false, statecount);
	for (Nstatenum i = 0; i < statecount; i++)
		statebuf[i] = hash[i / CHAR_BIT]
				& 1 << (CHAR_BIT * (i / CHAR_BIT + 1) - i - 1)
				? true : false;
}

static void
setEclosure(NFA *nfa, bool *states)
{
	/*
	 * TODO:
	 * Compute ϵ-closure using the stack method shown in fig. 3.33
	 * in Compilers Vol.2 by Alfred Aho (purple dragon book)
	 */
	bool changed; do {
		changed = false;
		for (Nstatenum i = 0; i < nfa->statecount; i++)
			if (states[i])
				for (Nstatenum j = 0; j < nfa->states[i]->etnum; j++)
					if (states[nfa->states[i]->etrans[j]] == false)
						states[nfa->states[i]->etrans[j]] = changed = true;
	} while(changed);
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

DFA *
nfa2dfa(NFA *nfa)
#define HASHLEN (nfa->statecount / CHAR_BIT + 1)
{
	NFA N = *nfa;

	hash_t *hashbuf = calloc(HASHLEN, sizeof(hash_t));
	perrif(hashbuf == NULL, "Could not allocate memory");

	bool *statebuf = calloc(N.statecount, sizeof(bool));
	perrif(statebuf == NULL, "Could not allocate memory");

	/* Initialize dstates */
	struct Dstate {
		bool marked;
		hash_t *hash;
		struct Dstate *trans[ALSIZ];
	};
	size_t dscount = 0;
	struct Dstate *dstates = NULL;

#define dsc (dscount - 1)

	/* First state (unmarked) */
	reallocarr(dstates, ++dscount);		// allocate
	dstates[dsc] = (struct Dstate){0};	// initialize
	statebuf[N.start] = true;
	setEclosure(nfa, statebuf);
	hash(statebuf, hashbuf, N.statecount);
	reallocarr(dstates[dsc].hash, HASHLEN);
	memcpy(dstates[dsc].hash, hashbuf, HASHLEN);
	dstates[dsc].marked = false;

	// reset buffers
	memset(hashbuf, 0, HASHLEN);
	memset(statebuf, false, N.statecount);

#define bufreset() {\
	memset(hashbuf, 0, HASHLEN);\
	memset(statebuf, false, N.statecount);}

	/* main loop */
	bool unmarked = true;
	while (unmarked) {
		unmarked = false;
		for (size_t i = 0; i < dscount; i++)
			if (!dstates[i].marked) {
				unmarked = dstates[i].marked = true;
				for (size_t c = 0; c < ALSIZ; c++)
				{
					if (!N.alphabet[c])
						continue;

					bufreset();
					unhash(dstates[i].hash, statebuf, N.statecount);
					move(nfa, statebuf, c);
					setEclosure(nfa, statebuf);
					hash(statebuf, hashbuf, N.statecount);

					/* check if this state is in Dstates */
					bool exists = false;
					for (size_t ii = 0; ii < dscount; ii++)
						if (0 == memcmp(dstates[ii].hash, hashbuf, HASHLEN)) {
							exists = true;
							break;
						}
					if (!exists) {
						reallocarr(dstates, ++dscount);		// allocate
						dstates[dsc] = (struct Dstate){0};	// initialize
						reallocarr(dstates[dsc].hash, HASHLEN);
						memcpy(dstates[dsc].hash, hashbuf, HASHLEN);
						dstates[dsc].marked = false;
					}
					dstates[i].trans[c] = &dstates[dsc];
				}
			}
	}


	// free dstates
	for (size_t i = 0; i < dscount; i++)
		free(dstates[i].hash);
	free(dstates);

	// free buffers
	free(statebuf);
	free(hashbuf);

	return NULL;
}

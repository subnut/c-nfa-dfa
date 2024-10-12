#include "common.h"
#include "macros.h"
#include "dfa.h"
#include "nfa.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * limits.h	- CHAR_BIT
 * stdbool.h	- bool, true, false
 * stdlib.h	- calloc
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

	/*
	 * TODO
	 * Use some kind of hashmap (preferably red-black tree) for `dstates`
	 * instead of using just an array.
	 *
	 * Keep in mind that there should be a way to iterate over all the
	 * elements of the map, as we shall need to do so when creating the
	 * final DFA.
	 */

	/* Initialize dstates */
	struct dstate {
		bool marked;
		bool endstate;
		hash_t *hash;
		size_t trans[ALSIZ];
	};
	size_t dscount = 1;	// zero-th state
	struct dstate *dstates = calloc(++dscount, sizeof *dstates);
	perrif(NULL == dstates, "Could not allocate memory");
	dstates[0] = (struct dstate) {
		.marked = true,		// skip this state in the main loop below
		.endstate = true,	// emulator should stop when it reaches this state
		.hash = NULL,
		.trans = {0},
	};

	// dsc is the index of the last-added element
#define dsc (dscount - 1)

	/* First state (unmarked) */
	dstates[dsc] = (struct dstate) {
		.marked = false,
		.endstate = statebuf[N.end],
		.hash = calloc(HASHLEN, sizeof (hash_t)),
		.trans = {0},
	};
	perrif(NULL == dstates[dsc].hash, "Could not allocate memory");
	statebuf[N.start] = true;
	setEclosure(nfa, statebuf);
	hash(statebuf, hashbuf, N.statecount);
	memcpy(dstates[dsc].hash, hashbuf, HASHLEN);

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
		for (size_t i = 0; i < dscount; i++) {
			if (dstates[i].marked) continue;
			dstates[i].marked = unmarked = true;
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
				for (size_t ii = 1; ii < dscount; ii++)	// ignore the zero-th state
					if (0 == memcmp(dstates[ii].hash, hashbuf, HASHLEN)) {
						// Found. Add to Dtrans.
						dstates[i].trans[c] = ii;
						exists = true;
						break;
					}
				if (!exists) {
					/* state not in Dstates. add. */
					ensure(++dscount > 0, "Overflow");
					dstates = reallocarray(dstates, dscount, sizeof *dstates);
					dstates[dsc] = (struct dstate) {
						.marked = false,
						.endstate = statebuf[N.end],
						.hash = calloc(HASHLEN, sizeof (hash_t)),
						.trans = {0},
					};
					perrif(NULL == dstates[dsc].hash, "Could not allocate memory");
					memcpy(dstates[dsc].hash, hashbuf, HASHLEN);
					// Add to Dtrans.
					dstates[i].trans[c] = dsc;
				}
			}
		}
	}
#undef dsc
#undef bufreset

	DFA *dfa = newDFA(dscount);
	DFA D = *dfa;

	for (size_t i = 0; i < dscount; i++) {
		D.states[i].isendstate = dstates[i].endstate;
		for (size_t j = 0; j < ALSIZ; j++)
			D.states[i].dtrans[j] = dstates[i].trans[j];
	}

	// free dstates
	for (size_t i = 1; i < dscount; i++)	// zero-th state don't need free-ing
		free(dstates[i].hash);
	free(dstates);

	// free buffers
	free(statebuf);
	free(hashbuf);

	return dfa;
}
#undef HASHLEN

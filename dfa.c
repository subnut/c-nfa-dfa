#include "dfa.h"
#include "macros.h"

#include <stdlib.h>

/*
 * stdlib.h	- calloc
 */

DFA *
newDFA(Dstatenum statenum)
{
	new(dfa, DFA);

	dfa->arrsiz = statenum;
	add(dfa->arrsiz, 1);	// for the zero-th (INVALID) state

	dfa->states = calloc(dfa->arrsiz, sizeof *dfa->states);
	perrif(dfa->states == NULL, "Could not allocate memory");

#if 0
	// Should we initialize everything?
	// or should we leave it for the caller to initialize?
	for (Dstatenum i = 0; i < dfa->arrsiz; i++)
		dfa->states[i] = (Dstate){false, 0};
#endif

        /*
         * zero-th state, a.k.a. INVALID state
         *
         * By default, all values of `dtrans` should be set to zero.
         * i.e. all invalid transitions shall cause the emulator to end up at
         * the zero-th state.
         *
         * Thus, by setting isendstate = true, and not using this state for
         * regular purposes, we shall be able to immediately tell if any
         * transition we do is invalid (as we'll end up on state zero).
         */
        dfa->states[0].isendstate = true;
	for (Dstatenum i = 0; i < ALSIZ; i++)
		dfa->states[0].dtrans[i] = 0;

	return dfa;
}

void
freeDFA(DFA *dfa)
{
	free(dfa->states);
	free(dfa);
}

#include "common.h"
#include "macros.h"
#include "dfa.h"

DFA *newDFA(void) {
	new(dfa, DFA);
	/*
	 * Add the 0th state, which is the invalid state
	 * ie. any invalid character will cause us to end up here.
	 *
	 * XXX:	Should we set endstate[0] = true?
	 *	That way we won't have to add additional logic to the ingester.
	 *
	 * (We'll still need to check AFTER the ingestor has stopped whether
	 *  we're on an end state or not, and what that end state corresponds
	 *  to. Maybe we can add an entry called INVALID there?)
	 */
	add(dfa->statecount, 1);
	reallocarr(dfa->states, dfa->statecount);
	reallocarr(dfa->endstate, dfa->statecount);
	dfa->endstate[0] = true; // see comment above.
	return dfa;
}

Dstatenum addDstate(DFA *dfa) {
	Dstatenum idx = dfa->statecount;
	add(dfa->statecount, 1);
	reallocarr(dfa->states, dfa->statecount);
	reallocarr(dfa->endstate, dfa->statecount);
	dfa->endstate[idx] = false;
	return idx;
}

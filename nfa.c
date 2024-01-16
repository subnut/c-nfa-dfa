#include "common.h"
#include "nfa.h"

NFA *newNFA(void) {
	new(p, NFA);
	alloc(p->states, sizeof(Nstate));
	p->statecount = 1;	// The start state S0
	return p;
}

Nstatenum newNstate(NFA *nfa) {
	add(nfa->states, 1);
	reallocarr(nfa->states, nfa->statecount, sizeof(Nstate));
	return nfa->statecount - 1;
}

/* Returns the value of nfa->statecount before appendNFA was called */
Nstatenum appendNFA(NFA *nfa, NFA *extend) {
	Nstatenum oldcount = nfa->statecount;
	add(nfa->statecount, extend->statecount);
	reallocarr(nfa->states, nfa->statecount, sizeof(Nstate));
	for (Nstatenum i = 0; i < extend->statecount; i++) {
		nfa->states[oldcount + i] = extend->states[i];
		for (Nstatenum j = 0; j < extend->states[i]->etnum; j++)
			extend->states[i]->etrans[j] += oldcount;
		for (int j = 0; j < NFA_ALSIZ; j++)
			for (Nstatenum k = 0; k < extend->states[i]->tnum[j]; k++)
				extend->states[i]->trans[j][k] += oldcount;
	}
	for (int i = 0; i < NFA_ALSIZ; i++)
		if (extend->alphabet[i])
			nfa->alphabet[i] = true;
	return oldcount;
}

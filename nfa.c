#include "common.h"
#include "nfa.h"

NFA *newNFA(void) {
	new(p, NFA);
	p->start = addNstate(p);
	return p;
}

/* Adds a new state to nfa and returns its number */
Nstatenum addNstate(NFA *nfa) {
	Nstatenum newidx = nfa->statecount;
	add(nfa->statecount, 1);
	reallocarr(nfa->states, nfa->statecount);
	new(nstate, Nstate);
	nfa->states[newidx] = nstate;
	return newidx;
}

/* Returns pointer to a new (allocated) Nstate which is a deepcopy of *old */
Nstate *copyNstate(Nstate *old) {
	new(new, Nstate);
	new->etnum = old->etnum;
	reallocarr(new->etrans, new->etnum);
	for (Nstatenum i = 0; i < new->etnum; i++)
		new->etrans[i] = old->etrans[i];
	for (int i = 0; i < NFA_ALSIZ; i++) {
		new->tnum[i] = old->tnum[i];
		reallocarr(new->trans[i], new->tnum[i]);
		for (Nstatenum j = 0; j < new->tnum[i]; j++)
			new->trans[i][j] = old->trans[i][j];
	}
	return new;
}

/* Returns the value of nfa.statecount before appendNFA was called
 * NOTE: Does NOT touch the nfa.endstates */
Nstatenum appendNFA(NFA *nfa, NFA *extend) {
	Nstatenum oldcount = nfa->statecount;
	add(nfa->statecount, extend->statecount);
	reallocarr(nfa->states, nfa->statecount);
	for (Nstatenum i = 0; i < extend->statecount; i++) {
		nfa->states[oldcount + i] = copyNstate(extend->states[i]);
		for (Nstatenum j = 0; j < nfa->states[oldcount + i]->etnum; j++)
			nfa->states[oldcount + i]->etrans[j] += oldcount;
		for (int j = 0; j < NFA_ALSIZ; j++)
			for (Nstatenum k = 0; k < nfa->states[oldcount + i]->tnum[j]; k++)
				nfa->states[oldcount + i]->trans[j][k] += oldcount;
	}
	for (int i = 0; i < NFA_ALSIZ; i++)
		if (extend->alphabet[i])
			nfa->alphabet[i] = true;
	return oldcount;
}

void addEndState(NFA *nfa, Nstatenum statenum) {
	 add(nfa->endcount, 1);
	 reallocarr(nfa->endstates, nfa->endcount);
	 nfa->endstates[nfa->endcount - 1] = statenum;
}

void addTrans(Nstate *nstate, char c, Nstatenum to) {
	add(nstate->tnum[c], 1);
	reallocarr(nstate->trans[c], nstate->tnum[c]);
	nstate->trans[c][nstate->tnum[c] - 1] = to;
}

NFA *nfa4str(const char *str) {
	NFA *nfa = newNFA();
	Nstatenum oldstate = nfa->statecount - 1;
	Nstatenum newstate = oldstate;
	for (size_t i = 0; i < strlen(str); i++) {
		nfa->alphabet[i] = true;
		oldstate = newstate; newstate = addNstate(nfa);
		addTrans(nfa->states[oldstate], str[i], newstate);
	}
	addEndState(nfa, newstate);
	return nfa;
}

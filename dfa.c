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
	dfa->states = calloc(dfa->arrsiz, sizeof *dfa->states);
	perrif(dfa->states == NULL, "Could not allocate memory");

#if 0	// Should we initialize everything?
	// or should we leave it for the caller to initialize?
	for (Dstatenum i = 0; i < dfa->arrsiz; i++)
		dfa->states[i] = (Dstate){false, {0}};
#endif

        /*
         * NOTE: we are not creating the zero-th state here.
         * It will be the responsibility of the caller to create and initialize
         * the zero-th state themselves.
         */

        return dfa;
}

void
freeDFA(DFA *dfa)
{
	free(dfa->states);
	free(dfa);
}

void
printDFA(DFA *dfa)
{
	puts("digraph {\n"
		"\trankdir=LR\n"
		"\tnode [shape=circle]\n");

	// skip zero-th state
	for (Dstatenum i = 1; i < dfa->arrsiz; i++) {
		puts("");
		if (dfa->states[i].isendstate)
			printf("\t\"%d\" [shape=doublecircle]\n", i);

		for (int c = 0; c < ALSIZ; c++) {
			if (0 == dfa->states[i].dtrans[c])
				// don't print transitions to state zero
				// those are probably just unpopulated (and therefore, invalid)
				continue;
			printf("\t\"%d\" -> \"%d\" [label=\"%c\"]\n",
					i, dfa->states[i].dtrans[c], c);
		}
	}

	puts("}");
}

#include "common.h"
#include "nfa.h"

#include <stdio.h>
#define EPS "ϵ"

int main(void) {
	NFA *nfa = nfa4str("Hi!");
	puts("graph {");
	puts("\trankdir=\"LR\"");
	puts("\tnode [shape=\"circle\"]");

	puts("");
	for (Nstatenum i = 0; i < nfa->endcount; i++)
		printf("\t\"%d\" [shape=\"doublecircle\"]\n", nfa->endstates[i]);

	puts("");
	for (Nstatenum i = 0; i < nfa->statecount; i++) {
		for (Nstatenum j = 0; j < nfa->states[i]->etnum; j++)
			printf("\t\"%d\" -> \"%d\" [label=\""EPS"\"]\n", i, nfa->states[i]->etrans[j]);
		for (int c = 0; c < NFA_ALSIZ; c++)
			for (Nstatenum j = 0; j < nfa->states[i]->tnum[c]; j++)
				printf("\t\"%d\" -> \"%d\" [label=\"%c\"]\n", i, nfa->states[i]->trans[c][j], c);
	}
	puts("}");
}

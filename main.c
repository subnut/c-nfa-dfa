#include "common.h"
#include "nfa.h"

#include <stdio.h>
#define EPS "ϵ"

int main(void) {
	NFA *nfa = nfaOR(nfa4str("meh."), nfaOR(nfa4str("Hiya!"), nfa4str("Hello")));

	puts("digraph {");
	puts("\trankdir=\"LR\"");
	puts("\tnode [shape=\"circle\"]");
	printf("\t\"%d\" [shape=\"doublecircle\"]\n", nfa->end);
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

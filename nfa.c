#include "common.h"
#include "macros.h"
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
	for (int i = 0; i < ALSIZ; i++) {
		new->tnum[i] = old->tnum[i];
		reallocarr(new->trans[i], new->tnum[i]);
		for (Nstatenum j = 0; j < new->tnum[i]; j++)
			new->trans[i][j] = old->trans[i][j];
	}
	return new;
}

/* Returns the value of nfa.statecount before appendNFA was called */
Nstatenum appendNFA(NFA *nfa, NFA *extend) {
	Nstatenum oldcount = nfa->statecount;
	add(nfa->statecount, extend->statecount);
	reallocarr(nfa->states, nfa->statecount);
	for (Nstatenum i = 0; i < extend->statecount; i++) {
		nfa->states[oldcount + i] = copyNstate(extend->states[i]);
		for (Nstatenum j = 0; j < nfa->states[oldcount + i]->etnum; j++)
			nfa->states[oldcount + i]->etrans[j] += oldcount;
		for (int j = 0; j < ALSIZ; j++)
			for (Nstatenum k = 0; k < nfa->states[oldcount + i]->tnum[j]; k++)
				nfa->states[oldcount + i]->trans[j][k] += oldcount;
	}
	for (int i = 0; i < ALSIZ; i++)
		if (extend->alphabet[i])
			nfa->alphabet[i] = true;
	return oldcount;
}

void addTrans(Nstate *nstate, char c, Nstatenum to) {
	add(nstate->tnum[c], 1);
	reallocarr(nstate->trans[c], nstate->tnum[c]);
	nstate->trans[c][nstate->tnum[c] - 1] = to;
}
void addEtrans(Nstate *nstate, Nstatenum to) {
	add(nstate->etnum, 1);
	reallocarr(nstate->etrans, nstate->etnum);
	nstate->etrans[nstate->etnum - 1] = to;
}

NFA *nfa4str(const char *str) {
	NFA *nfa = newNFA();
	Nstatenum oldstate = nfa->statecount - 1;
	Nstatenum newstate = oldstate;
	for (size_t i = 0; i < strlen(str); i++) {
		nfa->alphabet[str[i]] = true;
		oldstate = newstate; newstate = addNstate(nfa);
		addTrans(nfa->states[oldstate], str[i], newstate);
	}
	nfa->end = newstate;
	return nfa;
}

NFA *nfaMultiOR(int argc, NFA **argv) {
	NFA *nfa = newNFA();
	nfa->end = addNstate(nfa);

	for (int i = 0; i < argc; i++) {
		Nstatenum istart = appendNFA(nfa, argv[i]);
		addEtrans(nfa->states[nfa->start], istart);
		addEtrans(nfa->states[argv[i]->end + istart], nfa->end);
	}

	return nfa;
}

NFA *nfaSTAR(NFA *a) {
	NFA *nfa = newNFA();
	nfa->end = addNstate(nfa);

	Nstatenum astart = appendNFA(nfa, a);
	addEtrans(nfa->states[nfa->start], astart);
	addEtrans(nfa->states[a->end + astart], astart);
	addEtrans(nfa->states[a->end + astart], nfa->end);
	addEtrans(nfa->states[nfa->start], nfa->end);

	return nfa;
}
NFA *nfaPLUS(NFA *a) {
	NFA *nfa = newNFA();
	nfa->end = addNstate(nfa);

	Nstatenum astart = appendNFA(nfa, a);
	addEtrans(nfa->states[nfa->start], astart);
	addEtrans(nfa->states[a->end + astart], astart);
	addEtrans(nfa->states[a->end + astart], nfa->end);

	return nfa;
}
NFA *nfaOPT(NFA *a) {
	NFA *nfa = newNFA();
	nfa->end = addNstate(nfa);

	Nstatenum astart = appendNFA(nfa, a);
	addEtrans(nfa->states[nfa->start], astart);
	addEtrans(nfa->states[nfa->start], nfa->end);
	addEtrans(nfa->states[a->end + astart], nfa->end);

	return nfa;
}

void freeNstate(Nstate *nstate) {
	if (nstate->etrans != NULL)
		free(nstate->etrans);
	for (Nstatenum i = 0; i < ALSIZ; i++)
		if (nstate->trans[i] != NULL)
			free(nstate->trans[i]);
	free(nstate);
}
void freeNFA(NFA *nfa) {
	for (Nstatenum i = 0; i < nfa->statecount; i++)
		freeNstate(nfa->states[i]);
	free(nfa->states);
	free(nfa);
}


#define EPS "\317\265" // ϵ
#define puts(s) fputs(s"\n", f)
#define printf(x...) fprintf(f, x)
void fprintNFA(FILE *restrict f, NFA *nfa) {
	puts("digraph {");
	puts("\trankdir=LR");
	puts("\tnode [shape=circle]");
	printf("\t\"%d\" [shape=doublecircle]\n", nfa->end);
	puts("");
	for (Nstatenum i = 0; i < nfa->statecount; i++) {
		for (Nstatenum j = 0; j < nfa->states[i]->etnum; j++)
			printf("\t\"%d\" -> \"%d\" [label=\""EPS"\"]\n",
					i, nfa->states[i]->etrans[j]);
		for (int c = 0; c < ALSIZ; c++)
			for (Nstatenum j = 0; j < nfa->states[i]->tnum[c]; j++)
				printf("\t\"%d\" -> \"%d\" [label=\"%c\"]\n",
						i, nfa->states[i]->trans[c][j], c);
	}
	puts("}");
}
#undef printf
#undef puts
#undef EPS


/*
 * This whole thing is a bunch of hacky (to me) if-else-ifs
 * Here be dragons!!!  (That you can fight if you've read THE Dragon Book) :P
 */
NFA *nfaMultiSEQ(int argc, NFA **argv) {
	new(nfa, NFA);
	nfa->start = argv[0]->start;
	nfa->end = argv[0]->end;
	appendNFA(nfa, argv[0]);

	for (argv++; --argc; argv++) {
		Nstatenum newidx = appendNFA(nfa, *argv);
		// Copy current start's transitions to previous end
		{
			Nstate *end = nfa->states[nfa->end];
			Nstate *cur = nfa->states[newidx + argv[0]->start];
			if (cur->etnum > 0) {
				Nstatenum oldetnum = end->etnum;
				add(end->etnum, cur->etnum);
				reallocarr(end->etrans, end->etnum);
				for (Nstatenum i = 0; i < cur->etnum; i++)
					end->etrans[oldetnum + i] = cur->etrans[i];
			}
			for (int i = 0; i < ALSIZ; i++)
				if (cur->tnum[i] > 0) {
					Nstatenum oldtnum = end->tnum[i];
					add(end->tnum[i], cur->tnum[i]);
					reallocarr(end->trans[i], end->tnum[i]);
					for (Nstatenum j = 0; j < cur->tnum[i]; j++)
						end->trans[i][oldtnum + j] = cur->trans[i][j];
				}
		}

		// Destroy current start state's struct
		freeNstate(nfa->states[newidx + argv[0]->start]);

		/*
		 * NOTE: These kinda loops are buffer overruns waiting to happen.
		 *       Case in hand, this loop was previously written like this -
		 *
		 * for (Nstatenum i = newidx + argv[0]->start; i < nfa->statecount; i++)
		 * 	nfa->states[i] = nfa->states[i + 1];
		 *
		 * Can you see the error? It's OK if you can't; I couldn't see it either.
		 * HINT: note the +1 in the [i + 1], and that the loop condition is on i.
		 */

		// For all states after current start, shift them -1
		// Also reduce statecount accordingly
		for (Nstatenum i = newidx + argv[0]->start,
				j = i + 1; j < nfa->statecount; i++, j++)
			nfa->states[i] = nfa->states[j];
		add(nfa->statecount, -1);

		// Fix broken references
		for (Nstatenum i = newidx; i < nfa->statecount; i++) {
			Nstate *ns = nfa->states[i];
			for (Nstatenum i = 0; i < ns->etnum; i++)
				if (ns->etrans[i] > newidx + argv[0]->start)
					ns->etrans[i] -= 1;
				else if (ns->etrans[i] == newidx + argv[0]->start)
					ns->etrans[i] = nfa->end;
			for (int i = 0; i < ALSIZ; i++)
				for (Nstatenum j = 0; j < ns->tnum[i]; j++)
					if (ns->trans[i][j] > newidx + argv[0]->start)
						ns->trans[i][j] -= 1;
					else if (ns->trans[i][j] == newidx + argv[0]->start)
						ns->trans[i][j] = nfa->end;
		}
		// We gotta fix the references we copied into nfa->end
		Nstatenum i = nfa->end;
		{
			Nstate *ns = nfa->states[i];
			for (Nstatenum i = 0; i < ns->etnum; i++)
				if (ns->etrans[i] > newidx + argv[0]->start)
					ns->etrans[i] -= 1;
				else if (ns->etrans[i] == newidx + argv[0]->start)
					ns->etrans[i] = nfa->end;
			for (int i = 0; i < ALSIZ; i++)
				for (Nstatenum j = 0; j < ns->tnum[i]; j++)
					if (ns->trans[i][j] > newidx + argv[0]->start)
						ns->trans[i][j] -= 1;
					else if (ns->trans[i][j] == newidx + argv[0]->start)
						ns->trans[i][j] = nfa->end;
		}

		// Set the new end
		if (newidx + argv[0]->end > newidx + argv[0]->start)
			nfa->end = newidx + argv[0]->end - 1;
		else if (newidx + argv[0]->end != newidx + argv[0]->start)
			nfa->end = newidx + argv[0]->end;
	}

	return nfa;
}

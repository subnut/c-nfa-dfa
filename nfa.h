#include "common.h"
#include <stdbool.h>
#include <stdio.h>	// FILE, stdout

#ifndef NFA_H
#define NFA_H

/*
 * Currently we are assuming each NFA only has at most INT_MAX number of states.
 * If the number of states were to increase beyond that, then change this typedef.
 */
typedef int Nstatenum;

/*
 * NOTE:
 * Each Nstate refers to the next Nstate by its index in NFA.states[]
 * Pay attention to the fact that this only contains indexes, not pointers
 */
typedef struct {
	Nstatenum etnum, *etrans;
	Nstatenum tnum[ALSIZ], *trans[ALSIZ];
} Nstate;

/*
 * NOTE:
 * The current data structure only supports one end state.
 *
 * A general NFA can have multiple end states, but we'll implement that
 * if/when we need it. Right now, we only need one end state per NFA.
 *
 * Actually, the way we're using it now, the start state is always zero.
 * So, even the start field can be removed if/when necessary.
 */
typedef struct {
	Nstate **states;
	Nstatenum statecount;
	Nstatenum start, end;
	bool alphabet[ALSIZ];
} NFA;


NFA *		newNFA(void);
Nstatenum	addNstate(NFA *);
Nstatenum	appendNFA(NFA *, NFA *);

Nstate *	copyNstate(Nstate *);
void		addTrans(Nstate *, char, Nstatenum);


NFA *	nfa4str(const char *);


NFA *	nfaSTAR(NFA *);
NFA *	nfaPLUS(NFA *);
NFA *	nfaOPT(NFA *);

#define nfaOR(a, b)  nfaMultiOR(2, (NFA*[]){a, b})
#define nfaSEQ(a, b) nfaMultiSEQ(2, (NFA*[]){a, b})
NFA *	nfaMultiOR(int, NFA **);
NFA *	nfaMultiSEQ(int, NFA **);


void	freeNstate(Nstate *);
void	freeNFA(NFA *);


#define printNFA(NFA) \
	fprintNFA(stdout, NFA)
void	fprintNFA(FILE *, NFA *);

#endif//NFA_H

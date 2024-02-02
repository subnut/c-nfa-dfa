#include "common.h"

#ifndef NFA_H
#define NFA_H
#include <stdbool.h>
#include <stdio.h>	// for FILE type

/*
 * Currently we are assuming each NFA only has at most INT_MAX number of states.
 * If the number of states were to increase beyond that, then change this typedef.
 */
typedef int Nstatenum;


typedef struct {
	Nstatenum etnum, *etrans;
	Nstatenum tnum[ALSIZ], *trans[ALSIZ];
} Nstate;

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

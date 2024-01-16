#ifndef NFA_H
#define NFA_H
#include <stdbool.h>

/*
 * Number of items in the NFA input alphabet
 * eg. ASCII has 128 possible symbols, therefore an NFA that supports
 *     ASCII as its input alphabet shall have an NFA_ALSIZ of 128
 */
#define NFA_ALSIZ 128

/*
 * Currently we are assuming each NFA only has at most INT_MAX number of states.
 * If the number of states were to increase beyond that, then change this typedef.
 */
typedef int Nstatenum;


typedef struct {
	Nstatenum etnum, *etrans;
	Nstatenum tnum[NFA_ALSIZ], *trans[NFA_ALSIZ];
} Nstate;

typedef struct {
	Nstate **states;
	Nstatenum statecount;
	Nstatenum start, end;
	bool alphabet[NFA_ALSIZ];
} NFA;


NFA *		newNFA(void);
Nstatenum 	addNstate(NFA *);
Nstatenum	appendNFA(NFA *, NFA *);

Nstate *	copyNstate(Nstate *);
void		addTrans(Nstate *, char, Nstatenum);

NFA *		nfa4str(const char *);
NFA *		nfaOR(NFA *, NFA *);

#endif//NFA_H

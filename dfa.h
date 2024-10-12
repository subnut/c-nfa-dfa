#ifndef DFA_H
#define DFA_H

#include <stdbool.h>
#include "common.h"

/*
 * Currently we are assuming each DFA only has at most UINT_MAX number of states.
 * If the number of states were to increase beyond that, then change this typedef.
 *
 * NOTE: This type should always be unsigned, because signed overflow is UB.
 */
typedef unsigned int Dstatenum;

typedef struct {
	bool isendstate; // is endstate
	Dstatenum dtrans[ALSIZ];
} Dstate;

typedef struct {
	Dstatenum arrsiz;
	Dstate *states;
} DFA;

DFA *newDFA(Dstatenum statenum);
//Dstate *addDstate(DFA *dfa);
void printDFA(DFA *dfa);
void freeDFA(DFA *dfa);

#endif//DFA_H

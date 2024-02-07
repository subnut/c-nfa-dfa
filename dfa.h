#include "common.h"
#include <stdbool.h>

#ifndef DFA_H
#define DFA_H

/*
 * Currently we are assuming each DFA only has at most INT_MAX number of states.
 * If the number of states were to increase beyond that, then change this typedef.
 */
typedef int Dstatenum;

/*
 * NOTE:
 *
 */
typedef Dstatenum Dstate[ALSIZ];

/*
 * NOTE:
 * The first state (index = 0) is an invalid state. It's where you'll end up
 * if you enter an invalid input. States 1 .. statecount are the valid states.
 *
 * The aforementioned convention is needed since Dstatenums are initialized to
 * zero by default, and changing them to anything else would result in having
 * to loop through the whole array and set each value. That's unneccesary.
 *
 * Also, this way we can have Dstatenum be unsigned.
 *
 * P.S.
 * Do remember to allocate and initialize endstate[i] = false whenever you add
 * a new Dstate (at index i, of course.)
 *
 * XXX:	Should we set endstate[0] = true?
 * 	That way we won't have to add additional logic to the DFA emulator
 */
typedef struct {
	Dstatenum statecount;
	Dstatenum start;
	Dstate *states;	// array of Dstates
	bool *endstate;	// array of bools; length = statecount;
} DFA;


DFA *		newDFA(void);
Dstatenum	addDstate(DFA *);

#endif /* DFA_H */

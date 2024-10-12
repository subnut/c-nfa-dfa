#include "dfa.h"
#include "nfa.h"
#include "nfa2dfa.h"

int main(void) {
	NFA *nfa1 = nfa4str("a");
	NFA *nfa2 = nfa4str("b");
	NFA *nfa3 = nfaOR(nfa1, nfa2);
	NFA *nfa4 = nfaSTAR(nfa3);
	NFA *nfa5 = nfa4str("abb");
	NFA *nfa = nfaSEQ(nfa4, nfa5);
	DFA *dfa = nfa2dfa(nfa);
	printNFA(nfa);
	printDFA(dfa);
	freeDFA(dfa);
	freeNFA(nfa);
	freeNFA(nfa1);
	freeNFA(nfa2);
	freeNFA(nfa3);
	freeNFA(nfa4);
	freeNFA(nfa5);
	return 0;
}

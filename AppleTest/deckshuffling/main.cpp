/*
Please complete the following task in C, C++, or Objective-C. You are free to use whatever data structures and design you prefer.

1) create a deck of cards, and print/display the cards
2) shuffle the deck of cards, and display the cards (in their shuffled order)
3) reshuffle the deck, and display the cards (in their shuffled order)
4) look for any 2-card sequence that was also present in the previous shuffle. If any are found, go back to step 3
5) when two back to back shuffles do not contain a duplicate sequence, you're done.

Questions:
1) How might you adapt your code to check for sequences of 3, 4, or more cards from the previous shuffle?
2) How might you adapt your code to check for combinations of N cards, rather than sequences?
3) How might you adapt your code to check for card sequences that were present in any previous shuffle? (i.e. comparing against all previous shuffles, not just the most recent one)
4) If necessary, how might you change your code to accommodate more than 4 suits, or more than 13 different card values.
5) Would you do things any differently if you were optimizing purely for speed? How about if you were trying to minimize memory usage?

*/


#include <iostream>
#include "cardDeck.h"

#define SUITSMAX 4
#define CARDINSUIT 13


int main(int argc, char* argv[]){

	Deck cardDeck(SUITSMAX,CARDINSUIT);

	uint64_t iterations = cardDeck.RunShuffles();

	std::cout << "All done after iteration number : " << iterations << std::endl;

	return 0;

}
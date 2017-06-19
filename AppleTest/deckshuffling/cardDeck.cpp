
#include "cardDeck.h"

	/**
	    Creates a deck of cards using the user provided 
	    values of maxSuits and maxCardsPerSuit.

	    This method also calculates the values that each
	    card will be assigned.

	*/

	void Deck::Create(){
		uint64_t value = 0;
        for (int i = 0; i < maxSuits; ++i) {
            for (int j = 0; j < maxCardsPerSuit; ++j) {
                value = i * maxCardsPerSuit + j;
                cards.emplace_back(Card(value));
            }
		}
	}	

	/**
	    Displays a deck of cards in the order that 
	    they are currently held.
	*/

	void Deck::DisplayDeck(){
		for(auto& card : cards){
			std::cout << card.GetCardValue() << "\t";
		}
		std::cout << "\n";
	}

	/**
	   Shuffle the current deck of cards using the 
	   Fisher-Yates shuffling algorithm.
	*/
	void Deck::ShuffleDeck(){

		srand(unsigned(time(0))); // Generate a seed for the pseudo random generator
		int random_integer;

		for(int i = cards.size()-1; i > 0; --i){
			random_integer = (rand()%(i+1)); // generate a random number in the range of 0 to i
			swap(cards[i], cards[random_integer]); //swap the random number element to the last element
		}
	}

	/**
		Go through the current shuffled array and check if 
		for each of the current cards, the next card in order 
		is the same as the next card in order in the previous
		shuffle.
		The previous shuffle is held in a map for quick reference.

		@return true only if there were no two pair of cards 
				that were common between two different shuffles.

				false otherwise. false triggers a reshuffle.
	*/
	bool Deck::CheckBetweenShuffles(){

		//Only the first time will the shuffle map be empty
		if(shuffleMap.empty())
			return false;

		for(int i = 0; i < cards.size()-1; ++i){
			if(shuffleMap[cards[i].GetCardValue()] == cards[i+1].GetCardValue()){
				std::cout << "Found match with " << cards[i].GetCardValue() << " : " << cards[i+1].GetCardValue() << std::endl;
				return false;
			}
		}
		return true;
	}

	/*
		Utility method to print the elements in the 
	 	shuffleMap with the format "card : next card"
	*/
	void Deck::PrintMap(){

		for(const auto& item : shuffleMap){
			std::cout << item.first << " : " << item.second << std::endl;
		}
	}

	/* 	Go through the shuffled array and populate
	 	the "shuffleMap" which holds a mapping between
		consecutive elements in the shuffled deck.
	*/
	void Deck::PopulateShuffleMap(){

		for(int i = 0; i < cards.size()-1; ++i){
			shuffleMap[cards[i].GetCardValue()] = cards[i+1].GetCardValue();
		}
	}

	/**
		Run the main loop in the program.

		Keep shuffling the deck until the current
		shuffle does not contain any pair of cards
		that are the same as a pair in the previous
		shuffle.

		@return The number of iterations that it took
				to create a distict deck of cards.
	*/

	uint64_t Deck::RunShuffles(){

	uint64_t iterations = 0;
	bool shufflesOK = false;

	do{
		// Increment the number of iterations.
		++iterations;

		ShuffleDeck(); // Shuffle the deck

		DisplayDeck(); // Shows the shuffled cards

		// Check if the current shuffle contains 
		// duplicate pair of cards
		shufflesOK = CheckBetweenShuffles();

		// if we found a duplicate pair, then populate
		// the shuffleMap for the next shuffle
		if(!shufflesOK){
			PopulateShuffleMap(); //add the current shuffle to the map.
		}

	}while(!shufflesOK);

	return iterations;

	}
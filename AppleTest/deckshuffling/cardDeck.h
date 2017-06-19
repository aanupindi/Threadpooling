
#include <vector>
#include <iostream>
#include <cstdlib> 
#include <ctime> 
#include <unordered_map>

// enum Face { ACE, TWO, TREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};
// enum suit {SPADE, CLOVER, HEART, DIAMONDS};

class Card{

	uint64_t cardValue;

public:

	//Default constructor
	Card(): cardValue(-1){}

	Card(uint64_t t_val): cardValue(t_val){
	}

	//Copy Constructor
	Card(const Card& c): cardValue(c.GetCardValue()){

	}

	// Reset the value for a current card
	void Reset(const Card& c){
		cardValue = c.GetCardValue();
	}

	// Getter for card value
	uint64_t GetCardValue() const{
		return cardValue;
	}
};

class Deck{

	//All the cards in the deck
	std::vector<Card> cards;

	//Max suits allowed in the deck 
	int maxSuits;

	//Max cards allowed per suit
	int maxCardsPerSuit;

	//map to store the mapping between <card value : next card value>
	std::unordered_map<int, int> shuffleMap;

	//Method to swap two card values in place.
	void swap(Card &src, Card &dst){
		Card tmp;
		tmp.Reset(src);
		src.Reset(dst);
		dst.Reset(tmp);
	}

	// Creating the card deck by initializing their values
	void Create();

	// Display the card values in the deck in their current ordering
	void DisplayDeck();

	//First step in Deck creation.
	// Create and displace the deck of cards.
	void InitializeDeck(){
		Create();
		DisplayDeck();
	}

	// Shuffle the deck using Fisher-Yates Algorithm.
	// Fisher-Yates Algorithm is a straight-forward 
	// in place algorithm that uses a random number
	// generator to shuffle the deck of cards.
	void ShuffleDeck();

	// Go through the shuffled array and populate
	// the "shuffleMap" which holds a mapping between
	// consecutive elements in the shuffled deck.
	void PopulateShuffleMap();

	// Utility method to print the elements in the 
	// shuffleMap with the format "card : next card"
	void PrintMap();

	//returns false if two shuffled decks 
	//contain a common sequence of two cards.
	bool CheckBetweenShuffles();

public:

	// constructor to initialize the deck 
	// specifications.
	// Initialize the card values in the deck.
	Deck(int t_maxSuits, int t_maxCardsPerSuit){
		maxSuits = t_maxSuits;
		maxCardsPerSuit = t_maxCardsPerSuit;

		InitializeDeck();
	}

	// This is the main method that shuffles 
	// the card deck continuosly and checks for
	// a sequence of cards that are common 
	// between two consecutive shuffles.
	// This also returns the number of iterations
	// of shuffles that took place before a set of
	// distinct shuffles were found.
	uint64_t RunShuffles();

};
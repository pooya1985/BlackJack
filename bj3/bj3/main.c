/* this is the simple text based game of BlackJack,
 with one player and one dealer and just has implemented 
 basic features of the game as hit and stand and has set
 the descision rule of 17 for the dealer, for simplofication
 and modular coding a new set of definitions are imported from the
 designed library of blackjack.h
 The game after each run keeps asking for the palyer will to repeat 
 and enter a new game, it keeps the percentage of player basic strategy on
 the ongoing basis. else of win and loss it has the tie option as well.
 the code also when all the cards are turned back shows the dealer's cards as well
 
 this code might not be optimized based on my time frame, and might need some debugging 
 if it is running under a windows based machine, but the code is fully functional and bug
 free in MAC
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Local header included*/
#include "blackjack.h"

/* Global variables defined in blackjack.h */
static const char *suit_names[NUMBER_SUITS] = {"Spade", "Heart", "Diamond", "Club"};
static const char *rank_names[NUMBER_RANKS] = {"Ace", "2", "3", "4", "5", "6", "7",
    "8", "9", "10", "Jack", "Queen", "King"};

/* Random shuffle a deck of cards */
static inline void shuffle(Card *cards)
{
    Card *aCard = cards;
    const Card *cardEnd = cards + NUMBER_CARDS;
    
    printf("Shuffling... ");
    
    /* Shuffle the cards */
    srand(time(NULL));
    aCard = cards;
    do
    {
        Card *swapPosition = cards + rand() % NUMBER_CARDS;
        Card swap = *aCard;
        *aCard = *swapPosition;
        *swapPosition = swap;
    }
    while (++aCard != cardEnd);
    
    printf("done\n");
}

/* Show a player or dealer's hand */
static inline void show_hand(Card **hand, Card **end, uint points)
{
    Card **aCard = hand;
    
    printf("Hand (%u) - %s of %s", points,
           rank_names[(*aCard)->rank],
           suit_names[(*aCard)->suit]);
    
    while (++aCard != end)
    {
        printf(", %s of %s",
               rank_names[(*aCard)->rank],
               suit_names[(*aCard)->suit]);
    }
    
    printf(".\n");
}

int main(void)
{
    Card cards[NUMBER_CARDS];
    Card *aCard = cards;
    uint roundCount = NUMBER_ROUNDS_SHUFFLE, gamesPlayed = 0, gamesWon = 0;
    char buffer[80];
    
    /* Load the cards */
    uint i = 0;
    do
    {
        uint j = 0;
        do
        {
            aCard->suit = i;
            aCard->rank = j;
            ++aCard;
        }
        while (++j < NUMBER_RANKS);
    }
    while (++i < NUMBER_SUITS);
    
    /* Play */
    do
    {
        uint aces = 0, playerPoints = 0, dealerPoints = 0;
        Card *playerHand[MAXIMUM_HAND];
        Card **aHandCard = playerHand;
        
        /* Shuffle the cards after a specific number of rounds */
        if (++roundCount >= NUMBER_ROUNDS_SHUFFLE)
        {
            shuffle(cards);
            aCard = cards;
            roundCount = 0;
        }
        
        /* Player plays */
        do
        {
            /* Add the card's rank to player point */
            if (aCard->rank == ACE)
            {
                playerPoints += 11;
                ++aces;
            }
            else if (aCard->rank < JACK)
                playerPoints += aCard->rank + 1;
            else
                playerPoints += 10;
            
            /* Print the new card */
            printf("New Card... %s of %s\n", rank_names[aCard->rank],
                   suit_names[aCard->suit]);
            
            *aHandCard = aCard;
            ++aCard;
            assert(aCard != cards + NUMBER_CARDS);
            
            /* Let the second card be dealt */
            if (aHandCard == playerHand)
            {
                ++aHandCard;
                *buffer = 'h';
                continue;
            }
            
            ++aHandCard;
            
            /* Convert Aces to 1 */
            while (playerPoints > 21 && aces > 0)
            {
                playerPoints -= 10;
                --aces;
            }
            
            /* Print the hand */
            show_hand(playerHand, aHandCard, playerPoints);
            
            /* On a bust, dealer doesn't need to play.
             * This single "goto" simplifies the code.
             */
            if (playerPoints > 21)
            {
                printf("Bust!\n");
                goto done;
            }
            
            /* On Blackjack, let the dealer play */
            if (playerPoints == 21)
            {
                printf("Blackjack!\n");
                break;
            }
            
            /* Hit or stand? */
            do
            {
                printf("Hit or stand? (h/s) : ");
                scanf("%s", buffer);
            }
            while (*buffer != 'h' && *buffer != 'H' && *buffer != 's' && *buffer != 'S');
        }
        while (*buffer == 'h' || *buffer == 'H');
        
        /* Dealer plays */
        aHandCard = playerHand;
        aces = 0;
        printf("Dealer plays...\n");
        do
        {
            /* Add the card's rank to dealer point */
            if (aCard->rank == ACE)
            {
                dealerPoints += 11;
                ++aces;
            }
            else if (aCard->rank < JACK)
                dealerPoints += aCard->rank + 1;
            else
                dealerPoints += 10;
            
            /* Print the new card */
            printf("New Card... %s of %s\n", rank_names[aCard->rank],
                   suit_names[aCard->suit]);
            
            *aHandCard = aCard;
            ++aCard;
            assert(aCard != cards + NUMBER_CARDS);
            
            /* Let the second card be dealt */
            if (aHandCard == playerHand)
            {
                ++aHandCard;
                continue;
            }
            
            ++aHandCard;
            
            /* Convert Aces to 1 */
            while (dealerPoints > 21 && aces > 0)
            {
                dealerPoints -= 10;
                --aces;
            }
            
            /* Print the hand */
            show_hand(playerHand, aHandCard, dealerPoints);
            
            if (dealerPoints > 21)
            {
                printf("Bust!\n");
                break;
            }
            
            /* Blackjack */
            if (dealerPoints == 21)
            {
                printf("Blackjack!\n");
                break;
            }
            
            printf("Hit or stand? (h/s) : ");
            
            /* Dealer must hit on soft 17 */
            if (dealerPoints < 17 || (dealerPoints == 17 && aces))
            {
                printf("hit\n");
                continue;
            }
            
            printf("stand\n");
            break;
        }
        while (1);
    done:
        
        /* Keep track of win percentage for the player. */
        if (playerPoints <= 21 && (playerPoints > dealerPoints || dealerPoints > 21))
        {
            ++gamesWon;
            printf("Player wins!\n");
        }
        else if (playerPoints <= 21 && playerPoints == dealerPoints)
            printf("Tie!\n");
        else
            printf("Dealer wins!\n");
        
        ++gamesPlayed;
        
        printf("Win percentage - %u%%\n", (gamesWon * 100) / gamesPlayed);
        
        /* Ask to play again. */
        do
        {
            printf("Play again? (y/n) : ");
            scanf("%s", buffer);
        }
        while (*buffer != 'n' && *buffer != 'N' && *buffer != 'y' && *buffer != 'Y');
    }
    while (*buffer == 'y' || *buffer == 'Y');
    
    return 0;
}
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <limits>

using namespace std;

// ======================================================
//                  ANSI COLOR CODES
// ======================================================
// These are used for colorful output in the console
const string RED_COLOR    = "\033[31m";
const string BLUE_COLOR   = "\033[34m";
const string GREEN_COLOR  = "\033[32m";
const string YELLOW_COLOR = "\033[33m";
const string RESET_COLOR  = "\033[0m";  // Reset to default

// ======================================================
//                     CARD SYSTEM
// ======================================================
enum Color { RED = 0, BLUE = 1, GREEN = 2, YELLOW = 3, WILD = 4, UNKNOWN_COLOR = -1 };
enum CardType {
    NUMBER = 0, SKIP = 10, REVERSE = 11, DRAW_TWO = 12,
    WILD_CARD = 13, WILD_DRAW_FOUR = 14
};

// Card structure to represent a UNO card
struct Card {
    Color color;
    CardType type;
    int value;

    // Constructors
    Card() : color(RED), type(NUMBER), value(0) {}
    Card(Color c, CardType t, int v) : color(c), type(t), value(v) {}

    // Converts number value to string
    string numberToString(int v) const {
        stringstream ss; ss << v; return ss.str();
    }

    // Get card color name as text
    string getColorName() const {
        switch (color) {
            case RED:    return "Red";
            case BLUE:   return "Blue";
            case GREEN:  return "Green";
            case YELLOW: return "Yellow";
            case WILD:   return "Wild";
            default:     return "Unknown";
        }
    }

    // Get card color with ANSI colors for console display
    string getColorNameColored() const {
        switch (color) {
            case RED:    return RED_COLOR + string("Red") + RESET_COLOR;
            case BLUE:   return BLUE_COLOR + string("Blue") + RESET_COLOR;
            case GREEN:  return GREEN_COLOR + string("Green") + RESET_COLOR;
            case YELLOW: return YELLOW_COLOR + string("Yellow") + RESET_COLOR;
            case WILD:   return "Wild";   // Wild usually has no single color
            default:     return "Unknown";
        }
    }

    // Get type as string
    string getTypeName() const {
        switch (type) {
            case NUMBER: return numberToString(value);
            case SKIP: return "Skip";
            case REVERSE: return "Reverse";
            case DRAW_TWO: return "+2";
            case WILD_CARD: return "Wild";
            case WILD_DRAW_FOUR: return "+4";
        }
        return "Unknown";
    }

    // Convert card to string representation (with color)
    string toString() const {
        string colorCode = "";
        switch (color) {
            case RED: colorCode = RED_COLOR; break;
            case BLUE: colorCode = BLUE_COLOR; break;
            case GREEN: colorCode = GREEN_COLOR; break;
            case YELLOW: colorCode = YELLOW_COLOR; break;
            case WILD: colorCode = ""; break;  // Wild cards default
        }

        string text = "";
        if (type == WILD_CARD || type == WILD_DRAW_FOUR)
            text = getTypeName();
        else
            text = getColorName() + " " + getTypeName();

        return colorCode + text + RESET_COLOR;
    }
};

// ======================================================
//                   GAME CONFIG STRUCT
// ======================================================
// Holds game configuration like number of players, names, and cards per player
struct GameConfig {
    int numPlayers;
    string playerNames[6];
    int cardsPerPlayer;

    GameConfig() : numPlayers(0), cardsPerPlayer(7) {}
};

// ======================================================
//                 MODULE 1: DECK MANAGEMENT
// ======================================================
// Handles creating, shuffling, dealing, and drawing cards
class DeckManagement {
private:
    queue<Card> deckQueue;  // The main deck
    GameConfig config;
    Card initialTopCard;    // First card on discard pile

public:
    // Welcome message
    void displayWelcomeMessage() {
       cout << "\033[38;2;255;182;193m============================================\n";
       cout << "           WELCOME TO UNO GAME!\n";
       cout << "============================================\033[0m\n\n";
    }

    // Input number of players, names, and cards per player
    void inputGameConfiguration() {
        do {
            cout << "Enter number of players (2-6): ";
            cin >> config.numPlayers;
            if (config.numPlayers < 2 || config.numPlayers > 6)
                cout << "\033[31m ERROR: Enter between 2 and 6 players.\033[0m\n";
        } while (config.numPlayers < 2 || config.numPlayers > 6);

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < config.numPlayers; i++) {
            cout << "Enter name for Player " << i + 1 << ": ";
            getline(cin, config.playerNames[i]);
        }

        // Input number of cards per player
        do {
            cout << "Enter number of cards per player (5-10): ";
            cin >> config.cardsPerPlayer;
            if (config.cardsPerPlayer < 5 || config.cardsPerPlayer > 10)
                cout << "\033[31m ERROR: Enter between 5 and 10 cards numbers.\033[0m\n";
        } while (config.cardsPerPlayer < 5 || config.cardsPerPlayer > 10);

        cout << "\nConfiguration saved!\n\n";
    }

    // Create a full UNO deck
    void createDeck() {
        cout << "Creating Deck...\n";
        vector<Card> tempDeck;
        Color colors[] = { RED, BLUE, GREEN, YELLOW };

        // Add number and action cards for each color
        for (int c = 0; c < 4; c++) {
            tempDeck.push_back(Card(colors[c], NUMBER, 0));
            for (int v = 1; v <= 9; v++) {
                tempDeck.push_back(Card(colors[c], NUMBER, v));
                tempDeck.push_back(Card(colors[c], NUMBER, v));
            }
            for (int s = 0; s < 2; s++) {
                tempDeck.push_back(Card(colors[c], SKIP, 20));
                tempDeck.push_back(Card(colors[c], REVERSE, 20));
                tempDeck.push_back(Card(colors[c], DRAW_TWO, 20));
            }
        }

        // Add wild cards
        for (int w = 0; w < 4; w++) {
            tempDeck.push_back(Card(WILD, WILD_CARD, 50));
            tempDeck.push_back(Card(WILD, WILD_DRAW_FOUR, 50));
        }

        // Shuffle deck
        srand(time(0));
        random_shuffle(tempDeck.begin(), tempDeck.end());

        while (!deckQueue.empty()) deckQueue.pop();
        for (size_t i = 0; i < tempDeck.size(); ++i) deckQueue.push(tempDeck[i]);

        cout << "Deck created! Total cards: " << tempDeck.size() << "\n\n";
    }

    // Deal cards to players
    void dealCards(Card playerHands[][50], int handSizes[]) {
        for (int i = 0; i < config.numPlayers; i++)
            handSizes[i] = 0;

        for (int c = 0; c < config.cardsPerPlayer; c++) {
            for (int p = 0; p < config.numPlayers; p++) {
                if (deckQueue.empty()) {
                    cout << "Deck ran out while dealing!\n";
                    return;
                }
                playerHands[p][handSizes[p]++] = deckQueue.front();
                deckQueue.pop();
            }
        }

        cout << "Cards dealt successfully!\n\n";
    }

    // Set initial top card on discard pile
    void setTopCard() {
        while (!deckQueue.empty()) {
            Card c = deckQueue.front();
            deckQueue.pop();
            if (c.type == NUMBER) {
                initialTopCard = c;
                break;
            }
            deckQueue.push(c);
        }
        cout << "Initial top card: " << initialTopCard.toString() << "\n\n";
    }

    // Draw a card from the deck
    Card drawCard() {
        if (deckQueue.empty()) return Card();
        Card c = deckQueue.front();
        deckQueue.pop();
        return c;
    }

    bool isDeckEmpty() const { return deckQueue.empty(); }
    GameConfig getConfig() const { return config; }
    Card getInitialTopCard() const { return initialTopCard; }
};

// ======================================================
//               MODULE 2: PLAYER MANAGEMENT
// ======================================================
// This class handles all player-related actions in the UNO game
class PlayerManagement {
public:

    // -------------------------------------------------
    // Display all cards in a player's hand
    // -------------------------------------------------
    void displayPlayerHand(string name, Card hand[], int size) {
        cout << "\n--- " << name << "'s Hand ---\n";   // Display player name
        for (int i = 0; i < size; i++)                  // Loop through player's cards
            cout << i + 1 << ". " << hand[i].toString() << endl;  // Show each card with index
        cout << endl;
    }

    // -------------------------------------------------
    // Check whether the played card is valid or not
    // -------------------------------------------------
    bool isValidMove(Card played, Card top) {

        // Wild cards can always be played
        if (played.type == WILD_CARD || played.type == WILD_DRAW_FOUR) 
            return true;

        // If colors match, the move is valid
        if (played.color == top.color) 
            return true;

        // If both are number cards and values are same
        if (played.type == NUMBER && top.type == NUMBER && played.value == top.value) 
            return true;

        // If both are special cards of the same type
        if (played.type != NUMBER && played.type == top.type) 
            return true;

        // Otherwise, it is an invalid move
        return false;
    }

    // -------------------------------------------------
    // Check whether the player correctly calls UNO
    // -------------------------------------------------
    void checkUNO(string name, Card hand[], int &handSize, DeckManagement &deck) {

        // If player has only 1 card left
        if (handSize == 1) {

            cout << "\nYou have 1 card left! Say 'UNO': ";
            string call;
            cin >> call;

            // If player fails to say UNO correctly
            if (call != "UNO" && call != "uno") {
                cout << "\nYou failed to say UNO! Drawing 2 penalty cards.\n";

                // Draw two penalty cards
                if (!deck.isDeckEmpty()) hand[handSize++] = deck.drawCard();
                if (!deck.isDeckEmpty()) hand[handSize++] = deck.drawCard();
            }
        }
    }

    // -------------------------------------------------
    // Handles the complete turn of a single player
    // -------------------------------------------------
    bool playerTurn(string name, Card hand[], int &handSize, Card &topCard,
                    DeckManagement &deck, bool &playedThisTurn)
    {
        playedThisTurn = false;   // Assume no card is played initially

        // Display the player's hand
        displayPlayerHand(name, hand, handSize);

        // Display the top card on the discard pile
        cout << "Top card: " << topCard.toString() << endl;

        // If the top card is a wild card, show chosen color
        if (topCard.type == WILD_CARD || topCard.type == WILD_DRAW_FOUR)
            cout << "Current color: " << topCard.getColorNameColored() << endl;

        // Ask player to choose a card number or 0 to draw
        cout << name << ", choose a card to play (0 to draw): ";
        int choice;
        cin >> choice;

        // -------------------------------------------------
        // If player chooses to draw a card
        // -------------------------------------------------
        if (choice == 0) {
            if (!deck.isDeckEmpty()) {
                Card newCard = deck.drawCard();
                cout << "You drew: " << newCard.toString() << "\n";
                hand[handSize++] = newCard;   // Add drawn card to hand
            } else {
                cout << "Deck is empty — cannot draw.\n";
            }
            return false;   // Turn ends without playing
        }

        // -------------------------------------------------
        // Validate the selected card index
        // -------------------------------------------------
        int index = choice - 1;
        if (index < 0 || index >= handSize) {
            cout << "Invalid choice! You draw 1 card.\n";
            if (!deck.isDeckEmpty()) {
                hand[handSize++] = deck.drawCard();
                cout << "You drew: " << hand[handSize - 1].toString() << "\n";
            }
            return false;
        }

        // -------------------------------------------------
        // Validate whether the selected card is playable
        // -------------------------------------------------
        if (!isValidMove(hand[index], topCard)) {
            cout << "Invalid move! You draw 1 card.\n";
            if (!deck.isDeckEmpty()) {
                hand[handSize++] = deck.drawCard();
                cout << "You drew: " << hand[handSize - 1].toString() << "\n";
            }
            return false;
        }

        // -------------------------------------------------
        // VALID MOVE
        // -------------------------------------------------
        cout << name << " played: " << hand[index].toString() << endl;

        topCard = hand[index];    // Update the top card
        playedThisTurn = true;    // Mark that a card was played

        // Remove the played card from the player's hand
        for (int i = index; i < handSize - 1; i++)
            hand[i] = hand[i + 1];

        handSize--;   // Reduce hand size

        // -------------------------------------------------
        // WIN CHECK
        // -------------------------------------------------
        if (handSize == 0) {
            cout << "\n" << name << " has no cards left! " 
                 << name << " WINS the game!\n";
            return true;   // Game ends
        }

        // Check for UNO rule
        checkUNO(name, hand, handSize, deck);

        return false;  // Game continues
    }
};

// ======================================================
//                     GAME RULES
// ======================================================
// This class handles all the game rules like direction, special cards, etc.
class GameRules {
private:
    bool isClockwise;     // Stores the direction of play (true = clockwise, false = anti-clockwise)
    bool effectApplied;   // Ensures special card effect is applied only once per turn

public:
    // Default constructor: game always starts in clockwise direction
    GameRules() : isClockwise(true), effectApplied(false) {}

    // This function returns the index of the next player based on direction
    int nextPlayer(int current, int numPlayers) {
        // If clockwise ? move forward, else ? move backward safely using modulo
        return isClockwise ? (current + 1) % numPlayers
                           : (current - 1 + numPlayers) % numPlayers;
    }

    // This function reverses the direction of play
    void reverseDirection() { 
        isClockwise = !isClockwise;  // Toggles direction
    }

    // Converts user input string into a Color enum value
    Color parseColor(string input) {
        // Convert all letters of input to lowercase
        for (int i = 0; i < (int)input.size(); i++)
            input[i] = tolower(input[i]);

        // Match input with valid colors
        if (input == "red") return RED;
        if (input == "blue") return BLUE;
        if (input == "green") return GREEN;
        if (input == "yellow") return YELLOW;

        // If input does not match any valid color
        return UNKNOWN_COLOR;
    }

    // Resets the special card effect flag for the next turn
    void resetEffectFlag() { 
        effectApplied = false; 
    }

    // This function applies special effects of cards like SKIP, DRAW_TWO, WILD, etc.
    void applySpecialCard(Card &playedCard,      // The card that was just played
                          int &currentPlayer,    // Current player's index (passed by reference)
                          int numPlayers,        // Total number of players
                          Card playerHands[][50],// 2D array storing all player cards
                          int handSizes[],       // Stores number of cards each player has
                          DeckManagement &deck)  // Deck object to draw more cards
    {
        // Stop if effect was already applied
        if (effectApplied) return;

        // If card is a normal number card, no special effect applies
        if (playedCard.type == NUMBER) return;

        // Apply effect based on card type
        switch (playedCard.type) {

            // -------- SKIP CARD --------
            case SKIP: {
                cout << "Next player is skipped!\n";
                currentPlayer = nextPlayer(currentPlayer, numPlayers); // Skip next player
                break;
            }

            // -------- REVERSE CARD --------
            case REVERSE: {
                if (numPlayers == 2) {
                    // With only 2 players, reverse acts like skip
                    cout << "Reverse card played!\n";
                    currentPlayer = nextPlayer(currentPlayer, numPlayers);
                } else {
                    // With more than 2 players, reverse direction
                    cout << "Direction reversed!\n";
                    reverseDirection();
                }
                break;
            }

            // -------- DRAW TWO CARD --------
            case DRAW_TWO: {
                int next = nextPlayer(currentPlayer, numPlayers);
                cout << "Player " << next + 1 << " draws 2 cards!\n";

                // Next player draws two cards from the deck
                for (int i = 0; i < 2; i++) {
                    if (!deck.isDeckEmpty()) {
                        playerHands[next][handSizes[next]++] = deck.drawCard();
                    }
                }

                currentPlayer = next; // Turn moves to the next player
                break;
            }

            // -------- WILD CARD --------
            case WILD_CARD: {
                string choice;

                // Keep asking until valid color is entered
                do {
                    cout << "Choose a color: "
                         << RED_COLOR << "red "
                         << BLUE_COLOR << "blue "
                         << GREEN_COLOR << "green "
                         << YELLOW_COLOR << "yellow"
                         << RESET_COLOR << "\n";

                    cin >> choice;
                    playedCard.color = parseColor(choice);

                    // Display error for invalid input
                    if (playedCard.color == UNKNOWN_COLOR)
                        cout << "\033[31mERROR: Invalid color. Please enter red, blue, green, or yellow.\033[0m\n";

                } while (playedCard.color == UNKNOWN_COLOR);

                cout << "Color chosen: " << playedCard.getColorNameColored() << "\n";
                break;
            }

            // -------- WILD DRAW FOUR CARD --------
            case WILD_DRAW_FOUR: {
                int next = nextPlayer(currentPlayer, numPlayers);

                cout << "Player " << next + 1 << " draws 4 cards!\n";

                // Next player draws four cards
                for (int i = 0; i < 4; i++) {
                    if (!deck.isDeckEmpty()) {
                        playerHands[next][handSizes[next]++] = deck.drawCard();
                    }
                }

                // Keep asking until valid color is entered
                string choice;
                do {
                    cout << "Choose a color: "
                         << RED_COLOR << "red "
                         << BLUE_COLOR << "blue "
                         << GREEN_COLOR << "green "
                         << YELLOW_COLOR << "yellow"
                         << RESET_COLOR << "\n";

                    cin >> choice;
                    playedCard.color = parseColor(choice);

                    // Display error for invalid input
                    if (playedCard.color == UNKNOWN_COLOR)
                        cout << "\033[31mERROR: Invalid color. Please enter red, blue, green, or yellow.\033[0m\n";

                } while (playedCard.color == UNKNOWN_COLOR);

                cout << "Color chosen: " << playedCard.getColorNameColored() << "\n";

                // Turn also moves to the next player
                currentPlayer = next;
                break;
            }
        }

        // Mark effect as applied so it does not repeat
        effectApplied = true;
    }
};

// ======================================================
//                      MAIN PROGRAM
// ======================================================
int main() {
    DeckManagement module1;
    PlayerManagement module2;
    GameRules module3;

    Card playerHands[6][50];  // 6 players max, 50 cards max per player
    int handSizes[6] = {0};

    // Initial setup
    module1.displayWelcomeMessage();
    module1.inputGameConfiguration();
    module1.createDeck();
    GameConfig config = module1.getConfig();
    module1.dealCards(playerHands, handSizes);
    module1.setTopCard();

    Card topCard = module1.getInitialTopCard();
    cout << "\033[95m---------- GAME START ----------\033[0m\n";

    int currentPlayer = 0;
    bool gameOver = false;

    // Main game loop
    while (!module1.isDeckEmpty() && !gameOver) {
        cout << "\n--- " << config.playerNames[currentPlayer] << "'s TURN ---\n";

        bool playedThisTurn = false;
        bool won = module2.playerTurn(
            config.playerNames[currentPlayer],
            playerHands[currentPlayer],
            handSizes[currentPlayer],
            topCard,
            module1,
            playedThisTurn
        );

        if (playedThisTurn) {
            module3.resetEffectFlag();  // allow special card effect
        }

        if (won) {
            gameOver = true;
            break;
        }

        // Apply special card effects
        module3.applySpecialCard(
            topCard,
            currentPlayer,
            config.numPlayers,
            playerHands,
            handSizes,
            module1
        );

        // Move to next player
        currentPlayer = module3.nextPlayer(currentPlayer, config.numPlayers);
    }

    if (!gameOver)
        cout << "\033[95m\nDeck ended — game results in a draw.\033[0m\n";

    return 0;
}


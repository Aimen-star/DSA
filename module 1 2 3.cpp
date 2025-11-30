#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

using namespace std;

// ======================================================
//                     CARD SYSTEM
// ======================================================
enum Color { RED = 0, BLUE = 1, GREEN = 2, YELLOW = 3, WILD = 4 };
enum CardType {
    NUMBER = 0, SKIP = 10, REVERSE = 11, DRAW_TWO = 12,
    WILD_CARD = 13, WILD_DRAW_FOUR = 14
};

struct Card {
    Color color;
    CardType type;
    int value;

    Card() : color(RED), type(NUMBER), value(0) {}
    Card(Color c, CardType t, int v) : color(c), type(t), value(v) {}

    string numberToString(int v) const {
        stringstream ss;
        ss << v;
        return ss.str();
    }

    string getColorName() const {
        switch (color) {
            case RED: return "Red";
            case BLUE: return "Blue";
            case GREEN: return "Green";
            case YELLOW: return "Yellow";
            case WILD: return "Wild";
        }
        return "Unknown";
    }

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

    string toString() const {
        if (type == WILD_CARD || type == WILD_DRAW_FOUR)
            return getTypeName();
        return getColorName() + " " + getTypeName();
    }
};

// ======================================================
//                   GAME CONFIG STRUCT
// ======================================================
struct GameConfig {
    int numPlayers;
    string playerNames[4];
    int cardsPerPlayer;

    GameConfig() : numPlayers(0), cardsPerPlayer(7) {}
};

// ======================================================
//                 MODULE 1: DECK MANAGEMENT
// ======================================================
class DeckManagement {
private:
    queue<Card> deckQueue;
    GameConfig config;
    Card initialTopCard;

public:
    void displayWelcomeMessage() {
        cout << "============================================\n";
        cout << "           WELCOME TO UNO GAME!\n";
        cout << "============================================\n\n";
    }

    void inputGameConfiguration() {
        do {
            cout << "Enter number of players (2-4): ";
            cin >> config.numPlayers;
        } while (config.numPlayers < 2 || config.numPlayers > 4);

        cin.ignore();
        for (int i = 0; i < config.numPlayers; i++) {
            cout << "Enter name for Player " << i + 1 << ": ";
            getline(cin, config.playerNames[i]);
        }

        do {
            cout << "Enter number of cards per player (5-10): ";
            cin >> config.cardsPerPlayer;
        } while (config.cardsPerPlayer < 5 || config.cardsPerPlayer > 10);

        cout << "\nConfiguration saved!\n\n";
    }

    void createDeck() {
        cout << "Creating Deck...\n";
        vector<Card> tempDeck;
        Color colors[] = { RED, BLUE, GREEN, YELLOW };

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

        for (int w = 0; w < 4; w++) {
            tempDeck.push_back(Card(WILD, WILD_CARD, 50));
            tempDeck.push_back(Card(WILD, WILD_DRAW_FOUR, 50));
        }

        srand((unsigned)time(0));
        random_shuffle(tempDeck.begin(), tempDeck.end());

        while (!deckQueue.empty())
            deckQueue.pop();

        for (size_t i = 0; i < tempDeck.size(); i++)
            deckQueue.push(tempDeck[i]);

        cout << "Deck created! Total cards: " << tempDeck.size() << "\n\n";
    }

    void dealCards(Card playerHands[][50], int handSizes[]) {
        for (int i = 0; i < config.numPlayers; i++)
            handSizes[i] = 0;

        for (int c = 0; c < config.cardsPerPlayer; c++) {
            for (int p = 0; p < config.numPlayers; p++) {
                playerHands[p][handSizes[p]++] = deckQueue.front();
                deckQueue.pop();
            }
        }

        cout << "Cards dealt successfully!\n\n";
    }

    void setTopCard() {
        initialTopCard = deckQueue.front();
        deckQueue.pop();
        cout << "Initial top card: " << initialTopCard.toString() << "\n\n";
    }

    Card drawCard() {
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
class PlayerManagement {
public:

    void displayPlayerHand(string name, Card hand[], int size) {
        cout << "\n--- " << name << "'s Hand ---\n";
        for (int i = 0; i < size; i++)
            cout << i + 1 << ". " << hand[i].toString() << endl;
        cout << endl;
    }

    bool isValidMove(Card played, Card top) {
        return (played.color == top.color || played.type == top.type ||
                played.type == WILD_CARD || played.type == WILD_DRAW_FOUR);
    }

    void checkUNO(string name, Card hand[], int &handSize, DeckManagement &deck) {
        if (handSize == 1) {
            cout << "\nYou have 1 card left! Say 'UNO': ";
            string call;
            cin >> call;

            if (call != "UNO" && call != "uno") {
                cout << "\nYou failed to say UNO! Drawing 2 penalty cards.\n";
                if (!deck.isDeckEmpty()) hand[handSize++] = deck.drawCard();
                if (!deck.isDeckEmpty()) hand[handSize++] = deck.drawCard();
            }
        }
    }

    // RETURN TRUE = PLAYER WON (GAME SHOULD END)
    bool playerTurn(string name,
                    Card hand[], int &handSize,
                    Card &topCard,
                    DeckManagement &deck)
    {
        displayPlayerHand(name, hand, handSize);

        cout << "Top card: " << topCard.toString() << endl;
        cout << name << ", choose a card to play (0 to draw): ";
        int choice;
        cin >> choice;

        if (choice == 0) {
            if (!deck.isDeckEmpty()) {
                Card newCard = deck.drawCard();
                cout << "You drew: " << newCard.toString() << "\n";
                hand[handSize++] = newCard;
            }
            return false;
        }

        int index = choice - 1;
        if (index < 0 || index >= handSize) {
            cout << "Invalid choice! You draw 1 card.\n";
            if (!deck.isDeckEmpty()) {
                hand[handSize++] = deck.drawCard();
                cout << "You drew: " << hand[handSize - 1].toString() << "\n";
            }
            return false;
        }

        if (!isValidMove(hand[index], topCard)) {
            cout << "Invalid move! You draw 1 card.\n";
            if (!deck.isDeckEmpty()) {
                hand[handSize++] = deck.drawCard();
                cout << "You drew: " << hand[handSize - 1].toString() << "\n";
            }
            return false;
        }

        // VALID MOVE
        cout << name << " played: " << hand[index].toString() << endl;
        topCard = hand[index];

        for (int i = index; i < handSize - 1; i++)
            hand[i] = hand[i + 1];
        handSize--;

        // WIN CHECK
        if (handSize == 0) {
            cout << "\n" << name << " has no cards left! " << name << " WINS the game!\n";
            return true;   // SIGNAL GAME END
        }

        checkUNO(name, hand, handSize, deck);
        return false;
    }
};

// ======================================================
//                     GAME RULES
// ======================================================
class GameRules {
private:
    bool isClockwise;

public:
    GameRules() : isClockwise(true) {}

    int nextPlayer(int current, int numPlayers) {
        return isClockwise ? (current + 1) % numPlayers
                           : (current - 1 + numPlayers) % numPlayers;
    }

    void reverseDirection() { isClockwise = !isClockwise; }

    Color parseColor(string input) {
        for (int i = 0; i < input.size(); i++)
            input[i] = tolower(input[i]);

        if (input == "red") return RED;
        if (input == "blue") return BLUE;
        if (input == "green") return GREEN;
        if (input == "yellow") return YELLOW;
        return WILD;
    }

    void applySpecialCard(Card &playedCard,
                          int &currentPlayer,
                          int numPlayers,
                          Card playerHands[4][50],
                          int handSizes[4],
                          DeckManagement &deck)
    {
        switch (playedCard.type) {
            case SKIP: {
                cout << "Next player is skipped!\n";
                currentPlayer = nextPlayer(currentPlayer, numPlayers);
                break;
            }
            case REVERSE: {
                cout << "Direction reversed!\n";
                reverseDirection();
                break;
            }
            case DRAW_TWO: {
                int next = nextPlayer(currentPlayer, numPlayers);
                cout << "Player " << next + 1 << " draws 2 cards!\n";
                for (int i = 0; i < 2; i++)
                    if (!deck.isDeckEmpty())
                        playerHands[next][handSizes[next]++] = deck.drawCard();
                currentPlayer = next;
                break;
            }
            case WILD_CARD: {
                cout << "Choose a color: ";
                string choice;
                cin >> choice;
                playedCard.color = parseColor(choice);
                break;
            }
            case WILD_DRAW_FOUR: {
                int next = nextPlayer(currentPlayer, numPlayers);
                cout << "Player " << next + 1 << " draws 4 cards!\n";
                for (int i = 0; i < 4; i++)
                    if (!deck.isDeckEmpty())
                        playerHands[next][handSizes[next]++] = deck.drawCard();

                cout << "Choose a color: ";
                string choice;
                cin >> choice;
                playedCard.color = parseColor(choice);

                currentPlayer = next;
                break;
            }
        }
    }
};

// ======================================================
//                      MAIN PROGRAM
// ======================================================
int main() {
    DeckManagement module1;
    PlayerManagement module2;
    GameRules module3;

    Card playerHands[4][50];
    int handSizes[4] = {0};

    module1.displayWelcomeMessage();
    module1.inputGameConfiguration();
    module1.createDeck();

    GameConfig config = module1.getConfig();

    module1.dealCards(playerHands, handSizes);
    module1.setTopCard();

    Card topCard = module1.getInitialTopCard();
    cout << "---------- GAME START ----------\n";

    int currentPlayer = 0;
    bool gameOver = false;

    while (!module1.isDeckEmpty() && !gameOver) {
        cout << "\n--- " << config.playerNames[currentPlayer] << "'s TURN ---\n";

        bool won = module2.playerTurn(
            config.playerNames[currentPlayer],
            playerHands[currentPlayer],
            handSizes[currentPlayer],
            topCard,
            module1
        );

        if (won) {
            gameOver = true;
            break;
        }

        module3.applySpecialCard(
            topCard,
            currentPlayer,
            config.numPlayers,
            playerHands,
            handSizes,
            module1
        );

        currentPlayer = module3.nextPlayer(currentPlayer, config.numPlayers);
    }

    if (!gameOver)
        cout << "\nDeck ended — game results in a draw.\n";

    return 0;
}


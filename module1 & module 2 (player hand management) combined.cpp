#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

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
            case NUMBER: return to_string(value);
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

        for (auto &card : tempDeck)
            deckQueue.push(card);

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
        for (int i = 0; i < size; i++) {
            cout << i + 1 << ". " << hand[i].toString() << endl;
        }
        cout << endl;
    }

    bool isValidMove(Card played, Card top) {
        if (played.color == top.color) return true;
        if (played.type == top.type) return true;
        if (played.type == WILD_CARD || played.type == WILD_DRAW_FOUR) return true;
        return false;
    }

    // ========== UNO RULE ADDED HERE ==========
    void checkUNO(string name, Card hand[], int &handSize, DeckManagement &deck) {
        if (handSize == 1) {
            cout << "\n? You have 1 card left! Say 'UNO': ";
            string call;
            cin >> call;

            if (call != "UNO" && call != "uno") {
                cout << "\n? You failed to say UNO! Drawing 2 penalty cards.\n";
                if (!deck.isDeckEmpty()) hand[handSize++] = deck.drawCard();
                if (!deck.isDeckEmpty()) hand[handSize++] = deck.drawCard();
            }
        }
    }

    // Performs a single player's action
    bool playerTurn(
        string name,
        Card hand[], int &handSize,
        Card &topCard,
        DeckManagement &deck
    ) {
        displayPlayerHand(name, hand, handSize);

        cout << "Top card: " << topCard.toString() << endl;
        cout << name << ", choose a card to play (0 to draw): ";
        int choice;
        cin >> choice;

        if (choice == 0) {
            Card newCard = deck.drawCard();
            cout << "You drew: " << newCard.toString() << "\n";
            hand[handSize++] = newCard;
            return false;
        }

        int index = choice - 1;
        if (index < 0 || index >= handSize) {
            cout << "Invalid choice! Turn skipped.\n";
            return false;
        }

        if (!isValidMove(hand[index], topCard)) {
            cout << "Invalid move! Cannot play this card. Turn skipped.\n";
            return false;
        }

        cout << name << " played: " << hand[index].toString() << endl;
        topCard = hand[index];

        for (int i = index; i < handSize - 1; i++)
            hand[i] = hand[i + 1];
        handSize--;

        // ?? APPLY UNO RULE
        checkUNO(name, hand, handSize, deck);

        return true;
    }
};

// ======================================================
//                      MAIN PROGRAM
// ======================================================
int main() {
    DeckManagement module1;
    PlayerManagement module2;

    Card playerHands[4][50];
    int handSizes[4] = {0};

    module1.displayWelcomeMessage();
    module1.inputGameConfiguration();
    module1.createDeck();

    GameConfig config = module1.getConfig();

    module1.dealCards(playerHands, handSizes);
    module1.setTopCard();

    Card topCard = module1.getInitialTopCard();

    cout << "---------- GAME START (Module 1 + Module 2 only) ----------\n";

    string winner = "";

    while (winner == "") {
        for (int p = 0; p < config.numPlayers; p++) {

            if (handSizes[p] == 0) continue;

            cout << "\n--- " << config.playerNames[p] << "'s TURN ---\n";

            module2.playerTurn(
                config.playerNames[p],
                playerHands[p],
                handSizes[p],
                topCard,
                module1
            );

            if (handSizes[p] == 0) {
                winner = config.playerNames[p];
                break;
            }
        }

        if (module1.isDeckEmpty())
            break;
    }

    if (winner != "")
        cout << "\n?? " << winner << " wins the game! ??\n";
    else
        cout << "\nDeck is empty — game ends in a draw.\n";

    return 0;
}



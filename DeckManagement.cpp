#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// Defining card
enum Color {
    RED = 0, BLUE = 1, GREEN = 2, YELLOW = 3, WILD = 4
};

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
            default: return "Unknown";
        }
    }
    
    string getTypeName() const {
        switch (type) {
            case NUMBER: return to_string(value);
            case SKIP: return "Skip";
            case REVERSE: return "Reverse";
            case DRAW_TWO: return "+2";
            case WILD_CARD: return "Wild";
            case WILD_DRAW_FOUR: return "+4";
            default: return "Unknown";
        }
    }
    
    string toString() const {
        if (type == WILD_CARD || type == WILD_DRAW_FOUR) {
            return getTypeName();
        }
        return getColorName() + " " + getTypeName();
    }
};

struct GameConfig {
    int numPlayers;
    string playerNames[4];
    int cardsPerPlayer;
    
    GameConfig() : numPlayers(0), cardsPerPlayer(7) {}
};
class DeckManagement {
private:
    queue<Card> deckQueue;
    GameConfig config;
    Card initialTopCard;
    
public:
    DeckManagement() {}
    
    void displayWelcomeMessage() {
        cout << "=========================================\n";
        cout << "       WELCOME TO UNO CARD GAME\n";
        cout << "=========================================\n\n";
    }
    
    void inputGameConfiguration() {
        do {
            cout << "Enter number of players (2-4): ";
            cin >> config.numPlayers;
        } while (config.numPlayers < 2 || config.numPlayers > 4);
        
        cin.ignore();
        for (int i = 0; i < config.numPlayers; i++) {
            cout << "Enter name for Player " << (i + 1) << ": ";
            getline(cin, config.playerNames[i]);
        }
        
        do {
            cout << "Enter number of cards per player (5-10): ";
            cin >> config.cardsPerPlayer;
        } while (config.cardsPerPlayer < 5 || config.cardsPerPlayer > 10);
        
        cout << "\nConfiguration saved!\n";
        cout << "Players: " << config.numPlayers << "\n";
        cout << "Cards per player: " << config.cardsPerPlayer << "\n\n";
    }
    
    void createDeck() {
        cout << "=== Deck Creation ===" << endl;
        vector<Card> tempDeck;
        Color colors[] = {RED, BLUE, GREEN, YELLOW};
        
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
        
        cout << "Deck created with " << tempDeck.size() << " cards.\n";
        
        srand(static_cast<unsigned>(time(0)));
        for (int i = tempDeck.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(tempDeck[i], tempDeck[j]);
        }
        
        while (!deckQueue.empty()) deckQueue.pop();
        
        for (size_t i = 0; i < tempDeck.size(); i++) {
            deckQueue.push(tempDeck[i]);
        }
    }
    
    void shuffleDeck() {
        cout << "\n=== Deck shuffling ===\n";
        cout << "Deck already shuffled during creation.\n";
    }
    
    void dealCards(Card playerHands[][50], int handSizes[]) { 
        cout << "\n=== Card Dealing ===\n";
        for (int i = 0; i < config.numPlayers; i++) {
            handSizes[i] = 0;
        }
        
        for (int card = 0; card < config.cardsPerPlayer; card++) {
            for (int player = 0; player < config.numPlayers; player++) {
                if (!deckQueue.empty()) {
                    playerHands[player][handSizes[player]++] = deckQueue.front();
                    deckQueue.pop();
                }
            }
        }
        
        cout << "Dealt " << config.cardsPerPlayer << " cards to each player.\n";
        for (int i = 0; i < config.numPlayers; i++) {
            cout << config.playerNames[i] << " received " 
                 << handSizes[i] << " cards.\n";
        }
        cout << "\n";
    }
    
    void setTopCard() {
        cout << "\n=== Top Card ===\n";
        if (!deckQueue.empty()) {
            initialTopCard = deckQueue.front();
            deckQueue.pop();
            cout << "Initial top card set: " << initialTopCard.toString() << "\n";
        }
        cout << "\n";
    }
    
    Card drawCard() {
        if (!deckQueue.empty()) {
            Card drawn = deckQueue.front();
            deckQueue.pop();
            return drawn;
        }
        return Card();
    }
    
    GameConfig getConfig() const {
         return config; 
        }
    Card getInitialTopCard() const {
         return initialTopCard; 
        }
    bool isDeckEmpty() const {
         return deckQueue.empty(); 
        }
};

int main() {
    DeckManagement module1;
    
    module1.displayWelcomeMessage();
    module1.inputGameConfiguration();
    module1.createDeck();
    module1.shuffleDeck();
    
    GameConfig config = module1.getConfig();
    Card playerHands[4][50];
    int handSizes[4] = {0};
    
    module1.dealCards(playerHands, handSizes);
    module1.setTopCard();
    
    return 0;
}

#include <iostream>
#include <string>
using namespace std;

// ---------------- Book Class ----------------
class Book {
private:
    string bookId;
    string bookName;
    double bookPrice;
    string bookAuthor;
    string bookISBN;

public:
    // Default Constructor
    Book() {
        bookId = "";
        bookName = "";
        bookPrice = 0.0;
        bookAuthor = "";
        bookISBN = "";
    }

    // Parameterized Constructor
    Book(string id, string name, double price, string author, string isbn) {
        bookId = id;
        bookName = name;
        bookPrice = price;
        bookAuthor = author;
        bookISBN = isbn;
    }

    // Getters
    string getBookId() { return bookId; }
    string getBookName() { return bookName; }
    double getBookPrice() { return bookPrice; }
    string getBookAuthor() { return bookAuthor; }
    string getBookISBN() { return bookISBN; }

    // Setters
    void setBookId(string id) { bookId = id; }
    void setBookName(string name) { bookName = name; }
    void setBookPrice(double price) { bookPrice = price; }
    void setBookAuthor(string author) { bookAuthor = author; }
    void setBookISBN(string isbn) { bookISBN = isbn; }

    // Display book details
    void display() {
        cout << "Book ID: " << bookId
             << ", Name: " << bookName
             << ", Price: " << bookPrice
             << ", Author: " << bookAuthor
             << ", ISBN: " << bookISBN << endl;
    }
};

// ---------------- Node Class ----------------
class Node {
private:
    Book book;
    Node* next;
    Node* prev;

public:
    // Default Constructor
    Node() {
        next = prev = NULL;
    }

    // Parameterized Constructor
    Node(Book b) {
        book = b;
        next = prev = NULL;
    }

    // Getters & Setters
    Book getBook() { return book; }
    void setBook(Book b) { book = b; }

    Node* getNext() { return next; }
    void setNext(Node* n) { next = n; }

    Node* getPrev() { return prev; }
    void setPrev(Node* p) { prev = p; }
};

// ---------------- BookList Class ----------------
class BookList {
private:
    Node* head;

public:
    BookList() { head = NULL; }

    // Add Book
    void addBook(string id, string name, double price, string author, string isbn) {
        Book b(id, name, price, author, isbn);
        Node* newNode = new Node(b);

        if (head == NULL) {
            head = newNode;
            head->setNext(head);
            head->setPrev(head);
        } else {
            Node* tail = head->getPrev();
            tail->setNext(newNode);
            newNode->setPrev(tail);
            newNode->setNext(head);
            head->setPrev(newNode);
        }
        cout << "Book with ID " << id << " added successfully!" << endl;
    }

    // Remove Book
    void removeBook(string id) {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }

        Node* curr = head;
        do {
            if (curr->getBook().getBookId() == id) {
                if (curr->getNext() == curr) { // only one node
                    delete curr;
                    head = NULL;
                } else {
                    Node* prev = curr->getPrev();
                    Node* next = curr->getNext();
                    prev->setNext(next);
                    next->setPrev(prev);
                    if (curr == head)
                        head = next;
                    delete curr;
                }
                cout << "Book with ID " << id << " removed successfully!" << endl;
                return;
            }
            curr = curr->getNext();
        } while (curr != head);

        cout << "Book with ID " << id << " not found!" << endl;
    }

    // Update Book
    void updateBook(string id, string name, double price, string author, string isbn) {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }

        Node* curr = head;
        do {
            if (curr->getBook().getBookId() == id) {
                Book updatedBook(id, name, price, author, isbn);
                curr->setBook(updatedBook);
                cout << "Book with ID " << id << " updated successfully!" << endl;
                return;
            }
            curr = curr->getNext();
        } while (curr != head);

        cout << "Book with ID " << id << " not found!" << endl;
    }

    // Print all books
    void printBooks() {
        if (head == NULL) {
            cout << "No books in the list!" << endl;
            return;
        }

        Node* curr = head;
        cout << "--- Book List ---" << endl;
        do {
            curr->getBook().display();
            curr = curr->getNext();
        } while (curr != head);
    }

    // Print a particular book
    void printBook(string id) {
        if (head == NULL) {
            cout << "No books in the list!" << endl;
            return;
        }

        Node* curr = head;
        do {
            if (curr->getBook().getBookId() == id) {
                curr->getBook().display();
                return;
            }
            curr = curr->getNext();
        } while (curr != head);

        cout << "Book with ID " << id << " not found!" << endl;
    }
};

// ---------------- Main Function ----------------
int main() {
    BookList list;

    // Add 10 books
    for (int i = 1; i <= 10; i++) {
        list.addBook("B" + to_string(i), "Book" + to_string(i), i * 100, "Author" + to_string(i), "ISBN" + to_string(i));
    }

    // Print one book
    cout << "\nPrinting one book (B5):" << endl;
    list.printBook("B5");

    // Remove two books
    cout << "\nRemoving B3:" << endl;
    list.removeBook("B3"); // valid
    cout << "\nTrying to remove invalid B30:" << endl;
    list.removeBook("B30"); // invalid

    // Print one book again
    cout << "\nPrinting one book (B5):" << endl;
    list.printBook("B5");

    // Update a book
    cout << "\nUpdating B5:" << endl;
    list.updateBook("B5", "UpdatedBook5", 555.5, "UpdatedAuthor5", "UpdatedISBN5");

    // Print updated book
    cout << "\nPrinting updated B5:" << endl;
    list.printBook("B5");

    // Print all books
    cout << "\nPrinting all books:" << endl;
    list.printBooks();

    return 0;
}

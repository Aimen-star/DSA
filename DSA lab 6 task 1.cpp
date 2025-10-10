#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
};

class CircularLinkedList {
private:
    Node* head;

public:
    CircularLinkedList() {
        head = NULL;
    }

    // InsertBefore (add at beginning)
    void insertBefore(int value) {
        Node* newNode = new Node();
        newNode->data = value;

        if (head == NULL) {
            head = newNode;
            head->next = head; // self-loop
        } else {
            Node* temp = head;
            while (temp->next != head) {
                temp = temp->next;
            }
            newNode->next = head;
            temp->next = newNode;
            head = newNode; // update head
        }
        cout << value << " inserted at beginning!" << endl;
    }

    // InsertAfter (add at end)
    void insertAfter(int value) {
        Node* newNode = new Node();
        newNode->data = value;

        if (head == NULL) {
            head = newNode;
            head->next = head;
        } else {
            Node* temp = head;
            while (temp->next != head) {
                temp = temp->next;
            }
            temp->next = newNode;
            newNode->next = head;
        }
        cout << value << " inserted at end!" << endl;
    }

    // Delete node (by value)
    void deleteNode(int value) {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }

        Node* curr = head;
        Node* prev = NULL;

        // case 1: only one node
        if (head->data == value && head->next == head) {
            delete head;
            head = NULL;
            cout << value << " deleted (only node)." << endl;
            return;
        }

        // case 2: deleting head
        if (head->data == value) {
            Node* last = head;
            while (last->next != head) {
                last = last->next;
            }
            last->next = head->next;
            Node* del = head;
            head = head->next;
            delete del;
            cout << value << " deleted (head node)." << endl;
            return;
        }

        // case 3: deleting other nodes
        do {
            prev = curr;
            curr = curr->next;
            if (curr->data == value) {
                prev->next = curr->next;
                delete curr;
                cout << value << " deleted successfully." << endl;
                return;
            }
        } while (curr != head);

        cout << value << " not found!" << endl;
    }

    // Display
    void display() {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }
        Node* temp = head;
        cout << "Circular Linked List: ";
        do {
            cout << temp->data << " ";
            temp = temp->next;
        } while (temp != head);
        cout << endl;
    }
};

// Main function
int main() {
    CircularLinkedList cll;

    cll.insertBefore(10);
    cll.insertBefore(20);
    cll.insertAfter(30);
    cll.insertAfter(40);

    cll.display();

    cll.deleteNode(20); // delete head
    cll.display();

    cll.deleteNode(40); // delete last
    cll.display();

    cll.deleteNode(99); // invalid
    cll.display();

    return 0;
}

#include <iostream>
#include <string>
using namespace std;

// Node structure
struct Node {
    string name;
    Node* next;
};

class CircularLinkedList {
private:
    Node* head;

public:
    CircularLinkedList() {
        head = NULL;
    }

    // Add Employee
    void addEmployee(string empName) {
        Node* newNode = new Node();
        newNode->name = empName;

        if (head == NULL) {
            head = newNode;
            newNode->next = head;  // points to itself
        } else {
            Node* temp = head;
            while (temp->next != head) {
                temp = temp->next;
            }
            temp->next = newNode;
            newNode->next = head;
        }
        cout << empName << " added successfully!" << endl;
    }

    // Delete Employee
    void deleteEmployee(string empName) {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }

        Node* curr = head;
        Node* prev = NULL;

        // searching for employee
        do {
            if (curr->name == empName) {
                if (curr == head) { // deleting head node
                    Node* last = head;
                    while (last->next != head) {
                        last = last->next;
                    }
                    if (head == head->next) { // only 1 node
                        delete head;
                        head = NULL;
                    } else {
                        last->next = head->next;
                        Node* del = head;
                        head = head->next;
                        delete del;
                    }
                } else { // deleting other node
                    prev->next = curr->next;
                    delete curr;
                }
                cout << empName << " deleted successfully!" << endl;
                return;
            }
            prev = curr;
            curr = curr->next;
        } while (curr != head);

        cout << empName << " not found!" << endl;
    }

    // Update Employee
    void updateEmployee(string oldName, string newName) {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }

        Node* curr = head;
        do {
            if (curr->name == oldName) {
                curr->name = newName;
                cout << "Updated successfully! " << oldName << " -> " << newName << endl;
                return;
            }
            curr = curr->next;
        } while (curr != head);

        cout << oldName << " not found!" << endl;
    }

    // Search Employee
    void searchEmployee(string empName) {
        if (head == NULL) {
            cout << "List is empty!" << endl;
            return;
        }

        Node* curr = head;
        do {
            if (curr->name == empName) {
                cout << empName << " found successfully!" << endl;
                return;
            }
            curr = curr->next;
        } while (curr != head);

        cout << empName << " not found!" << endl;
    }

    // Display all employees
    void displayEmployees() {
        if (head == NULL) {
            cout << "No employees in the list!" << endl;
            return;
        }

        Node* curr = head;
        cout << "Employees: ";
        do {
            cout << curr->name << " ";
            curr = curr->next;
        } while (curr != head);
        cout << endl;
    }
};

// Main function
int main() {
    CircularLinkedList list;
    int choice;
    string name, newName;

    do {
        cout << "\n--- Employee Management ---\n";
        cout << "1. Add Employee\n";
        cout << "2. Delete Employee\n";
        cout << "3. Update Employee\n";
        cout << "4. Search Employee\n";
        cout << "5. Display All Employees\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter name to add: ";
            cin >> name;
            list.addEmployee(name);
            break;
        case 2:
            cout << "Enter name to delete: ";
            cin >> name;
            list.deleteEmployee(name);
            break;
        case 3:
            cout << "Enter old name: ";
            cin >> name;
            cout << "Enter new name: ";
            cin >> newName;
            list.updateEmployee(name, newName);
            break;
        case 4:
            cout << "Enter name to search: ";
            cin >> name;
            list.searchEmployee(name);
            break;
        case 5:
            list.displayEmployees();
            break;
        case 6:
            cout << "Exiting program..." << endl;
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 6);

    return 0;
}

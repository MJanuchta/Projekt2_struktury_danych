#include <iostream>
#include <chrono>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;
using namespace std::chrono;

struct Node {
    int data;
    int priority;
    Node* prev;
    Node* next;
    Node(int d, int p) : data(d), priority(p), prev(nullptr), next(nullptr) {}
};


class UnsortedDoublyLinkedListPQ {
private:
    Node* head;
    Node* tail;
    int size;

public:
    UnsortedDoublyLinkedListPQ() : head(nullptr), tail(nullptr), size(0) {}

    ~UnsortedDoublyLinkedListPQ() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void enqueue(int data, int priority) {
        Node* newNode = new Node(data, priority);
        if (!tail) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }

    void dequeue() {
        if (!head) return;
        if (head == tail) {
            delete head;
            head = tail = nullptr;
            size--;
            return;
        }

        Node* maxNode = head;
        Node* current = head->next;

        while (current) {
            if (current->priority > maxNode->priority) {
                maxNode = current;
            }
            current = current->next;
        }

        if (maxNode == head) {
            head = head->next;
            head->prev = nullptr;
        }
        else if (maxNode == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        }
        else {
            maxNode->prev->next = maxNode->next;
            maxNode->next->prev = maxNode->prev;
        }
        delete maxNode;
        size--;
    }

    int getSize() {
        return size;
    }

    Node peek() {
        if (!head) throw out_of_range("Kolejka jest pusta");
        Node* maxNode = head;
        Node* current = head->next;
        while (current) {
            if (current->priority > maxNode->priority) {
                maxNode = current;
            }
            current = current->next;
        }
        return *maxNode;
    }

    void modifyPriority(int data, int newPriority) {
        Node* current = head;
        while (current) {
            if (current->data == data) {
                current->priority = newPriority;
                return; 
            }
            current = current->next;
        }
    }
};

double runBenchmark(int operationChoice, int n, int iterations) {
    double totalTimeNs = 0;

    for (int i = 0; i < iterations; ++i) {
        UnsortedDoublyLinkedListPQ pq;

        for (int j = 0; j < n; ++j) {
            pq.enqueue(j, j);
        }

        auto start = high_resolution_clock::now();
        auto end = start;

        switch (operationChoice) {
        case 1: { 
            start = high_resolution_clock::now();
            pq.enqueue(-1, n + 1);
            end = high_resolution_clock::now();
            break;
        }
        case 2: { 
            start = high_resolution_clock::now();
            pq.dequeue();
            end = high_resolution_clock::now();
            break;
        }
        case 3: { 
            start = high_resolution_clock::now();
            volatile int s = pq.getSize(); (void)s;
            end = high_resolution_clock::now();
            break;
        }
        case 4: {
            start = high_resolution_clock::now();
            volatile Node e = pq.peek(); (void)e;
            end = high_resolution_clock::now();
            break;
        }
        case 5: { 
            start = high_resolution_clock::now();
            pq.modifyPriority(0, n + 2); 
            end = high_resolution_clock::now();
            break;
        }
        case 6: { 
            start = high_resolution_clock::now();
            pq.modifyPriority(n / 2, n + 2);
            end = high_resolution_clock::now();
            break;
        }
        case 7: {
            start = high_resolution_clock::now();
            pq.modifyPriority(n - 1, n + 2); 
            end = high_resolution_clock::now();
            break;
        }
        }

        totalTimeNs += duration<double, std::nano>(end - start).count();
    }

    return totalTimeNs / iterations;
}

int main() {
    vector<int> sizes = { 1000, 5000, 10000, 50000, 100000, 250000, 500000, 1000000 };
    int iterations = 1000;

    vector<string> operations = {
        "1. Dodawanie (Zawsze O(1))",
        "2. Usuwanie elementu (Zawsze O(N))",
        "3. Zwracanie rozmiaru",
        "4. Podejrzenie nastepnego (Peek - O(N))",
        "5a. Modyfikacja (Optymistyczny - head)",
        "5b. Modyfikacja (Sredni - srodek)",
        "5c. Modyfikacja (Pesymistyczny - tail)"
    };

    string filename = "wyniki_lista_nieposortowana_pelne.txt";
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        cerr << "Blad podczas otwierania pliku do zapisu!" << endl;
        return 1;
    }

    outFile << left << setw(45) << "Operacja"
        << setw(15) << "Rozmiar (N)"
        << "Sredni czas (ns)" << endl;
    outFile << string(80, '-') << endl;

    cout << "Rozpoczeto testowanie Nieposortowanej Listy (z 3 przypadkami modyfikacji)." << endl;
    cout << "Uwaga: Zlozonosc O(N) moze sprawic, ze test dla 1 000 000 zajmie chwile.\n" << endl;

    for (int opIndex = 1; opIndex <= 7; ++opIndex) {
        cout << "Testowanie: " << operations[opIndex - 1] << "..." << endl;

        for (int n : sizes) {
            double avgTime = runBenchmark(opIndex, n, iterations);

            outFile << left << setw(45) << operations[opIndex - 1]
                << setw(15) << n
                << fixed << setprecision(2) << avgTime << endl;
        }
        outFile << string(80, '-') << endl;
    }

    outFile.close();
    cout << "\nWszystkie testy zakonczone. Plik " << filename << " jest gotowy." << endl;

    return 0;
}
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
    Node* next;
    Node(int d, int p) : data(d), priority(p), next(nullptr) {}
};

class SortedSinglyLinkedListPQ {
private:
    Node* head;
    int size;

public:
    SortedSinglyLinkedListPQ() : head(nullptr), size(0) {}

    ~SortedSinglyLinkedListPQ() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void enqueue(int data, int priority) {
        Node* newNode = new Node(data, priority);

        if (!head || priority > head->priority) {
            newNode->next = head;
            head = newNode;
        }
        else {
            Node* current = head;
            while (current->next != nullptr && current->next->priority >= priority) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
        size++;
    }

    void dequeue() {
        if (!head) return;
        Node* temp = head;
        head = head->next;
        delete temp;
        size--;
    }

    int getSize() {
        return size;
    }

    Node peek() {
        if (!head) throw out_of_range("Kolejka jest pusta");
        return *head;
    }
    void modifyPriority(int data, int newPriority) {
        if (!head) return;

        Node* current = head;
        Node* prev = nullptr;

        while (current != nullptr && current->data != data) {
            prev = current;
            current = current->next;
        }

        if (!current) return; 
        
        if (prev == nullptr) {
            head = current->next;
        }
        else {
            prev->next = current->next;
        }
        delete current;
        size--;


        enqueue(data, newPriority);
    }
};
double runBenchmark(int operationChoice, int n, int iterations) {
    double totalTimeNs = 0;

    for (int i = 0; i < iterations; ++i) {
        SortedSinglyLinkedListPQ pq;

        for (int j = 1; j <= n; ++j) {
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
            pq.enqueue(-1, n / 2); 
            end = high_resolution_clock::now();
            break;
        }
        case 3: { 
            start = high_resolution_clock::now();
            pq.enqueue(-1, -1);
            end = high_resolution_clock::now();
            break;
        }
        case 4: { 
            start = high_resolution_clock::now();
            pq.dequeue();
            end = high_resolution_clock::now();
            break;
        }
        case 5: { 
            start = high_resolution_clock::now();
            volatile int s = pq.getSize(); (void)s;
            end = high_resolution_clock::now();
            break;
        }
        case 6: { 
            start = high_resolution_clock::now();
            volatile Node e = pq.peek(); (void)e;
            end = high_resolution_clock::now();
            break;
        }
        case 7: { 
            start = high_resolution_clock::now();
            pq.modifyPriority(n, n + 2); 
            end = high_resolution_clock::now();
            break;
        }
        case 8: { 
            start = high_resolution_clock::now();
            pq.modifyPriority(n / 2, n + 2); 
            end = high_resolution_clock::now();
            break;
        }
        case 9: { 
            start = high_resolution_clock::now();
            pq.modifyPriority(1, n + 2); 
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
        "1a. Dodawanie (Optymistyczny - poczatek)",
        "1b. Dodawanie (Sredni - srodek)",
        "1c. Dodawanie (Pesymistyczny - koniec)",
        "2. Usuwanie elementu (Zawsze O(1))",
        "3. Zwracanie rozmiaru (Zawsze O(1))",
        "4. Podejrzenie nastepnego (Peek - O(1))",
        "5a. Modyfikacja (Optymistyczny - poczatek)",
        "5b. Modyfikacja (Sredni - srodek)",
        "5c. Modyfikacja (Pesymistyczny - koniec)"
    };

    string filename = "wyniki_lista_posortowana_pelne.txt";
    ofstream outFile(filename);

    if (!outFile.is_open()) return 1;

    outFile << left << setw(45) << "Operacja"
        << setw(15) << "Rozmiar (N)"
        << "Sredni czas (ns)" << endl;
    outFile << string(80, '-') << endl;

    cout << "Rozpoczeto pelne testowanie Posortowanej Listy Jednokierunkowej." << endl;
    cout << "Uwaga: Generowanie danych dla testow O(N) przy 1 000 000 moze potrwac chwilke.\n" << endl;

    for (int opIndex = 1; opIndex <= 9; ++opIndex) {
        cout << "Testowanie: " << operations[opIndex - 1] << "..." << endl;

        for (int n : sizes) {
            double avgTime = runBenchmark(opIndex, n, iterations);
            outFile << left << setw(45) << operations[opIndex - 1]
                << setw(15) << n
                << fixed << setprecision(2) << avgTime << endl;

            cout << "  - N = " << left << setw(8) << n << " zakonczono." << endl;
        }
        outFile << string(80, '-') << endl;
    }

    outFile.close();
    cout << "\nWszystkie testy zakonczone. Plik " << filename << " jest gotowy." << endl;

    return 0;
}
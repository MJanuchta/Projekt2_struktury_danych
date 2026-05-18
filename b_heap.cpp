#include <iostream>
#include <chrono>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;
using namespace std::chrono;

struct Element {
    int data;
    int priority;
};

class BinaryHeap {
private:
    Element* heap;
    int size;
    int capacity;

    void resize() {
        capacity *= 2;
        Element* newHeap = new Element[capacity];
        for (int i = 0; i < size; ++i) {
            newHeap[i] = heap[i];
        }
        delete[] heap;
        heap = newHeap;
    }

    int getParent(int index) { return (index - 1) / 2; }
    int getLeftChild(int index) { return 2 * index + 1; }
    int getRightChild(int index) { return 2 * index + 2; }

    void heapifyUp(int index) {
        while (index > 0 && heap[getParent(index)].priority < heap[index].priority) {
            swap(heap[getParent(index)], heap[index]);
            index = getParent(index);
        }
    }

    void heapifyDown(int index) {
        int maxIndex = index;
        int left = getLeftChild(index);
        int right = getRightChild(index);

        if (left < size && heap[left].priority > heap[maxIndex].priority) {
            maxIndex = left;
        }
        if (right < size && heap[right].priority > heap[maxIndex].priority) {
            maxIndex = right;
        }
        if (index != maxIndex) {
            swap(heap[index], heap[maxIndex]);
            heapifyDown(maxIndex);
        }
    }

public:
    BinaryHeap(int initialCapacity = 1) {
        capacity = initialCapacity;
        size = 0;
        heap = new Element[capacity];
    }

    ~BinaryHeap() {
        delete[] heap;
    }

    void enqueue(int data, int priority) {
        if (size == capacity) resize();
        heap[size] = { data, priority };
        heapifyUp(size);
        size++;
    }

    void dequeue() {
        if (size == 0) return;
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
    }

    int getSize() {
        return size;
    }

    Element peek() {
        if (size == 0) throw out_of_range("Kolejka jest pusta");
        return heap[0];
    }

    void modifyPriority(int data, int newPriority) {
        for (int i = 0; i < size; ++i) {
            if (heap[i].data == data) {
                int oldPriority = heap[i].priority;
                heap[i].priority = newPriority;

                if (newPriority > oldPriority) {
                    heapifyUp(i);
                }
                else {
                    heapifyDown(i);
                }
                return;
            }
        }
    }
};
double runBenchmark(int operationChoice, int n, int iterations) {
    double totalTimeNs = 0;

    for (int i = 0; i < iterations; ++i) {
        BinaryHeap pq(n + 1);
        for (int j = 0; j < n; ++j) {
            pq.enqueue(j, j);
        }

        auto start = high_resolution_clock::now();
        auto end = start;

        switch (operationChoice) {
        case 1: { 
            start = high_resolution_clock::now();
            pq.enqueue(-1, -1);
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
            pq.enqueue(-1, n + 1);
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
            volatile Element e = pq.peek(); (void)e;
            end = high_resolution_clock::now();
            break;
        }
        case 7: { 
            start = high_resolution_clock::now();
            pq.modifyPriority(n - 1, n + 2);
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
            pq.modifyPriority(0, n + 2);
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
        "1a. Dodawanie (Optymistyczny - dol)",
        "1b. Dodawanie (Sredni - srodek)",
        "1c. Dodawanie (Pesymistyczny - gora)",
        "2. Usuwanie elementu",
        "3. Zwracanie rozmiaru",
        "4. Podejrzenie nastepnego (Peek)",
        "5a. Modyfikacja (Optymistyczny - korzen)",
        "5b. Modyfikacja (Sredni - srodek)",
        "5c. Modyfikacja (Pesymistyczny - lisc)"
    };

    string filename = "wyniki_kopiec_pelne.txt";
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        cerr << "Blad podczas otwierania pliku do zapisu!" << endl;
        return 1;
    }

    outFile << left << setw(45) << "Operacja"
        << setw(15) << "Rozmiar (N)"
        << "Sredni czas (ns)" << endl;
    outFile << string(80, '-') << endl;

    cout << "Rozpoczeto pelne testowanie Kopca Binarnego (z podzialem na 3 przypadki)." << endl;
    cout << "Wyniki zostana zapisane do pliku: " << filename << "\n" << endl;

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
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <cmath>

using namespace std;
using namespace std::chrono;

// ======================= SORTS =======================

void quickSort(int* arr, int low, int high) {
    if (low >= high) return;

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    int pi = i + 1;

    quickSort(arr, low, pi - 1);
    quickSort(arr, pi + 1, high);
}

void heapify(int* arr, int n, int i) {
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(int* arr, int n) {
    for (int i = n/2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n-1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void shellSort(int* arr, int n) {
    for (int gap = n/2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j-gap] > temp; j -= gap) {
                arr[j] = arr[j-gap];
            }
            arr[j] = temp;
        }
    }
}

void merge(int* arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergeSort(int* arr, int l, int r) {
    if (l >= r) return;

    int m = l + (r - l) / 2;

    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

// ===== IntroSort =====

void insertionSortIntro(int* arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int medianOfThree(int* arr, int a, int b, int c) {
    if (arr[a] < arr[b] && arr[b] < arr[c]) return b;
    if (arr[a] < arr[c] && arr[c] <= arr[b]) return c;
    if (arr[b] <= arr[a] && arr[a] < arr[c]) return a;
    if (arr[b] < arr[c] && arr[c] <= arr[a]) return c;
    if (arr[c] <= arr[a] && arr[a] < arr[b]) return a;
    return b;
}

int partitionIntro(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void introsortUtil(int* arr, int low, int high, int depthLimit) {
    int size = high - low + 1;

    if (size < 16) {
        insertionSortIntro(arr, low, high);
        return;
    }

    if (depthLimit == 0) {
        heapSort(arr + low, size); // 🔥 subarray correcto
        return;
    }

    int mid = low + size / 2;
    int pivotIdx = medianOfThree(arr, low, mid, high);
    swap(arr[pivotIdx], arr[high]);

    int p = partitionIntro(arr, low, high);

    introsortUtil(arr, low, p - 1, depthLimit - 1);
    introsortUtil(arr, p + 1, high, depthLimit - 1);
}

void introSort(int* arr, int n) {
    int depthLimit = 2 * log(n);
    introsortUtil(arr, 0, n - 1, depthLimit);
}

// ======================= UTIL =======================

int* generateData(size_t bytes) {
    size_t n = bytes / sizeof(int);
    int* data = new int[n];

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1000000);

    for (size_t i = 0; i < n; i++)
        data[i] = dis(gen);

    return data;
}

template<typename Func>
long long measure(Func func, int* data, int n) {
    auto start = high_resolution_clock::now();

    func(data, n);

    auto end = high_resolution_clock::now();

    return duration_cast<milliseconds>(end - start).count();
}

// ======================= MAIN =======================

int main() {
    size_t sizes[] = {
        32ULL * 1024 * 1024,
        64ULL * 1024 * 1024,
        128ULL * 1024 * 1024
    };

    ofstream csv("results.csv");
    csv << "SizeMB,Algorithm,TimeMs\n";

    for (size_t size : sizes) {
        int sizeMB = size / (1024 * 1024);
        int n = size / sizeof(int);

        cout << "\n=== " << sizeMB << " MB ===" << endl;

        auto run = [&](string name, auto func) {

            int* data = generateData(size); // ⚠️ datos nuevos cada vez

            long long t = measure(func, data, n);

            cout << name << ": " << t << " ms" << endl;
            csv << sizeMB << "," << name << "," << t << "\n";

            delete[] data; // 🔥 MUY IMPORTANTE
        };

        run("IntroSort", [](int* d, int n){ introSort(d, n); });
        run("QuickSort", [](int* d, int n){ quickSort(d, 0, n-1); });
        run("HeapSort", [](int* d, int n){ heapSort(d, n); });
        run("ShellSort", [](int* d, int n){ shellSort(d, n); });
        run("MergeSort", [](int* d, int n){ mergeSort(d, 0, n-1); });
    }

    csv.close();

    cout << "\nResultados guardados en results.csv" << endl;

    return 0;
}
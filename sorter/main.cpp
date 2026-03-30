#include <complex>
#include <iostream>
#include <string>
#include "PagedArray.h"
#include <vector>

using namespace std;

void insertionSort(PagedArray& arr, int n) {
    // Start from the second element (index 1) as the first is assumed sorted
    for (int i = 1; i < n; ++i) {
        int key = arr[i]; // Store the current element to be inserted
        int j = i - 1;    // Start comparing with the element before the current one

        // Move elements of the sorted subarray that are greater than key
        // one position to the right
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }

        // Place the key in its correct position in the sorted subarray
        arr[j + 1] = key;
    }
}

void bubbleSort(PagedArray& arr, int n) {
    bool swapped;
    // Outer loop controls the number of passes
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        // Inner loop performs the comparisons and swaps
        // The last 'i' elements are already in place after 'i' passes
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]); // Using std::swap
                swapped = true;
            }
        }
        // If no two elements were swapped by the inner loop,
        // then the array is already sorted, and we can break.
        if (!swapped) {
            break;
        }
    }
}

void heapify(PagedArray& arr, int n, int i) {
    int largest = i; // Initialize largest as root
    int left = 2 * i + 1; // Left child index
    int right = 2 * i + 2; // Right child index

    // If left child is larger than root
    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    // If right child is larger than the current largest
    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    // If the largest element is not the root, swap and recursively heapify the affected sub-tree
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(PagedArray& arr, int size) {
    int n = size;

    // Build a max-heap (rearrange array)
    // Start from the last non-leaf node (n/2 - 1) up to the root (0)
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // One by one extract an element from the heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root (largest element) to the end
        swap(arr[0], arr[i]);

        // Call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

void merge(PagedArray& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary vectors to hold the sub-array data
    vector<int> L(n1);
    vector<int> R(n2);

    // Copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Merge the temp arrays back into arr[left..right]
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(PagedArray& arr, int left, int right) {
    if (left >= right) {
        return; // Base case: array has 0 or 1 element
    }
    int mid = left + (right - left) / 2; // Find the middle point

    // Sort first and second halves
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    // Merge the sorted halves
    merge(arr, left, mid, right);
}

int partition(PagedArray& arr, int low, int high) {
    int pivot = arr[high]; // Choose the last element as the pivot
    int i = (low - 1); // Index of the smaller element

    for (int j = low; j < high; j++) {
        // If the current element is smaller than or equal to the pivot
        if (arr[j] <= pivot) {
            i++; // Increment index of smaller element
            std::swap(arr[i], arr[j]); // Swap current element with the smaller element at 'i'
        }
    }

    std::swap(arr[i + 1], arr[high]); // Swap the pivot element to its correct position
    return i + 1; // Return the partition point
}

void quickSort(PagedArray& arr, int low, int high) {
    if (low < high) {
        // pi is the partitioning index, arr[pi] is now at the right place
        int pi = partition(arr, low, high);

        // Recursively sort the sub-arrays
        quickSort(arr, low, pi - 1); // Sort elements before the pivot
        quickSort(arr, pi + 1, high); // Sort elements after the pivot
    }
}

int CopyFile(char* oldRoute, char* newRoute) {
    FILE* newFile = fopen(newRoute, "rb");
    FILE* oldFile = fopen(oldRoute, "wb");
    if (oldFile == NULL || newFile == NULL) {
        cout << "Error al abrir/crear el archivo de salida o entrada" << endl;
        return 1;
    }
    char buffer[4096];
    size_t bytesRead = 0;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), oldFile)) > 0) {
        fwrite(buffer, 1, bytesRead, newFile);
    }
    return 0;
}

int sort(char* argv[]) {
    if (CopyFile(argv[2], argv[4]) != 0) {
        return 1;
    }
    PagedArray* arr = new PagedArray(stoi(argv[8]), stoi(argv[10]), argv[4]);
    string alg = string(argv[6]);
    if (alg == "Quick") {
        quickSort(*arr, 0, arr->GetSize());
    }
    else if (alg == "Merge") {
        mergeSort(*arr,0, arr->GetSize());
    }
    else if (alg == "Heap") {
        heapSort(*arr, arr->GetSize());
    }
    else if (alg == "Insertion") {
        insertionSort(*arr, arr->GetSize());
    }
    else {
        bubbleSort(*arr, arr->GetSize());
    }
    arr->WriteAllFrames();
    delete arr;
    return 0;
}

int checkArgs(int args, char* argv[]) {
    if (args != 11) {
        cout << "Comando invalido" << endl;
        return 1;
    }
    if (string(argv[1]) != "-input") {
        cout << "Debe especificar el archivo de entrada" << endl;
        return 1;
    }
    if (string(argv[3]) != "-output") {
        cout << "Debe especificar el archivo de salida" << endl;
        return 1;
    }
    if (string(argv[5]) != "-alg") {
        cout << "Debe especificar algortimo" << endl;
        cout << "Opciones:" << endl;
        cout << "Merge" << endl;
        cout << "Quick" << endl;
        cout << "Heap" << endl;
        cout << "Insertion" << endl;
        cout << "Bubble" << endl;
        return 1;
    }
    string algoritmo = argv[6];
    if (algoritmo != "Merge" && algoritmo != "Quick" && algoritmo != "Heap" && algoritmo != "Insertion" && algoritmo != "Bubble") {
        cout << "Algoritmo invalido" << endl;
        return 1;
    }
    if (string(argv[7]) != "-pageSize") {
        cout << "Debe especificar el tamaño de página" << endl;
        return 1;
    }
    int pageSize;
    try {
        pageSize = stoi(argv[8]);
        if (pageSize <= 0) {
            cout << "pageSize invalido" << endl;
            return 1;
        }
    } catch (...) {
        cout << "pageSize invalido" << endl;
        return 1;
    }
    if (string(argv[9]) != "-pageCount") {
        cout << "Debe especificar la cantidad de páginas" << endl;
        return 1;
    }
    int pageCount;
    try {
        pageCount = stoi(argv[10]);
        if (pageCount <= 0) {
            cout << "pageCount invalido" << endl;
            return 1;
        }
    } catch (...) {
        cout << "pageCount invalido" << endl;
        return 1;
    }
    return 0;
}

int main(int args, char* argv[]) {
    if (checkArgs(args, argv) != 0) return 1;
    return sort(argv);
}
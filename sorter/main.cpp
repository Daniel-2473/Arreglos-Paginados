#include <complex>
#include <iostream>
#include <string>
#include "PagedArray.h"
#include <string.h>
#include <chrono>

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

    // Build heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // Sorting phase
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);

        // 👇 PROGRESO
        if (i % (n / 100) == 0) { // cada 1%
            int progress = ((n - i) * 100) / n;
            cout << "\rProgreso: " << progress << "%" << flush;
        }
    }

    cout << "\rProgreso: 100%\n";
}

void selectionSort(PagedArray& arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        // Find the minimum element in the unsorted array
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        // Swap the found minimum element with the first element of the unsorted part
        std::swap(arr[min_idx], arr[i]);
    }
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
    FILE* oldFile = fopen(oldRoute, "rb");
    FILE* newFile = fopen(newRoute, "wb");
    if (oldFile == NULL || newFile == NULL) {
        cout << "Error al abrir/crear el archivo de salida o entrada" << endl;
        return 1;
    }
    char buffer[4096];
    size_t bytesRead = 0;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), oldFile)) > 0) {
        fwrite(buffer, 1, bytesRead, newFile);
    }
    fclose(oldFile);
    fclose(newFile);
    return 0;
}

void RewriteBinaryFile(char* fileRoute) {
    FILE* binaryFile = fopen(fileRoute, "rb");
    char outputPath[512];
    strcpy(outputPath, fileRoute);
    char* lastSlash = strrchr(outputPath, '/');
    if (lastSlash != NULL) {
        *(lastSlash + 1) = '\0';
    }
    strcat(outputPath, "sorted.txt");
    FILE* txtFile = fopen(outputPath, "w");
    if (!binaryFile || !txtFile) {
        cout << "Error abriendo archivos\n";
        return;
    }
    int value, first = 1;
    while (fread(&value, sizeof(int), 1, binaryFile) == 1) {
        if (!first) fprintf(txtFile, ",");
        fprintf(txtFile, "%d", value);
        first = 0;
    }
    fclose(binaryFile);
    fclose(txtFile);
}

int sort(int args, char* argv[]) {
    auto start = chrono::steady_clock::now();
    cout<< "Copiando archivo"<<endl;
    if (CopyFile(argv[2], argv[4]) != 0) {
        return 1;
    }
    PagedArray* arr;
    if (args == 13) {
        arr = new PagedArray(stoi(argv[8]), stoi(argv[10]), argv[4], string(argv[12]));
    }
    else {
        arr = new PagedArray(stoi(argv[8]), stoi(argv[10]), argv[4], "LRU");
    }
    string alg = string(argv[6]);
    cout<<"Ejecutando algortimo"<<endl;
    if (alg == "Quick") {
        quickSort(*arr, 0, arr->size - 1);
    }
    else if (alg == "Selection") {
        selectionSort(*arr, arr->size);
    }
    else if (alg == "Heap") {
        heapSort(*arr, arr->size);
    }
    else if (alg == "Insertion") {
        insertionSort(*arr, arr->size);
    }
    else {
        bubbleSort(*arr, arr->size);
    }
    cout<<"Escribiendo todos los frames"<<endl;
    arr->WriteAllFrames();
    cout<<"Rescribiendo en archivo legible"<<endl;
    RewriteBinaryFile(argv[4]);
    auto end = chrono::steady_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Tiempo transcurrido:" << duration.count() << "s" << endl;
    cout << "Algortimo:" + alg << endl;
    cout << "Page hits: " + to_string(arr->GetPageHits()) << endl;
    cout << "Page faults: " + to_string(arr->GetPageFaults()) << endl;
    delete arr;
    return 0;
}

int checkArgs(int args, char* argv[]) {
    if (args != 13 && args != 11) {
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
        cout << "Selection" << endl;
        cout << "Quick" << endl;
        cout << "Heap" << endl;
        cout << "Insertion" << endl;
        cout << "Bubble" << endl;
        return 1;
    }
    string algoritmo = argv[6];
    if (algoritmo != "Selection" && algoritmo != "Quick" && algoritmo != "Heap" && algoritmo != "Insertion" && algoritmo != "Bubble") {
        cout << "Algoritmo invalido" << endl;
        return 1;
    }
    if (args == 13) {
        if (string(argv[11]) != "-algRemp") {
            cout << "formato invalido" << endl;
            return 1;
        }
        string algoritmoRemplazo = argv[12];
        if (algoritmoRemplazo != "FIFO" && algoritmoRemplazo != "LRU") {
            cout << "Algoritmp de remplazo invalido" << endl;
            return 1;
        }
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
    return sort(args,argv);
}
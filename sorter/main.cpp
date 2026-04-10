#include <iostream>
#include <string>
#include "PagedArray.h"
#include <chrono>
#include <cmath>

using namespace std;

void shellSort(PagedArray& arr) {
    int n = arr.GetSize();

    // Start with a large gap, then reduce it step by step
    for (int gap = n / 2; gap > 0; gap /= 2) {

        // Perform a "gapped" insertion sort for this gap size
        for (int i = gap; i < n; i++) {

            // Current element to be placed correctly
            int temp = arr[i];
            int j = i;

            // Shift elements that are greater than temp to make space
            while (j >= gap && arr[j - gap] > temp) {
                arr[j] = arr[j - gap];
                j -= gap;
            }

            // Place temp in its correct location
            arr[j] = temp;
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

    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    for (int i = n - 1; i > 0; i--) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

void merge(PagedArray& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i++];
        } else {
            arr[k] = R[j++];
        }
        k++;
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergeSort(PagedArray& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
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

void insertionSortIntro(PagedArray& arr, int left, int right) {
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

int medianOfThree(PagedArray& arr, int a, int b, int c) {
    if (arr[a] < arr[b] && arr[b] < arr[c]) return b;
    if (arr[a] < arr[c] && arr[c] <= arr[b]) return c;
    if (arr[b] <= arr[a] && arr[a] < arr[c]) return a;
    if (arr[b] < arr[c] && arr[c] <= arr[a]) return c;
    if (arr[c] <= arr[a] && arr[a] < arr[b]) return a;
    return b;
}

int partitionIntro(PagedArray& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void introsortUtil(PagedArray& arr, int low, int high, int depthLimit) {
    int size = high - low + 1;
    if (size < 16) {
        insertionSortIntro(arr, low, high);
        return;
    }
    if (depthLimit == 0) {
        // Usar heapSort en el subarreglo
        // Reusamos heapify ya definido
        for (int i = low + size/2 - 1; i >= low; i--)
            heapify(arr, high + 1, i);
        for (int i = high; i > low; i--) {
            std::swap(arr[low], arr[i]);
            heapify(arr, i, low);
        }
        return;
    }
    int mid = low + size / 2;
    int pivotIdx = medianOfThree(arr, low, mid, high);
    std::swap(arr[pivotIdx], arr[high]);
    int partitionPoint = partitionIntro(arr, low, high);
    introsortUtil(arr, low, partitionPoint - 1, depthLimit - 1);
    introsortUtil(arr, partitionPoint + 1, high, depthLimit - 1);
}

void introSort(PagedArray& arr, int low, int high) {
    int depthLimit = 2 * log(high - low + 1);
    introsortUtil(arr, low, high, depthLimit);
}

int CopyFile(char* oldRoute, char* newRoute) { //Copiar un archivo de una ruta a otra
    FILE* oldFile = fopen(oldRoute, "rb"); //Abrir archivos
    FILE* newFile = fopen(newRoute, "wb");
    if (oldFile == NULL || newFile == NULL) {
        cout << "Error al abrir/crear el archivo de salida o entrada" << endl;
        return 1;
    }
    char buffer[4096]; //Buffer para escribir por partes
    size_t bytesRead = 0;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), oldFile)) > 0) { //Leemos y guardamos en el buffer un pedazo del arhivo hasta que ya no lea nada
        fwrite(buffer, 1, bytesRead, newFile); //Escribe en el nuevo archivo los datos
    }
    fclose(oldFile);
    fclose(newFile);
    return 0;
}

void RewriteBinaryFile(const string& fileRoute) { //Rescribir archivo binario en formato legible
    FILE* binaryFile = fopen(fileRoute.c_str(), "rb");  //Abrir archivo binario
    size_t lastSlash = fileRoute.find_last_of('/'); //Busca el último slash
    string outputPath;
    string fileName;
    if (lastSlash != string::npos) { //Si existe el slash separamos en dos
        outputPath = fileRoute.substr(0, lastSlash + 1);
        fileName = fileRoute.substr(lastSlash + 1);
    } else { //Si no, solo tomamos el nombre del archivo
        fileName = fileRoute;
    }
    size_t lastDot = fileName.find_last_of('.'); //Buscar el último punto (extensión del archivo)
    if (lastDot != std::string::npos) {
        fileName = fileName.substr(0, lastDot); //Obtener el nombre del archivo
    }

    outputPath += "sorted" + fileName + ".txt"; //Construir nombre del nuevo archivo

    FILE* txtFile = fopen(outputPath.c_str(), "w"); //Abrir archivo de salida
    if (!binaryFile || !txtFile) {
        cout << "Error abriendo archivos\n";
        return;
    }
    int value, first = 1;
    while (fread(&value, sizeof(int), 1, binaryFile) == 1) { //Escribir valor por valor
        if (!first) fprintf(txtFile, ","); //Evitar poner coma en el primer numero
        fprintf(txtFile, "%d", value); //Escribir numero/valor
        first = 0; //Marcar que ya se usó el primero
    }
    fclose(binaryFile);
    fclose(txtFile);
}

int sortFile(int args, char* argv[]) {
    cout<< "Copiando archivo"<<endl;
    if (CopyFile(argv[2], argv[4]) != 0) { //Copiar archivo
        return 1;
    }
    PagedArray* arr; //Crear instancia
    if (args == 13) { //Si el usuario especifica el algortimo de remplazo
        try {
            arr = new PagedArray(stoi(argv[8]), stoi(argv[10]), argv[4], string(argv[12]));
        } catch (const runtime_error& e) { //En caso de ocurrir un error al crear la clase
            cout << e.what() << endl;
            return 1;
        }
    }
    else {
        try {
            arr = new PagedArray(stoi(argv[8]), stoi(argv[10]), argv[4], "LRU"); //Utilizar LRU por defecto
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
            return 1;
        }
    }
    string alg = string(argv[6]);
    cout<<"Ejecutando algortimo"<<endl;
    auto start = chrono::steady_clock::now();
    if (alg == "Quick") {
        quickSort(*arr, 0, arr->GetSize() - 1);
    }
    else if (alg == "Merge") {
        mergeSort(*arr, 0, arr->GetSize()-1);
    }
    else if (alg == "Heap") {
        heapSort(*arr, arr->GetSize() );
    }
    else if (alg == "Shell") {
        shellSort(*arr);
    }
    else {
        introSort(*arr, 0, arr->GetSize() - 1);
    }
    arr->WriteAllFrames();
    auto end = chrono::steady_clock::now();
    cout<<"Rescribiendo en archivo legible"<<endl;
    RewriteBinaryFile(argv[4]);

    chrono::duration<double> duration = end - start;
    cout << "Tiempo transcurrido: " << duration.count() << "s" << endl;
    cout << "Algortimo: " + alg << endl;
    cout << "Page hits: " + to_string(arr->GetPageHits()) << endl;
    cout << "Page faults: " + to_string(arr->GetPageFaults()) << endl;
    delete arr;
    return 0;
}

int checkArgs(int args, char* argv[]) { //Validaciones
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
        cout << "Merge" << endl;
        cout << "Quick" << endl;
        cout << "Heap" << endl;
        cout << "Shell" << endl;
        cout << "Intro" << endl;
        return 1;
    }
    string algoritmo = argv[6];
    if (algoritmo != "Merge" && algoritmo != "Quick" && algoritmo != "Heap" && algoritmo != "Shell" && algoritmo != "Intro") {
        cout << "Algoritmo invalido" << endl;
        return 1;
    }
    if (args == 13) { //Verifica si hay más argumentos, o sea, se especificó el algortimo de remplazo
        if (string(argv[11]) != "-algRemp") {
            cout << "formato invalido" << endl;
            return 1;
        }
        string algoritmoRemplazo = argv[12];
        if (algoritmoRemplazo != "FIFO" && algoritmoRemplazo != "LRU") {
            cout << "Algoritmo de remplazo invalido" << endl;
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
    return sortFile(args,argv);
}
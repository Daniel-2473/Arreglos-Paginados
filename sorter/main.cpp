#include <complex>
#include <iostream>
#include <string>
#include "PagedArray.h"
#include <vector>

using namespace std;



int checkArgs(int args, char* argv[]) {

    if (args != 11) {
        cout << "Comando invalido" << endl;
        return 1;
    }

    if (string(argv[1]) != "-input") {
        cout << "Debe especificar -input" << endl;
        return 1;
    }
    string inputPath = argv[2];

    if (string(argv[3]) != "-output") {
        cout << "Debe especificar -output" << endl;
        return 1;
    }
    string outputPath = argv[4];

    if (string(argv[5]) != "-alg") {
        cout << "Debe especificar -alg" << endl;
        return 1;
    }

    string algoritmo = argv[6];
    if (algoritmo != "MERGE" && algoritmo != "QUICK" && algoritmo != "HEAP") {
        cout << "Algoritmo invalido" << endl;
        return 1;
    }

    if (string(argv[7]) != "-pageSize") {
        cout << "Debe especificar -pageSize" << endl;
        return 1;
    }

    int pageSize;
    try {
        pageSize = stoi(argv[8]);
        if (pageSize <= 0) {
            cout << "Page size invalido" << endl;
            return 1;
        }
    } catch (...) {
        cout << "Page size invalido" << endl;
        return 1;
    }

    if (string(argv[9]) != "-pageCount") {
        cout << "Debe especificar -pageCount" << endl;
        return 1;
    }

    int pageCount;
    try {
        pageCount = stoi(argv[10]);
        if (pageCount <= 0) {
            cout << "Page count invalido" << endl;
            return 1;
        }
    } catch (...) {
        cout << "Page count invalido" << endl;
        return 1;
    }

    return 0;
}

int main(int args, char* argv[]) {
    if (checkArgs(args, argv) != 0) return 1;
    return sort(argv);
}
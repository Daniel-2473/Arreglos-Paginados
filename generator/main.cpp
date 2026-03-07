#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;
int fileSize;
string fileRoute;

int createFile() {
    ofstream numbersFile;
    numbersFile.open(fileRoute, ios::out | ios::binary);
    if (numbersFile.is_open()) {
        for (int i = 0; i < (fileSize*1000*1000 / 4); i++) {
            int num = rand();
            numbersFile.write(reinterpret_cast<char*>(&num), sizeof(int));
        }
        numbersFile.close();
        return 0;
    }
    return 1;
}

int checkArgs(int argc, char* argv[]) { //Hacer la validacion del comando en terminal
    if (argc != 5) {
        cout << "Comando invalido" << endl;
        return 1;
    }
    if (string(argv[1]) == "-size") { //Validar tamano correcto
        if (string(argv[2]) == "SMALL") {
            fileSize = 512;
        }
        else if (string(argv[2]) == "MEDIUM") {
            fileSize = 1024;
        }
        else if (string(argv[2]) == "LARGE") {
            fileSize = 2048;
        }
        else {
            cout << "Tamano invalido. Opciones: SMALL, MEDIUM o LARGE" << endl;
            return 1;
        }
    }
    else {
        cout << "Debe especificar el tamano del archivo" << endl;
        return 1;
    }
    if (string(argv[3]) != "-output") {
        cout << "Debe especificar el archivo de salida" << endl;
        return 1;
    }
    else {
        fileRoute = string(argv[4]);
        return createFile();
    }
    return 1;
}

int main(int args, char* argv[]) {
    srand(time(NULL));
    return checkArgs(args, argv);
}


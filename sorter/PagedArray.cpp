#include "PagedArray.h"

#include <stdexcept>


PagedArray::PagedArray(int pageSize, int pageCount, char* outputFilePath, std::string alg) {
    this->outputFile = fopen(outputFilePath, "rb+");
    if (this->outputFile == NULL) {
        throw std::runtime_error("Error al abrir el archivo");
    }
    setvbuf(outputFile, NULL, _IOFBF, 1 << 22); //Buffer interno para mejorar el rendimiento
    this->pageSize = pageSize;
    this->pageCount = pageCount;
    this->frames = new int*[this->pageCount]; //Referencias al primer elemento de cada pagina cargada en memoria
    for (int i = 0; i < this->pageCount; i++) {
        this->frames[i] = new int[this->pageSize]; //Pagina cargada en memoria, contiene los datos de cada pagina
    }
    usedPages = new int[this->pageCount]; //Indica cual numero de pagina hay en cada frame
    for (int i = 0; i<this->pageCount; i++) {
        usedPages[i] = -1; //Incializar en -1
    }
    framesFull = false;
    this-> lastUsed = new long long[this->pageCount]; //Array para LRU, almacena el tiempo cuando fue usada un frame
    for (int i = 0; i < pageCount; i++) {
        lastUsed[i] = -1; //Incializar en -1
    }
    this->alg = alg; //Guardar algoritmo de remplazo
    this->time = 0; //Contador para utilizar LRU
    this->oldestFrame = 0; //Almacena el índice del frame más viejo
    this->pageHits = 0;
    this->pageFaults = 0;
    this->size = MeasureFile(); //Tamaño del archivo
    int totalPages = (size + pageSize - 1) / pageSize;
    this->pageToFrame = new int[totalPages]; //Mapeo de todas las paginas, indica el indice del frame
    for (int i = 0; i < totalPages; i++) {
        pageToFrame[i] = -1; //Si la pagina no esta en memoria guarda -1
    }
}

bool PagedArray::CheckPage(int &frame, int page) { //Indica si una pagina esta cargada en memoria
    if (pageToFrame[page] != -1) {
        frame = pageToFrame[page];
        return true;
    }
    return false;
}

int& PagedArray::operator[](int index) {
    int page = index/pageSize; //Numero de pagina donde se encuentra el indice
    int frame = -1; //Variable para guardar el frame en memoria donde esta la pagina
    bool pageHit = CheckPage(frame, page);
    if (pageHit == true) {
        return PageHit(frame, index % pageSize);
    }
    else {
        return PageFault(page, index % pageSize);
    }
}

int& PagedArray::PageHit(int frame, int pageIndex) { //Devuelve el valor del indice de una pagina
    pageHits++;
    if (alg == "LRU") {
        lastUsed[frame] = time;
        time++;
    }
    return frames[frame][pageIndex];
}

int& PagedArray::PageFault(int page, int pageIndex) { //Carga la página en memoria y devulve el valor del indice
    int frameToUse;
    if (!framesFull) { //Revisa si hay frame libre
        frameToUse = CalculateFreeFrame();
    } else { //Si no, escoge el frame a remplazar utilizando el algortimo designado
        if (alg == "FIFO") {
            frameToUse = oldestFrame;
            IncreaseOldestFrame();
        } else {
            frameToUse = GetLRUFrame();
        }
        WriteFrame(frameToUse); //Escribir datos del frame viejo en disco
    }
    LoadPage(frameToUse, page); //Cargar nuevo frame
    if (alg == "LRU") {
        lastUsed[frameToUse] = time++;
    }
    pageFaults++;
    return frames[frameToUse][pageIndex];
}

int PagedArray::GetLRUFrame() { //Determina el frame menos usado en memoria
    int frame = -1;
    for (int i = 0; i < pageCount; i++) {
        if (usedPages[i] == -1) continue;
        if (frame == -1 || lastUsed[i] < lastUsed[frame]) {
            frame = i;
        }
    }
    return frame;
}

int PagedArray::CalculateFreeFrame() { //Determina cual frame esta libre (los frames libre guardan valor -1 en usedPages)
    for (int i = 0; i < pageCount; i++) {
        if (usedPages[i] == -1) {
            if (pageCount - 1 == i) {
                framesFull = true;
            }
            return i;
        }
    }
    return -1;
}

void PagedArray::WriteFrame(int frame) { //Escribe un frame a disco
    if (usedPages[frame] != -1){ //Verificar que el frame exista
        int start = usedPages[frame] * pageSize;
        int remaining = size - start; //Cuantos datos quedan desde el incio del frame
        int toWrite;
        if (remaining > pageSize) { //Escribir pageSize si hay mas datos que un frame
            toWrite = pageSize;
        } else { //Escribir solo remaining (Este es el caso de la última página que puede ser menor a pageSize)
            toWrite = remaining;
        }
        fseek(outputFile, start * sizeof(int), SEEK_SET); //Posicionar el puntero del archivo al inicio del frame
        fwrite(frames[frame], sizeof(int), toWrite, outputFile); //Escribir el frame en el archivo de salida
        pageToFrame[usedPages[frame]] = -1; //Marcar libre
    }
}

void PagedArray::LoadPage(int frameToLoadIn, int pageToLoad) { //Cargar una página en un frame
    fseek(outputFile, pageToLoad * pageSize * sizeof(int), SEEK_SET); //Posicionar el puntero del archivo en la página
    int start = pageToLoad * pageSize;
    int remaining = size - start;
    int toRead;
    if (remaining > pageSize) { //Leer toda la pagina si quedan más datos que pageSize
        toRead = pageSize;
    } else { //Escribir solo remaining en caso de ser la última página y así no escribir datos malos
        toRead = remaining;
    }
    size_t read = fread(frames[frameToLoadIn], sizeof(int), toRead, outputFile); //Leer los datos de la pagina y escribirlos en el frame
    for (int i = read; i < pageSize; i++) { //Si se escribio menos, el resto del frame lo incializa en 0
        frames[frameToLoadIn][i] = 0;
    }
    usedPages[frameToLoadIn] = pageToLoad; //Actualizar la informacion del frame
    pageToFrame[pageToLoad] = frameToLoadIn;
}

void PagedArray::IncreaseOldestFrame() { //Aumenta el valor de oldesFrame, vuelve a 0 si es mayor de la cantidad de frames
    oldestFrame++;
    if (oldestFrame >= pageCount) {
        oldestFrame = 0;
    }
}

void PagedArray::WriteAllFrames() { //Escribir todos lo frames en disco, se utiliza al final del programa, de lo contrario puede que información se pierda
    for (int i = 0; i < pageCount; i++) {
        WriteFrame(i);
    }
}

int PagedArray::MeasureFile() { //Calcular el tamaño del archivo (En ints)
    long current = ftell(outputFile);
    fseek(outputFile, 0, SEEK_END); //Mover el puntero del archivo al final
    int fileSize = ftell(outputFile)/sizeof(int); //Calcular tamaño
    fseek(outputFile, current, SEEK_SET); //Devolver el puntero a su posicion original
    return fileSize;
}

long long PagedArray::GetPageFaults() {
    return pageFaults;
}

long long PagedArray::GetPageHits() {
    return pageHits;
}

int PagedArray::GetSize() {
    return size;
}

PagedArray::~PagedArray() { //Borrar todo el heap
    delete[] lastUsed;

    for (int i = 0; i < pageCount; i++) {
        delete[] frames[i];
    }
    delete[] frames;

    delete[] usedPages;

    delete[] pageToFrame;

    fclose(outputFile);
}


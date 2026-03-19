#include "PagedArray.h"
#include <fstream>

PagedArray::PagedArray(int pageSize, int pageCount, string outputFilePath) {
    this->outputFilePath = outputFilePath;
    this->pageSize = pageSize;
    this->pageCount = pageCount;
    this->frames = new int*[this->pageCount]; //Referencias al primer elemento de cada pagina cargada en memoria
    for (int i = 0; i < this->pageCount; i++) {
        this->frames[i] = new int[this->pageSize]; //Pagina cargada en memoria, contiene los datos de cada pagina
    }
    usedPages = new int[this->pageCount]; //Indica cual numero de pagina hay en cada frame
    for (int i = 0; i<this->pageCount; i++) {
        *(usedPages + i) = -1;
    }
    framesFull = false;
}
int& PagedArray::operator[](int index) {
    int page = index/pageSize; //Numero de pagina donde se encuentra el indice
    int pageIndex = index % pageSize; //Indice dentro del array de la pagina donde esta
    int frame = -1; //Variable para guardar el frame en memoria donde esta la pagina
    bool pageHit = false;
    for (int i=0; i < pageCount; i++) {
        if (page == usedPages[i]) {
            pageHit = true;
            frame = i;
            break;
        }
    }
    if (pageHit == true) {
        return PageHit(frame, pageIndex);
    }
    else {
        return PageFault(page, pageIndex);
    }
}

int& PagedArray::PageHit(int frame, int pageIndex) {
    return frames[frame][pageIndex];
}

int& PagedArray::PageFault(int page, int pageIndex) {
    if (!framesFull) {
        int freeFrame = CalculateFreeFrame();
        LoadPage(freeFrame, page, pageIndex);
    }
}

int PagedArray::CalculateFreeFrame() {
    for (int i = 0; i < pageCount; i++) {
        if (usedPages[i] == -1) {
            if (pageCount == i-1) {
                framesFull = true;
            }
            return i;
        }
    }
}

int& PagedArray::LoadPage(int frameToLoadIn, int pageToLoad, int pageIndex) {
    FILE* file = fopen(outputFilePath.c_str(), "rb");
    if (file == NULL) {
        printf("Error al abrir archivo\n");
    }
    fseek(file, pageToLoad * pageSize * sizeof(int), SEEK_SET);
    size_t read = fread(frames[frameToLoadIn], sizeof(int), pageSize, file);
    if (read != pageSize) {
        printf("Lectura incompleta\n");
    }
    fclose(file);
    return frames[frameToLoadIn][pageIndex];
}




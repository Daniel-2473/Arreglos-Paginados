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
        return LoadPage(freeFrame, page, pageIndex);
    }
    return LoadPageNoSpace(oldestFrame,page, pageIndex);
}

int PagedArray::CalculateFreeFrame() {
    for (int i = 0; i < pageCount; i++) {
        if (usedPages[i] == -1) {
            if (pageCount - 1 == i) {
                framesFull = true;
            }
            return i;
        }
    }
    printf("Error al calcular frame\n");
    return -1;
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
    usedPages[frameToLoadIn] = pageToLoad;
    return frames[frameToLoadIn][pageIndex];
}

int &PagedArray::LoadPageNoSpace(int frameToLoadIn, int pageToLoad, int pageIndex) {
    FILE* file = fopen(outputFilePath.c_str(), "rb+");
    if (file == NULL) {
        printf("Error al abrir archivo\n");
    }
    fseek(file, usedPages[frameToLoadIn] * pageSize * sizeof(int), SEEK_SET);

    size_t written = fwrite(frames[frameToLoadIn], sizeof(int), pageSize, file);

    if (written != pageSize) {
        printf("Escritura incompleta\n");
    }

    fseek(file, pageToLoad * pageSize * sizeof(int), SEEK_SET);

    size_t read = fread(frames[frameToLoadIn], sizeof(int), pageSize, file);

    if (read != pageSize) {
        printf("Lectura incompleta\n");
    }

    fclose(file);
    usedPages[frameToLoadIn] = pageToLoad;
    oldestFrame++;
    if (oldestFrame >= pageCount) {
        oldestFrame = 0;
    }
    return frames[frameToLoadIn][pageIndex];
}

void PagedArray::LoadPagesFinal() {
    FILE* file = fopen(outputFilePath.c_str(), "rb+");
    if (file == NULL) {
        printf("Error al abrir archivo\n");
    }
    for (int i = 0; i < pageCount; i++) {

        fseek(file, usedPages[i] * pageSize * sizeof(int), SEEK_SET);
        size_t written = fwrite(frames[i], sizeof(int), pageSize, file);
    }
    fclose(file);
}




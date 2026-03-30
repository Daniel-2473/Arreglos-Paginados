#include "PagedArray.h"
#include <fstream>

PagedArray::PagedArray(int pageSize, int pageCount, char* outputFilePath) {
    this->outputFile = fopen(outputFilePath, "rb+");
    if (this->outputFile == NULL) {
        printf("Error al abrir el archivo\n");
        exit(1);
    }
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

bool PagedArray::CheckPage(int &frame, int page) {
    for (int i=0; i < pageCount; i++) {
        if (page == usedPages[i]) {
            frame = i;
            return true;
        }
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

int& PagedArray::PageHit(int frame, int pageIndex) {
    return frames[frame][pageIndex];
}

int& PagedArray::PageFault(int page, int pageIndex) {
    if (!framesFull) {
        int freeFrame = CalculateFreeFrame();
        LoadPage(freeFrame, page);
        return frames[freeFrame][pageIndex];
    }
    WriteFrame(oldestFrame);
    LoadPage(oldestFrame, page);
    IncreaseOldestFrame();
    return frames[oldestFrame-1][pageIndex];
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

void PagedArray::WriteFrame(int frame) {
    fseek(outputFile, usedPages[frame] * pageSize * sizeof(int), SEEK_SET);
    fwrite(frames[frame], sizeof(int), pageSize, outputFile);
}

void PagedArray::LoadPage(int frameToLoadIn, int pageToLoad) {
    fseek(outputFile, pageToLoad * pageSize * sizeof(int), SEEK_SET);
    fread(frames[frameToLoadIn], sizeof(int), pageSize, outputFile);
    usedPages[frameToLoadIn] = pageToLoad;
}

void PagedArray::IncreaseOldestFrame() {
    oldestFrame++;
    if (oldestFrame >= pageCount) {
        oldestFrame = 0;
    }
}

void PagedArray::WriteAllFrames() {
    for (int i = 0; i < pageCount; i++) {
        WriteFrame(i);
    }
}

int PagedArray::GetSize() {
    fseek(outputFile, 0, SEEK_END);
    return ftell(outputFile);
}




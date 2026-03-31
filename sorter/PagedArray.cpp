#include "PagedArray.h"
#include <fstream>

PagedArray::PagedArray(int pageSize, int pageCount, char* outputFilePath, string alg) {
    this->outputFile = fopen(outputFilePath, "rb+");
    if (this->outputFile == NULL) {
        printf("Error al abrir el archivo\n");
        exit(1);
    }
    setvbuf(outputFile, NULL, _IOFBF, 1 << 20);
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
    this-> lastUsed = new int[this->pageCount];
    for (int i = 0; i < pageCount; i++) {
        lastUsed[i] = -1;
    }
    this->alg = alg;
    this->time = 0;
    this->oldestFrame = 0;
    this->pageHits = 0;
    this->pageFaults = 0;
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
    pageHits++;
    if (alg == "LRU") {
        lastUsed[frame] = time;
        time++;
    }
    return frames[frame][pageIndex];
}

int& PagedArray::PageFault(int page, int pageIndex) {
    int frameToUse;
    if (!framesFull) {
        frameToUse = CalculateFreeFrame();
    } else {
        if (alg == "FIFO") {
            frameToUse = oldestFrame;
            IncreaseOldestFrame();
        } else {
            frameToUse = GetLRUFrame();
        }
        WriteFrame(frameToUse);
    }
    LoadPage(frameToUse, page);
    if (alg == "LRU") {
        lastUsed[frameToUse] = time++;
    }
    pageFaults++;
    return frames[frameToUse][pageIndex];
}

int PagedArray::GetLRUFrame() {
    int frame = 0;
    for (int i = 1; i < pageCount; i++) {
        if (lastUsed[i] < lastUsed[frame]) {
            frame = i;
        }
    }
    return frame;
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
    if (usedPages[frame] != -1){
        fseek(outputFile, usedPages[frame] * pageSize * sizeof(int), SEEK_SET);
        fwrite(frames[frame], sizeof(int), pageSize, outputFile);
        fflush(outputFile);
    }
}
void PagedArray::LoadPage(int frameToLoadIn, int pageToLoad) {
    fseek(outputFile, pageToLoad * pageSize * sizeof(int), SEEK_SET);
    size_t read = fread(frames[frameToLoadIn], sizeof(int), pageSize, outputFile);
    if (read != pageSize) {
        for (size_t i = read; i < pageSize; i++) {
            frames[frameToLoadIn][i] = 0;
        }
    }
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
    long current = ftell(outputFile);
    fseek(outputFile, 0, SEEK_END);
    int size = ftell(outputFile)/sizeof(int);
    fseek(outputFile, current, SEEK_SET);
    return size;
}

int PagedArray::GetPageFaults() {
    return pageFaults;
}

int PagedArray::GetPageHits() {
    return pageHits;
}

PagedArray::~PagedArray() {
    delete[] lastUsed;

    for (int i = 0; i < pageCount; i++) {
        delete[] frames[i];
    }
    delete[] frames;

    delete[] usedPages;

    fclose(outputFile);
}


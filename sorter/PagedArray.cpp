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
    this-> lastUsed = new long long[this->pageCount];
    for (int i = 0; i < pageCount; i++) {
        lastUsed[i] = -1;
    }
    this->alg = alg;
    this->time = 0;
    this->oldestFrame = 0;
    this->pageHits = 0;
    this->pageFaults = 0;
    this->size = GetSize();
    int totalPages = (size + pageSize - 1) / pageSize;
    this->pageToFrame = new int[totalPages];
    for (int i = 0; i < totalPages; i++) {
        pageToFrame[i] = -1;
    }
}

bool PagedArray::CheckPage(int &frame, int page) {
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
    int frame = -1;
    for (int i = 0; i < pageCount; i++) {
        if (usedPages[i] == -1) continue; // ignorar frames vacíos
        if (frame == -1 || lastUsed[i] < lastUsed[frame]) {
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
        int totalSize = size;
        int start = usedPages[frame] * pageSize;
        int remaining = totalSize - start;
        int toWrite = (remaining > pageSize) ? pageSize : remaining;
        fseek(outputFile, start * sizeof(int), SEEK_SET);
        fwrite(frames[frame], sizeof(int), toWrite, outputFile);
        pageToFrame[usedPages[frame]] = -1;
    }
}

void PagedArray::LoadPage(int frameToLoadIn, int pageToLoad) {
    fseek(outputFile, pageToLoad * pageSize * sizeof(int), SEEK_SET);
    int totalSize = size;
    int start = pageToLoad * pageSize;
    int remaining = totalSize - start;
    int toRead = (remaining > pageSize) ? pageSize : remaining;
    size_t read = fread(frames[frameToLoadIn], sizeof(int), toRead, outputFile);
    for (int i = read; i < pageSize; i++) {
        frames[frameToLoadIn][i] = 0;
    }
    usedPages[frameToLoadIn] = pageToLoad;
    pageToFrame[pageToLoad] = frameToLoadIn;
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


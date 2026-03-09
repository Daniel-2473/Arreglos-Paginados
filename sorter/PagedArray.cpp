#include "PagedArray.h"
#include <fstream>

PagedArray::PagedArray(int pageSize, int pageCount, string outputFilePath, string inputFilePath) {
    this->inputFilePath = inputFilePath;
    this->outputFilePath = outputFilePath;
    this->pageSize = pageSize;
    this->pageCount = pageCount;
    this->pages = new int*[this->pageCount];
    for (int i = 0; i < this->pageCount; i++) {
        this->pages[i] = new int[this->pageSize];
    }
    usedPages = new int[this->pageCount];
    for (int i = 0; i<this->pageCount; i++) {
        *(usedPages + i) = -1;
    }
}
int& PagedArray::operator[](int index) {
    int page = index/pageSize;
    int pageIndex = index % pageSize;
    int frame = -1;
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
}

int& PagedArray::PageHit(int frame, int pageIndex) {
    return pages[frame][pageIndex];
}


//
// Created by cloudzzz on 3/7/26.
//

#ifndef SORTER_PAGEDARRAY_H
#define SORTER_PAGEDARRAY_H

#include <iostream>
using namespace std;

class PagedArray {
private:
    int** frames;
    int pageCount;
    int pageSize;
    FILE* outputFile;
    int* usedPages;
    int firstInPage = 0;
    bool framesFull;
    int oldestFrame = 0;
    int& PageHit(int frame, int pageIndex);
    int& PageFault(int page, int pageIndex);
    int CalculateFreeFrame();
    bool CheckPage(int& frame, int page);
    void IncreaseOldestFrame();
    void LoadPage(int frameToLoadIn, int pageToLoad);
    void WriteFrame(int frame);
    long long pageHits = 0;
    long long pageFaults = 0;
    long long* lastUsed;
    long long time = 0;
    string alg;
    int GetLRUFrame();
    int GetSize();
    int* pageToFrame;

public:
    PagedArray(int pageSize, int pageCount, char* outputFilePath, string alg);
    int& operator[](int index);
    void WriteAllFrames();
    int size;
    int GetPageHits();
    int GetPageFaults();
    ~PagedArray();
};

#endif //SORTER_PAGEDARRAY_H
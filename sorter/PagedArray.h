//
// Created by cloudzzz on 3/7/26.
//

#ifndef SORTER_PAGEDARRAY_H
#define SORTER_PAGEDARRAY_H

#include <string>
#include <cstdio>
#include <stdexcept>

class PagedArray {
private:
    int** frames;
    int pageCount;
    int pageSize;
    FILE* outputFile;
    int* usedPages;
    bool framesFull;
    int oldestFrame;
    int& PageHit(int frame, int pageIndex);
    int& PageFault(int page, int pageIndex);
    int CalculateFreeFrame();
    bool CheckPage(int& frame, int page);
    void IncreaseOldestFrame();
    void LoadPage(int frameToLoadIn, int pageToLoad);
    void WriteFrame(int frame);
    long long pageHits;
    long long pageFaults;
    long long* lastUsed;
    long long time;
    std::string alg;
    int GetLRUFrame();
    int MeasureFile();
    int* pageToFrame;
    int size;

public:
    PagedArray(int pageSize, int pageCount, char* outputFilePath, std::string alg);
    int& operator[](int index);
    void WriteAllFrames();
    int GetSize();
    long long GetPageHits();
    long long GetPageFaults();
    ~PagedArray();
};

#endif //SORTER_PAGEDARRAY_H
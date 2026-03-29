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
    string outputFilePath;
    string inputFilePath;
    int* usedPages;
    int firstInPage = 0;
    bool framesFull;
    int oldestFrame = 0;

public:
    PagedArray(int pageSize, int pageCount, string outputFilePath);
    int& operator[](int index);
    int& PageHit(int frame, int pageIndex);
    int& PageFault(int page, int pageIndex);
    int CalculateFreeFrame();
    int& LoadPage(int frameToLoadIn, int pageToLoad, int pageIndex);
    int& LoadPageNoSpace(int frameToLoadIn, int pageToLoad, int pageIndex);
    void LoadPagesFinal();
};

#endif //SORTER_PAGEDARRAY_H
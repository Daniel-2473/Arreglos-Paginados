//
// Created by cloudzzz on 3/7/26.
//

#ifndef SORTER_PAGEDARRAY_H
#define SORTER_PAGEDARRAY_H

#include <iostream>
using namespace std;

class PagedArray {
private:
    int** pages;
    int pageCount;
    int pageSize;
    string outputFilePath;
    string inputFilePath;
    int* usedPages;

public:
    PagedArray(int pageSize, int pageCount, string outputFilePath, string inputFilePath);
    int& operator[](int index);
    int& PageHit(int frame, int pageIndex);
    void PageFault();

};

#endif //SORTER_PAGEDARRAY_H
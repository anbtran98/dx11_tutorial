#ifndef _MODELLIST_H
#define _MODELLIST_H

#include <stdlib.h>
#include <time.h>

class ModelList {
    struct ModelInfoType { float positionX, positionY, positionZ; };
    int mModelCount;
    ModelInfoType* mModelInfoList;

 public:
    ModelList();
    ModelList(const ModelList&);
    ~ModelList();
    void Initialize(int);
    void Shutdown();
    int GetModelCount();
    void GetData(int, float&, float&, float&);
};

#endif // _MODELLIST_H

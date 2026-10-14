#include "modelList.h"

ModelList::ModelList(const ModelList&) {}
ModelList::~ModelList() {}
int ModelList::GetModelCount() { return mModelCount; }
ModelList::ModelList() {
    mModelInfoList = nullptr;
}

void ModelList::Initialize(int numModels) {
    mModelCount = numModels;
    mModelInfoList = new ModelInfoType[mModelCount];
    srand((unsigned int)time(NULL));
    for (int i = 0; i < mModelCount; i++) {
        mModelInfoList[i].positionX = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f);
        mModelInfoList[i].positionY = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f);
        mModelInfoList[i].positionZ = (((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f);
    }
}

void ModelList::Shutdown() {
    if (mModelInfoList) {
        delete [] mModelInfoList;
        mModelInfoList = nullptr;
    }
}

void ModelList::GetData(int index, float& positionX, float& positionY, float& positionZ) {
    positionX = mModelInfoList[index].positionX;
    positionY = mModelInfoList[index].positionY;
    positionZ = mModelInfoList[index].positionZ;
}




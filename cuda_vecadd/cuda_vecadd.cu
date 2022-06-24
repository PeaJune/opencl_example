#include <iostream>
#include "cuda_runtime.h"
#include <vector>

using namespace std;

__global__ void VecAdd(int* A, int* B, int* C) {
    int i = threadIdx.x;
    C[i] = A[i] + B[i];
}

class CudaVecAdd {
public:
    bool operator()(vector<int> &a, vector<int> &b, vector<int> &c);
};

bool CudaVecAdd::operator()(vector<int>& a, vector<int>& b, vector<int>& c) {
    cudaError_t err = cudaSuccess;
    int *A, *B, *C;
    cout<<"size: "<<a.size()<<endl;
    err = cudaMalloc<int>(&A, a.size() * sizeof(int));
    if (err != cudaSuccess) {
        cerr<<"cudaMalloc failed. err: "<<err<<endl;
        return false;
    }
    err = cudaMalloc<int>(&B, a.size() * sizeof(int));
    if (err != cudaSuccess) {
        cout<<"cudaMalloc failed. err: "<<err<<endl;
        return false;
    }

    err = cudaMalloc<int>(&C, a.size() * sizeof(int));
    if (err != cudaSuccess) {
        cout<<"the cudaMalloc on GPU is failed. err: "<<err<<endl;
        return false;
    }
    err  = cudaMemcpy(A, &(a.front()), a.size() * sizeof(int), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        cerr<<"cudaMmecpy failed. err: "<<err<<endl;
    }
    err = cudaMemcpy(B, &(b.front()), sizeof(int) * b.size(), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        cerr<<"cudaMmecpy failed. err: "<<err<<endl;
    }
    VecAdd<<<1,a.size()>>>(A, B, C);
    err = cudaMemcpy(&c.front(), C, a.size() * sizeof(int), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        cerr<<"cudaMemcpy device to host failed. err:"<<err<<endl;
    }
    cudaFree(A);
    cudaFree(B);
    cudaFree(C);

    return true;
}

// 一个block 内存线程数据有限，如果数据量很大，需要用多个block来做。
int main() {
    CudaVecAdd cuda_va;
    vector<int> a;
    vector<int> b;
    vector<int> c;
    const int size = 1000;  

    a.resize(size, 0);
    b.resize(size, 0);
    c.resize(size, 0);

    for (int i = 0; i < size; ++i) {
        a[i] = i;
        b[i] = i;
    }
    cuda_va(a, b, c);
    for(int i = 0; i < 100; ++i) {
        cout<<" "<<c[i];
    }
    cout<<endl;

    return 0;
}

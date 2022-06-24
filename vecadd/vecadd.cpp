#include "CL/cl.h"
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

struct Timer {
    chrono::time_point<chrono::high_resolution_clock> start;
    void tic() {
        start = chrono::high_resolution_clock::now();
    }
    double toc() {
        chrono::duration<double> diff = chrono::high_resolution_clock::now() - start;
        return diff.count() * 1000; //ms
    }
};  

class VecAdd {
public:
    VecAdd();
    ~VecAdd();
    bool operator()(vector<int>& a, vector<int>& b, vector<int>& c);
private:
    const char *programSource = 
    "__kernel void vecadd(__global int* inputA, __global int* inputB, __global int* inputC) \n"
    "{ \n"
    "   int index = get_global_id(0); \n"
    "   inputC[index] = inputA[index] + inputB[index]; \n"
    "} \n";
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue cmdQueue;
    cl_program program;
    cl_kernel kernel;
};

VecAdd::VecAdd() {
    cl_int status;
    platform = nullptr;
    device = nullptr;
    context = nullptr;
    cmdQueue = nullptr;
    program = nullptr;
    kernel = nullptr;

    status = clGetPlatformIDs(1, &platform, nullptr);
    status  |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
    if (status != CL_SUCCESS) {
        cerr<<"GetDeviceIDs failed. err:"<<status<<endl;
        return ;
    }
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &status);
    if (status != CL_SUCCESS) {
        cerr<<"clCreateContext failed, err:"<<status<<endl;
        return ;
    }
    cmdQueue = clCreateCommandQueueWithProperties(context, device, nullptr, &status);
    if (status != CL_SUCCESS) {
        cerr<<"clCreateCommandQueueWithProperties failed. err:"<<status<<endl;
        return ;
    }
    program = clCreateProgramWithSource(context, 1, (const char**)&programSource, nullptr, &status);
    if (status != CL_SUCCESS) {
        cerr<<"clCreateProgramWithSource failed. err:"<<status<<endl;
        return ;
    }
    status = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (status != CL_SUCCESS) {
        cerr<<"clBuildProgram failed. err: "<<status<<endl;
        return ;
    }
    kernel = clCreateKernel(program, "vecadd", &status);
    if (status != CL_SUCCESS) {
        cerr<<"clCreateKernel failed. err: "<<status<<endl;
        return ;
    }
}

VecAdd::~VecAdd() {
    if (kernel != nullptr) {
        clReleaseKernel(kernel);
    }
    if (program != nullptr) {
        clReleaseProgram(program);
    }
    if (cmdQueue != nullptr) {
        clReleaseCommandQueue(cmdQueue);
    }
    if (context != nullptr) {
        clReleaseContext(context);
    }
}

bool VecAdd::operator()(vector<int>& a, vector<int>& b, vector<int>& c) {
    int datasize = a.size() * sizeof(int);
    cl_int status;
    cl_mem bufA;
    cl_mem bufB;
    cl_mem bufC;
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, nullptr, &status);
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, nullptr, &status);
    bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY,datasize, nullptr, &status);
    if (status != CL_SUCCESS) {
        cerr<<"clCreateBuffer failed. err:"<<status<<endl;
        return false;
    }
    //copy data to device
    status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize, &a.front(), 0, nullptr, nullptr);
    status |= clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, datasize, &b.front(), 0, nullptr, nullptr);
    if (status != CL_SUCCESS) {
        cerr<<"clEnqueueWriteBuffer failed, err: "<<status<<endl;
        return false;
    }
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);
    if (status != CL_SUCCESS) {
        cerr<<"clSetKernelArg failed. err: "<<status<<endl;
        return false;
    }
    size_t indexSpaceSize[1] = {a.size()};
    //size_t workGroupSize[1] = {}
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, nullptr, indexSpaceSize, nullptr, 0, nullptr, nullptr);
    if (status != CL_SUCCESS) {
        cerr <<"clEnqueueNDRangeKernel failed. err: "<<status<<endl;
        return false;
    }
    status = clFinish(cmdQueue);
    if (status != CL_SUCCESS) {
        cerr<<"clFinish failed. err : "<<status<<endl;
        return false;
    }
    //reade output
    status = clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, datasize, &c.front(), 0, nullptr, nullptr);
    if (status != CL_SUCCESS) {
        cerr<<"clEnqueueReadBuffer failed. err: "<<status<<endl;
        return false;
    }
    if (bufA != nullptr) {
        clReleaseMemObject(bufA);
    }
    if (bufB != nullptr) {
        clReleaseMemObject(bufB);
    }
    if (bufC != nullptr) {
        clReleaseMemObject(bufC);
    }
    return true;
}

//使用第一个平台的第一个设备
int main() {
    VecAdd vecadd;
    vector<int> a;
    vector<int> b;
    vector<int> c;
    int size = 1000000;
    a.resize(size);
    b.resize(size);
    c.resize(size);
    for (int i = 0; i < size; ++i) {
        a[i] = i;
        b[i] = i;
    }
    Timer t;
    t.tic();
    vecadd(a, b, c);
    cout<<"timeout:"<<t.toc()<<" ms"<<endl;
    for(int i = 0; i < 100; ++i) {
        cout<<" "<<c[i];
    }
    cout<<endl;
    return 0;
}
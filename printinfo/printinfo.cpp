#include "CL/cl.h"
#include <iostream>
#include <vector>

using namespace std;

class PrintInfo {
public:
    void operator()();
private:
    cl_int status ;
    cl_uint numPlatforms;
    cl_platform_id *platforms;
    cl_uint numDevices;
    cl_device_id *devices;
};

void PrintInfo::operator()() {
    status = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (status != CL_SUCCESS) {
        cerr<<"clGetPlatfromIDs failed. err:"<<status<<endl;
        return ;
    }
    platforms = new cl_platform_id[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platforms, nullptr);
    if (status != CL_SUCCESS) {
        cerr<<"clGetPlatformIDs failed. err:"<<status<<endl;
        return ;
    }
    for (int i = 0; i < numPlatforms; ++i) {
        cout<<"platform id :"<<platforms[0]<<endl;
        size_t size;
        string str(512, '\0');
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, nullptr, &size);
        str.resize(size, '\0');
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, size, &str.front(), nullptr);
        cout<<"\tName   = "<<str;

		clGetPlatformInfo( platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &size );
		str.resize(size, '\0');
		clGetPlatformInfo( platforms[i], CL_PLATFORM_VENDOR, size, &str.front(), NULL );
		cout<<"\tVendor  = "<<str<<endl;

		clGetPlatformInfo( platforms[i], CL_PLATFORM_VERSION, 0, NULL, &size );
		str.resize(size, '\0');
		clGetPlatformInfo( platforms[i], CL_PLATFORM_VERSION, size, &str.front(), NULL );
		cout<<"\tVersion = "<<str<<endl;

		clGetPlatformInfo( platforms[i], CL_PLATFORM_PROFILE, 0, NULL, &size );
		str.resize(size, '\0');;
		clGetPlatformInfo( platforms[i], CL_PLATFORM_PROFILE, size, &str.front(), NULL );
		cout<<"\tProfile = "<<str<<endl;
        
        //device
        status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
        if (status != CL_SUCCESS) {
            cerr<<"clGetDeviceIDs failed. err:"<<status<<endl;
            return ;
        }
        devices = new cl_device_id[numDevices];
        status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, numDevices, devices, nullptr);
        if (status != CL_SUCCESS) {
            cerr<<"clGetDeviceIDs failed. err:"<<status<<endl;
            return ;
        }
        for (int j = 0; j < numDevices; ++j) {
            cl_uint ui;
            cout<<"\tDevice "<<devices[i]<<" : "<<endl;
            clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, 0, nullptr, &size);
            str.resize(size, '\0');
            clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, size, &str.front(), &size);
            cout<<"\t\tVendor:"<<str<<endl;

            clGetDeviceInfo( devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(ui), &ui, NULL );
			cout<<"\t\tDevice Maximum Compute Units = "<<ui<<endl;

			clGetDeviceInfo( devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(ui), &ui, NULL );
		    cout<<"\t\tDevice Maximum Work Item Dimensions = "<<ui<<endl;
            size_t sizes[3] = {0};
			clGetDeviceInfo( devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(sizes), sizes, NULL );
			cout<<"\t\tDevice Maximum Work Item Sizes = "<<sizes[0]<<"x"<<sizes[1]<<"x"<<sizes[2]<<endl;

			clGetDeviceInfo( devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size), &size, NULL );
			cout<<"\t\tDevice Maximum Work Group Size = "<<size<<endl;

			clGetDeviceInfo( devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(ui), &ui, NULL );
			cout<<"\t\tDevice Maximum Clock Frequency = "<<ui<<" MHZ"<<endl;

            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 0, nullptr, &size);
            str.resize(size, '\0');
            clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, size, &str.front(), nullptr);
            cout<<"\t\tDevice Extensions: \n"<<str<<endl;
        }
    }
}

int main() {
    PrintInfo()();
    return 0;
}
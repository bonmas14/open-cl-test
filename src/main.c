// C standard includes
#include <stdio.h>

// OpenCL includes
#include <CL/cl.h>
void CL_CALLBACK ErrorCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data) {
    printf_s("error occured %u\n", *errinfo);
}

int main()
{
    cl_int CL_err = CL_SUCCESS;
    cl_platform_id platform;

    CL_err = clGetPlatformIDs(1, &platform, NULL);
 
    if (CL_err != CL_SUCCESS) {
        printf_s("no platforms\n");
        return 0;
    }

    cl_device_id device;
    cl_uint num_devices;

    CL_err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);

    if (CL_err != CL_SUCCESS) {
        printf_s("no devices\n");
        return 0;
    }

    printf_s("device count:%u. device id:%u\n", num_devices, device);

    cl_context context = clCreateContext(NULL, 1, &device, ErrorCallback, NULL, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf_s("error creating context. code %u\n", CL_err);
        return 0;
    }
    
    CL_err = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device, NULL);

    if (CL_err != CL_SUCCESS) {
        printf_s("error in context info retrive. code %i\n", CL_err);
        return 0;
    }
    
    size_t work_group_size = 0;
    CL_err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);

    if (CL_err != CL_SUCCESS) {
        printf_s("error when retrieving device info\n");
        return 0;
    }

    printf_s("device %zu, workgroup size: %zu\n", 0, work_group_size);
    
    clReleaseContext(context);
    return 0;
}


// C standard includes
#include <stdio.h>

// OpenCL includes
#include <CL/cl.h>

int main()
{
    cl_int CL_err = CL_SUCCESS;
    cl_platform_id platform;

    CL_err = clGetPlatformIDs(1, &platform, NULL);
 
    if (CL_err != CL_SUCCESS) {
        printf("no platforms\n");
        return 0;
    }

    cl_device_id device;
    cl_uint num_devices;

    CL_err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);

    if (CL_err != CL_SUCCESS) {
        printf("no devices\n");
        return 0;
    }

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating context\n");
        return 0;
    }
    
    CL_err = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device, NULL);

    if (CL_err != CL_SUCCESS) {
        printf("error in context info retrive.\n");
        return 0;
    }
    
    size_t work_group_size = 0;
    CL_err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);

    if (CL_err != CL_SUCCESS) {
        printf("error when retrieving device info\n");
        return 0;
    }
    
    char data[] = "__kernel void multiply_krnl(global const int* a, global const int* b, global int* c) { c[get_global_id(0)]=a[get_global_id(0)]*b[get_global_id(0)]; }";

    char* source[] = { data };

    cl_program program = clCreateProgramWithSource(
            context,
            1,
            source,
            NULL,
            &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating program\n");
        return 0;
    }

    CL_err = clBuildProgram(program, 1, &device, "", NULL, NULL);

    if (CL_err != CL_SUCCESS) {
        printf("error building program\n");
        return 0;
    }

    char kernel_name[] = "multiply_krnl";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating kernel in program\n");

    }

    clReleaseContext(context);
    return 0;
}

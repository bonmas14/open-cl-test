// C standard includes
#include <stdio.h>

// OpenCL includes
#include <CL/cl.h>

int main() {
    cl_int CL_err = CL_SUCCESS;
    cl_platform_id platform;

    CL_err = clGetPlatformIDs(1, &platform, NULL);
 
    if (CL_err != CL_SUCCESS) {
        printf("no platforms\n");
        return CL_err;
    }

    cl_device_id device;
    cl_uint num_devices;

    CL_err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);

    if (CL_err != CL_SUCCESS) {
        printf("no devices\n");
        return CL_err;
    }

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating context\n");
        return CL_err;
    }
    
    CL_err = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device, NULL);

    if (CL_err != CL_SUCCESS) {
        printf("error in context info retrive.\n");
        return CL_err;
    }
    
    size_t work_group_size = 0;
    CL_err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);

    if (CL_err != CL_SUCCESS) {
        printf("error when retrieving device info\n");
        return CL_err;
    }

    cl_command_queue command_queue = clCreateCommandQueue(context, device, 0, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error when creating queue\n");
        return CL_err;
    }
    
    char data[] = "__kernel void multiply_krnl(global const float* a, global const float* b, global float* c) { c[get_global_id(0)]=a[get_global_id(0)]*b[get_global_id(0)]; }";

    char* source[] = { data };

    cl_program program = clCreateProgramWithSource(
            context,
            1,
            source,
            NULL,
            &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating program\n");
        return CL_err;
    }

    CL_err = clBuildProgram(program, 1, &device, "", NULL, NULL);

    if (CL_err != CL_SUCCESS) {
        printf("error building program\n");
        return CL_err;
    }

    char kernel_name[] = "multiply_krnl";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating kernel in program\n");
        return CL_err;
    }
    
    float c_data[8];
    float a_data[] = { 32, 4, 12, 43, 24, 54, 70, 10 };
    float b_data[] = { 12, 3, 4, 54, 34, 10, 43, 29 };

    cl_mem a_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * 8, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error creating buffer A\n");
        return CL_err;
    }

    cl_mem b_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * 8, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error creating buffer B\n");
        return CL_err;
    }

    cl_mem c_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * 8, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error creating buffer C\n");
        return CL_err;
    }

    CL_err = clEnqueueWriteBuffer(command_queue, a_mem, CL_TRUE, 0, sizeof(cl_float) * 8, a_data, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        printf("error enqueueing buffer A\n");
        return CL_err;
    }

    CL_err = clEnqueueWriteBuffer(command_queue, b_mem, CL_TRUE, 0, sizeof(cl_float) * 8, b_data, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        printf("error enqueueing buffer B\n");
        return CL_err;
    }

    cl_event exec = 0;
    size_t dim[3] = { 0, 0, 0 };
    size_t work[3] = { 8, 0, 0 };
    size_t loc_work[3] = { 1, 0, 0 };

    CL_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_mem);
    if (CL_err != CL_SUCCESS) {
        printf("error setting up 3\n");
        return CL_err;
    }
    CL_err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_mem);
    if (CL_err != CL_SUCCESS) {
        printf("error setting up 3\n");
        return CL_err;
    }
    CL_err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &c_mem);
    if (CL_err != CL_SUCCESS) {
        printf("error setting up 3\n");
        return CL_err;
    }

    CL_err = clEnqueueNDRangeKernel(command_queue, kernel, 1, dim, work, loc_work, 0, NULL, &exec);
    if (CL_err != CL_SUCCESS) {
        printf("error executing 0\n");
        return CL_err;
    }

    CL_err = clFinish(command_queue);
    if (CL_err != CL_SUCCESS) {
        printf("error executing 2\n");
        return CL_err;
    }

    cl_event output = 0;

    CL_err = clEnqueueReadBuffer(command_queue, c_mem, CL_TRUE, 0, sizeof(float) * 8, c_data, 0, NULL, &output); 
    if (CL_err != CL_SUCCESS) {
        printf("error reading\n");
        return CL_err;
    }
    CL_err = clFinish(command_queue);
    if (CL_err != CL_SUCCESS) {
        printf("error executing 2\n");
        return CL_err;
    }

    for (size_t i = 0; i < 8; i++) {
        printf("%2f\t", c_data[i]);
    }
    printf("\n");

    clReleaseContext(context);
    return 0;
}

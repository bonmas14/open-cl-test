// C standard includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define ELEMENTS (1 << 16)
#define AMPLITUDE 10

// OpenCL includes
#include <CL/cl.h>
#include <string.h>

typedef struct {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
} context_t;

context_t create_context(void);

cl_program load_program(context_t context, char* path);

int main() {
    printf("init\n");
    cl_int CL_err = CL_SUCCESS;
    context_t cntx = create_context();
    
    cl_program program = load_program(cntx, "./runtime/test.cl");

    char kernel_name[] = "multiply_krnl";
    cl_kernel kernel = clCreateKernel(program, kernel_name, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error creating kernel in program\n");
        return CL_err;
    }

    float* c_data = calloc(ELEMENTS, sizeof(float));
    float* a_data = calloc(ELEMENTS, sizeof(float));
    float* b_data = calloc(ELEMENTS, sizeof(float));

    for (size_t i = 0; i < ELEMENTS; i++) {
        a_data[i] = (float)rand()/(float)(RAND_MAX / AMPLITUDE);
        b_data[i] = (float)rand()/(float)(RAND_MAX / AMPLITUDE);
    }

    printf("count of elements: %d\n", ELEMENTS);

    cl_mem a_mem = clCreateBuffer(cntx.context, CL_MEM_READ_WRITE, sizeof(cl_float) * ELEMENTS, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error creating buffer A\n");
        return CL_err;
    }

    cl_mem b_mem = clCreateBuffer(cntx.context, CL_MEM_READ_WRITE, sizeof(cl_float) * ELEMENTS, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error creating buffer B\n");
        return CL_err;
    }

    cl_mem c_mem = clCreateBuffer(cntx.context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * ELEMENTS, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        printf("error creating buffer C\n");
        return CL_err;
    }

    cl_command_queue command_queue = clCreateCommandQueue(cntx.context, cntx.device, 0, &CL_err);

    if (CL_err != CL_SUCCESS) {
        printf("error when creating queue\n");
        return CL_err;
    }

    CL_err = clEnqueueWriteBuffer(command_queue, a_mem, CL_TRUE, 0, sizeof(cl_float) * ELEMENTS, a_data, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        printf("error enqueueing buffer A\n");
        return CL_err;
    }

    CL_err = clEnqueueWriteBuffer(command_queue, b_mem, CL_TRUE, 0, sizeof(cl_float) * ELEMENTS, b_data, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        printf("error enqueueing buffer B\n");
        return CL_err;
    }

    cl_event exec = 0;
    size_t dim[3] = { 0, 0, 0 };
    size_t work[3] = { ELEMENTS, 0, 0 };
    size_t loc_work[3] = { 128, 0, 0 };

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

    printf("starting\n");
    clock_t start = clock();

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

    clock_t stop = clock();
    printf("stopping\n");

    printf("clocks count: %d |\tclocks per sec: %d\n", stop - start, CLOCKS_PER_SEC);

    cl_event output = 0;

    printf("getting data\n");
    CL_err = clEnqueueReadBuffer(command_queue, c_mem, CL_TRUE, 0, sizeof(float) * ELEMENTS, c_data, 0, NULL, &output); 
    if (CL_err != CL_SUCCESS) {
        printf("error reading\n");
        return CL_err;
    }

    printf("got data\n");
    for (size_t i = 0; i < ELEMENTS; i++) {
        //printf("%.1f  ", c_data[i]);
    }
    printf("\n");

    clReleaseContext(cntx.context);
    (void)getchar();
    return 0;
}

context_t create_context(void) {
    cl_int error;

    cl_context context;
    cl_device_id device;
    cl_platform_id platform;

    error = clGetPlatformIDs(1, &platform, NULL);
 
    if (error != CL_SUCCESS) {
        printf("no platforms\n");
        exit(error);
    }

    cl_uint num_devices;

    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);

    if (error != CL_SUCCESS || num_devices == 0) {
        printf("no devices\n");
        exit(error);
    }

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);

    if (error != CL_SUCCESS) {
        printf("error creating context\n");
        exit(error);
    }

    return (context_t) {
        .platform = platform,
        .device   = device,
        .context  = context 
    };
}

cl_program load_program(context_t context, char* path) {
    cl_int error = 0;

    FILE* file = fopen(path, "r");
    
    if (file == NULL) {
        printf("file can not be opened\n");
        clReleaseContext(context.context);
        exit(0);
    }

    size_t counter = 0;
    char** source = malloc(sizeof(char*) * (counter + 1));
    
    char buffer[256] = { 0 };
    while (fgets(buffer, 255, file) != NULL) {
        buffer[255] = 0;

        void* new_arr = realloc(source, sizeof(char*) * (counter + 1));
        if (new_arr == NULL) {
            printf("error while realloc\n");
            clReleaseContext(context.context);
            fclose(file);
            exit(0);
        }
        source = new_arr;

        char* dest = calloc(strlen(buffer), sizeof(char));
        source[counter] = memcpy(dest, buffer, sizeof(char) * strlen(buffer));

        counter++;
    }

    fclose(file);
    
    cl_program program = clCreateProgramWithSource(
            context.context,
            (cl_uint)counter, // or ++
            source,
            NULL,
            &error);

    for (size_t i = 0; i < counter; i++) {
        free(source[i]);
    }

    free(source);


    if (error != CL_SUCCESS) {
        printf("error creating program\n");
        exit(error);
    }

    error = clBuildProgram(program, 1, &(context.device), "", NULL, NULL);

    if (error != CL_SUCCESS) {
        printf("error building program\n");
        exit(error);
    }

    return program;
}

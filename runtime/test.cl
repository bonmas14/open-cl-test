__kernel void multiply_krnl(global const float* a, global const float* b, global float* c)
{ 
    c[get_global_id(0)] = a[get_global_id(0)]*b[get_global_id(0)]; 
}

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;
// CUDA runtime
#include <cuda_runtime.h>
// helper functions and utilities to work with CUDA
#include <device_launch_parameters.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

#define N 38
#define count 274877906944
#define THREAD_NUM 1024
#define BLOCK_NUM 2048
#define roundnum 844

__device__ u8 Trivium(u8* gpu_key, u8* gpu_pos, u64 num, u8* gpu_Free_Cube, u8 iv_loc, u8 freecubedim)
{
    
    u32 value[N] = { 0 };
    u32 IV[3] = { 0,0,0 };
    u32 i;
    for (i = 0; i < N; i++)
        value[i] = ((num >> i) & 0x01);
    for (i = 0; i < N; i++)
        if (value[i] == 1)
            IV[(gpu_pos[i] >> 5)] |= value[i] << (gpu_pos[i] & 0x1f);

    for (i = 0; i < freecubedim; i++)
    {
        if (iv_loc & 1)
        {
            if (gpu_Free_Cube[i] < 32)
            {
                IV[0] = IV[0] ^ (1 << gpu_Free_Cube[i]);
            }
            else if (gpu_Free_Cube[i] < 64)
            {
                IV[1] = IV[1] ^ (1 << (gpu_Free_Cube[i] - 32));
            }
            else if (gpu_Free_Cube[i] < 80)
            {
                IV[2] = IV[2] ^ (1 << (gpu_Free_Cube[i] - 64));
            }
        }
        iv_loc = iv_loc >> 1;
    }

    



    u32 t1, t2, t3;
    u32 s0, s1, s2, s3, s4, s5, s6, s7, s8, s9;
    u32 temp1, temp2, temp3, temp4, temp5, temp6;
    u32 z, z1;

    s0 = gpu_key[0] ^ (gpu_key[1] << 8) ^ (gpu_key[2] << 16) ^ (gpu_key[3] << 24);
    s1 = gpu_key[4] ^ (gpu_key[5] << 8) ^ (gpu_key[6] << 16) ^ (gpu_key[7] << 24);
    s2 = gpu_key[8] ^ (gpu_key[9] << 8);
    s3 = IV[0];
    s4 = IV[1];
    s5 = IV[2];
    s6 = 0;
    s7 = 0;
    s8 = 0;
    s9 = 0x00007000;

    u32 roundnum_word = roundnum / 32;
    u32 roundnum_bit = roundnum % 32;
    for (i = 0; i < roundnum_word; i++)
    {
        temp1 = (s2 << 30) | (s1 >> 2);
        temp2 = (s2 << 3) | (s1 >> 29);
        temp3 = (s5 << 27) | (s4 >> 5);
        temp4 = (s5 << 12) | (s4 >> 20);
        temp5 = (s8 << 30) | (s7 >> 2);
        temp6 = (s9 << 17) | (s8 >> 15);

        t1 = temp1 ^ temp2;
        t2 = temp3 ^ temp4;
        t3 = temp5 ^ temp6;
        //z = t1^t2^t3;

        //  t1 = t1 + s91s92 + s171
        temp1 = (s2 << 5) | (s1 >> 27);
        temp2 = (s2 << 4) | (s1 >> 28);
        temp3 = (s5 << 18) | (s4 >> 14);
        t1 ^= (temp1 & temp2) ^ temp3;

        //  t2 = t2 + s175s176 + s264
        temp1 = (s5 << 14) | (s4 >> 18);
        temp2 = (s5 << 13) | (s4 >> 19);
        temp3 = (s8 << 9) | (s7 >> 23);
        t2 ^= (temp1 & temp2) ^ temp3;

        //  t3 = t3 + s286s287 + s69
        temp1 = (s9 << 19) | (s8 >> 13);
        temp2 = (s9 << 18) | (s8 >> 14);
        temp3 = (s2 << 27) | (s1 >> 5);
        t3 ^= (temp1 & temp2) ^ temp3;

        // update register 1
        s2 = (s1) & (0x1FFFFFFF);
        s1 = s0;
        s0 = t3;

        //  update register 2
        s5 = s4 & (0x000FFFFF);
        s4 = s3;
        s3 = t1;

        //  update register 3
        s9 = s8 & (0x00007FFF);
        s8 = s7;
        s7 = s6;
        s6 = t2;
    }
    if (roundnum_bit != 0)
    {
        temp1 = (s2 << 30) | (s1 >> 2);//���֮���൱��ֱ�Ӽ���
        temp2 = (s2 << 3) | (s1 >> 29);
        temp3 = (s5 << 27) | (s4 >> 5);
        temp4 = (s5 << 12) | (s4 >> 20);
        temp5 = (s8 << 30) | (s7 >> 2);
        temp6 = (s9 << 17) | (s8 >> 15);

        t1 = temp1 ^ temp2;
        t2 = temp3 ^ temp4;
        t3 = temp5 ^ temp6;
        z1 = temp1 ^ temp2 ^ temp3 ^ temp4 ^ temp5 ^ temp6;
        //z1=z;

        //t1 = t1 + s91s92 + s171
        temp1 = (s2 << 5) | (s1 >> 27);
        temp2 = (s2 << 4) | (s1 >> 28);
        temp3 = (s5 << 18) | (s4 >> 14);

        t1 ^= (temp1 & temp2) ^ temp3;

        //t2 = t2 + s175s176 + s264
        temp1 = (s5 << 14) | (s4 >> 18);
        temp2 = (s5 << 13) | (s4 >> 19);
        temp3 = (s8 << 9) | (s7 >> 23);

        t2 ^= (temp1 & temp2) ^ temp3;

        //t3 = t3 + s286s287 + s69
        temp1 = (s9 << 19) | (s8 >> 13);
        temp2 = (s9 << 18) | (s8 >> 14);
        temp3 = (s2 << 27) | (s1 >> 5);

        t3 ^= (temp1 & temp2) ^ temp3;

        // update register 1
        s2 = (s1) & (0x1FFFFFFF);
        s1 = s0;
        s0 = t3;

        //update register 2
        s5 = s4 & (0x000FFFFF);
        s4 = s3;
        s3 = t1;

        //update register 3
        s9 = s8 & (0x00007FFF);
        s8 = s7;
        s7 = s6;
        s6 = t2;
    }
    temp1 = (s2 << 30) | (s1 >> 2);
    temp2 = (s2 << 3) | (s1 >> 29);
    temp3 = (s5 << 27) | (s4 >> 5);
    temp4 = (s5 << 12) | (s4 >> 20);
    temp5 = (s8 << 30) | (s7 >> 2);
    temp6 = (s9 << 17) | (s8 >> 15);
    z = temp1 ^ temp2 ^ temp3 ^ temp4 ^ temp5 ^ temp6;
    if (roundnum_bit != 0)
        z = (z1 << roundnum_bit) | (z >> (32 - roundnum_bit));
    u8 tmp = z >> 31;
    return tmp;
}

__global__ void kernel(u8* gpu_truthtable, u8* gpu_key, u8* gpu_pos, u8* gpu_Free_Cube, u8 iv_loc, u8 freecubedim)
{

    const  u32 tidx = threadIdx.x;
    const  u32 bid = blockIdx.x;
    u8 result = 0;
    for (u64 i = bid * THREAD_NUM + tidx; i < count; i = i + BLOCK_NUM * THREAD_NUM)
    {
        result ^= Trivium(gpu_key, gpu_pos, i, gpu_Free_Cube, iv_loc, freecubedim);
    }
    gpu_truthtable[bid * THREAD_NUM + tidx] = result;
}

u8 excute(u8* Key, u8* Pos, u8* Free_Cube, u8 iv_loc, u8 freecubedim)
{
    u8* gpu_key, * gpu_pos, * gpu_truthtable, * gpu_Free_Cube;
    u8* truthtable = (u8*)calloc(THREAD_NUM * BLOCK_NUM, sizeof(u8));


    cudaMalloc((void**)&gpu_truthtable, THREAD_NUM * BLOCK_NUM * sizeof(u8));
    cudaMalloc((void**)&gpu_key, 10 * sizeof(u8));
    cudaMalloc((void**)&gpu_pos, N * sizeof(u8));
    cudaMalloc((void**)&gpu_Free_Cube, freecubedim * sizeof(u8));

    cudaMemcpy(gpu_key, Key, 10 * sizeof(u8), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_pos, Pos, N * sizeof(u8), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_Free_Cube, Free_Cube, freecubedim * sizeof(u8), cudaMemcpyHostToDevice);

    kernel << <BLOCK_NUM, THREAD_NUM >> > (gpu_truthtable, gpu_key, gpu_pos, gpu_Free_Cube, iv_loc, freecubedim);
    cudaError err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        printf("CUDA Error: %s\n", cudaGetErrorString(err));
    }
    cudaMemcpy(truthtable, gpu_truthtable, THREAD_NUM * BLOCK_NUM * sizeof(u8), cudaMemcpyDeviceToHost);

    u8 result = 0;
    for (u64 i = 0; i < THREAD_NUM * BLOCK_NUM; i++)
    {
        result = result ^ truthtable[i];
    }
    
    free(truthtable);
    cudaFree(gpu_truthtable);
    cudaFree(gpu_key);
    cudaFree(gpu_pos);
    cudaFree(gpu_Free_Cube);


    cudaDeviceReset();
    return result;
}


int qiumi(int x, int y)
{
    int result = 1;
    for (size_t i = 0; i < y; i++)
    {
        result = result * x;
    }
    return result;
}



int main()
{

    u8 cube[21][38] = {
        {0,2,4,6,8,9,11,13,15,17,19,21,23,26,28,30,32,34,36,38,41,43,45,47,49,51,53,56,58,60,62,64,66,68,71,73,75,79},
        {0,1,2,4,6,8,11,13,15,17,19,21,23,26,28,30,32,34,36,38,41,43,45,47,49,51,53,56,58,60,62,64,66,68,71,73,75,79},
        {1,3,5,7,8,10,12,14,16,18,20,22,25,27,29,31,33,35,37,40,42,44,46,48,50,52,55,57,59,61,63,65,67,70,72,74,76,78},
        {0,2,4,6,9,11,13,15,17,19,21,24,26,28,30,32,34,36,39,41,43,45,47,49,51,54,56,58,60,62,64,66,69,71,73,75,77,79},
        {0,2,4,6,8,10,12,15,17,19,21,23,25,27,30,32,34,36,38,40,42,45,47,49,51,53,55,57,60,62,64,66,68,70,72,75,77,79},
        {0,1,2,4,7,9,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,79},
        {0,2,4,6,9,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,77,79},
        {0,1,2,4,6,9,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,79},
        {1,3,5,7,9,10,12,14,16,18,20,22,25,27,29,31,33,35,37,40,42,44,46,48,50,52,55,57,59,61,63,65,67,70,72,74,76,78},
        {0,2,4,7,9,10,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,79},
        {0,1,3,5,7,9,11,13,16,18,20,22,24,26,28,31,33,35,37,39,41,43,46,48,50,52,54,56,58,61,63,65,67,69,71,73,76,78},
        {0,2,4,6,7,9,11,13,15,17,19,21,24,26,28,30,32,34,36,39,41,43,45,47,49,51,54,56,58,60,62,64,66,69,71,73,75,79},
        {1,3,5,6,7,10,12,14,16,18,20,22,25,27,29,31,33,35,37,40,42,44,46,48,50,52,55,57,59,61,63,65,67,70,72,74,76,78},
        {0,1,2,4,6,9,11,13,15,17,19,21,24,26,28,30,32,34,36,39,41,43,45,47,49,51,54,56,58,60,62,64,66,69,71,73,75,79},
        {0,2,4,6,8,10,11,13,15,17,19,21,23,26,28,30,32,34,36,38,41,43,45,47,49,51,53,56,58,60,62,64,66,68,71,73,75,79},
        {0,2,4,7,8,9,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,79},
        {0,2,4,6,8,9,11,13,15,17,19,21,24,26,28,30,32,34,36,39,41,43,45,47,49,51,54,56,58,60,62,64,66,69,71,73,75,79},
        {0,2,4,6,9,10,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,79},
        {0,2,4,7,9,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,77,79},
        {1,3,5,7,8,9,11,13,16,18,20,22,24,26,28,31,33,35,37,39,41,43,46,48,50,52,54,56,58,61,63,65,67,69,71,73,76,78},
        {0,2,4,8,9,10,11,13,15,17,19,22,24,26,28,30,32,34,37,39,41,43,45,47,49,52,54,56,58,60,62,64,67,69,71,73,75,79},
};

    u8 Free_Cube[21][4] = {
        {1,10,76,77},
        {9,10,76,77},
        {0,2,9,11},
        {1,7,8,10},
        {3,5,7},
        {6,8,10,77},
        {7,8,10,20},
        {8,10,20,77},
        {2,8,11},
        {1,5,6,8},
        {2,6,79},
        {1,5,8,10},
        {0,8,9,11},
        {8,10,76,77},
        {1,9,76,77},
        {6,10,77},
        {1,7,10,76},
        {1,7,8,20},
        {1,5,6,8},
        {0,6,79},
        {5,6,7,77},
};

    u8 freecubedim[21] = {4,4,4,4,3,4,4,4,3,4,3,4,4,4,4,3,4,4,4,3,4};


    clock_t start_time, finish_time;
    u8 key[10] = { 0 };
    u8 result, i, j;
    u8 iv_loc;
    srand((unsigned)time(NULL)); 
    u32 flag = 0;

    for (i = 0; i < 128; i++)
    {
        for (j = 0; j < 10; j++)
        { 
            key[j] += rand() % 256;
            printf("%d ", key[j]);
        } 
        printf("\n");

        start_time = clock();
        for (j = 0; j < 21; j++)
        {
            for (iv_loc = 0; iv_loc < qiumi(2,freecubedim[j]); iv_loc++)
            {
                flag++;
                result = excute(key, cube[j], Free_Cube[j], iv_loc, freecubedim[j]);
                if (result == 1)
                    break;
            }
            printf("%d,", result);
        }
        printf("\n");
        finish_time = clock();
        printf("IV:%d, time:%f\n", flag, (float)(finish_time - start_time) / CLOCKS_PER_SEC);
    }

    return 0;
}
 

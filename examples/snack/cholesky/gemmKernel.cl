typedef union GPtr {
    __global float *f;
    __global float2 *f2v;
    __global float4 *f4v;
    __global float8 *f8v;
    __global float16 *f16v;
} GPtr;

typedef union LPtr {
    __local float *f;
    __local float2 *f2v;
    __local float4 *f4v;
    __local float8 *f8v;
    __local float16 *f16v;
} LPtr;

typedef union PPtr {
    float *f;
    float2 *f2v;
    float4 *f4v;
    float8 *f8v;
    float16 *f16v;
} PPtr;

__attribute__((reqd_work_group_size(8, 8, 1)))
__kernel void 
sgemmBlock(
    uint M,
    uint N,
    uint K,
    const float alpha,
    const float beta,
    //__constant float8 *A,
    __global const float8 *A,
    __global const float8 *B,
    __global float8 *C,
    uint lda,
    uint ldb,
    uint ldc,
    const uint offA,
    const uint offB,
    const uint offC)
{
    float8 a0, a1, a2, a3;
    float8 b0, b1, b2, b3;
    float8 c0, c1, c2, c3, c4, c5, c6, c7;
    uint4 coord = 0u; /* contains coordB, coordA, k */

    lda /= 8;
    ldb /= 8;
    uint kif;
    uint get_group_id_1;
    uint get_global_id_1;
    A += (uint)get_global_id(0);
    get_group_id_1 = (get_group_id(0) + get_group_id(1))% get_num_groups(1);
    get_global_id_1 = get_group_id_1 * get_local_size(1) + get_local_id(1);
    kif = (N % 512 != 0);
    get_global_id_1 = (kif*(uint)get_global_id(1)) + ((1-kif)*get_global_id_1);
    B += get_global_id_1;
    coord.y = 8u * (uint)get_global_id(0);
    coord.x = 8u * (uint)get_global_id_1;
    A += offA / 8;
    B += offB / 8;
    C += offC / 8;
    c0 = 0;
    c1 = 0;
    c2 = 0;
    c3 = 0;
    c4 = 0;
    c5 = 0;
    c6 = 0;
    c7 = 0;

    for (uint k1 = 0; k1 < K; k1 += 4) {
        /* -- Tiles multiplier -- */
        b0 = B[0];
        b1 = B[ldb];
        b2 = B[(ldb << 1)];
        b3 = B[mad24(3u, ldb, 0u)];

        a0 = A[0];
        a1 = A[lda];
        a2 = A[(lda << 1)];
        a3 = A[mad24(3u, lda, 0u)];

        c0 = mad(a0, b0.s0, c0);
        c1 = mad(a0, b0.s1, c1);
        c2 = mad(a0, b0.s2, c2);
        c3 = mad(a0, b0.s3, c3);
        c4 = mad(a0, b0.s4, c4);
        c5 = mad(a0, b0.s5, c5);
        c6 = mad(a0, b0.s6, c6);
        c7 = mad(a0, b0.s7, c7);

        c0 = mad(a1, b1.s0, c0);
        c1 = mad(a1, b1.s1, c1);
        c2 = mad(a1, b1.s2, c2);
        c3 = mad(a1, b1.s3, c3);
        c4 = mad(a1, b1.s4, c4);
        c5 = mad(a1, b1.s5, c5);
        c6 = mad(a1, b1.s6, c6);
        c7 = mad(a1, b1.s7, c7);

        c0 = mad(a2, b2.s0, c0);
        c1 = mad(a2, b2.s1, c1);
        c2 = mad(a2, b2.s2, c2);
        c3 = mad(a2, b2.s3, c3);
        c4 = mad(a2, b2.s4, c4);
        c5 = mad(a2, b2.s5, c5);
        c6 = mad(a2, b2.s6, c6);
        c7 = mad(a2, b2.s7, c7);

        c0 = mad(a3, b3.s0, c0);
        c1 = mad(a3, b3.s1, c1);
        c2 = mad(a3, b3.s2, c2);
        c3 = mad(a3, b3.s3, c3);
        c4 = mad(a3, b3.s4, c4);
        c5 = mad(a3, b3.s5, c5);
        c6 = mad(a3, b3.s6, c6);
        c7 = mad(a3, b3.s7, c7);

        A += (lda << 2);
        B += (ldb << 2);
        /* ---------------------- */
    }


    GPtr uC;

    uC.f = C + (coord.x * ldc + coord.y)/8;

    __global float8 *pC = uC.f8v;

    float8 tempC0, tempC1, tempC2, tempC3, tempC4, tempC5, tempC6, tempC7;

    tempC0 = pC[0];
    tempC1 = pC[(ldc >> 3)];
    tempC2 = pC[(ldc >> 2)];
    tempC3 = pC[mad24(3u, (ldc >> 3), 0u)];
    tempC4 = pC[(ldc >> 1)];
    tempC5 = pC[mad24(5u, (ldc >> 3), 0u)];
    tempC6 = pC[mad24(6u, (ldc >> 3), 0u)];
    tempC7 = pC[mad24(7u, (ldc >> 3), 0u)];
    tempC0 = mad(tempC0, beta, 0);
    tempC1 = mad(tempC1, beta, 0);
    tempC2 = mad(tempC2, beta, 0);
    tempC3 = mad(tempC3, beta, 0);
    tempC4 = mad(tempC4, beta, 0);
    tempC5 = mad(tempC5, beta, 0);
    tempC6 = mad(tempC6, beta, 0);
    tempC7 = mad(tempC7, beta, 0);
    tempC0 = mad(c0, alpha, tempC0);
    tempC1 = mad(c1, alpha, tempC1);
    tempC2 = mad(c2, alpha, tempC2);
    tempC3 = mad(c3, alpha, tempC3);
    tempC4 = mad(c4, alpha, tempC4);
    tempC5 = mad(c5, alpha, tempC5);
    tempC6 = mad(c6, alpha, tempC6);
    tempC7 = mad(c7, alpha, tempC7);
    pC[0] = tempC0;
    pC[(ldc >> 3)] = tempC1;
    pC[(ldc >> 2)] = tempC2;
    pC[mad24(3u, (ldc >> 3), 0u)] = tempC3;
    pC[(ldc >> 1)] = tempC4;
    pC[mad24(5u, (ldc >> 3), 0u)] = tempC5;
    pC[mad24(6u, (ldc >> 3), 0u)] = tempC6;
    pC[mad24(7u, (ldc >> 3), 0u)] = tempC7;
}

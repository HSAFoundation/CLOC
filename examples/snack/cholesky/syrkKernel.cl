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

__attribute__((reqd_work_group_size(64, 1, 1)))
__kernel void ssyrkBlock(
    uint N,
    const uint K,
    const float alpha,
    __global const float8 *A,
    uint lda,
    const float beta,
    __global float *C,
    uint ldc,
    const uint startN,
    const uint origN,
    uint offA,
    uint offB,
    uint offC)
{
    float8 a0, a1, a2, a3;
    float8 b0, b1, b2, b3;
    float8 c0, c1, c2, c3, c4, c5, c6, c7;
    uint argN = N;
    __global float8 *B;
    uint4 coord = 0;
    uint k0 = 0;

    const int lid = get_local_id(0);
    uint block = get_group_id(0);

    k0 = (N + 63) / 64;
    if (block < k0 * (startN / 64)) {
        coord.x = (block / k0) * 64;
        block %= k0;
    }
    else {
        block -= k0 * (startN / 64);
        while (block >= k0) {
            block -= k0;
            coord.x += 64;
            k0 = ((N + 7) / 8 * 8 - coord.x + 63) / 64;
        }
        coord.x += startN;
    }
    coord.y = (N + 7) / 8 * 8 - (block + 1) * 64;
    coord.y = (coord.y + 7) / 8 * 8;
    coord.y += startN + lid % 8 * 8;
    coord.x += lid / 8 * 8;
    if (coord.y >= startN + N || coord.x >= startN + N) {
        return;
    }
    if (coord.x >= coord.y + 8) {
        return;
    }


    c0 = 0;
    c1 = 0;
    c2 = 0;
    c3 = 0;
    c4 = 0;
    c5 = 0;
    c6 = 0;
    c7 = 0;

    // Set N to initial argument of blas function, not divided one
    N = origN;
    lda /= 8;

    B = A + (coord.x >> 3) + (offA >> 3);
    A = A + (coord.y >> 3) + (offA >> 3);
    C = C + offC;
    for (k0 = 0; k0 < K; k0 += 4) {
        /* -- Tiles multiplier -- */
        b0 = B[0];
        b1 = B[lda];
        b2 = B[(lda << 1)];
        b3 = B[mad24(3u, lda, 0u)];

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
        B += (lda << 2);
        /* ---------------------- */
    }


    if ( !( (coord.y >= startN + argN) || (coord.x >= startN + argN) || (coord.x >= coord.y + 8) ) ) {
        if (coord.x + 8 > coord.y) {
            __global float *dst = C + coord.x * ldc + coord.y;

            float8 tempC0, tempC1, tempC2, tempC3, tempC4, tempC5, tempC6, tempC7;




            tempC0 = *(__global float8*)(&dst[0]);
            tempC1.s1 = *(__global float*)(&dst[ldc + 1]);
            tempC1.s23 = *(__global float2*)(&dst[ldc + 2]);
            tempC1.s4567 = *(__global float4*)(&dst[ldc + 4]);
            tempC2.s23 = *(__global float2*)(&dst[mad24(2u, ldc, 2u)]);
            tempC2.s4567 = *(__global float4*)(&dst[mad24(2u, ldc, 4u)]);
            tempC3.s3 = *(__global float*)(&dst[mad24(3u, ldc, 3u)]);
            tempC3.s4567 = *(__global float4*)(&dst[mad24(3u, ldc, 4u)]);
            tempC4.s4567 = *(__global float4*)(&dst[mad24(4u, ldc, 4u)]);
            tempC5.s5 = *(__global float*)(&dst[mad24(5u, ldc, 5u)]);
            tempC5.s67 = *(__global float2*)(&dst[mad24(5u, ldc, 6u)]);
            tempC6.s67 = *(__global float2*)(&dst[mad24(6u, ldc, 6u)]);
            tempC7.s7 = *(__global float*)(&dst[mad24(7u, ldc, 7u)]);

            tempC0 = mad(tempC0, beta, 0);
            tempC0 = mad(c0, alpha, tempC0);
            tempC1.s1 = mad(tempC1.s1, beta, 0);
            tempC1.s1 = mad(c1.s1, alpha, tempC1.s1);
            tempC1.s23 = mad(tempC1.s23, beta, 0);
            tempC1.s23 = mad(c1.s23, alpha, tempC1.s23);
            tempC1.s4567 = mad(tempC1.s4567, beta, 0);
            tempC1.s4567 = mad(c1.s4567, alpha, tempC1.s4567);
            tempC2.s23 = mad(tempC2.s23, beta, 0);
            tempC2.s23 = mad(c2.s23, alpha, tempC2.s23);
            tempC2.s4567 = mad(tempC2.s4567, beta, 0);
            tempC2.s4567 = mad(c2.s4567, alpha, tempC2.s4567);
            tempC3.s3 = mad(tempC3.s3, beta, 0);
            tempC3.s3 = mad(c3.s3, alpha, tempC3.s3);
            tempC3.s4567 = mad(tempC3.s4567, beta, 0);
            tempC3.s4567 = mad(c3.s4567, alpha, tempC3.s4567);
            tempC4.s4567 = mad(tempC4.s4567, beta, 0);
            tempC4.s4567 = mad(c4.s4567, alpha, tempC4.s4567);
            tempC5.s5 = mad(tempC5.s5, beta, 0);
            tempC5.s5 = mad(c5.s5, alpha, tempC5.s5);
            tempC5.s67 = mad(tempC5.s67, beta, 0);
            tempC5.s67 = mad(c5.s67, alpha, tempC5.s67);
            tempC6.s67 = mad(tempC6.s67, beta, 0);
            tempC6.s67 = mad(c6.s67, alpha, tempC6.s67);
            tempC7.s7 = mad(tempC7.s7, beta, 0);
            tempC7.s7 = mad(c7.s7, alpha, tempC7.s7);

            *(__global float8*)(&dst[0]) = tempC0;
            *(__global float*)(&dst[ldc + 1]) = tempC1.s1;
            *(__global float2*)(&dst[ldc + 2]) = tempC1.s23;
            *(__global float4*)(&dst[ldc + 4]) = tempC1.s4567;
            *(__global float2*)(&dst[mad24(2u, ldc, 2u)]) = tempC2.s23;
            *(__global float4*)(&dst[mad24(2u, ldc, 4u)]) = tempC2.s4567;
            *(__global float*)(&dst[mad24(3u, ldc, 3u)]) = tempC3.s3;
            *(__global float4*)(&dst[mad24(3u, ldc, 4u)]) = tempC3.s4567;
            *(__global float4*)(&dst[mad24(4u, ldc, 4u)]) = tempC4.s4567;
            *(__global float*)(&dst[mad24(5u, ldc, 5u)]) = tempC5.s5;
            *(__global float2*)(&dst[mad24(5u, ldc, 6u)]) = tempC5.s67;
            *(__global float2*)(&dst[mad24(6u, ldc, 6u)]) = tempC6.s67;
            *(__global float*)(&dst[mad24(7u, ldc, 7u)]) = tempC7.s7;

        }
        else {

            GPtr uC;

            uC.f = C + coord.x * ldc + coord.y;

            __global float8 *pC = uC.f;

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
    }
}


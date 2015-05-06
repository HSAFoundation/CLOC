#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#pragma OPENCL EXTENSION cl_khr_select_fprounding_mode : enable

#define __ROUNDING_MODE__ rte

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

void
fcopyDBlockGL832(
    LPtr dst,
    GPtr src,
    uint startRow,
    uint startCol,
    uint ld)
{
    const int lid = get_local_id(0);

    src.f += (startRow + lid / 8) * ld + startCol + lid % 8 * 4;

    dst.f += (lid / 8) * 32 + (lid % 8 * 4) * 1;

    *dst.f4v++ = *src.f4v++;
}

__attribute__((reqd_work_group_size(64, 1, 1)))
__kernel void strsmCached(
    uint N,
    uint M,
    float alpha,
    const __global float *restrict A,
    uint lda,
    __global float *B,
    uint ldb,
    uint offA,
    uint offB)
{
    const int lid = get_local_id(0);
    const int gid = get_group_id(0);
    GPtr uA, uB;
    uint coordA, coordB;
    uint m0 = 0, k0, m1;
    float4 a0, a1, a2, a3;
    float4 b0, b1, b2, b3;
    float4 c0, c1, c2, c3;
    __local float4 tmpB[64];
    LPtr lB;
    LPtr lBMain = {(__local float*)(tmpB + lid % 8 * 1)};

    A += offA;
    B += offB;

    uB.f = B;

    for (m0 = 0; m0 < M; m0 += 32) {
        c0 = 0;
        c1 = 0;
        c2 = 0;
        c3 = 0;

        coordA = m0 + (lid / 8 * 4);
        k0 = 0;
        coordB = gid * 32 + (lid % 8 * 4);

        // Stage 1. Multiply and update with large blocks
        uA.f = A + k0 * lda;
        for (k0 = 0; k0 < m0; k0 += 8) {
            lB.f4v = tmpB;
            barrier(CLK_LOCAL_MEM_FENCE);
            fcopyDBlockGL832(lB, uB, k0, gid * 32, ldb);
            barrier(CLK_LOCAL_MEM_FENCE);

            lB = lBMain;

            /* -- Tiles multiplier -- */
            for (int k1 = 0; k1 < 8; k1 += 4) {
                b0 = lB.f4v[0];
                b1 = lB.f4v[8];
                b2 = lB.f4v[16];
                b3 = lB.f4v[24];

                a0 = uA.f4v[(coordA >> 2)];
                a1 = uA.f4v[(lda >> 2) + (coordA >> 2)];
                a2 = uA.f4v[(lda >> 1) + (coordA >> 2)];
                a3 = uA.f4v[mad24(3u, (lda >> 2), (coordA >> 2))];

                c0 = mad(b0, a0.s0, c0);
                c1 = mad(b0, a0.s1, c1);
                c2 = mad(b0, a0.s2, c2);
                c3 = mad(b0, a0.s3, c3);

                c0 = mad(b1, a1.s0, c0);
                c1 = mad(b1, a1.s1, c1);
                c2 = mad(b1, a1.s2, c2);
                c3 = mad(b1, a1.s3, c3);

                c0 = mad(b2, a2.s0, c0);
                c1 = mad(b2, a2.s1, c1);
                c2 = mad(b2, a2.s2, c2);
                c3 = mad(b2, a2.s3, c3);

                c0 = mad(b3, a3.s0, c0);
                c1 = mad(b3, a3.s1, c1);
                c2 = mad(b3, a3.s2, c2);
                c3 = mad(b3, a3.s3, c3);

                uA.f4v += lda;
                lB.f4v += 32;
            }
            /* ---------------------- */
        }
        uA.f = A;


        /*
         * Stage 2. A part of work items multiply got result on a respective
         * inverted diagonal block, and the remaining ones wait. Then they perform
         * one step of further intermediate result evaluation as multiplying tile by tile.
         * It continues until the whole panel of the matrix A is processed
         */
        for (m1 = 0; m1 < 8; m1++) {
            coordA = m0 + (lid / 8 * 4);
            k0 = m0 + m1 * 4;
            coordB = gid * 32 + (lid % 8 * 4);

            if (lid / 8 == m1) {
                 {
                    float beta = -1. / alpha;
                    float alpha = beta;
                    GPtr uC;

                    uC.f = B + coordA * ldb + coordB;

                    __global float4 *pC = uC.f;

                    float4 tempC0, tempC1, tempC2, tempC3;

                    tempC0 = pC[0];
                    tempC1 = pC[(ldb >> 2)];
                    tempC2 = pC[(ldb >> 1)];
                    tempC3 = pC[mad24(3u, (ldb >> 2), 0u)];
                    c0 = mad(c0, alpha, tempC0);
                    c1 = mad(c1, alpha, tempC1);
                    c2 = mad(c2, alpha, tempC2);
                    c3 = mad(c3, alpha, tempC3);
                }

                // Fetch and invert the square tile located on the diagonal
                b0 = uA.f4v[mad24(k0, (lda >> 2), (coordA >> 2))];
                b1 = uA.f4v[mad24(k0 + 1, (lda >> 2), (coordA >> 2))];
                b2 = uA.f4v[mad24(k0 + 2, (lda >> 2), (coordA >> 2))];
                b3 = uA.f4v[mad24(k0 + 3, (lda >> 2), (coordA >> 2))];

                // post fetch A
                {
                    uint zy = k0;
                    b0.s0 = zy > coordA ? 0 : b0.s0;
                    b0.s1 = zy > coordA + 1 ? 0 : b0.s1;
                    b0.s2 = zy > coordA + 2 ? 0 : b0.s2;
                    b0.s3 = zy > coordA + 3 ? 0 : b0.s3;
                    zy++;
                    b1.s0 = zy > coordA ? 0 : b1.s0;
                    b1.s1 = zy > coordA + 1 ? 0 : b1.s1;
                    b1.s2 = zy > coordA + 2 ? 0 : b1.s2;
                    b1.s3 = zy > coordA + 3 ? 0 : b1.s3;
                    zy++;
                    b2.s0 = zy > coordA ? 0 : b2.s0;
                    b2.s1 = zy > coordA + 1 ? 0 : b2.s1;
                    b2.s2 = zy > coordA + 2 ? 0 : b2.s2;
                    b2.s3 = zy > coordA + 3 ? 0 : b2.s3;
                    zy++;
                    b3.s0 = zy > coordA ? 0 : b3.s0;
                    b3.s1 = zy > coordA + 1 ? 0 : b3.s1;
                    b3.s2 = zy > coordA + 2 ? 0 : b3.s2;
                    b3.s3 = zy > coordA + 3 ? 0 : b3.s3;
                }
                // Invert tile
                a0 = 0;
                a1 = 0;
                a2 = 0;
                a3 = 0;

                a0.s0 = 1;
                a1.s1 = 1;
                a2.s2 = 1;
                a3.s3 = 1;

                a0.s0 /= b0.s0;
                a1.s0 /= b0.s0;
                a2.s0 /= b0.s0;
                a3.s0 /= b0.s0;

                a0.s1 -= a0.s0 * b0.s1;
                a0.s1 /= b1.s1;
                a1.s1 -= a1.s0 * b0.s1;
                a1.s1 /= b1.s1;
                a2.s1 -= a2.s0 * b0.s1;
                a2.s1 /= b1.s1;
                a3.s1 -= a3.s0 * b0.s1;
                a3.s1 /= b1.s1;
                a0.s2 -= a0.s0 * b0.s2;
                a1.s2 -= a1.s0 * b0.s2;
                a2.s2 -= a2.s0 * b0.s2;
                a3.s2 -= a3.s0 * b0.s2;
                a0.s3 -= a0.s0 * b0.s3;
                a1.s3 -= a1.s0 * b0.s3;
                a2.s3 -= a2.s0 * b0.s3;
                a3.s3 -= a3.s0 * b0.s3;

                a0.s2 -= a0.s1 * b1.s2;
                a0.s2 /= b2.s2;
                a1.s2 -= a1.s1 * b1.s2;
                a1.s2 /= b2.s2;
                a2.s2 -= a2.s1 * b1.s2;
                a2.s2 /= b2.s2;
                a3.s2 -= a3.s1 * b1.s2;
                a3.s2 /= b2.s2;
                a0.s3 -= a0.s1 * b1.s3;
                a1.s3 -= a1.s1 * b1.s3;
                a2.s3 -= a2.s1 * b1.s3;
                a3.s3 -= a3.s1 * b1.s3;

                a0.s3 -= a0.s2 * b2.s3;
                a0.s3 /= b3.s3;
                a1.s3 -= a1.s2 * b2.s3;
                a1.s3 /= b3.s3;
                a2.s3 -= a2.s2 * b2.s3;
                a2.s3 /= b3.s3;
                a3.s3 -= a3.s2 * b2.s3;
                a3.s3 /= b3.s3;

                b0 = c0;
                b1 = c1;
                b2 = c2;
                b3 = c3;

                c0 = 0;
                c1 = 0;
                c2 = 0;
                c3 = 0;

                c0 = mad(b0, a0.s0, c0);
                c1 = mad(b0, a0.s1, c1);
                c2 = mad(b0, a0.s2, c2);
                c3 = mad(b0, a0.s3, c3);
                c0 = mad(b1, a1.s0, c0);
                c1 = mad(b1, a1.s1, c1);
                c2 = mad(b1, a1.s2, c2);
                c3 = mad(b1, a1.s3, c3);
                c0 = mad(b2, a2.s0, c0);
                c1 = mad(b2, a2.s1, c1);
                c2 = mad(b2, a2.s2, c2);
                c3 = mad(b2, a2.s3, c3);
                c0 = mad(b3, a3.s0, c0);
                c1 = mad(b3, a3.s1, c1);
                c2 = mad(b3, a3.s2, c2);
                c3 = mad(b3, a3.s3, c3);

                // Write back the given result

                GPtr uC;

                uC.f = B + coordA * ldb + coordB;

                __global float4 *pC = uC.f;

                float4 tempC0, tempC1, tempC2, tempC3;

                tempC0 = mad(c0, alpha, 0);
                tempC1 = mad(c1, alpha, 0);
                tempC2 = mad(c2, alpha, 0);
                tempC3 = mad(c3, alpha, 0);
                pC[0] = tempC0;
                pC[(ldb >> 2)] = tempC1;
                pC[(ldb >> 1)] = tempC2;
                pC[mad24(3u, (ldb >> 2), 0u)] = tempC3;
            }
            barrier(CLK_GLOBAL_MEM_FENCE);

            if (lid / 8 > m1) {
                /* -- Tiles multiplier -- */
                b0 = uB.f4v[mad24(k0, (ldb >> 2), (coordB >> 2) % (ldb >> 2))];
                b1 = uB.f4v[mad24(k0 + 1, (ldb >> 2), (coordB >> 2) % (ldb >> 2))];
                b2 = uB.f4v[mad24(k0 + 2, (ldb >> 2), (coordB >> 2) % (ldb >> 2))];
                b3 = uB.f4v[mad24(k0 + 3, (ldb >> 2), (coordB >> 2) % (ldb >> 2))];

                a0 = uA.f4v[mad24(k0, (lda >> 2), (coordA >> 2))];
                a1 = uA.f4v[mad24(k0 + 1, (lda >> 2), (coordA >> 2))];
                a2 = uA.f4v[mad24(k0 + 2, (lda >> 2), (coordA >> 2))];
                a3 = uA.f4v[mad24(k0 + 3, (lda >> 2), (coordA >> 2))];

                c0 = mad(b0, a0.s0, c0);
                c1 = mad(b0, a0.s1, c1);
                c2 = mad(b0, a0.s2, c2);
                c3 = mad(b0, a0.s3, c3);

                c0 = mad(b1, a1.s0, c0);
                c1 = mad(b1, a1.s1, c1);
                c2 = mad(b1, a1.s2, c2);
                c3 = mad(b1, a1.s3, c3);

                c0 = mad(b2, a2.s0, c0);
                c1 = mad(b2, a2.s1, c1);
                c2 = mad(b2, a2.s2, c2);
                c3 = mad(b2, a2.s3, c3);

                c0 = mad(b3, a3.s0, c0);
                c1 = mad(b3, a3.s1, c1);
                c2 = mad(b3, a3.s2, c2);
                c3 = mad(b3, a3.s3, c3);
                /* ---------------------- */
            }
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
    }
}

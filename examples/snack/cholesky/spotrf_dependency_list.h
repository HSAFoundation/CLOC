typedef struct dep_container {
    int numDeps;
    hsa_signal_t sig[3];
} depContainer;

#define NULL1 (hsa_signal_t)NULL
#define NULL2 (hsa_signal_t)NULL, (hsa_signal_t)NULL
#define NULL3 (hsa_signal_t)NULL, (hsa_signal_t)NULL, (hsa_signal_t)NULL

#define PS(x) potrfSignal[x]
#define SS(x) syrkSignal[x]
#define GS(x) gemmSignal[x]
#define TS(x) trsmSignal[x]

#define P0_C &tmpDepContainer, 0, NULL3
#define P1_C &tmpDepContainer, 1, SS(0), NULL2
#define P3_C &tmpDepContainer, 1, SS(5), NULL2
#define P2_C &tmpDepContainer, 1, SS(2), NULL2
#define P4_C &tmpDepContainer, 1, SS(9), NULL2

#define T0_C &tmpDepContainer, 1, PS(0), NULL2
#define T1_C &tmpDepContainer, 1, PS(0), NULL2
#define T2_C &tmpDepContainer, 1, PS(0), NULL2
#define T3_C &tmpDepContainer, 1, PS(0), NULL2
#define T4_C &tmpDepContainer, 2, PS(1), GS(0), NULL1
#define T5_C &tmpDepContainer, 2, GS(1), PS(1), NULL1
#define T6_C &tmpDepContainer, 2, GS(2), PS(1), NULL1
#define T7_C &tmpDepContainer, 2, GS(4), PS(2), NULL1
#define T8_C &tmpDepContainer, 2, GS(6), PS(2), NULL1
#define T9_C &tmpDepContainer, 2, GS(9), PS(3), NULL1

#define S0_C &tmpDepContainer, 1, TS(0), NULL2
#define S1_C &tmpDepContainer, 1, TS(1), NULL2
#define S2_C &tmpDepContainer, 2, SS(1), TS(4), NULL1
#define S3_C &tmpDepContainer, 1, TS(2), NULL2
#define S4_C &tmpDepContainer, 2, SS(3), TS(5), NULL1
#define S5_C &tmpDepContainer, 2, SS(4), TS(7), NULL1
#define S6_C &tmpDepContainer, 1, TS(3), NULL2
#define S7_C &tmpDepContainer, 2, SS(6), TS(6), NULL1
#define S8_C &tmpDepContainer, 2, SS(7), TS(8), NULL1
#define S9_C &tmpDepContainer, 2, SS(8), TS(9), NULL1


#define G0_C &tmpDepContainer, 2, TS(1), TS(0), NULL1
#define G1_C &tmpDepContainer, 2, TS(2), TS(0), NULL1
#define G2_C &tmpDepContainer, 2, TS(3), TS(0), NULL1
#define G3_C &tmpDepContainer, 2, TS(2), TS(1), NULL1
#define G4_C &tmpDepContainer, 3, GS(3), TS(5), TS(4)
#define G5_C &tmpDepContainer, 2, TS(3), TS(1), NULL1
#define G6_C &tmpDepContainer, 3, GS(5), TS(6), TS(4)
#define G7_C &tmpDepContainer, 2, TS(2), TS(3), NULL1
#define G8_C &tmpDepContainer, 3, GS(7), TS(6), TS(5)
#define G9_C &tmpDepContainer, 3, GS(8), TS(7), TS(8)

#define DO_SYRK(x,y) doSyrk(x, y, dA, syrkSignal[x], tmpDepContainer.numDeps, tmpDepContainer.sig, n, nb)
#define DO_GEMM(x,y) doGemm(x, y, dA, gemmSignal[x], tmpDepContainer.numDeps, tmpDepContainer.sig, n, nb)
#define DO_TRSM(x,y) doTrsm(x, y, dA, trsmSignal[x], tmpDepContainer.numDeps, tmpDepContainer.sig, n, nb)
#define DO_POTRF(x,y) doPotrf(x, y, dA, dA_offset, ldda, potrfSignal[x], tmpDepContainer.numDeps, tmpDepContainer.sig, info, n)

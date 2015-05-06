#include <assert.h>
#include <stdio.h>

#include "magma_types_magmaWrapper.h"

// ----------------------------------------
// Convert LAPACK character constants to MAGMA constants.
// This is a one-to-many mapping, requiring multiple translators
// (e.g., "N" can be NoTrans or NonUnit or NoVec).
// These functions and cases are in the same order as the constants are
// declared in magma_types.h


magma_bool_t   magma_bool_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'N': case 'n': return MagmaFalse;
        case 'Y': case 'y': return MagmaTrue;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaFalse;
    }
}


magma_order_t  magma_order_const ( char lapack_char )
{
    switch( lapack_char ) {
        case 'R': case 'r': return MagmaRowMajor;
        case 'C': case 'c': return MagmaColMajor;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaRowMajor;
    }
}


magma_trans_t  magma_trans_const ( char lapack_char )
{
    switch( lapack_char ) {
        case 'N': case 'n': return MagmaNoTrans;
        case 'T': case 't': return MagmaTrans;
        case 'C': case 'c': return MagmaConjTrans;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaNoTrans;
    }
}


magma_uplo_t   magma_uplo_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'U': case 'u': return MagmaUpper;
        case 'L': case 'l': return MagmaLower;
        default:            return MagmaFull;        // see laset
    }
}


magma_diag_t   magma_diag_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'N': case 'n': return MagmaNonUnit;
        case 'U': case 'u': return MagmaUnit;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaNonUnit;
    }
}


magma_side_t   magma_side_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'L': case 'l': return MagmaLeft;
        case 'R': case 'r': return MagmaRight;
        case 'B': case 'b': return MagmaBothSides;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaLeft;
    }
}


magma_norm_t   magma_norm_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'O': case 'o': case '1': return MagmaOneNorm;
        case '2':           return MagmaTwoNorm;
        case 'F': case 'f': case 'E': case 'e': return MagmaFrobeniusNorm;
        case 'I': case 'i': return MagmaInfNorm;
        case 'M': case 'm': return MagmaMaxNorm;
        // MagmaRealOneNorm
        // MagmaRealInfNorm
        // MagmaRealMaxNorm
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaOneNorm;
    }
}


magma_dist_t   magma_dist_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'U': case 'u': return MagmaDistUniform;
        case 'S': case 's': return MagmaDistSymmetric;
        case 'N': case 'n': return MagmaDistNormal;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaDistUniform;
    }
}


magma_sym_t    magma_sym_const   ( char lapack_char )
{
    switch( lapack_char ) {
        case 'H': case 'h': return MagmaHermGeev;
        case 'P': case 'p': return MagmaHermPoev;
        case 'N': case 'n': return MagmaNonsymPosv;
        case 'S': case 's': return MagmaSymPosv;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaHermGeev;
    }
}


magma_pack_t   magma_pack_const  ( char lapack_char )
{
    switch( lapack_char ) {
        case 'N': case 'n': return MagmaNoPacking;
        case 'U': case 'u': return MagmaPackSubdiag;
        case 'L': case 'l': return MagmaPackSupdiag;
        case 'C': case 'c': return MagmaPackColumn;
        case 'R': case 'r': return MagmaPackRow;
        case 'B': case 'b': return MagmaPackLowerBand;
        case 'Q': case 'q': return MagmaPackUpeprBand;
        case 'Z': case 'z': return MagmaPackAll;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaNoPacking;
    }
}


magma_vec_t    magma_vec_const   ( char lapack_char )
{
    switch( lapack_char ) {
        case 'N': case 'n': return MagmaNoVec;
        case 'V': case 'v': return MagmaVec;
        case 'I': case 'i': return MagmaIVec;
        case 'A': case 'a': return MagmaAllVec;
        case 'S': case 's': return MagmaSomeVec;
        case 'O': case 'o': return MagmaOverwriteVec;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaNoVec;
    }
}


magma_range_t  magma_range_const ( char lapack_char )
{
    switch( lapack_char ) {
        case 'A': case 'a': return MagmaRangeAll;
        case 'V': case 'v': return MagmaRangeV;
        case 'I': case 'i': return MagmaRangeI;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaRangeAll;
    }
}


magma_vect_t magma_vect_const( char lapack_char )
{
    switch( lapack_char ) {
        case 'Q': case 'q': return MagmaQ;
        case 'P': case 'p': return MagmaP;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaQ;
    }
}


magma_direct_t magma_direct_const( char lapack_char )
{
    switch( lapack_char ) {
        case 'F': case 'f': return MagmaForward;
        case 'B': case 'b': return MagmaBackward;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaForward;
    }
}


magma_storev_t magma_storev_const( char lapack_char )
{
    switch( lapack_char ) {
        case 'C': case 'c': return MagmaColumnwise;
        case 'R': case 'r': return MagmaRowwise;
        default:
            fprintf( stderr, "Error in %s: unexpected value %c\n", __func__, lapack_char );
            return MagmaColumnwise;
    }
}


// ----------------------------------------
// Convert MAGMA constants to LAPACK constants.

const char *magma2lapack_constants[] =
{
    "No",                                    //  0: MagmaFalse
    "Yes",                                   //  1: MagmaTrue (zlatrs)
    "", "", "", "", "", "", "", "", "",      //  2-10
    "", "", "", "", "", "", "", "", "", "",  // 11-20
    "", "", "", "", "", "", "", "", "", "",  // 21-30
    "", "", "", "", "", "", "", "", "", "",  // 31-40
    "", "", "", "", "", "", "", "", "", "",  // 41-50
    "", "", "", "", "", "", "", "", "", "",  // 51-60
    "", "", "", "", "", "", "", "", "", "",  // 61-70
    "", "", "", "", "", "", "", "", "", "",  // 71-80
    "", "", "", "", "", "", "", "", "", "",  // 81-90
    "", "", "", "", "", "", "", "", "", "",  // 91-100
    "Row",                                   // 101: MagmaRowMajor
    "Column",                                // 102: MagmaColMajor
    "", "", "", "", "", "", "", "",          // 103-110
    "No transpose",                          // 111: MagmaNoTrans
    "Transpose",                             // 112: MagmaTrans
    "Conjugate transpose",                   // 113: MagmaConjTrans
    "", "", "", "", "", "", "",              // 114-120
    "Upper",                                 // 121: MagmaUpper
    "Lower",                                 // 122: MagmaLower
    "GFull",                                 // 123: MagmaFull; see lascl for "G"
    "", "", "", "", "", "", "",              // 124-130
    "Non-unit",                              // 131: MagmaNonUnit
    "Unit",                                  // 132: MagmaUnit
    "", "", "", "", "", "", "", "",          // 133-140
    "Left",                                  // 141: MagmaLeft
    "Right",                                 // 142: MagmaRight
    "Both",                                  // 143: MagmaBothSides (dtrevc)
    "", "", "", "", "", "", "",              // 144-150
    "", "", "", "", "", "", "", "", "", "",  // 151-160
    "", "", "", "", "", "", "", "", "", "",  // 161-170
    "1 norm",                                // 171: MagmaOneNorm
    "",                                      // 172: MagmaRealOneNorm
    "2 norm",                                // 173: MagmaTwoNorm
    "Frobenius norm",                        // 174: MagmaFrobeniusNorm
    "Infinity norm",                         // 175: MagmaInfNorm
    "",                                      // 176: MagmaRealInfNorm
    "Maximum norm",                          // 177: MagmaMaxNorm
    "",                                      // 178: MagmaRealMaxNorm
    "", "",                                  // 179-180
    "", "", "", "", "", "", "", "", "", "",  // 181-190
    "", "", "", "", "", "", "", "", "", "",  // 191-200
    "Uniform",                               // 201: MagmaDistUniform
    "Symmetric",                             // 202: MagmaDistSymmetric
    "Normal",                                // 203: MagmaDistNormal
    "", "", "", "", "", "", "",              // 204-210
    "", "", "", "", "", "", "", "", "", "",  // 211-220
    "", "", "", "", "", "", "", "", "", "",  // 221-230
    "", "", "", "", "", "", "", "", "", "",  // 231-240
    "Hermitian",                             // 241 MagmaHermGeev
    "Positive ev Hermitian",                 // 242 MagmaHermPoev
    "NonSymmetric pos sv",                   // 243 MagmaNonsymPosv
    "Symmetric pos sv",                      // 244 MagmaSymPosv
    "", "", "", "", "", "",                  // 245-250
    "", "", "", "", "", "", "", "", "", "",  // 251-260
    "", "", "", "", "", "", "", "", "", "",  // 261-270
    "", "", "", "", "", "", "", "", "", "",  // 271-280
    "", "", "", "", "", "", "", "", "", "",  // 281-290
    "No Packing",                            // 291 MagmaNoPacking
    "U zero out subdiag",                    // 292 MagmaPackSubdiag
    "L zero out superdiag",                  // 293 MagmaPackSupdiag
    "C",                                     // 294 MagmaPackColumn
    "R",                                     // 295 MagmaPackRow
    "B",                                     // 296 MagmaPackLowerBand
    "Q",                                     // 297 MagmaPackUpeprBand
    "Z",                                     // 298 MagmaPackAll
    "", "",                                  // 299-300
    "No vectors",                            // 301 MagmaNoVec
    "Vectors needed",                        // 302 MagmaVec
    "I",                                     // 303 MagmaIVec
    "All",                                   // 304 MagmaAllVec
    "Some",                                  // 305 MagmaSomeVec
    "Overwrite",                             // 306 MagmaOverwriteVec
    "", "", "", "",                          // 307-310
    "All",                                   // 311 MagmaRangeAll
    "V",                                     // 312 MagmaRangeV
    "I",                                     // 313 MagmaRangeI
    "", "", "", "", "", "", "",              // 314-320
    "",                                      // 321
    "Q",                                     // 322
    "P",                                     // 323
    "", "", "", "", "", "", "",              // 324-330
    "", "", "", "", "", "", "", "", "", "",  // 331-340
    "", "", "", "", "", "", "", "", "", "",  // 341-350
    "", "", "", "", "", "", "", "", "", "",  // 351-360
    "", "", "", "", "", "", "", "", "", "",  // 361-370
    "", "", "", "", "", "", "", "", "", "",  // 371-380
    "", "", "", "", "", "", "", "", "", "",  // 381-390
    "Forward",                               // 391: MagmaForward
    "Backward",                              // 392: MagmaBackward
    "", "", "", "", "", "", "", "",          // 393-400
    "Columnwise",                            // 401: MagmaColumnwise
    "Rowwise",                               // 402: MagmaRowwise
    "", "", "", "", "", "", "", ""           // 403-410
    // Remember to add a comma!
};


const char* lapack_const( int magma_const )
{
    assert( magma_const >= Magma2lapack_Min );
    assert( magma_const <= Magma2lapack_Max );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_bool_const( magma_bool_t magma_const )
{
    assert( magma_const >= MagmaFalse );
    assert( magma_const <= MagmaTrue  );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_order_const( magma_order_t magma_const )
{
    assert( magma_const >= MagmaRowMajor );
    assert( magma_const <= MagmaColMajor );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_trans_const( magma_trans_t magma_const )
{
    assert( magma_const >= MagmaNoTrans   );
    assert( magma_const <= MagmaConjTrans );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_uplo_const ( magma_uplo_t magma_const )
{
    assert( magma_const >= MagmaUpper );
    assert( magma_const <= MagmaFull  );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_diag_const ( magma_diag_t magma_const )
{
    assert( magma_const >= MagmaNonUnit );
    assert( magma_const <= MagmaUnit    );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_side_const ( magma_side_t magma_const )
{
    assert( magma_const >= MagmaLeft  );
    assert( magma_const <= MagmaBothSides );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_norm_const  ( magma_norm_t   magma_const )
{
    assert( magma_const >= MagmaOneNorm     );
    assert( magma_const <= MagmaRealMaxNorm );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_dist_const  ( magma_dist_t   magma_const )
{
    assert( magma_const >= MagmaDistUniform );
    assert( magma_const <= MagmaDistNormal );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_sym_const   ( magma_sym_t    magma_const )
{
    assert( magma_const >= MagmaHermGeev );
    assert( magma_const <= MagmaSymPosv  );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_pack_const  ( magma_pack_t   magma_const )
{
    assert( magma_const >= MagmaNoPacking );
    assert( magma_const <= MagmaPackAll   );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_vec_const   ( magma_vec_t    magma_const )
{
    assert( magma_const >= MagmaNoVec );
    assert( magma_const <= MagmaOverwriteVec );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_range_const ( magma_range_t  magma_const )
{
    assert( magma_const >= MagmaRangeAll );
    assert( magma_const <= MagmaRangeI   );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_vect_const( magma_vect_t magma_const )
{
    assert( magma_const >= MagmaQ );
    assert( magma_const <= MagmaP );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_direct_const( magma_direct_t magma_const )
{
    assert( magma_const >= MagmaForward );
    assert( magma_const <= MagmaBackward );
    return magma2lapack_constants[ magma_const ];
}


const char* lapack_storev_const( magma_storev_t magma_const )
{
    assert( magma_const >= MagmaColumnwise );
    assert( magma_const <= MagmaRowwise    );
    return magma2lapack_constants[ magma_const ];
}


// ----------------------------------------
// Convert magma constants to clBLAS constants.

#ifdef HAVE_clBLAS
const int magma2amdblas_constants[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,                      // 100
    clblasRowMajor,         // 101: MagmaRowMajor
    clblasColumnMajor,      // 102: MagmaColMajor
    0, 0, 0, 0, 0, 0, 0, 0,
    clblasNoTrans,          // 111: MagmaNoTrans
    clblasTrans,            // 112: MagmaTrans
    clblasConjTrans,        // 113: MagmaConjTrans
    0, 0, 0, 0, 0, 0, 0,
    clblasUpper,            // 121: MagmaUpper
    clblasLower,            // 122: MagmaLower
    0, 0, 0, 0, 0, 0, 0, 0,
    clblasNonUnit,          // 131: MagmaNonUnit
    clblasUnit,             // 132: MagmaUnit
    0, 0, 0, 0, 0, 0, 0, 0,
    clblasLeft,             // 141: MagmaLeft
    clblasRight,            // 142: MagmaRight
    0, 0, 0, 0, 0, 0, 0, 0
};


clblasOrder       clblas_order_const( magma_order_t magma_const )
{
    assert( magma_const >= MagmaRowMajor );
    assert( magma_const <= MagmaColMajor );
    return (clblasOrder)     magma2amdblas_constants[ magma_const ];
}


clblasTranspose   clblas_trans_const( magma_trans_t magma_const )
{
    assert( magma_const >= MagmaNoTrans   );
    assert( magma_const <= MagmaConjTrans );
    return (clblasTranspose) magma2amdblas_constants[ magma_const ];
}


clblasUplo        clblas_uplo_const ( magma_uplo_t magma_const )
{
    assert( magma_const >= MagmaUpper );
    assert( magma_const <= MagmaLower );
    return (clblasUplo)      magma2amdblas_constants[ magma_const ];
}


clblasDiag        clblas_diag_const ( magma_diag_t magma_const )
{
    assert( magma_const >= MagmaNonUnit );
    assert( magma_const <= MagmaUnit    );
    return (clblasDiag)      magma2amdblas_constants[ magma_const ];
}


clblasSide        clblas_side_const ( magma_side_t magma_const )
{
    assert( magma_const >= MagmaLeft  );
    assert( magma_const <= MagmaRight );
    return (clblasSide)      magma2amdblas_constants[ magma_const ];
}
#endif  // HAVE_clBLAS


// ----------------------------------------
// Convert magma constants to Nvidia CUBLAS constants.

#ifdef HAVE_CUBLAS
const int magma2cublas_constants[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,                      // 100
    0,                      // 101: MagmaRowMajor
    0,                      // 102: MagmaColMajor
    0, 0, 0, 0, 0, 0, 0, 0,
    CUBLAS_OP_N,            // 111: MagmaNoTrans
    CUBLAS_OP_T,            // 112: MagmaTrans
    CUBLAS_OP_C,            // 113: MagmaConjTrans
    0, 0, 0, 0, 0, 0, 0,
    CUBLAS_FILL_MODE_UPPER, // 121: MagmaUpper
    CUBLAS_FILL_MODE_LOWER, // 122: MagmaLower
    0, 0, 0, 0, 0, 0, 0, 0,
    CUBLAS_DIAG_NON_UNIT,   // 131: MagmaNonUnit
    CUBLAS_DIAG_UNIT,       // 132: MagmaUnit
    0, 0, 0, 0, 0, 0, 0, 0,
    CUBLAS_SIDE_LEFT,       // 141: MagmaLeft
    CUBLAS_SIDE_RIGHT,      // 142: MagmaRight
    0, 0, 0, 0, 0, 0, 0, 0
};


cublasOperation_t    cublas_trans_const ( magma_trans_t magma_const )
{
    assert( magma_const >= MagmaNoTrans   );
    assert( magma_const <= MagmaConjTrans );
    return (cublasOperation_t)  magma2cublas_constants[ magma_const ];
}


cublasFillMode_t     cublas_uplo_const  ( magma_uplo_t magma_const )
{
    assert( magma_const >= MagmaUpper );
    assert( magma_const <= MagmaLower );
    return (cublasFillMode_t)   magma2cublas_constants[ magma_const ];
}


cublasDiagType_t     cublas_diag_const  ( magma_diag_t magma_const )
{
    assert( magma_const >= MagmaNonUnit );
    assert( magma_const <= MagmaUnit    );
    return (cublasDiagType_t)   magma2cublas_constants[ magma_const ];
}


cublasSideMode_t     cublas_side_const  ( magma_side_t magma_const )
{
    assert( magma_const >= MagmaLeft  );
    assert( magma_const <= MagmaRight );
    return (cublasSideMode_t)   magma2cublas_constants[ magma_const ];
}
#endif  // HAVE_CUBLAS


// ----------------------------------------
// Convert magma constants to CBLAS constants.
// We assume that magma constants are consistent with cblas constants,
// so verify that with asserts.

#ifdef HAVE_CBLAS

enum CBLAS_ORDER     cblas_order_const  ( magma_order_t magma_const )
{
    assert( magma_const >= MagmaRowMajor );
    assert( magma_const <= MagmaColMajor );
    assert( (int)MagmaRowMajor == CblasRowMajor );
    return (enum CBLAS_ORDER)     magma_const;
}


enum CBLAS_TRANSPOSE cblas_trans_const  ( magma_trans_t magma_const )
{
    assert( magma_const >= MagmaNoTrans   );
    assert( magma_const <= MagmaConjTrans );
    assert( (int)MagmaNoTrans == CblasNoTrans );
    return (enum CBLAS_TRANSPOSE) magma_const;
}


enum CBLAS_UPLO      cblas_uplo_const   ( magma_uplo_t magma_const )
{
    assert( magma_const >= MagmaUpper );
    assert( magma_const <= MagmaLower );
    assert( (int)MagmaUpper == CblasUpper );
    return (enum CBLAS_UPLO)      magma_const;
}


enum CBLAS_DIAG      cblas_diag_const   ( magma_diag_t magma_const )
{
    assert( magma_const >= MagmaNonUnit );
    assert( magma_const <= MagmaUnit    );
    assert( (int)MagmaUnit == CblasUnit );
    return (enum CBLAS_DIAG)      magma_const;
}


enum CBLAS_SIDE      cblas_side_const   ( magma_side_t magma_const )
{
    assert( magma_const >= MagmaLeft  );
    assert( magma_const <= MagmaRight );
    assert( (int)MagmaLeft == CblasLeft );
    return (enum CBLAS_SIDE)      magma_const;
}
#endif  // HAVE_CBLAS

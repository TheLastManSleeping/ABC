#include <iostream>
#include <mmintrin.h>
#include <stdio.h>

//#define DEBUG_MODE
#define BYTE 8
#define WORD 4


//F[i] = A[i] + (B[i] * C[i]) - D[i], i = 1...8;


void SolutionIntrinsic(const __int8* A, const __int8* B, const __int8* C, __int16* D)
{
    __m64 zeroVector = _mm_setzero_si64();


    __m64 check = _m_pcmpgtb(zeroVector, *((__m64*)A)); // comparing with zero

    __m64 AHighHalf = _m_punpckhbw(*((__m64*)A), check); // A in _int8 to A1 in _int16
    __m64 ALowHalf = _m_punpcklbw(*((__m64*)A), check); // A in _int8 to A2 in _int16

    check = _m_pcmpgtb(zeroVector, *((__m64*)B)); // comparing with zero

    __m64 BHighHalf = _m_punpckhbw(*((__m64*)B), check); // B in _int8 to B1 in _int16
    __m64 BLowHalf = _m_punpcklbw(*((__m64*)B), check); // B in _int8 to B2 in _int16

    check = _m_pcmpgtb(zeroVector, *((__m64*)C)); // comparing with zero

    __m64 CHighHalf = _m_punpckhbw(*((__m64*)C), check); // C in _int8 to C1 in _int16
    __m64 CLowHalf = _m_punpcklbw(*((__m64*)C), check); // C in _int8 to C2 in _int16



    __m64 highHalfMul1 = _m_pmulhw(CLowHalf, BLowHalf); // B2 * C2 high half
    __m64 lowHalfMul1 = _m_pmullw(CLowHalf, BLowHalf); // B2 * C2 low half
    __m64 highHalfMul2 = _m_pmulhw(CHighHalf, BHighHalf); // B1 * C1 high half
    __m64 lowHalfMul2 = _m_pmullw(CHighHalf, BHighHalf); // B1 * C1 low half

    auto* temp = new __m64[4];
    temp[0] = _m_punpcklwd(lowHalfMul1, highHalfMul1);
    temp[1] = _m_punpckhwd(lowHalfMul1, highHalfMul1);
    temp[2] = _m_punpcklwd(lowHalfMul2, highHalfMul2);
    temp[3] = _m_punpckhwd(lowHalfMul2, highHalfMul2);


    __m64 ADHighHalf = _m_psubsw(ALowHalf, ((__m64*)D)[0]); // A1 - D1
    __m64 ADLowHalf = _m_psubsw(AHighHalf, ((__m64*)D)[1]); // A2 - D2


    check = _m_pcmpgtb(zeroVector, ADHighHalf); // comparing with zero
    __m64 AD1HighHalf = _m_punpckhwd(ADHighHalf, check); // AD1 in _int16 to ADH1 in _int32
    __m64 AD1LowHalf = _m_punpcklwd(ADHighHalf, check); // AD1 in _int16 to ADL1 in _int32

    check = _m_pcmpgtb(zeroVector, ADLowHalf); // comparing with zero
    __m64 AD2HighHalf = _m_punpckhwd(ADLowHalf, check); // AD2 in _int16 to ADH2 in _int32
    __m64 AD2LowHalf = _m_punpcklwd(ADLowHalf, check); // AD2 in _int16 to ADL2 in _int32



    temp[0] = _m_paddd(temp[0], AD1LowHalf); // BCL1 + ADL1
    temp[1] = _m_paddd(temp[1], AD1HighHalf); // BCH1 + ADH1
    temp[2] = _m_paddd(temp[2], AD2LowHalf); // BCL2 + ADL2
    temp[3] = _m_paddd(temp[3], AD2HighHalf); // BCH2 + ADH2

    for (int i = 0; i < 4; i++) {
        std::cout << (int)temp[i].m64_i32[0] << ' ' << (int)temp[i].m64_i32[1];
        std::cout << '\n';
    }
}



int main()
{
    __int8 A[BYTE * 1] = { 2, 5, -6, -1, 9, -4, 6, 5 };
    __int8 B[BYTE * 1] = { 3, 3, -7, 0, 8, -1, 3, 3 };
    __int8 C[BYTE * 1] = { 0, -2, 5, 4, 6, -3, 4, 4 };
    __int16 D[WORD * 2] = { 3, 6, 3, 7, 1, 0, -5, 2 };

    SolutionIntrinsic(A, B, C, D);

    return 0;
}
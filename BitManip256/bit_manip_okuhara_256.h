/**
 * @file bit_manip_okuhara.h
 * @author 奥原 俊彦 <okuhara＠amy.hi-ho.ne.jp>
 * Copyright ©2020 奥原 俊彦. All rights reserved.
 * @author web site <http: //www.amy.hi-ho.ne.jp/okuhara/bitboard.htm>
 * @date 2020 Original
 * @author Gold Smith
 * @date 2023 2024 modified
 */
#pragma once
#define NOMINMAX
#include <stdint.h>
#include "m256.h"
#include "mm256_custom.h"
#include "../DebugFnc/DebugFnc.h"
#include <atomic>

#if !defined(_M_X64) && !defined(__amd64__)
#error "This code must be compiled in x64 configuration."
#endif
#ifndef __AVX2__
#error "This code requires AVX2."
#endif

namespace bit_manip{
namespace AVX{
namespace Okuhara{
/// <summary>
/// 有効な手を探す。
/// </summary>
/// <param name="poms">m256共用体。</param>
/// <returns>有効な手（複数）。</returns>
uint64_t _vectorcall GetMoves256(const __m256i in_bwms)noexcept;;

inline uint64_t GetMovesPO256(const uint64_t p, const uint64_t o)noexcept{
   __m256i	PP, mOO, MM, flip_l, flip_r, pre_l, pre_r, shift2;
   __m128i	M;
   const __m256i shift1897 = _mm256_set_epi64x(7, 9, 8, 1);
   const __m256i mflipH = _mm256_set_epi64x(0x7e7e7e7e7e7e7e7e, 0x7e7e7e7e7e7e7e7e, -1, 0x7e7e7e7e7e7e7e7e);

   PP = _mm256_set1_epi64x(p);
   mOO = _mm256_and_si256(_mm256_set1_epi64x(o), mflipH);
   // mO = o& 0x7e;
   flip_l = _mm256_and_si256(mOO, _mm256_sllv_epi64(PP, shift1897));
   // flip1 = mO & (p << 1)
   flip_r = _mm256_and_si256(mOO, _mm256_srlv_epi64(PP, shift1897));
   flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(mOO, _mm256_sllv_epi64(flip_l, shift1897)));
   // flip1 |= mO & (flip1 << 1)
   flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(mOO, _mm256_srlv_epi64(flip_r, shift1897)));
   pre_l = _mm256_and_si256(mOO, _mm256_sllv_epi64(mOO, shift1897));
   // pre = mO & (mO << 1)  
   pre_r = _mm256_srlv_epi64(pre_l, shift1897);
   shift2 = _mm256_add_epi64(shift1897, shift1897);
   flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(pre_l, _mm256_sllv_epi64(flip_l, shift2)));
   // flip1 |= pre & (flip1 << 2)
   flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(pre_r, _mm256_srlv_epi64(flip_r, shift2)));
   flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(pre_l, _mm256_sllv_epi64(flip_l, shift2)));
   // flip1 |= pre & (flip1 <<2)
   flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(pre_r, _mm256_srlv_epi64(flip_r, shift2)));
   MM = _mm256_sllv_epi64(flip_l, shift1897);
   // m256i = flip1 << 1;
   MM = _mm256_or_si256(MM, _mm256_srlv_epi64(flip_r, shift1897));

   M = _mm_or_si128(_mm256_castsi256_si128(MM), _mm256_extracti128_si256(MM, 1));
   M = _mm_or_si128(M, _mm_unpackhi_epi64(M, M));
   return _mm_cvtsi128_si64(M) & ~(p | o);	// mask with empties
   // 全方向集約
   // return_value = m256i & ~(p|o)
}

inline uint64_t FlippedPO(const uint64_t p,const uint64_t o,const uint64_t m)noexcept{
   const __m256i shift1897 = _mm256_set_epi64x(7, 9, 8, 1);
   // masked opponent 
   const __m256i mOO = _mm256_and_si256(_mm256_set1_epi64x(o),
      _mm256_set_epi64x(0x007e7e7e7e7e7e00, 0x007e7e7e7e7e7e00, 0x00ffffffffffff00, 0x7e7e7e7e7e7e7e7e));	// (sentinel on the edge)
   const __m256i shift1897x2 = _mm256_add_epi64(shift1897, shift1897);
   const __m256i XX = _mm256_set1_epi64x(m);

   // 左上方向
//	flip = (X << shift) & mO;
   __m256i flipLU256 = _mm256_and_si256(_mm256_sllv_epi64(XX, shift1897), mOO);
   //flip |= (flip << shift) & mO;// 0 0 0 0 0 F&G G 0
   flipLU256 = _mm256_or_si256(flipLU256, _mm256_and_si256(_mm256_sllv_epi64(flipLU256, shift1897), mOO));
   // pre = mO & (mO << shift);// A&B B&C C&D D&E E&F F&G G&H 0
   __m256i preLU256 = _mm256_and_si256(mOO, _mm256_sllv_epi64(mOO, shift1897));
   //flip |= (flip << shift2) & pre;
   flipLU256 = _mm256_or_si256(flipLU256, _mm256_and_si256(_mm256_sllv_epi64(flipLU256, shift1897x2), preLU256));
   //flip |= (flip << shift2) & pre; 0 0 0 D&E&F&G E&F&G F&G G 0
   flipLU256 = _mm256_or_si256(flipLU256, _mm256_and_si256(_mm256_sllv_epi64(flipLU256, shift1897x2), preLU256));
   //outflank = p & (flip << shift);
   __m256i PP = _mm256_set1_epi64x(p);
   __m256i outflankLU256 = _mm256_and_si256(PP, _mm256_sllv_epi64(flipLU256, shift1897));
   //flip &= -(int)(outflank != 0);
   // Create a maskLU where outflank is not zero
   __m256i maskLU = _mm256_cmpeq_epi64(outflankLU256, _mm256_setzero_si256());
   // Invert the maskLU
   maskLU = _mm256_xor_si256(maskLU, _mm256_set1_epi64x(-1));
   // Perform bitwise AND on flip and maskLU
   flipLU256 = _mm256_and_si256(flipLU256, maskLU);

   // 右下方向
   __m256i flipRD256 = _mm256_and_si256(_mm256_srlv_epi64(XX, shift1897), mOO);
   //flip |= (flip << shift) & mO;// 0 0 0 0 0 F&G G 0
   flipRD256 = _mm256_or_si256(flipRD256, _mm256_and_si256(_mm256_srlv_epi64(flipRD256, shift1897), mOO));
   // pre = mO & (mO << shift);// A&B B&C C&D D&E E&F F&G G&H 0
   __m256i preRD256 = _mm256_and_si256(mOO, _mm256_srlv_epi64(mOO, shift1897));
   //flip |= (flip << shift2) & pre;
   flipRD256 = _mm256_or_si256(flipRD256, _mm256_and_si256(_mm256_srlv_epi64(flipRD256, shift1897x2), preRD256));
   //flip |= (flip << shift2) & pre; 0 0 0 D&E&F&G E&F&G F&G G 0
   flipRD256 = _mm256_or_si256(flipRD256, _mm256_and_si256(_mm256_srlv_epi64(flipRD256, shift1897x2), preRD256));
   //outflank = p & (flip << shift);
   __m256i outflankRD256 = _mm256_and_si256(PP, _mm256_srlv_epi64(flipRD256, shift1897));
   //flip &= -(int)(outflank != 0);
   // Create a mask where outflank is not zero
   __m256i maskRD = _mm256_cmpeq_epi64(outflankRD256, _mm256_setzero_si256());
   // Invert the mask
   maskRD = _mm256_xor_si256(maskRD, _mm256_set1_epi64x(-1));
   // Perform bitwise AND on flip and maskLU
   flipRD256 = _mm256_and_si256(flipRD256, maskRD);

   __m256i result = _mm256_or_si256(flipLU256, flipRD256);
   __m256i temp1 = _mm256_or_si256(result, _mm256_permute4x64_epi64(result, _MM_SHUFFLE(1, 0, 3, 2)));
   __m256i temp2 = _mm256_or_si256(temp1, _mm256_permute4x64_epi64(temp1, _MM_SHUFFLE(2, 3, 0, 1)));
   uint64_t finalResult = _mm256_extract_epi64(temp2, 0);
   return finalResult;
};

__m256i _vectorcall FlipM256(const __m256i in_bwms)noexcept;
}
}
}
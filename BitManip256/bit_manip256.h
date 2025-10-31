/**
 * @file bit_manip256.h
 * @brief m256ビットマニピュレータの宣言
 * @author Gold Smith
 * @date 2023 2024 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#define NOMINMAX
#include <cstdint>
#include <immintrin.h>
#include <string>
#include "../DebugFnc/DebugFnc.h"
#include "../BitManip/BitManipDef.h"
#include "../BitManip/bit_manip_syuzuk81.h"
#include "m256.h"
#include "bit_manip_okuhara_256.h"
#pragma once

#if !defined(_M_X64) && !defined(__amd64__)
#error "This code must be compiled in x64 configuration."
#endif
#ifndef __AVX2__
#error "This code requires AVX2."
#endif

namespace bit_manip{
namespace AVX{
   constexpr int mm_shuffle_1_0_3_2 = _MM_SHUFFLE(1, 0, 3, 2);
   constexpr int mm_shuffle_1_3_0_2 = _MM_SHUFFLE(1, 3, 0, 2);

   inline __m256i  delta_swap256(__m256i xs, __m256i masks, int delta){
      __m256i ts = _mm256_and_si256(_mm256_xor_si256(_mm256_srli_epi64(xs, delta), xs), masks);
      return _mm256_xor_si256(_mm256_xor_si256(ts, _mm256_slli_epi64(ts, delta)), xs);
   }

   inline __m128i _vectorcall delta_swap128(__m128i xs, __m128i masks, int delta){
      __m256i res = delta_swap256(_mm256_set_m128i(xs, xs), _mm256_set_m128i(masks, masks), delta);
      return _mm256_extracti128_si256(res, 0);
   }

   inline __m256i _vectorcall rotateLeft256(__m256i xs, int n){
      return _mm256_or_si256(_mm256_slli_epi64(xs, n), _mm256_srli_epi64(xs, 64 - n));
   }

   inline __m128i _vectorcall rotateLeft128(__m128i xs, int n){
      return _mm256_extracti128_si256(rotateLeft256(_mm256_set_m128i(xs, xs), n), 0);
   }

   inline __m256i _vectorcall rotateRight256(__m256i xs, int n){
      return _mm256_or_si256(_mm256_srli_epi64(xs, n), _mm256_slli_epi64(xs, 64 - n));
   }

   inline __m128i _vectorcall rotateRight128(__m128i xs, int n){
      return _mm256_extracti128_si256(rotateRight256(_mm256_set_m128i(xs, xs), n), 0);
   }

   inline __m256i _vectorcall flipHorizontal256(__m256i x){
      x = delta_swap256(x, _mm256_set1_epi64x(0x5555555555555555ULL), 1);
      x = delta_swap256(x, _mm256_set1_epi64x(0x3333333333333333ULL), 2);
      return delta_swap256(x, _mm256_set1_epi64x(0x0F0F0F0F0F0F0F0FULL), 4);
   }

   inline __m128i _vectorcall flipHorizontal128(__m128i x){
      return _mm256_extracti128_si256(flipHorizontal256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall flipVertical256(__m256i x){
      x = delta_swap256(x, _mm256_set1_epi64x(0x00FF00FF00FF00FFULL), 8);
      x = delta_swap256(x, _mm256_set1_epi64x(0x0000FFFF0000FFFFULL), 16);
      return delta_swap256(x, _mm256_set1_epi64x(0x00000000FFFFFFFFULL), 32);
   }

   inline __m128i _vectorcall flipVertical128(__m128i x){
      return _mm256_extracti128_si256(flipVertical256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall flipDiagonalA1H8256(__m256i x){
      x = delta_swap256(x, _mm256_set1_epi64x(0x00AA00AA00AA00AAULL), 7);
      x = delta_swap256(x, _mm256_set1_epi64x(0x0000CCCC0000CCCCULL), 14);
      return delta_swap256(x, _mm256_set1_epi64x(0x00000000F0F0F0F0ULL), 28);
   }

   inline __m128i _vectorcall flipDiagonalA1H8128(__m128i x){
      return _mm256_extracti128_si256(flipDiagonalA1H8256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall flipDiagonalA8H1256(__m256i x){
      x = delta_swap256(x, _mm256_set1_epi64x(0x0055005500550055ULL), 9);
      x = delta_swap256(x, _mm256_set1_epi64x(0x0000333300003333ULL), 18);
      return delta_swap256(x, _mm256_set1_epi64x(0x000000000F0F0F0FULL), 36);
   }

   inline __m128i _vectorcall flipDiagonalA8H1128(__m128i x){
      return _mm256_extracti128_si256(flipDiagonalA8H1256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall rotateC90256(__m256i x){
      return flipHorizontal256(flipDiagonalA1H8256(x));
   }

   inline __m128i _vectorcall rotateC90128(__m128i x){
      return _mm256_extracti128_si256(rotateC90256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m128i _vectorcall rotateAC90128(__m128i x){
      return flipVertical128(flipDiagonalA1H8128(x));
   }

   inline __m256i _vectorcall rotateAC90256(__m256i x){
      return flipVertical256(flipDiagonalA1H8256(x));
   }

   inline __m256i _vectorcall rotate180256(__m256i x){
      return flipVertical256(flipHorizontal256(x));
   }

   inline __m128i _vectorcall rotate180128(__m128i x){
      return _mm256_extracti128_si256(rotate180256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall rotateC45U256(__m256i x){
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0xAAAAAAAAAAAAAAAAULL)
         , _mm256_xor_si256(x, rotateLeft256(x, 8))));
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0xCCCCCCCCCCCCCCCCULL)
         , _mm256_xor_si256(x, rotateLeft256(x, 16))));
      return _mm256_and_si256(_mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0xF0F0F0F0F0F0F0F0ULL)
         , _mm256_xor_si256(x, rotateLeft256(x, 32)))), _mm256_set1_epi64x(mask_CW45U));
   }

   inline __m128i _vectorcall rotateC45U128(__m128i x){
      return _mm256_extracti128_si256(rotateC45U256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall rotateC45L256(__m256i x){
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0x5555555555555555ULL)
         , _mm256_xor_si256(x, rotateRight256(x, 8))));
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0x3333333333333333ULL)
         , _mm256_xor_si256(x, rotateRight256(x, 16))));
      return _mm256_and_si256(_mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0x0F0F0F0F0F0F0F0FULL)
         , _mm256_xor_si256(x, rotateRight256(x, 32)))), _mm256_set1_epi64x(mask_CW45L));
   }

   inline __m128i _vectorcall rotateC45L128(__m128i x){
      return _mm256_extracti128_si256(rotateC45L256(_mm256_set_m128i(x, x)), 0);
   }

   /// <summary>
   /// rotateCW45のLUとULを両方変換する。
   /// </summary>
   /// <param name="x">m256i_u64[0,1]LU、m256i_u64[2,3]ULに変換したい物を入れる。</param>
   /// <returns>m256i_u64[0,1]CW45LU、m256i_u64[2,3]CW45UL</returns>
   inline __m256i _vectorcall rotateC45UULL256(__m256i x)noexcept{
      return _mm256_set_m128i(_mm256_extracti128_si256(rotateC45L256(x), 1), _mm256_extracti128_si256(rotateC45U256(x), 0));
   }

   inline __m256i _vectorcall rotateAC45U256(__m256i x)noexcept{
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0x5555555555555555ULL)
         , _mm256_xor_si256(x, rotateLeft256(x, 8))));
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0x3333333333333333ULL)
         , _mm256_xor_si256(x, rotateLeft256(x, 16))));
      return _mm256_and_si256(_mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0x0F0F0F0F0F0F0F0FULL)
         , _mm256_xor_si256(x, rotateLeft256(x, 32)))), _mm256_set1_epi64x(mask_ACW45U));
   }

   inline __m128i _vectorcall rotateAC45U128(__m128i x){
      return _mm256_extracti128_si256(rotateAC45U256(_mm256_set_m128i(x, x)), 0);
   }

   inline __m256i _vectorcall rotateAC45L256(__m256i x)noexcept{
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0xAAAAAAAAAAAAAAAAULL)
         , _mm256_xor_si256(x, rotateRight256(x, 8))));
      x = _mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0xCCCCCCCCCCCCCCCCULL)
         , _mm256_xor_si256(x, rotateRight256(x, 16))));
      return _mm256_and_si256(_mm256_xor_si256(x, _mm256_and_si256(_mm256_set1_epi64x(0xF0F0F0F0F0F0F0F0ULL)
         , _mm256_xor_si256(x, rotateRight256(x, 32)))), _mm256_set1_epi64x(mask_ACW45L));
   }

   /// <summary>
   /// rotateACW45のLUとULを両方変換する。
   /// </summary>
   /// <param name="x">m256i_u64[0,1]LU、m256i_u64[2,3]ULに変換したい物を入れる。</param>
   /// <returns>m256i_u64[0,1]ACW45LU、m256i_u64[2,3]ACW45UL</returns>
   inline __m256i _vectorcall rotateAC45UULL256(__m256i x){
      // LUとULを結合して返す。
      return _mm256_set_m128i(_mm256_extracti128_si256(rotateAC45L256(x), 1), _mm256_extracti128_si256(rotateAC45U256(x), 0));
   }

   inline __m128i _vectorcall rotateAC45L128(__m128i x){
      __m256i LUUL = rotateAC45UULL256(_mm256_set_m128i(x, x));
      return _mm256_extracti128_si256(_mm256_setr_epi64x(_mm256_extract_epi64(LUUL, 0), _mm256_extract_epi64(LUUL, 3), 0ULL, 0ULL), 0);
   }

   /// <summary>
   /// 4つのビットボードの左側の着手可能な位置を探す。mobility_line64のm256i版。
   /// </summary>
   /// <param name="p">m256i_u64[0-3]:p[0-3]。</param>
   /// <param name="o">m256i_u64[0-3]:o[0-3]。</param>
   /// <returns>m256i_u64[0-3]:pos[0-3]。</returns>
   inline __m256i _vectorcall mobility_line256(__m256i p, __m256i o)noexcept{
      __m256i mask = _mm256_set1_epi64x(0x7F7F7F7F7F7F7F7FULL);
      __m256i p1 = _mm256_slli_epi64(_mm256_and_si256(p, mask), 1);
      return _mm256_andnot_si256(_mm256_or_si256(p1, o), _mm256_add_epi64(p1, o));
   }

   /// <summary>
   /// 2つのビットボードの着手可能な手を探す。
   /// </summary>
   /// <param name="po">m256i_u64[0]:p0。m256i_u64[1]:o0。</param>
   /// <param name="po">m256i_u64[0]:p0。m256i_u64[1]:o0。</param>
   /// <returns>m256i_u64[0]:pos。</returns>
   inline uint64_t _vectorcall mobility_BB128(__m128i po){
      using namespace DebugFnc;
      __m256i p0o0p0o0 = _mm256_setr_m128i(po, po);
      uint64_t result = {};
      __m256i pAC90oAC90pAC90oAC90 = rotateAC90256(p0o0p0o0);
      __m256i pAC135UoAC135UpAC135LoAC135L = rotateAC45UULL256(pAC90oAC90pAC90oAC90);
      __m256i pAC45UoAC45UpAC45LoAC45L = rotateAC45UULL256(p0o0p0o0);

      __m256i pAC45UpAC135UpAC45LpAC135L = _mm256_unpacklo_epi64(pAC45UoAC45UpAC45LoAC45L, pAC135UoAC135UpAC135LoAC135L);
      __m256i oAC45UoAC135UoAC45LoAC135L = _mm256_unpackhi_epi64(pAC45UoAC45UpAC45LoAC45L, pAC135UoAC135UpAC135LoAC135L);
      __m256i rAC45UrAC135UrAC45LrAC135L = mobility_line256(pAC45UpAC135UpAC45LpAC135L, oAC45UoAC135UoAC45LoAC135L);
      __m256i rAC45LrAC135LrAC45UrAC135U = _mm256_permute4x64_epi64(rAC45UrAC135UrAC45LrAC135L, mm_shuffle_1_0_3_2);
      __m256i r0rAC90r0rAC90 = rotateC45UULL256(rAC45LrAC135LrAC45UrAC135U);

      __m256i pC90oC90pC90oC90 = rotateC90256(p0o0p0o0);
      __m256i pC135UoC135UpC135LoC135L = rotateC45UULL256(pC90oC90pC90oC90);
      __m256i pC45UoC45UpC45LoC45L = rotateC45UULL256(p0o0p0o0);
      __m256i pC45UpC135UpC45LpC135L = _mm256_unpacklo_epi64(pC45UoC45UpC45LoC45L, pC135UoC135UpC135LoC135L);
      __m256i oC45UoC135UoC45LoC135L = _mm256_unpackhi_epi64(pC45UoC45UpC45LoC45L, pC135UoC135UpC135LoC135L);
      __m256i rC45UrC135UrC45LrC135L = mobility_line256(pC45UpC135UpC45LpC135L, oC45UoC135UoC45LoC135L);
      __m256i rC45LrC135LrC45UrC135U = _mm256_permute4x64_epi64(rC45UrC135UrC45LrC135L, mm_shuffle_1_0_3_2);
      __m256i r0rC90r0rC90 = rotateAC45UULL256(rC45LrC135LrC45UrC135U);

      __m256i p180o180p180o180 = rotate180256(p0o0p0o0);
      __m256i p0o0p180o180 = _mm256_set_m128i(_mm256_extracti128_si256(p180o180p180o180, 0), _mm256_extracti128_si256(p0o0p0o0, 0));
      __m256i pC90oC90pAC90oAC90 = _mm256_set_m128i(_mm256_extracti128_si256(pAC90oAC90pAC90oAC90, 0), _mm256_extracti128_si256(pC90oC90pC90oC90, 0));
      __m256i p0pC90p180pAC90 = _mm256_unpacklo_epi64(p0o0p180o180, pC90oC90pAC90oAC90);
      __m256i o0oC90o180oAC90 = _mm256_unpackhi_epi64(p0o0p180o180, pC90oC90pAC90oAC90);
      __m256i r0rC90r180rAC90 = mobility_line256(p0pC90p180pAC90, o0oC90o180oAC90);

      __m256i r0r0r0r0 = _mm256_unpacklo_epi64(r0rAC90r0rAC90, r0rC90r0rC90);
      for( int i(0); i < 3; ++i ){
         result |= r0r0r0r0.m256i_u64[i];
      }

      __m256i rAC90rAC90rAC90_0 = _mm256_setr_epi64x(r0rAC90r0rAC90.m256i_u64[1], r0rAC90r0rAC90.m256i_u64[3], r0rC90r180rAC90.m256i_u64[3], 0);
      __m256i r0r0r0_0 = rotateC90256(rAC90rAC90rAC90_0);
      for( int i(0); i <= 2; ++i ){
         result |= r0r0r0_0.m256i_u64[i];
      }

      __m256i rC90rC90rC90_0 = _mm256_setr_epi64x(r0rC90r0rC90.m256i_u64[1], r0rC90r0rC90.m256i_u64[3], r0rC90r180rAC90.m256i_u64[1], 0);
      r0r0r0_0 = rotateAC90256(rC90rC90rC90_0);
      for( int i(0); i <= 3; ++i ){
         result |= r0r0r0_0.m256i_u64[i];
      }

      result |= bit_manip::syuzuk81::rotate180(r0rC90r180rAC90.m256i_u64[2]);
      result |= r0rC90r180rAC90.m256i_u64[0];
      return result;
   }

   /// <summary>
   /// ２つのビットボードをラインフリップする。
   /// </summary>
   /// <param name="p1p2">プレイヤーボード。</param>
   /// <param name="o1o2">相手ボード。</param>
   /// <param name="pos1pos2">指し手</param>
   /// <returns>m256i_u64[0,1]p1o1の結果。m256i_u64[2,3]p2o2の結果。</returns>
   inline void _vectorcall flipDiscs2Line128(__m128i p0p1, __m128i o0o1, __m128i m0m1, __m128i* prp0rp1, __m128i* pro0ro1)noexcept{
      __m128i m01m11 = _mm_slli_epi64(m0m1, 1);
      __m128i masko0o1 = _mm_and_si128(o0o1, _mm_set1_epi64x(0x7F7F7F7F7F7F7F7F));
      __m128i p01p11 = _mm_add_epi64(masko0o1, m01m11);
      __m128i p02p12 = _mm_and_si128(p0p1, p01p11);
      __m128i p03p13 = _mm_sub_epi64(p02p12, m01m11);
      __m128i o03o13 = _mm_andnot_si128(p03p13, o0o1);
      __m128i p04p14 = _mm_or_si128(p0p1, p03p13);
      __m128i p05p15 = _mm_or_si128(p04p14, m0m1);

      __m128i zero_vec = _mm_setzero_si128();
      __m128i cmp_result = _mm_cmpeq_epi64(p02p12, zero_vec);
      // p02p12が0でない場合、マスクは全ビットが1になります
      __m128i mask = _mm_andnot_si128(cmp_result, _mm_set1_epi64x(-1));
      __m128i p06p16 = _mm_blendv_epi8(p0p1, p05p15, mask);
      __m128i o04o14 = _mm_blendv_epi8(o0o1, o03o13, mask);
      *prp0rp1 = p06p16;
      *pro0ro1 = o04o14;
      return;
   }

   /// <summary>
   /// 4つのビットボードをラインフリップする。
   /// </summary>
   /// <param name="p1p2p3p4">プレイヤーボード。</param>
   /// <param name="o1o2o3o4">相手ボード。</param>
   /// <param name="pos1pos2pos3pos4">指し手</param>
   /// <param name="p_rp">結果を保存するプレイヤーボードへのポインタ。</param>
   /// <param name="p_ro">結果を保存する相手ボードへのポインタ。</param>
   /// <returns>void</returns>
   inline void _vectorcall flipDiscs4Lines256(__m256i p0p1p2p3, __m256i o0o1o2o3, __m256i m0m1m2m3, __m256i* p_rp0rp1rp2rp3, __m256i* p_ro0ro1ro2ro3)noexcept{
      __m256i m01m11 = _mm256_slli_epi64(m0m1m2m3, 1);
      __m256i masko0o1o2o3 = _mm256_and_si256(o0o1o2o3, _mm256_set1_epi64x(0x7F7F7F7F7F7F7F7F));
      __m256i p01p11p21p31 = _mm256_add_epi64(masko0o1o2o3, m01m11);
      __m256i p02p12p22p32 = _mm256_and_si256(p0p1p2p3, p01p11p21p31);
      __m256i p03p13p23p33 = _mm256_sub_epi64(p02p12p22p32, m01m11);
      __m256i o03o13o23o33 = _mm256_andnot_si256(p03p13p23p33, o0o1o2o3);
      __m256i p04p14p24p34 = _mm256_or_si256(p0p1p2p3, p03p13p23p33);
      __m256i p05p15p25p35 = _mm256_or_si256(p04p14p24p34, m0m1m2m3);

      __m256i zero_vec = _mm256_setzero_si256();
      __m256i cmp_result = _mm256_cmpeq_epi64(p02p12p22p32, zero_vec);
      // p02p12が0でない場合、マスクは全ビットが1になります
      __m256i mask = _mm256_andnot_si256(cmp_result, _mm256_set1_epi64x(-1));
      __m256i p06p16p26p36 = _mm256_blendv_epi8(p0p1p2p3, p05p15p25p35, mask);
      __m256i o04o14o24o34 = _mm256_blendv_epi8(o0o1o2o3, o03o13o23o33, mask);
      *p_rp0rp1rp2rp3 = p06p16p26p36;
      *p_ro0ro1ro2ro3 = o04o14o24o34;
      return;
   }

   /// <summary>
   /// 右斜め上方向ひっくり返るかのテスト関数。
   /// </summary>
   /// <param name="p">player</param>
   /// <param name="o">opponent</param>
   /// <param name="X">move</param>
   /// <returns>ひっくり返った結果。</returns>
   inline __m128i flip_DiagonalA1H8_line(uint64_t p, uint64_t o, uint64_t X)noexcept{
      uint64_t shift = 0x09ULL;
      uint64_t shift2 = shift + shift;
      uint64_t mO = o & 0x7e7e7e7e7e7e7e7e;
      DPBD(mO);
      DPBD(X);
      uint64_t flip = {};// 0 0 0 0 0 0 G 0
      DPBD((X << shift) & mO);
      flip = (X << shift) & mO;
      DPBD(flip);
      DPBD((flip << shift) & mO);
      flip |= (flip << shift) & mO;// 0 0 0 0 0 F&G G 0
      DPBD(flip);
      DPBD((mO << shift));
      uint64_t pre;
      DPBD(mO << shift);
      pre = mO & (mO << shift);// A&B B&C C&D D&E E&F F&G G&H 0
      DPBD(pre);
      DPBD(flip << shift2);
      DPBD((flip << shift2) & pre);
      flip |= (flip << shift2) & pre;
      DPBD(flip);	// 0 0 0 D&E&F&G E&F&G F&G G 0
      DPBD((flip << shift2));
      DPBD((flip << shift2) & pre);
      flip |= (flip << shift2) & pre;
      DPBD(flip);// 0 B&C&D&E&F&G .. F&G G 0
      uint64_t outflank;
      outflank = p & (flip << shift);
      DPBD(outflank);
      DPBD((uint64_t)-(int)(outflank != 0));
      flip &= -(int)(outflank != 0);
      DPBD(flip);
      uint64_t rp = p | X | flip;
      DPBD(rp);
      uint64_t ro = ~rp & o;
      DPBD(ro);
      return _mm_set_epi64x(ro, rp);
   }

/// @brief プレーヤーの手番をチェンジする
/// @param Inm m256を__m256iにして渡す。
/// @return チェンジされた__m256i。
   inline constexpr __m256i _vectorcall SwapTurn(const __m256i Inm)noexcept{
      const bit_manip::m256& m = Inm;
      bit_manip::m256 mNext(m);
      mNext.bIsCurrentTurnWhite = !m.bIsCurrentTurnWhite;
      return mNext.m256i;
   }

   /// @brief 次のターンが有るか調べる。
   /// @param In 現在のターン。
   /// @return 次のターンが無ければ、m256{}を返す。有ればその手番に変更したm256を返す。
   /// 次の手番がパスならば、その次の手番にしたm256を返す。
   inline __m256i _vectorcall IsThereNextMove(const __m256i In){
      const bit_manip::m256 &Inm256 = In;
      bit_manip::m256 Flipped;
      DWORD64 cnt = __popcnt64(Inm256.M);

       switch(cnt) {
      case 0:
         Flipped = Inm256;
         break;
      case 1:
         Flipped = bit_manip::AVX::Okuhara::FlipM256(Inm256);
         break;
      default:
         throw std::runtime_error("There are multiple moves.");
      }

      bit_manip::m256 Next256 = Flipped;
      Next256.bIsCurrentTurnWhite ^= true;
      if ( Next256.M = bit_manip::AVX::Okuhara::GetMoves256(Next256); !Next256.M){
         bit_manip::m256 NextNext256 = Next256;
         NextNext256.bIsCurrentTurnWhite ^= true;
         if (NextNext256.M = bit_manip::AVX::Okuhara::GetMoves256(NextNext256); !NextNext256.M){
            return bit_manip::m256{}.m256i;
         }
         NextNext256.turn++;
         return NextNext256.m256i;
      }
      Next256.turn++;
      return Next256.m256i;
   }

   /// @brief ターゲットの次のターンのムーブの数を黒基準で返す。
   /// @brief ターゲットの次のターンが白の場合、マイナスの数を返す。
   /// @brief ターゲットの次のターンがない場合、0を返す。
   /// @param Tgt ターゲット。
   inline int GetNumMovesOfNextTurn(const bit_manip::m256 Tgt) {
      bit_manip::m256 FlippedTgt;
      switch(__popcnt64(Tgt.M)){
      case 0:
         FlippedTgt = Tgt;
         break;
      case 1:
         FlippedTgt = bit_manip::AVX::Okuhara::FlipM256(Tgt);
         break;
      default:
         throw std::runtime_error("Ellegal Data Format.");
      }
      bit_manip::m256 NextMoves = IsThereNextMove(FlippedTgt.m256i);
      return NextMoves.bIsCurrentTurnWhite ? -(int)__popcnt64(NextMoves.M) : (int)__popcnt64(NextMoves.M);
   }

   inline __m256i _vectorcall _mm256_popcnt_epi64_custom(__m256i m)noexcept{
      // x = x - ((x >> 1) & 0x5555555555555555);
      m = _mm256_sub_epi64(m, _mm256_and_si256(_mm256_srli_epi64(m, 1)
         , _mm256_set1_epi64x(0x5555555555555555ULL)));
      // x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
      __m256i m2 = _mm256_add_epi64(_mm256_and_si256(m, _mm256_set1_epi64x(0x3333333333333333ULL))
         , _mm256_and_si256(_mm256_srli_epi64(m, 2)
            , _mm256_set1_epi64x(0x3333333333333333ULL)));
      // x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F;
      __m256i m3 = _mm256_and_si256(_mm256_add_epi64(m2, _mm256_srli_epi64(m2, 4))
         , _mm256_set1_epi64x(0x0F0F0F0F0F0F0F0FULL));
      // x = (x * 0x0101010101010101) >> 56;
      __m256i m4 = _mm256_mullo_epu64_custom(m3, _mm256_set1_epi64x(0x0101010101010101ULL));
      __m256i m5 = _mm256_srli_epi64(m4, 56);
      return m5;
   }

   BOOL SaveM256ArrayW(const std::wstring& strPathFileName, const std::vector<bit_manip::m256>& in_vec);

   BOOL LoadM256ArrayW(const std::wstring& strPathFileName, std::vector<bit_manip::m256>& in_vec);

}
}
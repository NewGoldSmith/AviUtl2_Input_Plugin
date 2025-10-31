/**
 * @file mm256_custom.h
 * @brief ビットマニピュレータの宣言
 * @author Gold Smith
 * @date 2023 2024 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#pragma once
#include <immintrin.h>
#if !defined(_M_X64) && !defined(__amd64__)
#error "This code must be compiled in x64 configuration."
#endif
#ifndef __AVX2__
#error "This code requires AVX2."
#endif

 /** namespace bit_manip:ビット操作関数群 */
namespace bit_manip{
	namespace AVX{
		inline __m256i _mm256_mullo_epu64_custom(__m256i m1, __m256i m2){
			//uint32_t a,b,c,d; uint64_t ab,cd;
			//ab*d=(a*d)<<32+b*d; ab*cd=ab*d+(ab*c)<<32;
			__m256i a = _mm256_srli_epi64(m1, 32);
			__m256i b = _mm256_and_si256(m1, _mm256_set1_epi64x(0xFFFFFFFF));
			__m256i c = _mm256_srli_epi64(m2, 32);
			__m256i d = _mm256_and_si256(m2, _mm256_set1_epi64x(0xFFFFFFFF));
			__m256i ad_high = _mm256_slli_epi64(_mm256_mul_epu32(a, d), 32);
			__m256i abd = _mm256_add_epi64(_mm256_mul_epu32(b, d), ad_high);
			__m256i abc = _mm256_mul_epu32(b, c);
			__m256i abc_high = _mm256_slli_epi64(abc, 32);
			__m256i product = _mm256_add_epi64(abd, abc_high);
			return product;
		}
		inline __m256i _mm256_mulhi_epu64_custom(__m256i m1, __m256i m2){
			//uint32_t a,b,c,d; uint64_t ab,cd;
			//ab*d=(a*d)<<32+b*d; ab*cd=ab*d+(ab*c)<<32;
			__m256i a = _mm256_srli_epi64(m1, 32);
			__m256i b = _mm256_and_si256(m1, _mm256_set1_epi64x(0xFFFFFFFF));
			__m256i c = _mm256_srli_epi64(m2, 32);
			__m256i d = _mm256_and_si256(m2, _mm256_set1_epi64x(0xFFFFFFFF));

			__m256i ac = _mm256_mul_epu32(a, c);
			__m256i ad = _mm256_mul_epu32(a, d);
			__m256i bc = _mm256_mul_epu32(b, c);
			__m256i bd = _mm256_mul_epu32(b, d);

			__m256i carry = _mm256_add_epi64(_mm256_srli_epi64(bd, 32), _mm256_add_epi64(ad, bc));
			carry = _mm256_add_epi64(carry, _mm256_srli_epi64(carry, 32));

			__m256i high = _mm256_add_epi64(ac, _mm256_srli_epi64(carry, 32));

			return high;
		}
	}
}
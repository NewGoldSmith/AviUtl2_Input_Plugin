 /**
  * @file bit_manip_syuzuk81.h
  * @author ysuzuk81 <https: //qiita.com/ysuzuk81/>
  * @author website <https: //qiita.com/ysuzuk81/items/453b08a14d23fb8c6c11>
  * @date 2023 Original
  * @author Gold Smith
  * @date 2023 2024 modified
  */

#pragma once
#include "../DebugFnc/DebugFnc.h"
#include "BitManipDef.h"
#include <stdint.h>

namespace bit_manip{
	namespace syuzuk81{

		constexpr inline uint64_t delta_swap(const uint64_t x, const uint64_t mask, const int delta){
			uint64_t t = (x ^ (x >> delta)) & mask;
			return x ^ t ^ (t << delta);
		}

		constexpr inline uint64_t rotateLeft(const uint64_t x, const int n){
			return (x << n) | (x >> (64 - n));
		}

		constexpr inline uint64_t rotateRight(const uint64_t x, const int n){
			return (x >> n) | (x << (64 - n));
		}

		constexpr inline uint64_t flipHorizontal(const uint64_t in_x){
			uint64_t x(in_x);
			x = delta_swap(x, 0x5555555555555555ULL, 1);
			x = delta_swap(x, 0x3333333333333333ULL, 2);
			return delta_swap(x, 0x0F0F0F0F0F0F0F0FULL, 4);
		}

		constexpr inline uint64_t flipVertical(const uint64_t in_x){
			uint64_t x(in_x);
			x = delta_swap(x, 0x00FF00FF00FF00FFULL, 8);
			x = delta_swap(x, 0x0000FFFF0000FFFFULL, 16);
			return delta_swap(x, 0x00000000FFFFFFFFULL, 32);
		}

		constexpr inline uint64_t flipDiagonalA1H8(const uint64_t in_x){
			uint64_t x(in_x);
			x = delta_swap(x, 0x00AA00AA00AA00AAULL, 7);
			x = delta_swap(x, 0x0000CCCC0000CCCCULL, 14);
			return delta_swap(x, 0x00000000F0F0F0F0ULL, 28);
		}

		constexpr inline uint64_t flipDiagonalA8H1(const uint64_t in_x){
			uint64_t x(in_x);
			x = delta_swap(x, 0x0055005500550055ULL, 9);
			x = delta_swap(x, 0x0000333300003333ULL, 18);
			return delta_swap(x, 0x000000000F0F0F0FULL, 36);
		}

		constexpr inline uint64_t rotateC90(const uint64_t x){
			return flipHorizontal(flipDiagonalA1H8(x));
		}

		constexpr inline uint64_t rotateAC90(const uint64_t x){
			return flipVertical(flipDiagonalA1H8(x));
		}

		constexpr inline uint64_t rotate180(const uint64_t x){
			return flipVertical(flipHorizontal(x));
		}

		constexpr inline uint64_t rotateC45U(const uint64_t in_x){
			uint64_t x(in_x);
			x ^= 0xAAAAAAAAAAAAAAAAULL & (x ^ rotateLeft(x, 8));
			x ^= 0xCCCCCCCCCCCCCCCCULL & (x ^ rotateLeft(x, 16));
			return (x ^ 0xF0F0F0F0F0F0F0F0ULL & (x ^ rotateLeft(x, 32))) & mask_CW45U;
		}

		constexpr inline uint64_t rotateC45L(const uint64_t in_x){
			uint64_t x(in_x);
			x ^= 0x5555555555555555ULL & (x ^ rotateRight(x, 8));
			x ^= 0x3333333333333333ULL & (x ^ rotateRight(x, 16));
			return (x ^ 0x0F0F0F0F0F0F0F0FULL & (x ^ rotateRight(x, 32))) & mask_CW45L;
		}

		constexpr inline uint64_t rotateAC45U(const uint64_t in_x){
			uint64_t x(in_x);
			x ^= 0x5555555555555555ULL & (x ^ rotateLeft(x, 8));
			x ^= 0x3333333333333333ULL & (x ^ rotateLeft(x, 16));
			return (x ^ 0x0F0F0F0F0F0F0F0FULL & (x ^ rotateLeft(x, 32))) & mask_ACW45U;
		}

		constexpr inline uint64_t rotateAC45L(const uint64_t in_x){
			uint64_t x(in_x);
			x ^= 0xAAAAAAAAAAAAAAAAULL & (x ^ rotateRight(x, 8));
			x ^= 0xCCCCCCCCCCCCCCCCULL & (x ^ rotateRight(x, 16));
			return (x ^ 0xF0F0F0F0F0F0F0F0ULL & (x ^ rotateRight(x, 32))) & mask_ACW45L;
		}
	}
}


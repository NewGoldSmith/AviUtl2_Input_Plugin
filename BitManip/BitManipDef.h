/**
 * @file BitManipDef.h
 * @brief ビットマスクの定義
 * @author Gold Smith
 * @date 2023 2024 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#pragma once
#include <stdint.h>

#pragma push_macro("m_ACW45L")
#undef m_ACW45L 
#pragma push_macro("m_ACW45U")
#undef m_ACW45U
#pragma push_macro("m_CW45L")
#undef m_CW45L 
#pragma push_macro("m_CW45U")
#undef m_CW45U


#define m_ACW45L \
0b\
00000001\
00000011\
00000111\
00001111\
00011111\
00111111\
01111111\
11111111ULL
#define m_ACW45U \
0b\
11111111\
11111110\
11111100\
11111000\
11110000\
11100000\
11000000\
10000000ULL

#define m_CW45L \
0b\
10000000\
11000000\
11100000\
11110000\
11111000\
11111100\
11111110\
11111111ULL
#define m_CW45U \
0b\
11111111\
01111111\
00111111\
00011111\
00001111\
00000111\
00000011\
00000001ULL

namespace bit_manip{
	constexpr uint64_t mask_ACW45L = m_ACW45L;
	constexpr uint64_t mask_ACW45U = m_ACW45U;
	constexpr uint64_t mask_CW45L = m_CW45L;
	constexpr uint64_t mask_CW45U = m_CW45U;
}
#pragma pop_macro("m_ACW45L")
#pragma pop_macro("m_ACW45U")
#pragma pop_macro("m_CW45L")
#pragma pop_macro("m_CW45U")

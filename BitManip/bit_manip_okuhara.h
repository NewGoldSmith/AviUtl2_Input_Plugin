/**
 * @file bit_manip_okuhara.h
 * @author 奥原 俊彦 <okuhara＠amy.hi-ho.ne.jp>
 * @author web site <http: //www.amy.hi-ho.ne.jp/okuhara/bitboard.htm>
 * @date 2020 Original
 * @author Gold Smith
 * @date 2023 2024 modified
 */
#pragma once
#include <cstdint>
#include "../DebugFnc/DebugFnc.h"
#include "bit_manip_syuzuk81.h"

namespace bit_manip{
	namespace Okuhara{
		using namespace syuzuk81;

		/// @brief プレーヤーが打てる位置の左方向の探索。キャリー伝番を利用
		/// @param p プレーヤー
		/// @param o 相手
		/// @return 打てる位置
		constexpr inline uint8_t GetMobilityLineL8(uint8_t p, uint8_t o){
			uint8_t mO = o & 0x7eui8;
			uint8_t flip1 = mO & (p << 1);
			flip1 = mO + flip1;
			flip1 &= ~(p | o);	// mask with empties
			return flip1;
		}

		/// <summary>
		/// mobility_lineL8のビットボード版。左側の着手可能な位置をまとめて探す。
		/// </summary>
		/// <param name="p">プレイヤーボード。</param>
		/// <param name="o">相手ボード。</param>
		/// <returns>着手可能な左側のビットが立っている。</returns>
		constexpr inline uint64_t mobility_lineL64(uint64_t p, uint64_t o){
			uint64_t mO = o & 0x7E7E7E7E7E7E7E7EULL;
			uint64_t flip1 = mO & (p << 1);
			flip1 = mO + flip1;
			flip1 &= ~(p | o);	// mask with empties
			return flip1;
		}

		inline uint64_t GetFlipShiftLineL64(uint64_t p, uint64_t o, uint64_t m){
			uint64_t mO = o & 0x7e7e7e7e7e7e7e7e;	// except for vertical
			DPBD(mO);
			uint64_t flip = (m << 1) & mO;		// 0 0 0 0 0 0 G 0
			DPBD(flip);
			flip |= (flip << 1) & mO;	// 0 0 0 0 0 F&G G 0
			DPBD(flip);
			flip |= (flip << 1) & mO;	// 0 0 0 0 E&F&G F&G G 0
			DPBD(flip);
			flip |= (flip << 1) & mO;
			DPBD(flip);
			flip |= (flip << 1) & mO;
			DPBD(flip);
			flip |= (flip << 1) & mO;
			DPBD(flip);
			flip |= (flip << 1) & mO;	// 0 B&C&D&E&F&G .. F&G G 0
			DPBD(flip);
			uint64_t outflank = p & (flip << 1);
			DPBD(outflank);
			if( outflank == 0 )
				flip = 0;
			return flip;
		}

		constexpr inline uint64_t GetFlipParaLineL64(const uint64_t p, const uint64_t o, const uint64_t m){
			uint64_t mO = o & 0x7e7e7e7e7e7e7e7eULL;
			uint64_t flip = (m << 1) & mO;		// 0 0 0 0 0 0 G 0
			flip |= (flip << 1) & mO;	// 0 0 0 0 0 F&G G 0
			uint64_t pre = mO & (mO << 1);		// A&B B&C C&D D&E E&F F&G G&H 0
			flip |= (flip << 2) & pre;	// 0 0 0 D&E&F&G E&F&G F&G G 0
			flip |= (flip << 2) & pre;	// 0 B&C&D&E&F&G .. F&G G 0
			uint64_t outflank = p & (flip << 1);
			flip &= -(int)(outflank != 0);
			return flip;
		}

	}
}

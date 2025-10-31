/**
 * @file debug_fnc.cpp
 * @brief デバッグ関数の実装
 * @author Gold Smith
 * @date 2024 2025
 *
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#pragma once
#include "bit_manip_okuhara.h"
#include "bit_manip_syuzuk81.h"

namespace bit_manip{

	using namespace bit_manip::syuzuk81;
	using namespace bit_manip::Okuhara;

	/// @brief ビットボードの全ての方向のmove出来る場所を探す。
	/// @param p プレーヤー
	/// @param o 相手
	/// @return 置ける場所
	constexpr inline uint64_t GetMobilityBB64(const uint64_t p, const uint64_t o){
		// 時計短針0時、2時30分:、･･･、10時30分と探していく。見つけたい方向を左にする。
		uint64_t pos_result(0ULL);
		uint64_t pACW90 = rotateAC90(p);
		uint64_t oACW90 = rotateAC90(o);
		uint64_t p180 = rotate180(p);
		uint64_t o180 = rotate180(o);
		pos_result = rotateC90(mobility_lineL64(pACW90, oACW90));
		pos_result |= rotateC90(rotateC45L(mobility_lineL64(rotateAC45U(pACW90), rotateAC45U(oACW90))));
		pos_result |= rotateC90(rotateC45U(mobility_lineL64(rotateAC45L(pACW90), rotateAC45L(oACW90))));
		pos_result |= rotate180(mobility_lineL64(p180, o180));
		pos_result |= rotate180(rotateC45L(mobility_lineL64(rotateAC45U(p180), rotateAC45U(o180))));
		pos_result |= rotate180(rotateC45U(mobility_lineL64(rotateAC45L(p180), rotateAC45L(o180))));
		pos_result |= rotateAC90(mobility_lineL64(rotateC90(p), rotateC90(o)));
		pos_result |= rotateAC45L(mobility_lineL64(rotateC45U(p), rotateC45U(o)));
		pos_result |= rotateAC45U(mobility_lineL64(rotateC45L(p), rotateC45L(o)));
		pos_result |= mobility_lineL64(p, o);
		pos_result |= rotateC45L(mobility_lineL64(rotateAC45U(p), rotateAC45U(o)));
		pos_result |= rotateC45U(mobility_lineL64(rotateAC45L(p), rotateAC45L(o)));
		return pos_result;
	}

	constexpr uint64_t GetFlip64(const uint64_t p, const uint64_t o, const uint64_t m){
		const uint64_t pAC90 = rotateAC90(p);
		const uint64_t oAC90 = rotateAC90(o);
		const uint64_t mAC90 = rotateAC90(m);
		const uint64_t p180 = rotate180(p);
		const uint64_t o180 = rotate180(o);
		const uint64_t m180 = rotate180(m);
		uint64_t flipped;

		flipped = rotateC90(GetFlipParaLineL64(pAC90, oAC90, mAC90));
		flipped |= rotateC90(rotateC45L(GetFlipParaLineL64(rotateAC45U(pAC90), rotateAC45U(oAC90), rotateAC45U(mAC90))));
		flipped |= rotateC90(rotateC45U(GetFlipParaLineL64(rotateAC45L(pAC90), rotateAC45L(oAC90), rotateAC45L(mAC90))));
		flipped |= rotate180(GetFlipParaLineL64(p180, o180, m180));
		flipped |= rotate180(rotateC45L(GetFlipParaLineL64(rotateAC45U(p180), rotateAC45U(o180), rotateAC45U(m180))));
		flipped |= rotate180(rotateC45U(GetFlipParaLineL64(rotateAC45L(p180), rotateAC45L(o180), rotateAC45L(m180))));
		flipped |= rotateAC90(GetFlipParaLineL64(rotateC90(p), rotateC90(o), rotateC90(m)));
		flipped |= rotateAC45L(GetFlipParaLineL64(rotateC45U(p), rotateC45U(o), rotateC45U(m)));
		flipped |= rotateAC45U(GetFlipParaLineL64(rotateC45L(p), rotateC45L(o), rotateC45L(m)));
		flipped |= GetFlipParaLineL64(p, o, m);
		flipped |= rotateC45L(GetFlipParaLineL64(rotateAC45U(p), rotateAC45U(o), rotateAC45U(m)));
		return flipped;
	}
}

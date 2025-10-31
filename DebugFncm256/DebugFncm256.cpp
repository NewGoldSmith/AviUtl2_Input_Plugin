/**
 * @file DebugFncm256.cpp
 * @brief デバッグ関数の実装m256用
 * @author Gold Smith
 * @date 2023
 *
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#include "DebugFncm256.h"
using namespace std;

///// @brief 読み筋用m256チェーンを作る。最後のボードにはスコアをセットする。
///// @brief 関数テスト用。
//std::vector<bit_manip::m256> DebugFnc::make_array_and_set_score(bit_manip::m256 m,uint8_t turn,const int score){
//	std::vector<bit_manip::m256> vSrc{};
//	for(int i = 0; (m = bit_manip::AVX::IsThereNextMove(m.m256i))&&i<turn;++i){
//		DWORD index;
//		DWORD result = BitScanReverse64(&index,m.M);
//		m.M = 1ULL << index;
//		vSrc.push_back(m);
//	}
//	vSrc.back().score = score;
//	return vSrc;
//}

std::string DebugFnc::binary_to_string(const bit_manip::m128 b) {
	std::stringstream ss;
	for (int i = 0; i <= 7; ++i) {
		ss << binary_to_string(((uint8_t *)&b.B)[7 - i]);
		ss << " ";
		ss << binary_to_string(((uint8_t *)&b.W)[7 - i]);
		if (i == 7) {
			break;
		}
		ss << std::endl;
	}
	return ss.str().c_str();
}

void DebugFnc::print_binary(const __m128i b) {
	std::cout << binary_to_string((bit_manip::m128)b) << std::endl;
}

void DebugFnc::dout_binary(const __m128i b) {
	OutputDebugStringA((binary_to_string((bit_manip::m128)b) + "\r\n").c_str());
}

std::string DebugFnc::binary_to_string(const __m256i b) {
	std::stringstream ss;
	for (int i = 0; i <= 7; ++i) {
		ss << binary_to_string(((uint8_t *)&b.m256i_u64[0])[7 - i]);
		ss << " ";
		ss << binary_to_string(((uint8_t *)&b.m256i_u64[1])[7 - i]);
		ss << " ";
		ss << binary_to_string(((uint8_t *)&b.m256i_u64[2])[7 - i]);
		ss << " ";
		ss << binary_to_string(((uint8_t *)&b.m256i_u64[3])[7 - i]);
		if (i == 7) {
			break;
		}
		ss << std::endl;
	}
	return ss.str().c_str();
}

void DebugFnc::print_binary(const __m256i b) {
	std::cout << binary_to_string(b) << std::endl;
}

void DebugFnc::dout_binary(const __m256i b) {
	OutputDebugStringA((binary_to_string(b) + "\r\n").c_str());
}

void DebugFnc::dout_binary(const bit_manip::m256 m) {
	dout_binary(m.m256i);
}

void DebugFnc::DebugPrintBB256Satus(const __m256i m){
	OutputDebugStringA(BB256toStatusString(m).c_str());
}

void DebugFnc::DebugPrintBB256Satus(const bit_manip::m256 m){
	OutputDebugStringA(BB256toStatusString(m.m256i).c_str());
}

std::string DebugFnc::BB256toStatusString(const __m256i Inm){
	const bit_manip::m256& m = Inm;
	const std::vector<std::string> stvs = BB256toStatusStrings(m.m256i);
	stringstream ss;
	for( auto s : stvs ){
		ss << s << endl;
	}
	return ss.str();
	return string();
}

std::string DebugFnc::padding(const std::string& left, const std::string& right, const int total_width){
	const int space = total_width - std::min<int>((int)left.size()+ (int)right.size(), total_width);
	string str_space;
	for( int i = space; 0 < i; --i ){
		str_space += " ";
	}
	return left + str_space + right ;
}

std::vector<std::string> DebugFnc::BB256toStatusStrings(const __m256i Inm){
   constexpr char coordinates[64][3] = {
       "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
       "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
       "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
       "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
       "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
       "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
       "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
       "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1"
   };
   const bit_manip::m256& m = Inm;
	std::stringstream ss;
	char cp;
	char co;

		cp = 'X';
		co = 'C';

   std::vector<std::string> strings = BinaryBoardsToString(m.B,m.W,m.M,cp,co,'M','-');
   std::vector<std::string> vsResult;
	for( int i = 0; i <= 8; ++i ){
		constexpr int PADD1 = 22;
		constexpr int PADD2 = 2;
		constexpr const char* PADD_SPACE = "  ";
		switch( i ){
			case 0:
			{
				ss << "  a b c d e f g h";
				ss << PADD_SPACE;
				ss << padding("","",PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			case 1:
			{
            ss << "1 ";
            ss << strings[0];
            ss << PADD_SPACE;
            string str3;
            if(auto cnt = __popcnt64(m.M); cnt == 1){
               str3 = coordinates[([&]{ DWORD Index; _BitScanReverse64(&Index,m.M); return Index; }())];
            } else{
               str3 = to_string(cnt) + "個";
            }
            ss << padding("MOVE:",str3,PADD1);
            ss << PADD_SPACE;
            ss << padding("","",PADD2);
				break;
			}
			case 2:
			{
				ss << "2 ";
				ss << strings[1];
				ss << PADD_SPACE;
				ss	<< padding("手番:", m.bIsCurrentTurnWhite ? "白(C)" : "黒(X)", PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			case 3:
			{
				ss << "3 ";
				ss << strings[2];
				ss << PADD_SPACE;
            ss << padding( "黒(X)基準 SCORE:",to_string(m.score),PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			case 4:
			{
				ss << "4 ";
				ss << strings[3];
            ss << PADD_SPACE;
				ss << padding("Difference:",to_string((int)(__popcnt64(m.B) - __popcnt64(m.W))),PADD1);
            ss << PADD_SPACE;
            ss << padding("","",PADD2);
            break;
			}
			case 5:
			{
				ss << "5 ";
				ss << strings[4];
				ss << PADD_SPACE;
				ss << padding("DEPTH:",to_string(m.depth),PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			case 6:
			{
				ss << "6 ";
				ss << strings[5];
				ss << PADD_SPACE;
				ss << padding("NUM_TURN:",to_string(m.turn),PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			case 7:
			{
				ss << "7 ";
				ss << strings[6];
				ss << PADD_SPACE;
				ss << padding("ID:",to_string(m.ID),PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			case 8:
			{
				ss << "8 ";
				ss << strings[7];
				ss << PADD_SPACE;
				ss << padding("Ponder Board:",m.bIsPonderBoard ? "YES" : "NO",PADD1);
				ss << PADD_SPACE;
				ss << padding("","",PADD2);
				break;
			}
			default:
				break;
		}
		vsResult.push_back(ss.str());
		ss.str("");
		ss.clear();
	}

	return vsResult;
}

bool DebugFnc::chk_bb(const bit_manip::m256 m) noexcept{
	if (m.B & m.W) {
		return false;
	}
	return true;
}

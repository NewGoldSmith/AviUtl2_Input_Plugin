/**
 * @file RBFMaker.cpp
 * @brief RBFMakerメイン関数の実装
 * @author Gold Smith
 * @date 2025
 *
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */

#include "RBFMaker.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "../BitManip256/bit_manip256.h"
#include "../DebugFncM256/DebugFncm256.h"
#pragma comment(lib,  "../DebugFncM256/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFncM256-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../BitManip256/" _CRT_STRINGIZE($CONFIGURATION) "/BitManip256-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;
using namespace bit_manip;

#define INIT_B \
0b\
00000000\
00000000\
00000000\
00001000\
00010000\
00000000\
00000000\
00000000ULL
#define INIT_W \
0b\
00000000\
00000000\
00000000\
00010000\
00001000\
00000000\
00000000\
00000000ULL

#define INIT_M \
0b\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000ULL


#define B1 \
0b\
00000000\
00000000\
00000000\
00001000\
00010000\
00000000\
00000000\
00000000ULL
#define W1 \
0b\
00000000\
00000000\
00000000\
00010000\
00001000\
00000000\
00000000\
00000000ULL

#define M1 \
0b\
00000000\
00000000\
00000000\
00100000\
00000000\
00000000\
00000000\
00000000ULL

#define B2 \
0b\
00000000\
00000000\
00000000\
00111000\
00010000\
00000000\
00000000\
00000000ULL
#define W2 \
0b\
00000000\
00000000\
00000000\
00000000\
00001000\
00000000\
00000000\
00000000ULL

#define M2 \
0b\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000ULL

#define B3 \
0b\
00000000\
00000000\
00000000\
00111000\
00010000\
00000000\
00000000\
00000000ULL
#define W3 \
0b\
00000000\
00000000\
00000000\
00000000\
00001000\
00000000\
00000000\
00000000ULL

#define M3 \
0b\
00000000\
00000000\
00100000\
00000000\
00000000\
00000000\
00000000\
00000000ULL

#define B4 \
0b\
00000000\
00000000\
00000000\
00101000\
00010000\
00000000\
00000000\
00000000ULL
#define W4 \
0b\
00000000\
00000000\
00100000\
00010000\
00001000\
00000000\
00000000\
00000000ULL

#define M4 \
0b\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000\
00000000ULL

int main()
{
   m256 m{};
   m.bIsCurrentTurnWhite = true;
   const vector<m256> vm256 = {
      m256(_mm256_setr_epi64x(INIT_B,INIT_W,INIT_M,0))
      ,m256(_mm256_setr_epi64x(B1,W1,M1,0))
      ,m256(_mm256_setr_epi64x(B2,W2,M2,0))
      ,m256(_mm256_setr_epi64x(B3,W3,M3,m.m256i.m256i_u64[3]))
      ,m256(_mm256_setr_epi64x(B4,W4,M4,m.m256i.m256i_u64[3]))};

   for(const auto& m : vm256){
      DebugFnc::print_binary(m.m256i);
      cout << endl;
   }

   wstring filepathname(L"test.rbf");
   ::AVX::SaveM256ArrayW(filepathname, vm256);
   vector<m256> loadvm256;
   ::AVX::LoadM256ArrayW(filepathname, loadvm256);

   for(const auto& m : loadvm256){
      DebugFnc::print_binary(m.m256i);
      cout << endl;
   }

   if(vm256 != loadvm256){
      cout << "Failed." << endl;
      return -1;
   } else{
      cout << "It was a success." << endl;
      return 0;
   }
}



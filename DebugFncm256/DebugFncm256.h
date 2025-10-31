/**
 * @file DebugFncm256.h
 * @brief デバッグ関数の宣言m256用
 * @author Gold Smith
 * @date 2023 2025
 *
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#if !defined(_M_X64) && !defined(__amd64__)
#error "This code must be compiled in x64 configuration."
#endif
#ifndef __AVX2__
#error "This code requires AVX2."
#endif

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <vector>
#include "../DebugFnc/DebugFnc.h"
#include "../BitManip256/m256.h"
#pragma once
namespace DebugFnc {
//using namespace bit_manip;
   

//std::vector<bit_manip::m256> make_array_and_set_score(bit_manip::m256 m,uint8_t turn,const int score);

   std::string binary_to_string(const bit_manip::m128 b);
   void print_binary(const __m128i b);
   void dout_binary(const __m128i b);
   std::string binary_to_string(const __m256i b);
   void print_binary(const __m256i b);
   void dout_binary(const __m256i m);
   void dout_binary(const bit_manip::m256 m);
   void DebugPrintBB256Satus(const __m256i m);
   void DebugPrintBB256Satus(const bit_manip::m256 m);
//   std::string vector_to_string(const std::vector<m256>&vm);
//   void dout_status_v(const std::vector<m256>&vm);
//   void DebugPrintBBContainer(const std::vector<m256>& vm);


   std::string BB256toStatusString(const __m256i Inm);
   std::string padding(const std::string& left, const std::string& right, const int total_width);
   std::vector<std::string> BB256toStatusStrings(const __m256i Inm);
   template <typename T>
      requires bit_manip::IsM256Container<T>
   std::string Container256toStatusStringForPrint(const T vm){
      constexpr const char* PADD_SPACE = "  ";
      std::vector<std::vector<std::string>> vvsLinesUnits;

      auto createSubContainer = [](const T& src, size_t start, size_t count){
         T sub;
         // イテレータを開始位置まで進める
         auto startIt = std::next(src.begin(), start);            
         // 開始位置から count 進めた位置まで
         auto endIt = std::next(startIt, count);                 
         // 範囲を挿入
         sub.insert(sub.begin(), startIt, endIt);                
         return sub;
         };

      // 1つの行の文字列を構築する補助関数
      auto buildLineStrings = [&](const T& lineContainer){
         std::vector<std::vector<std::string>> vvs;
         std::vector<std::string> vsLines8;

         for (const bit_manip::m256& m : lineContainer){
            vvs.push_back(BB256toStatusStrings(m.m256i));
         }

         for (size_t Low = 0, LowSize = vvs[0].size(); Low < LowSize; ++Low){
            std::stringstream ss;
            for (size_t Col = 0, ColSize = lineContainer.size(); Col < ColSize; ++Col){
               ss << vvs[Col][Low];
               if (Col < ColSize - 1){
                  ss << PADD_SPACE;
               }
            }
            ss << std::endl;
            vsLines8.push_back(ss.str());
         }

         return vsLines8;
         };

      // メインロジック
      for (size_t h = 0; h < vm.size();){
         size_t remaining = vm.size() - h;
         size_t chunkSize = std::min<size_t>(3, remaining);

         T vm_sub = createSubContainer(vm, h, chunkSize);
         h += chunkSize;

         for (size_t i = 0; i < vm_sub.size(); i += chunkSize){
            T vm_1line = createSubContainer(vm_sub, i, std::min<size_t>(chunkSize, vm_sub.size() - i));
            vvsLinesUnits.push_back(buildLineStrings(vm_1line));
         }
      }

      // 最終的な文字列の結合
      std::stringstream ss;
      for (const auto& vvs : vvsLinesUnits){
         for (const auto& s : vvs){
            ss << s;
         }
      }

      return ss.str();
   }

   template <typename T>
      requires bit_manip::IsM256Container<T>
    void DebugPrintBBContainer(const T& vm){
      OutputDebugStringA(Container256toStatusStringForPrint(vm).c_str());
   }
   
   bool chk_bb(const bit_manip::m256 m)noexcept;
   /// @brief vectorに格納されたステータスの表示。
/// @brief デバッグ・プリント・コンテナ・ステータス
#define DPCS(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::DebugPrintBBContainer(b);}
/// @brief デバッグ・プリント・ステータス
#define DPS(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::DebugPrintBB256Satus(b);}
/// @brief プリント・ステータス
#define PS(b){std::cout << std::string(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");std::cout << DebugFnc::BB256toStatusString(b).c_str();}

#ifdef _DEBUG
/// @brief デバッグ・プリント・コンテナ・ストラクト・ステータス・デバッグ時のみ
#define DPCSD(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::DebugPrintBBContainer(b);}
/// @brief デバッグ・プリント・ステータス・デバッグ時のみ
#define DPSD(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::DebugPrintBBContainer(b);}
//#define DOSV(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::dout_status_v(b);}
/// @brief チェック・ビットボード・デバッグ時のみ
#define CHKBBD(b) {DebugFnc::chk_bb(b)?DPAD("CHKBBD:妥当です。"):DPAD("CHKBBD:不適切です。");MessageBeep(0xFFFFFFFF);}}
#else
#define DPCSD(b) __noop
#define DPSD(b) __noop
#define CHKBBD(b) __noop
#endif // _DEBUG

}
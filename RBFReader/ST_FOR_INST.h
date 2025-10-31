/**
 * @file ST_FOR_INST.h
 * @brief ST_FOR_INSTの宣言。ＫＥＮくん様のHANDLEを拡張
 * @author ＫＥＮくん
 * @author web site <https: //spring-fragrance.mints.ne.jp/aviutl/>
 * @date 2025 Original
 * @author Gold Smith
 * @date 2025 modified
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#pragma once
#define NOMINMAX
#include <windows.h>
#include <Vfw.h>
#include <string>
#include <map>
#include "../BitManip256/m256.h"
#include "../DebugFnc/DebugFnc.h"
#include "../CommonLib/CommonLib.h"

#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../BitManip256/" _CRT_STRINGIZE($CONFIGURATION) "/BitManip256-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

/// @brief インスタンスのためのデータ保持構造体
struct STRUCTURE_FOR_INSTANCE{
public:
   int				flag{};
   HWND hParent{};
   PAVIFILE		pfile{};
   PAVISTREAM		pvideo{}, paudio{};
   AVIFILEINFO		avi_file_info{};
   AVISTREAMINFO	video_stream_info{}, audio_stream_info{};
   //void* videoformat{};
   //LONG  videoformatsize{};
   void* audioformat{};
   LONG  audioformatsize{};
   std::wstring FilePathNameW{};
   std::wstring tmpAVIPathName{};
   std::vector<bit_manip::m256> vrbf{};

   /// @brief func_info_getで保持必要?
   BITMAPINFOHEADER bmih{};
   HMODULE hModule;
   STRUCTURE_FOR_INSTANCE(HMODULE h);
   ~STRUCTURE_FOR_INSTANCE();
};

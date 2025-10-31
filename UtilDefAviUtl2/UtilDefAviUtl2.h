/**
 * @file UtilDefAviUtl2.h
 * @brief UtilDefAviUtl2専用の定義及び、関数の宣言
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#pragma once
#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <map>

/// @brief AviUtl2用のユーティリティ
namespace UtilDefAviUtl2{


   /// @brief m256フォーマットの盤面をDIBに描画する。
   /// @param InBoard ボード盤面
   /// @param hInBmp 描画するDIBハンドル。
   /// @return 正常に描画が完了したらTRUE。それ以外FALSE。
   BOOL DrawBoardDIB(__m256i InBoard, HBITMAP hInBmp);

   /// @brief よく出てくるセクション名を定義
   static constexpr WCHAR STR_DEFAULT_INITIAL_FILE_SETTINGS[] = L"DEFAULT_INITIAL_FILE_SETTINGS";

   /// @brief INIファイルが作成されるときの
   /// STR_DEFAULT_INITIAL_FILE_SETTINGSセクションの初期設定
   static const std::map<std::wstring, std::wstring> default_initial_setteings = {
   {L"N_DOTS",  L"512"}
   ,{L"RATE",L"1"}
   ,{L"SCALE",L"1"}
   ,{L"FILE_PATH_NAME",L""}
   };

   /// @brief ファイルネームセクションを読む際に無視するセクションリスト。
   static const std::vector<std::wstring> IgnoreList =
   {
      {L"COMMON"}
      ,{L"DEFAULT_SETTINGS"}
      ,{L"META"}
      ,{L"DEFAULT_INITIAL_FILE_SETTINGS"} };

   BOOL MakeIniFile(HMODULE hModule, int BuildNumber);

   double CalcFPS(const double dRate, const double dScale);
   std::string CalcFPStoStringA(const double dRate, const double dScale);
   std::wstring CalcFPStoStringW(const double dRate, const double dScale);
}
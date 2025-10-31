/**
 * @file UtilDefAviUtl2.cpp
 * @brief UtilDefAviUtl2の実装
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#include "UtilDefAviUtl2.h"
#include <algorithm>
#include <memory>
#include <format>
#include "../BitManip256/m256.h"
#include "..\CommonLib\CommonLib.h"
#include "..\DebugFnc\DebugFnc.h"
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;
using namespace bit_manip;
using namespace CommonLib;

BOOL UtilDefAviUtl2::DrawBoardDIB(__m256i InBoard, HBITMAP hInBmp){
   const m256& board = InBoard;
   constexpr int N = 8; // 8x8
   if(!hInBmp){
      MessageBoxA(NULL,DPT("hInBmp is NULL").c_str(),"Error",MB_ICONEXCLAMATION);
      return FALSE;
   }

   // 一時的なメモリDCを作成
   unique_ptr<remove_pointer_t<HDC>, void(*)(HDC hdc)> hdcScreen
   { GetDC(NULL),[](HDC hdc){ReleaseDC(NULL,hdc); } };
   unique_ptr<remove_pointer_t< HDC>, decltype(DeleteDC)*> hdcMem
   (CreateCompatibleDC(hdcScreen.get()), DeleteDC);
   if(!hdcMem){
      MessageBoxA(NULL,DPT("CreateCompatibleDC Error").c_str(),"Error",MB_ICONEXCLAMATION);
      return FALSE;
   }

   GdiSelectAndRestorer<HBITMAP> uBmpRestorer(hdcMem.get(), hInBmp);

   // ビットマップサイズ取得
   BITMAP bmp = {};

   if(!GetObject(hInBmp, sizeof(BITMAP), &bmp)){
      MessageBoxA(NULL,DPT("GetObject Error").c_str(),"Error",MB_ICONEXCLAMATION);
      return FALSE;
   }

   int pixel = min(bmp.bmWidth, bmp.bmHeight); // ビットマップの描画可能領域

   double marginRatio = 0.5; // ← 1/2セル分の余白
   int InLineWidth{ std::max(1, pixel / 256) };

   // 1マスのサイズ
   // 総マス数 + 両端の余白（上下左右に marginRatio 分ずつ）
   double totalUnits = N + 2 * marginRatio;
   int cell = static_cast<int>(pixel / totalUnits);

   int margin = (int)InLineWidth; // 枠線分

   // 外枠の外側の余白
   int extraMargin = cell / 2; // ← 1/4マス分の余白

   // 左上基準
   int origin_x = static_cast<int>(cell * marginRatio);
   int origin_y = static_cast<int>(cell * marginRatio);

   int boardSize = cell * N; // ← 盤面サイズ（余白を除いた実描画領域）

      // 背景（緑）
   {
      COLORREF colorMaroon = RGB(128, 0, 0); // エンジ色

      // 全体領域
      RECT rcFull = {
          origin_x - extraMargin,
          origin_y - extraMargin,
          origin_x + boardSize + extraMargin,
          origin_y + boardSize + extraMargin
      };

      // 盤面領域
      RECT rcBoard = {
          origin_x,
          origin_y,
          origin_x + boardSize,
          origin_y + boardSize
      };

      // エンジ色で余白を塗る
      {
         unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrushMaroon
         { CreateSolidBrush(colorMaroon),DeleteObject };
         FillRect(hdcMem.get(), &rcFull, hBrushMaroon.get());
      }

      // 緑で盤面を塗る
      {
         unique_ptr<remove_pointer_t<HBRUSH>,decltype(DeleteObject)*> hBrushGreen
         { CreateSolidBrush(RGB(0, 128, 0)),DeleteObject };
         FillRect(hdcMem.get(), &rcBoard, hBrushGreen.get());
      }
   }

   //ラベル
   {
      GdiSelectAndRestorerF<int, SetBkMode>BkModeRestorer(hdcMem.get(), TRANSPARENT);
      GdiSelectAndRestorerF<COLORREF, SetTextColor>SetTextColorRestorer
      (hdcMem.get(), RGB(255, 255, 255));// 白文字（エンジ背景に映える）

      unique_ptr<remove_pointer_t<HFONT>, decltype(DeleteObject)*> hFontLabel
      { CreateFontW(
         cell * 2 / 3, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
         DEFAULT_PITCH | FF_DONTCARE, L"Arial"
      ),DeleteObject };

      GdiSelectAndRestorer<HFONT> FontRestorer(hdcMem.get(), hFontLabel.get());

      int quarter_x = cell / 4;
      int quarter_y = cell / 4;

      for(int x = 0; x < N; ++x){
         wchar_t label = L'A' + x;
         wstring str(1, label);

         int cx = origin_x + x * cell;
         SIZE sz = {};
         GetTextExtentPoint32W(hdcMem.get(), str.c_str(), 1, &sz);

         int tx = cx + (cell - sz.cx) / 2;
         int ty = quarter_y - cell / 2 + (cell - sz.cy) / 2;

         TextOutW(hdcMem.get(), tx, ty, str.c_str(), 1);
      }

      for(int y = 0; y < N; ++y){
         wchar_t label = L'1' + y;
         wstring str(1, label);

         int cy = origin_y + y * cell;
         SIZE sz = {};
         GetTextExtentPoint32W(hdcMem.get(), str.c_str(), 1, &sz);

         int tx = quarter_x - cell / 2 + (cell - sz.cx) / 2;
         int ty = cy + (cell - sz.cy) / 2;

         TextOutW(hdcMem.get(), tx, ty, str.c_str(), 1);
      }
   }

   // 枠線（黒、2倍太さ）
   {
      unique_ptr<remove_pointer_t<HPEN>, decltype(DeleteObject)*> hPenFrame
      { CreatePen(PS_SOLID, static_cast<int>(InLineWidth * 4), RGB(0, 0, 0))
         ,DeleteObject };
      GdiSelectAndRestorer<HPEN> PenRestorer(hdcMem.get(), hPenFrame.get());

      // 中抜き用ブラシ
      HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
      GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hNullBrush);

      // 外枠（盤面サイズに合わせて描画）
      int frame = margin / 2;
      Rectangle(
         hdcMem.get()
         , origin_x + frame
         , origin_y + frame
         , origin_x + boardSize - frame
         , origin_y + boardSize - frame
      );
   }

   // 盤の線（黒、In_line_dot）
   {
      unique_ptr<remove_pointer_t<HPEN>, decltype(DeleteObject)*> hPen
      { CreatePen(PS_SOLID, static_cast<int>(InLineWidth), RGB(0, 0, 0)),DeleteObject };
      GdiSelectAndRestorer<HPEN> PenRestorer(hdcMem.get(), hPen.get());

      for(int i = 0; i <= N; ++i){
         int pos = origin_x + margin + i * cell;
         // 横線
         MoveToEx(hdcMem.get(), origin_x + margin, pos, NULL);
         LineTo(hdcMem.get(), origin_x + margin + N * cell, pos);
         // 縦線
         MoveToEx(hdcMem.get(), pos, origin_y + margin, NULL);
         LineTo(hdcMem.get(), pos, origin_y + margin + N * cell);
      }
   }

   // 線の交点に黒点（直径2*InLineWidth）
   {
      unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hDotBrush
      { CreateSolidBrush(RGB(0, 0, 0)),DeleteObject };
      GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hDotBrush.get());
      int dot_r = static_cast<int>(InLineWidth * 4);

      bool abHasDot[N + 1][N + 1] = {};
      abHasDot[2][2] = true;
      abHasDot[6][6] = true;
      abHasDot[2][6] = true;
      abHasDot[6][2] = true;
      for(int i = 0; i <= N; ++i){
         for(int j = 0; j <= N; ++j){
            if(!abHasDot[i][j])
               continue;

            int x = origin_x + margin + i * cell;
            int y = origin_y + margin + j * cell;

            Ellipse(
               hdcMem.get()
               , x - dot_r
               , y - dot_r
               , x + dot_r
               , y + dot_r
            );
         }
      }
   }

   // 石の描画
   {
      unique_ptr<remove_pointer_t<HPEN>, decltype(DeleteObject)*> hPen
      { CreatePen(PS_SOLID, InLineWidth, RGB(0, 0, 0)), DeleteObject };
      GdiSelectAndRestorer<HPEN> PenRestorer(hdcMem.get(), hPen.get());

      for(int y = 0; y < N; ++y){
         for(int x = 0; x < N; ++x){
            int idx = y * N + x;
            int cx = origin_x + margin + x * cell;
            int cy = origin_y + margin + y * cell;
            int pad = cell / 8 + (int)InLineWidth;
            int left = cx + pad;
            int top = cy + pad;
            int right = cx + cell - pad;
            int bottom = cy + cell - pad;

            bool isBlack = (0x8000'0000'0000'0000ULL >> idx) & board.B;
            bool isWhite = (0x8000'0000'0000'0000ULL >> idx) & board.W;

            // 黒石
            if(isBlack){
               unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrush
               { CreateSolidBrush(RGB(0, 0, 0)),DeleteObject };
               GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hBrush.get());
               Ellipse(hdcMem.get(), left, top, right, bottom);
            }
            // 白石
            if(isWhite){
               unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrush
               { CreateSolidBrush(RGB(255, 255, 255)),DeleteObject };
               GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hBrush.get());
               Ellipse(hdcMem.get(), left, top, right, bottom);
            }
         }
      }
   }

   // ムーブ（最上位に描画）
   {
      for(int y = 0; y < N; ++y){
         for(int x = 0; x < N; ++x){

            int idx = y * N + x;
            if((0x8000'0000'0000'0000ULL >> idx) & board.M){
               int cx = origin_x + margin + x * cell;
               int cy = origin_y + margin + y * cell;
               int pad = cell / 8 + static_cast<int>(InLineWidth);
               int left = cx + pad;
               int top = cy + pad;
               int right = cx + cell - pad;
               int bottom = cy + cell - pad;

               // 手番による色設定
               COLORREF fillColor, mTextColor;
               if(board.bIsCurrentTurnWhite){
                  fillColor = RGB(255, 255, 255); // 白石
                  mTextColor = RGB(0, 0, 0);       // 黒文字
               } else{
                  fillColor = RGB(0, 0, 0);       // 黒石
                  mTextColor = RGB(255, 255, 255); // 白文字
               }

               // 円描画（塗りつぶしのみ）
               {
                  // 塗りつぶしブラシ（石の色）
                  unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrushFill
                  { CreateSolidBrush(fillColor),DeleteObject };
                  GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hBrushFill.get());
                  
                  Ellipse(hdcMem.get(), left, top, right, bottom);
               }

               // "M"文字描画（フチなし）
               {
                  GdiSelectAndRestorerF<int, SetBkMode>BkModeRestorer(hdcMem.get(), TRANSPARENT);

                  std::wstring mstr = L"M";
                  int fontSize = cell / 2;
                  unique_ptr<remove_pointer_t<HFONT>, decltype(DeleteObject)*> hFont
                  { CreateFontW(
                     fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_DONTCARE, L"Arial"),DeleteObject };

                  GdiSelectAndRestorer FontRestorer(hdcMem.get(),hFont.get());
 
                  SIZE sz = {};
                  GetTextExtentPoint32W(hdcMem.get(), mstr.c_str(), 1, &sz);
                  int tx = cx + (cell - sz.cx) / 2;
                  int ty = cy + (cell - sz.cy) / 2;

                  GdiSelectAndRestorerF<COLORREF, SetTextColor>SetTextColorRestorer
                  (hdcMem.get(), mTextColor);
                  TextOutW(hdcMem.get(), tx, ty, mstr.c_str(), 1);
               }
            }
         }
      }
   }
   return TRUE;
}

BOOL UtilDefAviUtl2::MakeIniFile(HMODULE hModule, int BuildNumber){
      /// @brief 共通設定の初期値を書き込み
   {
      const map<wstring, wstring> common_default_cfg = {
         /// 現在共通初期値項目なし
      };
      /// @brief 現在項目なしなのでこのセクションは作られない。
      if(!WriteMapToIniW(
         L"COMMON"
         , common_default_cfg
         , MakeIniPathNameW(hModule))){
         DPT("COMMON section write error.");
         return FALSE;
      }
   }

   /// @brief ファイルごとの動画のデフォルト設定書き込み
   {
      if(!WriteMapToIniW(
         STR_DEFAULT_INITIAL_FILE_SETTINGS
         , default_initial_setteings
         , MakeIniPathNameW(hModule))){
         DPT("DEFAULT_INITIAL_FILE_SETTINGS section write error.");
         return FALSE;
      }
   }
   return TRUE;
}

double UtilDefAviUtl2::CalcFPS(const double dRate, const double dScale){
   if(dScale <= 0){
      return 0.0;
   }
   return dRate / dScale;
}

string UtilDefAviUtl2::CalcFPStoStringA(const double dRate, const double dScale){
   double drFPS = round(CalcFPS(dRate, dScale) * 100.0) / 100.0;
   return format("{:.2f}", drFPS);
}

std::wstring UtilDefAviUtl2::CalcFPStoStringW(const double dRate, const double dScale){
   double drFPS = round(CalcFPS(dRate, dScale) * 100.0) / 100.0;
   return format(L"{:.2f}", drFPS);
}


/**
 * @file UtilDefAviUtl2.cpp
 * @brief UtilDefAviUtl2�̎���
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * ���̃t�@�C�����̂��ׂẴR�[�h�́A���ɖ��L����Ă��Ȃ�����AMIT���C�Z���X�ɏ]���܂��B
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

   // �ꎞ�I�ȃ�����DC���쐬
   unique_ptr<remove_pointer_t<HDC>, void(*)(HDC hdc)> hdcScreen
   { GetDC(NULL),[](HDC hdc){ReleaseDC(NULL,hdc); } };
   unique_ptr<remove_pointer_t< HDC>, decltype(DeleteDC)*> hdcMem
   (CreateCompatibleDC(hdcScreen.get()), DeleteDC);
   if(!hdcMem){
      MessageBoxA(NULL,DPT("CreateCompatibleDC Error").c_str(),"Error",MB_ICONEXCLAMATION);
      return FALSE;
   }

   GdiSelectAndRestorer<HBITMAP> uBmpRestorer(hdcMem.get(), hInBmp);

   // �r�b�g�}�b�v�T�C�Y�擾
   BITMAP bmp = {};

   if(!GetObject(hInBmp, sizeof(BITMAP), &bmp)){
      MessageBoxA(NULL,DPT("GetObject Error").c_str(),"Error",MB_ICONEXCLAMATION);
      return FALSE;
   }

   int pixel = min(bmp.bmWidth, bmp.bmHeight); // �r�b�g�}�b�v�̕`��\�̈�

   double marginRatio = 0.5; // �� 1/2�Z�����̗]��
   int InLineWidth{ std::max(1, pixel / 256) };

   // 1�}�X�̃T�C�Y
   // ���}�X�� + ���[�̗]���i�㉺���E�� marginRatio �����j
   double totalUnits = N + 2 * marginRatio;
   int cell = static_cast<int>(pixel / totalUnits);

   int margin = (int)InLineWidth; // �g����

   // �O�g�̊O���̗]��
   int extraMargin = cell / 2; // �� 1/4�}�X���̗]��

   // ����
   int origin_x = static_cast<int>(cell * marginRatio);
   int origin_y = static_cast<int>(cell * marginRatio);

   int boardSize = cell * N; // �� �ՖʃT�C�Y�i�]�������������`��̈�j

      // �w�i�i�΁j
   {
      COLORREF colorMaroon = RGB(128, 0, 0); // �G���W�F

      // �S�̗̈�
      RECT rcFull = {
          origin_x - extraMargin,
          origin_y - extraMargin,
          origin_x + boardSize + extraMargin,
          origin_y + boardSize + extraMargin
      };

      // �Ֆʗ̈�
      RECT rcBoard = {
          origin_x,
          origin_y,
          origin_x + boardSize,
          origin_y + boardSize
      };

      // �G���W�F�ŗ]����h��
      {
         unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrushMaroon
         { CreateSolidBrush(colorMaroon),DeleteObject };
         FillRect(hdcMem.get(), &rcFull, hBrushMaroon.get());
      }

      // �΂ŔՖʂ�h��
      {
         unique_ptr<remove_pointer_t<HBRUSH>,decltype(DeleteObject)*> hBrushGreen
         { CreateSolidBrush(RGB(0, 128, 0)),DeleteObject };
         FillRect(hdcMem.get(), &rcBoard, hBrushGreen.get());
      }
   }

   //���x��
   {
      GdiSelectAndRestorerF<int, SetBkMode>BkModeRestorer(hdcMem.get(), TRANSPARENT);
      GdiSelectAndRestorerF<COLORREF, SetTextColor>SetTextColorRestorer
      (hdcMem.get(), RGB(255, 255, 255));// �������i�G���W�w�i�ɉf����j

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

   // �g���i���A2�{�����j
   {
      unique_ptr<remove_pointer_t<HPEN>, decltype(DeleteObject)*> hPenFrame
      { CreatePen(PS_SOLID, static_cast<int>(InLineWidth * 4), RGB(0, 0, 0))
         ,DeleteObject };
      GdiSelectAndRestorer<HPEN> PenRestorer(hdcMem.get(), hPenFrame.get());

      // �������p�u���V
      HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
      GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hNullBrush);

      // �O�g�i�ՖʃT�C�Y�ɍ��킹�ĕ`��j
      int frame = margin / 2;
      Rectangle(
         hdcMem.get()
         , origin_x + frame
         , origin_y + frame
         , origin_x + boardSize - frame
         , origin_y + boardSize - frame
      );
   }

   // �Ղ̐��i���AIn_line_dot�j
   {
      unique_ptr<remove_pointer_t<HPEN>, decltype(DeleteObject)*> hPen
      { CreatePen(PS_SOLID, static_cast<int>(InLineWidth), RGB(0, 0, 0)),DeleteObject };
      GdiSelectAndRestorer<HPEN> PenRestorer(hdcMem.get(), hPen.get());

      for(int i = 0; i <= N; ++i){
         int pos = origin_x + margin + i * cell;
         // ����
         MoveToEx(hdcMem.get(), origin_x + margin, pos, NULL);
         LineTo(hdcMem.get(), origin_x + margin + N * cell, pos);
         // �c��
         MoveToEx(hdcMem.get(), pos, origin_y + margin, NULL);
         LineTo(hdcMem.get(), pos, origin_y + margin + N * cell);
      }
   }

   // ���̌�_�ɍ��_�i���a2*InLineWidth�j
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

   // �΂̕`��
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

            // ����
            if(isBlack){
               unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrush
               { CreateSolidBrush(RGB(0, 0, 0)),DeleteObject };
               GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hBrush.get());
               Ellipse(hdcMem.get(), left, top, right, bottom);
            }
            // ����
            if(isWhite){
               unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrush
               { CreateSolidBrush(RGB(255, 255, 255)),DeleteObject };
               GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hBrush.get());
               Ellipse(hdcMem.get(), left, top, right, bottom);
            }
         }
      }
   }

   // ���[�u�i�ŏ�ʂɕ`��j
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

               // ��Ԃɂ��F�ݒ�
               COLORREF fillColor, mTextColor;
               if(board.bIsCurrentTurnWhite){
                  fillColor = RGB(255, 255, 255); // ����
                  mTextColor = RGB(0, 0, 0);       // ������
               } else{
                  fillColor = RGB(0, 0, 0);       // ����
                  mTextColor = RGB(255, 255, 255); // ������
               }

               // �~�`��i�h��Ԃ��̂݁j
               {
                  // �h��Ԃ��u���V�i�΂̐F�j
                  unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrushFill
                  { CreateSolidBrush(fillColor),DeleteObject };
                  GdiSelectAndRestorer<HBRUSH> BrushRestorer(hdcMem.get(), hBrushFill.get());
                  
                  Ellipse(hdcMem.get(), left, top, right, bottom);
               }

               // "M"�����`��i�t�`�Ȃ��j
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
      /// @brief ���ʐݒ�̏����l����������
   {
      const map<wstring, wstring> common_default_cfg = {
         /// ���݋��ʏ����l���ڂȂ�
      };
      /// @brief ���ݍ��ڂȂ��Ȃ̂ł��̃Z�N�V�����͍���Ȃ��B
      if(!WriteMapToIniW(
         L"COMMON"
         , common_default_cfg
         , MakeIniPathNameW(hModule))){
         DPT("COMMON section write error.");
         return FALSE;
      }
   }

   /// @brief �t�@�C�����Ƃ̓���̃f�t�H���g�ݒ菑������
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


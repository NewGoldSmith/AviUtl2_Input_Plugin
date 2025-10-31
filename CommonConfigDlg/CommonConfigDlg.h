/**
 * @file CommonConfigDlg.h
 * @brief CommonConfigDlgの宣言
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
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <format>
#include <map>
#include "../SliderEditControl/SliderEditControl.h"
#include "..\CommonConfigDlg\FileConfigDlg.h"

class CommonConfigDlg{
   const enum LIST_COL_INDEX :UINT
   { COL_CONFIG, COL_FILENAME, COL_N_DOTS, COL_RATE, COL_SCALE, COL_FPS, COUNT };
   const std::map<UINT, std::wstring> ColString = {
      {COL_CONFIG,L"CONFIG"}
      ,{COL_FILENAME,L"FILE_PATH_NAME"}
      ,{COL_N_DOTS,L"N_DOTS"}
      ,{COL_RATE,L"RATE"}
      ,{COL_SCALE,L"SCALE"} };
   static constexpr UINT DOT_RANGE_LOW = 1;
   static constexpr UINT DOT_RANGE_HIGH = 1024;
   static constexpr UINT RATE_RANGE_LOW = 1;
   static constexpr UINT RATE_RANGE_HIGH = 30000;
   static constexpr UINT SCALE_RANGE_LOW = 1;
   static constexpr UINT SCALE_RANGE_HIGH = 1001;
   static constexpr WCHAR SLIDER_EDIT_CLASS_NAME[] = L"SliderEditClass";
   static constexpr int SECTION_NAME_BUFFER_SIZE = 1892;

   HINSTANCE hInst;
   SUBCLASSPROC pfnSubClass;
   HWND hWnd{};
   HMODULE hTargetModule{};
   BOOL bUpdate{};
   SliderEditControl InsSEDot;
   SliderEditControl InsSERate;
   SliderEditControl InsSEScale;
   FileConfigDialog InsFileCfg;
public:
   CommonConfigDlg();
   virtual ~CommonConfigDlg() = default;
   INT_PTR DialogBoxParamW(HWND  hWndParent, HMODULE hTarget);
private:
   BOOL LunchFileConfigDlg(HWND hWndParent
                           , HWND hWndListView
                           , int iItem);
   std::map<std::wstring, std::wstring>
      GetOneLineItemOfListViewW(HWND hWndListView, int iItem)const;
   BOOL UpdateOneLineItemOfListViewW(
      HWND hWndListView
      , int iItem
      , const std::map<std::wstring, std::wstring> mapLine);
};


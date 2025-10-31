/**
 * @file FileConfigDlg.h
 * @brief FileConfigDlgの宣言
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
#include <windowsx.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <cmath>
#include <iomanip>
#include <uxtheme.h>
#include "../BitManip256/m256.h"
#include "../SliderEditControl/SliderEditControl.h"
#include "../PreviewWindow/PreviewWindow.h"

class FileConfigDialog{

   static constexpr UINT DOT_RANGE_LOW = 1;
   static constexpr UINT DOT_RANGE_HIGH = 1024;
   static constexpr UINT RATE_RANGE_LOW = 1;
   static constexpr UINT RATE_RANGE_HIGH = 30000;
   static constexpr UINT SCALE_RANGE_LOW = 1;
   static constexpr UINT SCALE_RANGE_HIGH = 1001;
   static constexpr WCHAR CUSTOM_CONTROL_CLASS_NAME[] = L"SliderEditClass";
public:
   const std::set<std::wstring> requiredKeys {
      L"FILE_PATH_NAME"
      ,L"N_DOTS"
      , L"RATE"
      , L"SCALE"
   };
private:
   HINSTANCE hInst;
   HWND hWnd{};
   int IDDlg{};
   BOOL UpdateConfig{};
   SUBCLASSPROC pfnSubClass;
   SliderEditControl EditNumOfDots;
   SliderEditControl EditRate;
   SliderEditControl EditScale;
   PreviewWindow PrevWnd;
   /// @brief リバーシバイナリーフォーマット
   std::vector<bit_manip::m256> vRBF;
   int vRBFIndex{}; 
   std::map<std::wstring, std::wstring> Config;

public:
   FileConfigDialog();
   FileConfigDialog(const FileConfigDialog&) = delete;
   FileConfigDialog(FileConfigDialog&&) = delete;
   FileConfigDialog& operator =(const FileConfigDialog&) = delete;
   FileConfigDialog& operator =(FileConfigDialog&&) = delete;
   virtual ~FileConfigDialog();
   /// @brief モーダルConfigDialogを表示する。
   /// この後、GetConfigにより設定された設定値を得る。
   /// @param hWndParent 親ウインドウハンドル。
   /// @param InIDDlg ダイアログID
   /// @param InCfg ダイアログにセットする設定値が格納されたmap
   /// @return 1:成功、
   /// 0:親ウインドウハンドルが無効。
   /// -2:map設定値の要素が足りていない。
   /// -1:その他の理由で失敗。GetLastErrorでエラー値が取得可能。
   [[nodiscard]]INT_PTR DialogBoxParamW(
      HWND  hWndParent
      ,const std::map<std::wstring, std::wstring>& InCfg);
public:
   /// @brief ダイアログ内部のConfigを得る。
   /// @return std::map<std::string, std::string>形式のConfig。
   std::map<std::wstring, std::wstring> GetConfigW();
};


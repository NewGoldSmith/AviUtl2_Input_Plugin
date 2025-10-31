/**
 * @file SliderEditControl.h
 * @brief SliderEditControlの宣言
 * @date 2025
 * @author Gold Smith
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <CommCtrl.h>
#include <string>

class SliderEditControl{
   static constexpr UINT_PTR IDC_EDIT = 1000ULL;
   static constexpr UINT_PTR IDC_BUTTON_MINUS = 1001ULL;
   static constexpr UINT_PTR IDC_BUTTON_PLUS = 1002ULL;
   static constexpr INT_PTR WM_ADJUSTVALUE = WM_APP + 1;
   /// @brief Autorepeat
   static constexpr UINT_PTR TIMER_ID_AUTOREPEAT = 1001ULL;
   static constexpr UINT_PTR AUTOREPEAT_INITIAL_DELAY = 1000ULL;
   static constexpr UINT_PTR AUTOREPEAT_INTERVAL = 100;
   /// @brief インナーボタンクラスサフィックス
   static constexpr WCHAR INNER_BTN_CLS_STR[] = L"InnerButon";
   /// @brief バリューレッドバーティカルラインの太さ
   static constexpr INT VALUE_LINE_WIDTH = 4;

   HINSTANCE hInst{};
   ATOM atmCtmCtl{};
   ATOM atmCtmBtn{};
   std::wstring ClassName{};
   HWND hDlgParent{};
   int min{INT_MIN}, max{INT_MAX};
   HFONT hFontCtm{};
   HWND hWnd{};
   HWND hEdit{};
   HWND hMinusButton{};
   HWND hPlusButton{};
   SUBCLASSPROC pfnSliderPrc{};
   SUBCLASSPROC pfnEditPrc{};
   SUBCLASSPROC pfnButtonPrc{};
   bool initialized = false;
   int nIDDlgItem{};

   TRACKMOUSEEVENT tme{};
   BOOL Hovered = false;
   POINT ptLastPos{-1,-1};
   POINT ptDowned{-1,-1};
   BOOL DownedAndMoved{ FALSE };
   static constexpr int MoveBacklash = 5;

   RECT rcAll{};
   RECT rcMinus{};
   RECT rcPlus{};
   RECT rcSlider{};

   BOOL isInitialDelayPassed = FALSE;

   BOOL IsEditShowState{ FALSE };
   BOOL SwitchEditShowState(BOOL InSwCtl);
   void UpdateLayoutRects();
   void UpdateLayoutRects(LPARAM lParam);
   HFONT GetFontAdjusted(HWND hWnd, const RECT* prc)const;
   int AdjustValueRange(int InValue);

public:
   SliderEditControl() = delete;
   /// @brief ※重要　コンストラクタ　下記の設定を正確にする必要がある。
   /// @param hInst リソースのあるモジュールハンドル
   /// @param CustomClassName カスタムコントロールに設定した「クラス名」と同じ文字列。
   /// カスタムコントロールの文字列に「""」は要らない。名前が同じか設定していなければ
   /// ダイアログの起動は失敗する。
   SliderEditControl(HINSTANCE hInst,LPCWSTR CustomClassName);
   ~SliderEditControl();
   BOOL SetWindowSubclass(HWND hParentDlg,int ctlID);
   BOOL SetValue(int Inval);
   BOOL GetValue(int& val);
   BOOL SetRange(int min, int max);
};





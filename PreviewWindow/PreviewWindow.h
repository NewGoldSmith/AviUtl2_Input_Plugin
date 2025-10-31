/**
 * @file PreviewWindow.h
 * @brief PreviewWindow�̐錾
 * @date 2025
 * @author Gold Smith
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <CommCtrl.h>
#include <gdiplus.h>


class PreviewWindow{
   static constexpr INT INIT_WINDOW_WIDTH = 400;
   static constexpr INT INIT_WINDOW_HEIGHT = 300;
   static constexpr UINT_PTR VIEW_SCALE_TIMER_ID = 1;
   static constexpr UINT TIMER_SCALE_TIMER_ELAPSE = 4000;
   static constexpr double SCALE_STEP = 0.1;
public:
   static constexpr UINT WM_DRAWPREVIEW = WM_APP + 1;
private:
   static constexpr WCHAR CLASS_NAME[] = L"PreviewWndCls";

   HINSTANCE hInst{};
   HWND hWnd{};
   HWND hParentWnd{};
   HBITMAP hBitmap{};
   ATOM atm{};
   double scale = 1.0;
   BOOL IsModal{};
   ULONGLONG scaleTextTimestamp = 0;
   SUBCLASSPROC pfnSubClsPrevWndPrc;
public:
   PreviewWindow() = delete;
   PreviewWindow(HINSTANCE hInst);
   ~PreviewWindow();
   HWND CreatePrevWindow(HWND hParentWnd);
   INT_PTR DoModal(HWND hParentWnd, HBITMAP hBmp);
   void Close();
   BOOL DrawBitmap(HBITMAP hbmp);
   BOOL IsWindow() const;
   double SetScale(double newScale);
   double GetScale() const;
};


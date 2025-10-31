/**
 * @file MainDialog.h
 * @brief MainDialogの宣言
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
#include <memory>
#include "../SliderEditControl/SliderEditControl.h"
#include "../PreviewWindow/PreviewWindow.h"
#include "../BitManip256/m256.h"
#include "../DummyDlg/DummyDlg.h"



class MainDialog{
   static constexpr int DEFAULT_BITMAP_WIDTH = 400;
   static constexpr int DEFAULT_BITMAP_HEIGHT = 400;
   HINSTANCE hInst;
   SUBCLASSPROC pfnSubClass;
   SliderEditControl InstSliderEditControl;
   SliderEditControl InstSliderEditControl2;
   PreviewWindow InstPreviewWindow;
   DummyDlg InstDummyDlg;
   HWND hWnd{};
   std::unique_ptr<std::remove_pointer_t<HBITMAP>,decltype(DeleteObject)*>hBitmap;
   //HBITMAP hBitmap{};
public:
   MainDialog() = delete;
   MainDialog(HINSTANCE hInst);
   ~MainDialog() = default;
   INT_PTR DialogBoxParamW(HWND  hWndParent);
};


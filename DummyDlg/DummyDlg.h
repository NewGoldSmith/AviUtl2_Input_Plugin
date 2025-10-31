/**
 * @file DummyDlg.h
 * @brief DummyDlgの宣言
 * @date 2025
 * @author Gold Smith
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 * @detail dllファイルが成果物
 * インポートライブラリでアクセスすることを想定している
 */
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <CommCtrl.h>

class  __declspec(dllexport) DummyDlg{
   HINSTANCE hInst;
   SUBCLASSPROC pfnSubClass;
   HWND hWnd{};
public:
   DummyDlg();
   ~DummyDlg() = default;
   INT_PTR DialogBoxParamW(HWND  hWndParent);
};


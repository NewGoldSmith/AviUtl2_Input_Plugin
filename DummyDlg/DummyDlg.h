/**
 * @file DummyDlg.h
 * @brief DummyDlg�̐錾
 * @date 2025
 * @author Gold Smith
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 * @detail dll�t�@�C�������ʕ�
 * �C���|�[�g���C�u�����ŃA�N�Z�X���邱�Ƃ�z�肵�Ă���
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


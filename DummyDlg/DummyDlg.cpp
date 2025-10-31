/**
 * @file DummyDlg.cpp
 * @brief DummyDlgの実装
 * @date 2025
 * @author Gold Smith
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 * @detail dllファイルが成果物
 * インポートライブラリでアクセスすることを想定している
 */
#include "DummyDlg.h"
#include <string>
#include "../DebugFnc/DebugFnc.h"
#include "resource.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;

using namespace DebugFnc;

DummyDlg::DummyDlg():
   hInst(reinterpret_cast<HMODULE>(&__ImageBase))
   , pfnSubClass([](
      HWND hWnd,
      UINT uMsg,
      WPARAM wParam,
      LPARAM lParam,
      UINT_PTR uIdSubclass,
      DWORD_PTR dwRefData)->LRESULT{
         DummyDlg* self = (DummyDlg*)dwRefData;
         switch(uMsg){
            case WM_COMMAND:
            {
               DPTD("pfnSubClass WM_COMMAND");
               HWND hwndCtrl = (HWND)lParam;
               WORD id = LOWORD(wParam);
               WORD code = HIWORD(wParam);

               if(hwndCtrl != NULL && code == BN_CLICKED){
                  switch(id){
                     case IDOK:
                     case IDCANCEL:
                     {
#pragma push_macro("DEBUG_LOG_LEVEL")
#undef DEBUG_LOG_LEVEL
#define DEBUG_LOG_LEVEL 2
                        DPTDL("IDOK or IDCANCEL", 1);
#pragma pop_macro("DEBUG_LOG_LEVEL")
                        DPTD("IDOK or IDCANCEL");
                        EndDialog(hWnd, 0);
                        return TRUE;
                     }
                  }
               }
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnSubClass WM_NCDESTROY");
               RemoveWindowSubclass(hWnd, self->pfnSubClass, uIdSubclass);
               break;
            }
         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      }){
}

INT_PTR DummyDlg::DialogBoxParamW(HWND  hWndParent){
   INT_PTR result = ::DialogBoxParamW(
      hInst
      , MAKEINTRESOURCEW(IDD_DIALOG2)
      , hWndParent
      , [](
         HWND hWnd
         , UINT uMsg
         , WPARAM wParam
         , LPARAM lParam)->INT_PTR{
            DummyDlg* self = reinterpret_cast<DummyDlg*>(lParam);
            switch(uMsg){
               case WM_INITDIALOG:
                  DPFTD("WM_INITDIALOG");
                  if(!SetWindowSubclass(
                     hWnd
                     , self->pfnSubClass
                     , (UINT_PTR)self
                     , (DWORD_PTR)self)){
                     DPT("SetWindowSubclass failed");
                     return FALSE;
                  }
                  return TRUE;
               case WM_NCDESTROY:
                  DPTD("DummyDlg WM_NCDESTROY");
                  return 0;
            }
            return 0; }
   , (INT_PTR)this);

   if(result == -1){
      DPLE;
   };
   return result;

}


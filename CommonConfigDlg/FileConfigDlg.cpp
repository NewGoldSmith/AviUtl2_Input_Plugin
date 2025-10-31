/**
 * @file FileConfigDlg.cpp
 * @brief FileConfigDlgの実装
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#include "FileConfigDlg.h"
#include "resource.h"
#include "..\UtilDefAviUtl2\UtilDefAviUtl2.h"
#include "../DebugFnc/DebugFnc.h"
#include "../CommonLib/CommonLib.h"
#include "../BitManip256/bit_manip256.h"

#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../SliderEditControl/" _CRT_STRINGIZE($CONFIGURATION) "/SliderEditControl-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../PreviewWindow/" _CRT_STRINGIZE($CONFIGURATION) "/PreviewWindow-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../BitManip256/" _CRT_STRINGIZE($CONFIGURATION) "/BitManip256-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../UtilDefAviUtl2/" _CRT_STRINGIZE($CONFIGURATION) "/UtilDefAviUtl2-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;
using namespace bit_manip::AVX;
using namespace bit_manip;
using namespace UtilDefAviUtl2;
using namespace CommonLib;

extern "C" IMAGE_DOS_HEADER __ImageBase;

FileConfigDialog::FileConfigDialog():
   hInst(reinterpret_cast<HMODULE>(&__ImageBase))
   , IDDlg(IDD_DLG_FILE_CONFIG)
   , pfnSubClass([](
      HWND hWnd
      , UINT uMsg
      , WPARAM wParam
      , LPARAM lParam
      , UINT_PTR uIdSubclass
      , DWORD_PTR dwRefData)->LRESULT{
         FileConfigDialog* self = reinterpret_cast<FileConfigDialog*>(dwRefData);
         switch(uMsg){
            case WM_LBUTTONDOWN:
            {
               DPTD("pfnSubClass WM_LBUTTONDOWN");
               SetFocus(hWnd);
               break;
            }

            case WM_LBUTTONUP:
            {
               DPTD("pfnSubClass WM_LBUTTONUP");
               break;
            }

            case WM_NCLBUTTONDOWN:
            {
               DPTD("pfnSubClass WM_NCLBUTTONDOWN");
               SetFocus(hWnd);
               break;
            }

            case WM_NCLBUTTONUP:
            {
               DPTD("pfnSubClass WM_NCLBUTTONUP");
               break;
            }

            case WM_COMMAND:
            {
               DPTD("pfnSubClass WM_COMMAND");
               HWND hwndCtrl = (HWND)lParam;
               WORD id = LOWORD(wParam);
               WORD code = HIWORD(wParam);

               switch(id){
                  case IDOK:
                  {
                     DPTD("pfnSubClass IDOK");
                     if(code == BN_CLICKED)
                        self->UpdateConfig = TRUE;
                     EndDialog(hWnd, IDOK);
                     return 1;
                  }
                  case IDCANCEL:
                  {
                     DPTD("pfnSubClass IDCANCEL");
                     if(code == BN_CLICKED)
                        EndDialog(hWnd, IDCANCEL);
                     return 1;
                  }
                  case IDC_EDIT_RATE:
                  case IDC_EDIT_SCALE:
                  {
                     DPTD("pfnSubClass IDC_EDIT_RATE IDC_EDIT_SCALE");
                     if(code == EN_CHANGE){
                        DPTD("pfnSubClass EN_CHANGE");
                        int iRate, iScale;
                        self->EditRate.GetValue(iRate);
                        self->EditScale.GetValue(iScale);
                        SetWindowTextW(
                           GetDlgItem(hWnd, IDC_STATIC_FPS)
                           , CalcFPStoStringW(iRate,iScale).c_str());
                     }
                     return 1;
                  }

                  case IDC_BTN_VIEW_BOARD:
                  {
                     DPTD("pfnSubClass IDC_BUTTON_SHOW_PREVIEW");
                     if(!self->PrevWnd.IsWindow())
                        self->PrevWnd.CreatePrevWindow(hWnd);
                     int dots;
                     if(!self->EditNumOfDots.GetValue(dots))
                        return 1;
                     HBITMAP hDib = CreateDIB24(dots, dots);
                     if(!DrawBoardDIB(self->vRBF[self->vRBFIndex], hDib)){
                        return 1;
                     }
                     self->PrevWnd.DrawBitmap(hDib);

                     if(self->vRBF.size() > 0){
                        if(self->vRBFIndex < self->vRBF.size() - 1){
                           EnableWindow(GetDlgItem(hWnd, IDC_BTN_NEXT), TRUE);
                        } else{
                           EnableWindow(GetDlgItem(hWnd, IDC_BTN_NEXT), FALSE);
                        }
                     }
                     if(self->vRBFIndex == 0){
                        EnableWindow(GetDlgItem(hWnd, IDC_BTN_PREV), FALSE);
                     } else{
                        EnableWindow(GetDlgItem(hWnd, IDC_BTN_PREV), TRUE);
                     }

                     return 1;
                  }

                  case IDC_BTN_PREV:
                  {
                     DPTD("pfnSubClass IDC_BTN_PREV");
                     if(--self->vRBFIndex <= 0){
                        EnableWindow(GetDlgItem(hWnd, IDC_BTN_PREV), FALSE);
                     }
                     if(self->vRBFIndex + 1 <= self->vRBF.size())
                        EnableWindow(GetDlgItem(hWnd, IDC_BTN_NEXT), TRUE);

                     wstring wstr(L"Preview ");
                     wstr += to_wstring(self->vRBFIndex + 1) + L"/" + to_wstring(self->vRBF.size());
                     if(!SetWindowTextW(GetDlgItem(hWnd, IDC_BTN_VIEW_BOARD), wstr.c_str())){
                        DPLE;
                        return 1;
                     }
                     int dots;
                     if(!self->EditNumOfDots.GetValue(dots))
                        return 1;
                     unique_ptr<remove_pointer_t<HBITMAP>,decltype(DeleteObject)*> hDib
                     { CreateDIB24(dots, dots),DeleteObject };
                     if(!hDib){
                        DPT("DIB creation failed.");
                        return 1;
                     };
                     if(!DrawBoardDIB(self->vRBF[self->vRBFIndex], hDib.get())){
                        MessageBoxA(hWnd, DPT("DrawBoardDIB failed").c_str(), "Error", MB_ICONEXCLAMATION | MB_OK);
                        return 1;
                     }
                     self->PrevWnd.DrawBitmap(hDib.get());
                     return 1;
                  }

                  case IDC_BTN_NEXT:
                  {
                     DPTD("pfnSubClass IDC_BTN_NEXT");
                     if(++self->vRBFIndex + 1 >= self->vRBF.size())
                        EnableWindow(GetDlgItem(hWnd, IDC_BTN_NEXT), FALSE);
                     if(self->vRBFIndex > 0)
                        EnableWindow(GetDlgItem(hWnd, IDC_BTN_PREV), TRUE);

                     wstring wstr(L"Preview ");
                     wstr += to_wstring(self->vRBFIndex + 1) + L"/" + to_wstring(self->vRBF.size());
                     if(!SetWindowTextW(GetDlgItem(hWnd, IDC_BTN_VIEW_BOARD), wstr.c_str())){
                        DPLE;
                        return 0;
                     }
                     int dots;
                     if(!self->EditNumOfDots.GetValue(dots))
                        return 0;
                     unique_ptr<remove_pointer_t<HBITMAP>, decltype(DeleteObject)*> hDib
                     { CreateDIB24(dots, dots),DeleteObject };
                     if(!DrawBoardDIB(self->vRBF[self->vRBFIndex], hDib.get())){
                        return 0;
                     }
                     self->PrevWnd.DrawBitmap(hDib.get());
                     return 1;
                  }

                  case IDC_EDIT_NUM_DOTS:
                  {
                     DPTD("pfnSubClass IDC_EDIT_NUM_DOTS");
                     switch(code){
                        case EN_CHANGE:
                        {
                           DPTD("pfnSubClass EN_CHANGE");
                           if(self->PrevWnd.IsWindow()){
                              int dots;
                              if(!self->EditNumOfDots.GetValue(dots))
                                 return 1;
                              unique_ptr<remove_pointer_t<HBITMAP>, decltype(DeleteObject)*> hDib{ CreateDIB24(dots, dots),DeleteObject };
                              if(!DrawBoardDIB(self->vRBF[self->vRBFIndex], hDib.get())){
                                 return 1;
                              }
                              self->PrevWnd.DrawBitmap(hDib.get());
                           }
                        }
                     }
                     break;
                  }
               }
               break;
            }

            case WM_SYSCOMMAND:
            {
               DPTD("pfnSubClass WM_SYSCOMMAND");
               switch(wParam & 0xFFF0){
                  case SC_CLOSE:
                     DPTD("pfnSubClass SC_CLOSE");
                     EndDialog(hWnd, SC_CLOSE);
                     return TRUE;
               }
               break;
            }

            case WM_DESTROY:
            {
               DPTD("pfnSubClass WM_DESTROY");
               if(self->UpdateConfig){
                  {
                     int value;
                     self->EditNumOfDots.GetValue(value);
                     self->Config[L"N_DOTS"] = to_wstring(value);
                  }
                  {
                     int value;
                     self->EditRate.GetValue(value);
                     self->Config[L"RATE"] = to_wstring(value);
                  }
                  {
                     int value;
                     self->EditScale.GetValue(value);
                     self->Config[L"SCALE"] = to_wstring(value);
                  }
               }
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnSubClass WM_NCDESTROY");
               ::RemoveWindowSubclass(
                  hWnd
                  , self->pfnSubClass
                  , uIdSubclass);
               break;
            }

         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      })

   , EditNumOfDots(hInst, CUSTOM_CONTROL_CLASS_NAME)
   , EditRate(hInst, CUSTOM_CONTROL_CLASS_NAME)
   , EditScale(hInst, CUSTOM_CONTROL_CLASS_NAME)
   , PrevWnd(hInst)
{
}

FileConfigDialog::~FileConfigDialog(){
}


[[nodiscard]] INT_PTR FileConfigDialog::DialogBoxParamW(
   HWND hWndParent
   , const map<wstring, wstring>& InCfg){

   for(const auto& key : requiredKeys){
      if(InCfg.find(key) == InCfg.end()){
         DPFTD("There are not enough keys.");
         return -2;
      }
   }
   Config = InCfg;
   UpdateConfig = FALSE;

   INT_PTR result = ::DialogBoxParamW(
      hInst
      , MAKEINTRESOURCEW(IDDlg)
      , hWndParent
      , [](
         HWND hWnd
         , UINT uMsg
         , WPARAM wParam
         , LPARAM lParam)->INT_PTR{

            switch(uMsg){
               case WM_INITDIALOG:
               {
                  DPFTD("WM_INITDIALOG");
                  /// WM_INITDIALOGより前に来るメッセージもあるのでそれは無視して、
                  /// このタイミングで各種設定をする。
                  FileConfigDialog* self = reinterpret_cast<FileConfigDialog*>(lParam);
                  self->hWnd = hWnd;
                  /// 途中でエラーがあった場合、ウィンドウを閉じるunique_ptr
                  unique_ptr<remove_pointer_t<HWND>, void(*)(HWND h)> uhWnd
                  { hWnd,[](HWND h){EndDialog(h,-1); } };

                  {
                     if(!self->EditNumOfDots.SetWindowSubclass(
                        hWnd
                        , IDC_EDIT_NUM_DOTS
                     )){
                        MessageBoxA(
                           NULL
                           , DPT("SetWindowSubclass failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     }

                     self->EditNumOfDots.SetValue(stoi(self->Config[L"N_DOTS"]));

                     if(!self->EditNumOfDots.SetRange(DOT_RANGE_LOW, DOT_RANGE_HIGH)){
                        MessageBoxA(
                           NULL
                           , DPT("self->EditDot.SetRange failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     };
                  }

                  {
                     if(!self->EditRate.SetWindowSubclass(
                        hWnd
                        , IDC_EDIT_RATE
                     )){
                        MessageBoxA(
                           NULL
                           , DPT("SetWindowSubclass failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     }

                     self->EditRate.SetValue(stoi(self->Config[L"RATE"]));

                     if(!self->EditRate.SetRange(RATE_RANGE_LOW, RATE_RANGE_HIGH)){
                        MessageBoxA(
                           NULL
                           , DPT("self->EditDot.SetRange failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     };
                  }

                  {
                     if(!self->EditScale.SetWindowSubclass(
                        hWnd
                        , IDC_EDIT_SCALE
                     )){
                        MessageBoxA(
                           NULL
                           , DPT("SetWindowSubclass failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     }

                     self->EditScale.SetValue(stoi(self->Config[L"SCALE"]));

                     if(!self->EditScale.SetRange(SCALE_RANGE_LOW, SCALE_RANGE_HIGH)){
                        MessageBoxA(
                           NULL
                           , DPT("self->EditDot.SetRange failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     };
                  }

                  {
                     SetWindowTextW(
                        GetDlgItem(hWnd, IDC_STATIC_FILE_PATH_NAME)
                        , self->Config[L"FILE_PATH_NAME"].c_str());
                  }

                  {
                     int iRate, iScale;
                     self->EditRate.GetValue(iRate);
                     self->EditScale.GetValue(iScale);
                     SetWindowTextW(
                        GetDlgItem(hWnd, IDC_STATIC_FPS)
                        , CalcFPStoStringW(iRate, iScale).c_str());
                  }

                  /// @brief ファイル読み込み
                  {

                     if(!LoadM256ArrayW(self->Config[L"FILE_PATH_NAME"], self->vRBF)){
                        MessageBoxA(
                           NULL
                           , DPT("LoadM256Array failed.").c_str()
                           , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                        return FALSE;
                     };
                     EnableWindow(GetDlgItem(hWnd, IDC_BTN_VIEW_BOARD), TRUE);
                     wstring wstrBtnText(L"Preview");
                     self->vRBFIndex = 0;
                     wstrBtnText += L" 1/" + to_wstring(self->vRBF.size());
                     SetWindowTextW(GetDlgItem(hWnd, IDC_BTN_VIEW_BOARD), wstrBtnText.c_str());
                  }

                  if(!SetWindowSubclass(
                     hWnd
                     , self->pfnSubClass
                     , (UINT_PTR)hWnd
                     , (DWORD_PTR)self)){
                     DPT("SetWindowSubclass failed");
                     return FALSE;
                  }

                  /// プレビューウィンドウを開くメッセージをポスト 
                  PostMessageA(
                     hWnd
                     , WM_COMMAND
                     , MAKEWPARAM(IDC_BTN_VIEW_BOARD, BN_CLICKED)
                     , (LPARAM)GetDlgItem(hWnd, IDC_BTN_VIEW_BOARD));

                  /// エラーが無かったのでリリースする。
                  uhWnd.release();
                  return TRUE;
               }
            }
            return 0; }
   , (INT_PTR)this);

   if(result == -1){
      DPLE;
   };

   return result;
}

map<wstring, wstring> FileConfigDialog::GetConfigW(){
   return Config;
}

/**
 * @file MainDialog.cpp
 * @brief MainDialogの宣言
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#include "TestDlg.h"
#include <pathcch.h>
#include <wrl/client.h>
#include "..\CommonConfigDlg\PublicCfgDlgInterface.h"
#include "..\CommonLib\CommonLib.h"
#include "resource.h"
#include "..\UtilDefAviUtl2\UtilDefAviUtl2.h"
#include "..\DebugFnc\DebugFnc.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../SliderEditControl/" _CRT_STRINGIZE($CONFIGURATION) "/SliderEditControl-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../PreviewWindow/" _CRT_STRINGIZE($CONFIGURATION) "/PreviewWindow-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../BitManip256/" _CRT_STRINGIZE($CONFIGURATION) "/BitManip256-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../DummyDlg/" _CRT_STRINGIZE($CONFIGURATION) "/DummyDlg-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../UtilDefAviUtl2/" _CRT_STRINGIZE($CONFIGURATION) "/UtilDefAviUtl2-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;
using namespace Microsoft::WRL;
using namespace bit_manip;
using namespace UtilDefAviUtl2;
using namespace CommonLib;
extern "C" IMAGE_DOS_HEADER __ImageBase;
#pragma push_macro("B1")
#pragma push_macro("W1")
#pragma push_macro("M1")
#define B1 \
0b\
00000000\
00000000\
00000000\
00001000\
00010000\
00000000\
00000000\
00000000ULL
#define W1 \
0b\
00000000\
00000000\
00000000\
00010000\
00001000\
00000000\
00000000\
00000000ULL

#define M1 \
0b\
00000000\
00000000\
00000000\
00100000\
00000000\
00000000\
00000000\
00000000ULL

MainDialog::MainDialog(HINSTANCE hInst):
   hInst(hInst)
   ,pfnSubClass([](
      HWND hWnd,
      UINT uMsg,
      WPARAM wParam,
      LPARAM lParam,
      UINT_PTR uIdSubclass,
      DWORD_PTR dwRefData)->LRESULT{
         MainDialog* self = reinterpret_cast<MainDialog*>(dwRefData);
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

               if(hwndCtrl != NULL && code == BN_CLICKED){
                  switch(id){
                     case IDOK:
                        DPTD("pfnSubClass IDOK");
                        EndDialog(hWnd, IDOK);
                        return 0;
                     case IDCANCEL:
                        DPTD("pfnSubClass IDCANCEL");
                        EndDialog(hWnd, IDCANCEL);
                        return 0;

                     case IDC_BUTTON_PREVIEW:
                     {
                        if(HWND hPreview; hPreview = self->InstPreviewWindow.CreatePrevWindow(hWnd)){
                           self->InstPreviewWindow.DrawBitmap(self->hBitmap.get());
                        }
                        return 0;
                     }

                     case IDC_BTN_DO_MODAL:
                     {
                        self->InstPreviewWindow.DoModal(hWnd, self->hBitmap.get());
                        break;
                     }

                     case IDC_BTN_DUMMY_DLG:
                     {
                        DPTD("pfnSubClass IDC_BTN_DUMMY_DLG");
                        self->InstDummyDlg.DialogBoxParamW(hWnd);
                        return TRUE;
                     }

                     case IDC_BTN_COMMON_CFG_DLG:
                     {
                        DPTD("pfnSubClass IDC_BTN_COMMON_CFG_DLG");
                        /// モジュールファイルを探しロードし、関数ポインタから実行。
                        WCHAR path[MAX_PATH + 1];
                        if(!::GetModuleFileNameW(self->hInst, path, MAX_PATH))
                           return FALSE;
                        if(::PathCchRemoveFileSpec(path, MAX_PATH) != S_OK){
                           return FALSE;
                        }

                        wstring strfind = path;
                        strfind += L"\\";
                        /// ワイルドカードも可能な文字列
                        strfind += FIND_MODULE_STRING;
                        wstring strfound;
                        if(strfound = FindFirstMatchingFile(strfind);
                           strfound.empty()){
                           DPT("DLL not found.");
                           MessageBoxA(hWnd
                                       , "CommonConfigDlg*.dllが見つかりません。プラグインと同じディレクトリに配置してください。"
                                       , "Error"
                                       , MB_OK | MB_ICONEXCLAMATION);
                           return FALSE;
                        }

                        unique_ptr<remove_pointer_t<HMODULE>, decltype(FreeLibrary)*>hDll
                        { LoadLibraryW(strfound.c_str()),FreeLibrary };
                        if(!hDll){
                           DPLE;
                           return FALSE;
                        }

                        using PFN_CO_CREATE_INSTANCE = HRESULT(WINAPI*)(
                           REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*
                           );

                        PFN_CO_CREATE_INSTANCE pfnCoCreateInstance =
                           (PFN_CO_CREATE_INSTANCE)GetProcAddress(hDll.get(), CO_CREATE_INSTANCE_STRING);
                        if(!pfnCoCreateInstance){
                            // エラー処理
                           return FALSE;
                        }

                        ComPtr<ICfgDlg> pDlg;
                        HRESULT hr = pfnCoCreateInstance(
                           CLSID_CFG_DLG
                           , NULL
                           , CLSCTX_INPROC_SERVER
                           , IID_ICfgDlg
                           , (void**)pDlg.GetAddressOf());

                        if(SUCCEEDED(hr) && pDlg){
                           LONGLONG result{};
                           pDlg->DialogBoxParamW(hWnd, (HMODULE)&__ImageBase, result);
                        }
                        return TRUE;
                     }

                     case IDC_BTN_COMMON_CFG_COM_DLG:
                     {
                        ScopedCOMInitializer comInit;
                        if(!comInit.succeeded()) return 1;

                        ComPtr<ICfgDlg> pDlg;
                        HRESULT hr = ::CoCreateInstance(CLSID_CFG_DLG, NULL, CLSCTX_INPROC_SERVER, IID_ICfgDlg, (void**)pDlg.GetAddressOf());
                        if(SUCCEEDED(hr)){
                           LONGLONG result;
                           pDlg->DialogBoxParamW(hWnd, self->hInst, result);
                        } else{
                           MessageBoxW(NULL
                                       , L"COMオブジェクトの生成に失敗しました。\
                                       CommonConfigDlg*.dllが見つかりません。登録が必要です。"
                                       , L"エラー", MB_ICONERROR);
                        }
                     }
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

                  case SC_MINIMIZE:
                     DPTD("pfnSubClass SC_MINIMIZE");
                     break;

                  case SC_MAXIMIZE:
                     DPTD("pfnSubClass SC_MAXIMIZE");
                     break;

                  default:
                     DPTD("pfnSubClass default WM_SYSCOMMAND");
                     break;
               }
               break;
            }

            case WM_DESTROY:
            {
               DPTD("pfnSubClass WM_DESTROY");
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnSubClass WM_NCDESTROY");
               RemoveWindowSubclass(
                  hWnd
                  , self->pfnSubClass
                  , uIdSubclass);
               break;
            }
         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      })

   , hBitmap{ [](){
   m256 m{};
   m.B = B1;
   m.W = W1;
   m.M = M1;
#pragma pop_macro("B1")
#pragma pop_macro("W1")
#pragma pop_macro("M1")
   HBITMAP h = CreateDIB24(DEFAULT_BITMAP_WIDTH,DEFAULT_BITMAP_HEIGHT);
   DrawBoardDIB(m, h);
   return h;
   }() ,DeleteObject }
   ,InstSliderEditControl(hInst,L"CustomControl1")
   ,InstSliderEditControl2(hInst,L"CustomControl1")
   ,InstPreviewWindow(hInst)
   ,InstDummyDlg()
{
   INITCOMMONCONTROLSEX inictrl{
      sizeof(INITCOMMONCONTROLSEX)
      ,ICC_STANDARD_CLASSES | ICC_USEREX_CLASSES | ICC_WIN95_CLASSES };
   InitCommonControlsEx(&inictrl);
}

INT_PTR MainDialog::DialogBoxParamW(HWND hWndParent){

   INT_PTR result = ::DialogBoxParamW(
      hInst
      , MAKEINTRESOURCEW(IDD_DIALOG1)
      , hWndParent
      , [](
         HWND hWnd
         , UINT uMsg
         , WPARAM wParam
         , LPARAM lParam)->INT_PTR{
            switch(uMsg){
            case WM_INITDIALOG:
               DPFTD("WM_INITDIALOG");
               MainDialog* self = reinterpret_cast<MainDialog*>(lParam);
               self->hWnd = hWnd;
               if(!self->InstSliderEditControl.SetWindowSubclass(
                  hWnd
                  , IDC_CUSTOM1
               )){
                  MessageBoxA(
                     NULL
                     , DPLEM("SetWindowSubclass failed.").c_str()
                     , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                  EndDialog(hWnd, -1);
                  return FALSE;
               }

               self->InstSliderEditControl.SetValue(512);

               if(!self->InstSliderEditControl.SetRange(0, 1000)){
                  MessageBoxA(
                     NULL
                     , DPT("self->InstSliderEditControl.SetRange failed.").c_str()
                     , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
               };

               if(!self->InstSliderEditControl2.SetWindowSubclass(
                  hWnd
                  , IDC_CUSTOM2
               )){
                  MessageBoxA(
                     NULL
                     , DPT("SetWindowSubclass failed.").c_str()
                     , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
                  EndDialog(hWnd, -1);
                  return FALSE;
               }

               self->InstSliderEditControl2.SetValue(512);

               if(!self->InstSliderEditControl2.SetRange(0, 1000)){
                  MessageBoxA(
                     NULL
                     , DPT("self->InstSliderEditControl.SetRange failed.").c_str()
                     , _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
               };


               if(!SetWindowSubclass(
                  hWnd
                  , self->pfnSubClass
                  , (UINT_PTR)self
                  , (DWORD_PTR)self)){
                  DPT("SetWindowSubclass failed");
                  return FALSE;
               }
               return TRUE;
            }
            return 0; }
   , (INT_PTR)this);

   if(result == -1){
      DPLE;
   };
   return result;
}

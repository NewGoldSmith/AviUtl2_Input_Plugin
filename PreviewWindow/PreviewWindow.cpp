/**
 * @file PreviewWindow.cpp
 * @brief PreviewWindowの実装
 * @date 2025
 * @author Gold Smith
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#include "PreviewWindow.h"
#include <iomanip>
#include "../DebugFnc/DebugFnc.h"
#include "../CommonLib/CommonLib.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
using namespace std;
using namespace Gdiplus;
using namespace CommonLib;
PreviewWindow::PreviewWindow(HINSTANCE hInst):
   hInst{ hInst }
   , pfnSubClsPrevWndPrc([](
      HWND hWnd
      , UINT uMsg
      , WPARAM wParam
      , LPARAM lParam
      , UINT_PTR uIdSubclass
      , DWORD_PTR dwRefData)->LRESULT{
         PreviewWindow* self = reinterpret_cast<PreviewWindow*>(dwRefData);
         switch(uMsg){
            case WM_PAINT:
            {
               PAINTSTRUCT ps;
               HDC hdc = BeginPaint(hWnd, &ps);

               if(self->hBitmap){
                  BITMAP bmp;
                  GetObject(self->hBitmap, sizeof(BITMAP), &bmp);

                  HDC hMemDC = CreateCompatibleDC(hdc);
                  SelectObject(hMemDC, self->hBitmap);
                  //BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
                  StretchBlt(
                     hdc
                     , 0
                     , 0
                     , (int)(bmp.bmWidth * self->scale)
                     , (int)(bmp.bmHeight * self->scale)
                     , hMemDC
                     , 0
                     , 0
                     , bmp.bmWidth
                     , bmp.bmHeight
                     , SRCCOPY
                  );
                  DeleteDC(hMemDC);

                  {
                     ULONGLONG now = GetTickCount64();
                     if(now - self->scaleTextTimestamp < 1000) // 1秒以内なら表示
                     {
                        std::wstringstream ss;
                        ss << L"x" << std::fixed << std::setprecision(1) << self->scale;
                        std::wstring text = ss.str();

                        std::vector<std::wstring> lines = {
                           text.c_str()
                           , L"R key x1.0"
                           , L"Ctl + wheels to zoom in and out."
                           , L"+/- keys are also possible."
                        };

                        RECT rcClient;
                        GetClientRect(hWnd, &rcClient);
                        SetBkMode(hdc, TRANSPARENT);

                        int fontSize = max(12, (int)((rcClient.bottom - rcClient.top) / 20));
                        unique_ptr<remove_pointer_t<HFONT>, decltype(DeleteObject)*> hFont
                        (CreateFontW(
                           fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE
                           , DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                           , DEFAULT_PITCH | FF_DONTCARE, L"Arial"), DeleteObject);
                        GdiSelectAndRestorer<HFONT> FontRestorer(hdc, hFont.get());

                        int tx = (int)(rcClient.left + fontSize * 1.5 + 16);
                        int ty = (int)(rcClient.top + fontSize * 1.0 + 16);
                        const int outlineOffset = 1;
                        const int lineSpacing = fontSize + 4;

                        for(const auto& line : lines){
                           SIZE sz = {};
                           GetTextExtentPoint32W(hdc, line.c_str(), (int)line.size(), &sz);

                           // 黒の縁取り
                           SetTextColor(hdc, RGB(0, 0, 0));
                           for(int dx = -outlineOffset; dx <= outlineOffset; ++dx){
                              for(int dy = -outlineOffset; dy <= outlineOffset; ++dy){
                                 if(dx == 0 && dy == 0) continue;
                                 TextOutW(hdc, tx + dx, ty + dy, line.c_str(), (int)line.size());
                              }
                           }

                           // 白の本体
                           SetTextColor(hdc, RGB(255, 255, 255));
                           TextOutW(hdc, tx, ty, line.c_str(), (int)line.size());
                           ty += lineSpacing;
                        }
                     }
                  }
               } else{
                   /// @brief ビットマップが未設定の場合のフォールバック描画
                  RECT rc;
                  GetClientRect(hWnd, &rc);
                  FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));
                  DrawTextW(hdc
                            , L"No preview available"
                            , -1
                            , &rc
                            , DT_CENTER | DT_VCENTER | DT_SINGLELINE);
               }
               EndPaint(hWnd, &ps);
               break;
            }

            case PreviewWindow::WM_DRAWPREVIEW:
            {
               DPTD("pfnSubClsPrevWndPrc WM_DRAWPREVIEW");
               if(HBITMAP hbmp;!(hbmp = DupDIB((HBITMAP)wParam))){
                  DPTD("pfnSubClsPrevWndPrc WM_DRAWPREVIEW faled.");
                  return FALSE;
               } else{
                  DeleteObject(self->hBitmap);
                  self->hBitmap = hbmp;
               }
               BITMAP bmp;
               GetObject(self->hBitmap, sizeof(BITMAP), &bmp);

               RECT rc = { 0, 0, (LONG)(bmp.bmWidth * self->scale), (LONG)(bmp.bmHeight * self->scale) };
               AdjustWindowRectEx(
                  &rc
                  , GetWindowLong(hWnd, GWL_STYLE)
                  , FALSE
                  , GetWindowLong(hWnd, GWL_EXSTYLE));

               SetWindowPos(
                  hWnd
                  , NULL
                  , 0
                  , 0
                  , rc.right - rc.left
                  , rc.bottom - rc.top
                  , SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

               self->SetScale(self->scale);
               return TRUE;
            }

            case WM_MOUSEWHEEL:
            {
               if(!(GetKeyState(VK_CONTROL) & 0x8000))
                  return DefSubclassProc(hWnd, uMsg, wParam, lParam);

               short delta = GET_WHEEL_DELTA_WPARAM(wParam);
               double step = 0.1;
               double newScale = self->GetScale() + (delta > 0 ? step : -step);
               newScale = std::max(0.1, std::min(newScale, 10.0));

               if(newScale != self->GetScale())
                  self->SetScale(newScale);

               return 0;

            }

            case WM_TIMER:
            {
               DPTD("pfnSubClsPrevWndPrc WM_TIMER");
               if(wParam == VIEW_SCALE_TIMER_ID) // 倍率表示用タイマー
               {
                  KillTimer(hWnd, VIEW_SCALE_TIMER_ID); // 一度きり
                  self->scaleTextTimestamp = 0; // 表示終了
                  InvalidateRect(hWnd, NULL, TRUE); // 再描画
               }
               return 0;
            }

            case WM_KEYDOWN:
            {
               DPTD("pfnSubClsPrevWndPrc WM_KEYDOWN");
               double newScale = self->GetScale();

               switch(wParam){
                  case VK_ADD:   // テンキー '+'
                  case 0xBB:     // メインキー '+'
                     newScale += SCALE_STEP;
                     break;

                  case VK_SUBTRACT: // テンキー '-'
                  case 0xBD:        // メインキー '-'
                     newScale -= SCALE_STEP;
                     break;

                  case 'R': // リセット
                     newScale = 1.0;
                     break;

                  default:
                     return DefSubclassProc(hWnd, uMsg, wParam, lParam);
               }

               newScale = std::max(0.1, std::min(newScale, 10.0));
               if(newScale != self->GetScale())
                  self->SetScale(newScale);

               return 0;
            }

            case WM_MOUSEMOVE:
            {
               DPTD("pfnSubClsPrevWndPrc WM_MOUSEMOVE");
               if(GetFocus() != hWnd)
                  SetFocus(hWnd); // PreviewWindow にフォーカスを移す
               break;
            }

            case WM_NCHITTEST:
            {
               DPTD("pfnSubClsPrevWndPrc WM_NCHITTEST");
               return HTCAPTION;
            }

            case WM_CLOSE:
            {
               DPTD("pfnSubClsPrevWndPrc WM_CLOSE");
               break;
            }

            case WM_DESTROY:
            {
               DPTD("pfnSubClsPrevWndPrc WM_DESTROY");
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnSubClsPrevWndPrc WM_NCDESTROY");
               KillTimer(hWnd, VIEW_SCALE_TIMER_ID);
               DeleteObject(self->hBitmap);
               self->hBitmap = NULL;
               if(!::RemoveWindowSubclass(hWnd, self->pfnSubClsPrevWndPrc, uIdSubclass))
                  DPTD("pfnSubClsPrevWndPrc RemoveWindowSubclass failed");
               self->hWnd = NULL;
               if(self->IsModal){
                  self->IsModal = FALSE;
                  PostQuitMessage(0);
               }
               break;
            }
         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      })
{
   WNDCLASSEXW wndcls{};
   if(::GetClassInfoExW(hInst, CLASS_NAME, &wndcls))
      return;

   wndcls.cbSize = sizeof(WNDCLASSEXW);
   wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wndcls.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT{
      switch(uMsg){
         case WM_CREATE:
         {
            CREATESTRUCTW* pcs = (CREATESTRUCTW*)(lParam);
            PreviewWindow* self = (PreviewWindow*)pcs->lpCreateParams;
            if(!::SetWindowSubclass(
               hWnd
               , self->pfnSubClsPrevWndPrc
               , (UINT_PTR)self
               , (DWORD_PTR)self)){
               DPLE;
               return -1;
            }
            return 0;
         }
         break;
      }
      return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
      };
   wndcls.cbClsExtra = 0;
   wndcls.cbWndExtra = 0;
   wndcls.hInstance = hInst;
   wndcls.hIcon = NULL;
   wndcls.hCursor = ::LoadCursorW(hInst, IDC_ARROW);
   wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wndcls.lpszMenuName = NULL;
   wndcls.lpszClassName = CLASS_NAME;

   if(!(atm = RegisterClassExW(&wndcls))){
      DPLE;
   }
   return;
}

PreviewWindow::~PreviewWindow(){
   UnregisterClassW(CLASS_NAME, hInst);
}

HWND PreviewWindow::CreatePrevWindow(HWND hParentWnd){
   if(hWnd) return 0;

   this->hParentWnd = hParentWnd;
   RECT rcParent = {};
   GetWindowRect(hParentWnd, &rcParent);

   RECT rc{
      rcParent.right
      ,rcParent.top
      ,rcParent.right + INIT_WINDOW_WIDTH
      ,rcParent.top + INIT_WINDOW_HEIGHT };

   if(!AdjustRectToFitScreenWithOverflow(rc)){
      DPTD("AdjustRectToFitScreenWithOverflow failed.");
      return NULL;
   };

   if(!(hWnd = CreateWindowExW(
      WS_EX_TRANSPARENT
      , CLASS_NAME
      , L"Preview"
      , WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE
      , rc.left
      , rc.top
      , (int)((rc.right - rc.left) * scale)
      , (int)((rc.bottom - rc.top) * scale)
      , hParentWnd
      , NULL
      , hInst
      , this))){
      DPLE;
      return NULL;
   }
   return hWnd;
}

INT_PTR PreviewWindow::DoModal(HWND hParentWnd, HBITMAP hBmp){
   if(hWnd)
      return IDCANCEL;
   IsModal = TRUE;
   this->hParentWnd = hParentWnd;
   RECT rcParent = {};
   GetWindowRect(hParentWnd, &rcParent);

   RECT rc{
      rcParent.right
      ,rcParent.top
      ,rcParent.right + INIT_WINDOW_WIDTH
      ,rcParent.top + INIT_WINDOW_HEIGHT };

    // ビットマップを複製して保管
   hBitmap = DupDIB(hBmp);
   if(!hBitmap)
      return IDCANCEL;

  // ウィンドウ生成
   hWnd = CreateWindowExW(
      WS_EX_TRANSPARENT
      , MAKEINTATOM(atm)
      , L"Preview"
      , WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE
      , rc.left
      , rc.top
      , (int)((rc.right - rc.left) * scale)
      , (int)((rc.bottom - rc.top) * scale)
      , hParentWnd
      , (HMENU)NULL
      , hInst
      , this);
   if(!hWnd)
      return IDCANCEL;

   // サブクラスプロシージャ登録
   // uIdSubclass はお好みで一意のIDを
   if(!SetWindowSubclass(hWnd, pfnSubClsPrevWndPrc, (UINT_PTR)this, (DWORD_PTR)this))
      return IDCANCEL;

  // 親を無効化 → 疑似モーダル開始
   EnableWindow(hParentWnd, FALSE);

   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);

   // 描画要求：WM_DRAWPREVIEW→サブクラスでサイズ調整＆Invalidate→WM_PAINT
   DrawBitmap(hBitmap);

   // -- 疑似モーダルループ --
   MSG msg;
   while(GetMessage(&msg, nullptr, 0, 0)){

      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   // モーダル解除
   EnableWindow(hParentWnd, TRUE);
   SetForegroundWindow(hParentWnd);

   return IDCANCEL;  // 「×」で閉じた＝キャンセル扱い
}

BOOL PreviewWindow::DrawBitmap(HBITMAP hbmp){
   return (BOOL)SendMessageW(hWnd, WM_DRAWPREVIEW, (WPARAM)hbmp, 0);
}

BOOL PreviewWindow::IsWindow() const{
   return hWnd && ::IsWindow(hWnd);
}

double PreviewWindow::SetScale(double newScale){
   double prevScale = scale;
   if(newScale <= 0.0)
      return -1;

   scale = newScale;

   if(hBitmap && IsWindow()){
      BITMAP bmp;
      GetObject(hBitmap, sizeof(BITMAP), &bmp);

      int scaledWidth = static_cast<int>(bmp.bmWidth * scale);
      int scaledHeight = static_cast<int>(bmp.bmHeight * scale);

      RECT rc = { 0, 0, scaledWidth, scaledHeight };
      AdjustWindowRectEx(
         &rc
         , GetWindowLong(hWnd, GWL_STYLE)
         , FALSE
         , GetWindowLong(hWnd, GWL_EXSTYLE)
      );

      SetWindowPos(
         hWnd
         , NULL
         , 0
         , 0
         , rc.right - rc.left
         , rc.bottom - rc.top
         , SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE
      );
      SetTimer(hWnd, VIEW_SCALE_TIMER_ID, TIMER_SCALE_TIMER_ELAPSE, NULL);
      scaleTextTimestamp = GetTickCount64();
      InvalidateRect(hWnd, NULL, TRUE);
   }
   return prevScale;
}

double PreviewWindow::GetScale() const{
   return scale;
}

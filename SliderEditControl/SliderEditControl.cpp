/**
 * @file SliderEditControl.cpp
 * @brief SliderEditControlの実装
 * @author Gold Smith
 * @date 2025
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#include "SliderEditControl.h"
#include <windowsx.h>
#include <format>
#include <algorithm>
#include "../DebugFnc/DebugFnc.h"
#include "../CommonLib/CommonLib.h"
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;
using namespace CommonLib;

BOOL SliderEditControl::SwitchEditShowState(BOOL InShow){

   BOOL tmp = IsEditShowState;
   IsEditShowState = InShow;
   if(InShow){
      if(Hovered){
         Hovered = FALSE;
         TRACKMOUSEEVENT tme = { sizeof(tme), TME_CANCEL | TME_HOVER | TME_LEAVE, hWnd,HOVER_DEFAULT };
         if(!TrackMouseEvent(&tme)){
            DPLE;
            return FALSE;
         }
      }

      if(GetCapture() == hWnd)
         ReleaseCapture();

      SetWindowPos(
         hEdit
         , hDlgParent
         , rcSlider.left
         , rcSlider.top
         , rcSlider.right - rcSlider.left
         , rcSlider.bottom - rcSlider.top
         , SWP_SHOWWINDOW);
      ::ShowWindow(hEdit, SW_SHOW);
      ::SetFocus(hEdit);
      ::CreateCaret(hEdit, NULL, 1, rcAll.bottom - rcAll.top);
      ::ShowCaret(hEdit);
      int len = ::GetWindowTextLength(hEdit);
      ::SendMessage(hEdit, EM_SETSEL, 0, -1);
   } else{
      ::DestroyCaret();
      ::ShowWindow(hEdit, SW_HIDE);
      if(GetFocus() == hEdit){
         SetFocus(hWnd);
         TRACKMOUSEEVENT tme = { sizeof(tme), TME_HOVER | TME_LEAVE, hWnd,HOVER_DEFAULT };
         if(!TrackMouseEvent(&tme)){
            DPLE;
            ;
         }
         Hovered = TRUE;
      }
   }
   return tmp;
}

void SliderEditControl::UpdateLayoutRects(){
   RECT rc;
   ::GetWindowRect(GetDlgItem(hDlgParent, nIDDlgItem), &rc);
   ::MapWindowPoints(NULL, GetDlgItem(hDlgParent, nIDDlgItem), (LPPOINT)&rc, 2);
   UpdateLayoutRects(MAKELPARAM(rc.right, rc.bottom));
}

void SliderEditControl::UpdateLayoutRects(LPARAM lParam){
   rcAll = { 0,0,LOWORD(lParam),HIWORD(lParam) };
   rcMinus = { 0,0,rcAll.bottom,rcAll.bottom };
   rcSlider = { rcMinus.right, 0,rcMinus.right + rcAll.right - rcMinus.right * 2,rcMinus.bottom };
   rcPlus = { rcSlider.right,0,rcAll.right,rcMinus.bottom };
}

HFONT SliderEditControl::GetFontAdjusted(HWND hWnd, const RECT* prc) const{
   int clientHeight = prc->bottom - prc->top;

   int low = 1, high = clientHeight;
   int bestFit = low;

   TEXTMETRIC tm;
   HDC hdc = GetDC(hWnd);
   while(low <= high){
      int mid = (low + high) / 2;
      HFONT testFont = CreateFontW(
         -mid
         , 0
         , 0
         , 0
         , FW_NORMAL
         , FALSE
         , FALSE
         , FALSE
         , DEFAULT_CHARSET
         , OUT_DEFAULT_PRECIS
         , CLIP_DEFAULT_PRECIS
         , DEFAULT_QUALITY
         , DEFAULT_PITCH | FF_DONTCARE
         , L"Segoe UI");

      SelectObject(hdc, testFont);
      GetTextMetrics(hdc, &tm);
      DeleteObject(testFont);

      if(tm.tmHeight <= clientHeight){
         bestFit = mid;
         low = mid + 1;
      } else{
         high = mid - 1;
      }
   }

   return ::CreateFontW(-bestFit, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

int SliderEditControl::AdjustValueRange(int InValue){
   return clamp(InValue, min, max);
}

SliderEditControl::SliderEditControl(HINSTANCE hInst, LPCWSTR CustomClassName):
   ClassName(CustomClassName)
   , pfnSliderPrc([](
      HWND hWnd
      , UINT uMsg
      , WPARAM wParam
      , LPARAM lParam
      , UINT_PTR uIdSubclass
      , DWORD_PTR dwRefData)->LRESULT{

         auto* self = reinterpret_cast<SliderEditControl*>(dwRefData);

         switch(uMsg){
            case WM_GETDLGCODE:
            {
               DPTD("pfnSliderPrc WM_GETDLGCODE");
               return DLGC_WANTALLKEYS;
            }

            case WM_PAINT:
            {
               PaintDC hdc(hWnd);

               if(!IsWindowVisible(hWnd)){
                  DPTD("pfnSliderPrc Continue to hide");
                  return 0;
               }

               if(self->IsEditShowState){
                  return 0;
               }

               /// @brief Slider描画
               {
                  {
                  /// @brief バックグランド描画
                     COLORREF bgColor =
                        (PtInRect(&self->rcSlider, self->ptLastPos) || self->Hovered)
                        ? RGB(127, 127, 127) : RGB(0, 0, 0);
                     unique_ptr<remove_pointer_t<HBRUSH>,decltype(DeleteObject)*> hBrush
                     { CreateSolidBrush(bgColor), DeleteObject };
                     FillRect(hdc, &self->rcSlider, hBrush.get());
                  }

                  /// @brief 半透明な赤線描画（中央の縦線）
                  {
                    // スライダー幅・高さ
                     int sliderWidth = self->rcSlider.right - self->rcSlider.left;
                     int sliderHeight = self->rcSlider.bottom - self->rcSlider.top;

                     // 現在値を取得
                     int value{};
                     self->GetValue(value);

                     // 比率を計算
                     double ratio = 0.0;
                     ratio = double(value - self->min) / double(self->max - self->min);
                     ratio = clamp(ratio, 0.0, 1.0);

                     // X座標を比率で決定（クライアント座標系）
                     int x = int(ratio * sliderWidth);

                     // 赤線描画
                     BLENDFUNCTION blend = { AC_SRC_OVER, 0, 128, 0 };
                     unique_ptr<remove_pointer_t<HDC>, decltype(DeleteDC)*> hMemDC
                     { CreateCompatibleDC(hdc),DeleteDC };
                     unique_ptr<remove_pointer_t< HBITMAP>, decltype(DeleteObject)*>hBmp
                     { CreateCompatibleBitmap(hdc, sliderWidth, sliderHeight), DeleteObject };

                     GdiSelectAndRestorer BmpRestrorer(hMemDC.get(), hBmp.get());
                     unique_ptr<remove_pointer_t<HBRUSH>, decltype(DeleteObject)*> hBrush
                     { CreateSolidBrush(RGB(255, 0, 0)),DeleteObject };

                     RECT lineRc = {
                        clamp(x ,0,sliderWidth - VALUE_LINE_WIDTH)
                        , self->rcSlider.top
                        , clamp(x + VALUE_LINE_WIDTH,VALUE_LINE_WIDTH,sliderWidth)
                        , self->rcSlider.bottom };
                     FillRect(hMemDC.get(), &lineRc, hBrush.get());

                     AlphaBlend(
                        hdc
                        , self->rcSlider.left
                        , self->rcSlider.top
                        , sliderWidth
                        , sliderHeight
                        , hMemDC.get()
                        , 0
                        , 0
                        , sliderWidth
                        , sliderHeight
                        , blend);
                  }

                  // 中央に数値を描画（白文字）
                  {
                     int value;
                     if(!self->GetValue(value)){
                        DPTD("GetValue failed");
                        return TRUE;
                     }
                     wstring text = to_wstring(value);
                     SetBkMode(hdc, TRANSPARENT);
                     SetTextColor(hdc, RGB(255, 255, 255));

                     {
                        GdiSelectAndRestorer<HFONT> FontRestorer(hdc, self->hFontCtm);
                        SIZE sz{};
                        GetTextExtentPoint32(hdc, text.c_str(), (int)text.size(), &sz);
                        int tx = (self->rcAll.right - sz.cx) / 2;
                        int ty = (self->rcSlider.bottom - sz.cy) / 2;

                        TextOut(hdc, tx, ty, text.c_str(), static_cast<int>(text.size()));
                     }

                     /// @brief focus rect
                     if(GetFocus() == hWnd){
                        RECT rcFocus;
                        GetClientRect(hWnd, &rcFocus);

                        unique_ptr<remove_pointer_t< HPEN>,decltype(DeleteObject)*> hPen
                        { CreatePen(PS_DOT, 1, RGB(255, 255, 255)),DeleteObject }; // 白の点線
                        GdiSelectAndRestorer<HGDIOBJ> PenRestorer(hdc, hPen.get());
                        GdiSelectAndRestorer<HGDIOBJ> BrushRestorer(hdc, GetStockObject(NULL_BRUSH));// 塗りつぶしなし

                        Rectangle(hdc, rcFocus.left, rcFocus.top, rcFocus.right, rcFocus.bottom);
                     }
                  }
               }
               return 0;
            }

            case WM_LBUTTONDOWN:
            {
               DPTD("pfnSliderPrc WM_LBUTTONDOWN");
               if(self->IsEditShowState)
                  return 0;
               const POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
               self->ptDowned = pt;
               self->ptLastPos = pt;
               SetFocus(hWnd);
               SetCapture(hWnd);
               if(PtInRect(&self->rcSlider, pt)){
                  SetCursor(LoadCursorW(NULL, IDC_CROSS));
               }
               InvalidateRect(hWnd, NULL, TRUE);
               UpdateWindow(hWnd);
               return 0;
            }

            case WM_MOUSEMOVE:
            {
               DPTD("pfnSliderPrc WM_MOUSEMOVE");

               if(self->IsEditShowState)
                  break;
               
               const POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };

               /// @brief WM_MOUSELEAVEを受け取る為に設定。
               if(!self->Hovered && PtInRect(&self->rcSlider, pt)){
                  TRACKMOUSEEVENT tme = { sizeof(tme), TME_HOVER | TME_LEAVE, hWnd,HOVER_DEFAULT };
                  if(!TrackMouseEvent(&tme)){
                     DPLE;
                     break;
                  }
                  self->Hovered = TRUE;
               }

               if(PtInRect(&self->rcSlider, self->ptDowned)){
                  self->DownedAndMoved = abs(pt.x - self->ptDowned.x) >= MoveBacklash;
                  int dx = GET_X_LPARAM(lParam) - self->ptLastPos.x;
                  int value;
                  if(!self->GetValue(value)){
                     DPTD("GetValue failed");
                     __debugbreak();
                     break;
                  }
                  if(!self->SetValue(value + dx)){
                     DPTD("SetValue failed");
                     __debugbreak();
                     break;
                  }
               }
               self->ptLastPos = pt;
               InvalidateRect(hWnd, &self->rcSlider, TRUE);
               UpdateWindow(hWnd);
               return 0;
            }

            case WM_LBUTTONUP:
            {
               DPTD("pfnSliderPrc WM_LBUTTONUP");
               const POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };

               if(GetCapture() == hWnd){
                  DPTD("ReleaseCapture()");
                  ReleaseCapture();
               }

               if(PtInRect(&self->rcSlider, self->ptDowned)){
                  if(!self->DownedAndMoved){
                     SetCursor(LoadCursorW(NULL, IDC_ARROW));
                     self->DownedAndMoved = FALSE;
                     self->ptLastPos = pt;
                     self->ptDowned = { -1, -1 };
                     self->SwitchEditShowState(TRUE);
                     InvalidateRect(hWnd, NULL, TRUE);
                     UpdateWindow(hWnd);
                     return 0;
                  }
               }

               if(PtInRect(&self->rcSlider, pt)){
                  SetCursor(LoadCursorW(NULL, IDC_SIZEWE));
               } else{
                  SetCursor(LoadCursorW(NULL, IDC_ARROW));
               }

               self->DownedAndMoved = FALSE;
               self->ptLastPos = pt;
               self->ptDowned = { -1, -1 };
               InvalidateRect(hWnd, NULL, TRUE);
               UpdateWindow(hWnd);
               return 0;
            }

            case WM_CAPTURECHANGED:
            {
               DPTD("pfnSliderPrc WM_CAPTURECHANGED");
               break;
            }

            case WM_MOUSEHOVER:
            {
               DPTD("pfnSliderPrc WM_MOUSEHOVER");
               if(self->Hovered){
                  TRACKMOUSEEVENT tme = { sizeof(tme), TME_HOVER | TME_LEAVE, hWnd,HOVER_DEFAULT };
                  if(!TrackMouseEvent(&tme)){
                     DPLE;
                     break;
                  }
               }
               return 0;
            }

            case WM_MOUSELEAVE:
            {
               DPTD("pfnSliderPrc WM_MOUSELEAVE");
               self->ptLastPos = { -1,-1 };
               self->Hovered = FALSE;
               InvalidateRect(hWnd, NULL, TRUE);
               UpdateWindow(hWnd);
               return 0;
            }

            case WM_SETCURSOR:
            {
               DPTD("pfnSliderPrc WM_SETCURSOR");

               if(!self->IsEditShowState){
                  POINT pt;
                  GetCursorPos(&pt);
                  MapWindowPoints(NULL, hWnd, &pt, 1);
                  if(PtInRect(&self->rcSlider, self->ptLastPos)){
                     if(PtInRect(&self->rcSlider, self->ptDowned))
                        SetCursor(LoadCursorW(nullptr, IDC_CROSS));
                     else
                        SetCursor(LoadCursorW(NULL, IDC_SIZEWE));
                     return 0;
                  }
                  SetCursor(LoadCursor(nullptr, IDC_ARROW));
                  return 0;
               }
               break;
            }

            case WM_KEYDOWN:
            {
               DPTD("pfnSliderPrc WM_KEYDOWN");
               switch(wParam){
                  case VK_RETURN:
                  {
                     DPTD("pfnSliderPrc VK_RETURN");
                     self->SwitchEditShowState(TRUE);
                     return 0;
                  }

                  case VK_TAB:
                  {
                     DPTD("pfnSliderPrc VK_TAB");
                     BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                     HWND hwndNext = GetNextDlgTabItem(GetParent(hWnd), hWnd, bShift);
                     if(hwndNext){
                        SetFocus(hwndNext);
                     }
                     return 0;
                  }

                  case VK_RIGHT:
                  {
                     DPTD("pfnSliderPrc VK_RIGHT");
                     BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                     if(!bShift){
                        int value;
                        self->GetValue(value);
                        value++;
                        self->SetValue(value);
                        InvalidateRect(hWnd, &self->rcSlider, TRUE);
                     }
                     return 0;
                  }

                  case VK_LEFT:
                  {
                     DPTD("pfnSliderPrc VK_LEFT");
                     BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                     if(!bShift){
                        int value;
                        self->GetValue(value);
                        value--;
                        self->SetValue(value);
                        InvalidateRect(hWnd, &self->rcSlider, TRUE);
                     }
                     return 0;
                  }
               }
            }
            break;

            case WM_SETFOCUS:
            {
               DPTD("pfnSliderPrc WM_SETFOCUS");
               InvalidateRect(hWnd, &self->rcSlider, TRUE);
               return 0;
            }

            case WM_KILLFOCUS:
            {
               DPTD("pfnSliderPrc WM_KILLFOCUS");
               self->Hovered = FALSE;
               self->ptLastPos = { -1,-1 };
               self->ptDowned = { -1,-1 };
               self->DownedAndMoved = FALSE;
               InvalidateRect(hWnd, &self->rcSlider, TRUE);
               return 0;
            }

            case WM_SIZE:
            {
               DPTD("pfnSliderPrc WM_SIZE");
               self->UpdateLayoutRects(lParam);
               if(self->hFontCtm){
                  DeleteObject(self->hFontCtm);
               }
               self->hFontCtm = self->GetFontAdjusted(hWnd, &self->rcAll);
               break;
            }

            case WM_COMMAND:
            {
               DPTD("pfnSliderPrc WM_COMMAND");
               switch(LOWORD(wParam)){
                  case  self->IDC_BUTTON_MINUS:
                  {
                     DPTD("pfnSliderPrc IDC_BUTTON_MINUS");
                     int val;
                     self->GetValue(val);
                     --val;
                     self->SetValue(val);
                     InvalidateRect(hWnd, &self->rcSlider, TRUE);
                     SetFocus(hWnd);
                     UpdateWindow(hWnd);
                     return 0;
                  }

                  case self->IDC_BUTTON_PLUS:
                  {
                     DPTD("pfnSliderPrc IDC_BUTTON_PLUS");
                     int val;
                     self->GetValue(val);
                     ++val;
                     self->SetValue(val);
                     SetFocus(hWnd);
                     InvalidateRect(hWnd, &self->rcSlider, TRUE);
                     return 0;
                  }

                  case self->IDC_EDIT:
                  {
                     DPTD("pfnSliderPrc IDC_EDIT");
                     WORD notifyCode = HIWORD(wParam);
                     DPTD(format("pfnSliderPrc notifyCode = {}", notifyCode).c_str());

                     SendMessage(
                        self->hDlgParent
                        , WM_COMMAND
                        , MAKEWPARAM(self->nIDDlgItem, notifyCode)
                        , (LPARAM)hWnd);
                     return 0;
                  }

                  default: { break; }
                         break;
               }

               if(self->IsEditShowState)
                  SetFocus(self->hEdit);
               else
                  SetFocus(hWnd);
               
               return 0;
            }

            case WM_SETTEXT:
            {
               DPTD("pfnSliderPrc WM_SETTEXT");
               const wchar_t* newText = reinterpret_cast<const wchar_t*>(lParam);
               int value = 0;
               if(swscanf_s(newText, L"%d", &value) == 1){
                  self->SetValue(value);
                  return TRUE;
               } else{
                  DPTD("It's not a number.");
                  return FALSE;
               }
            }

            case WM_DESTROY:
            {
               DPTD("pfnSliderPrc WM_DESTROY");
               SendMessage(self->hEdit, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
               SendMessage(self->hMinusButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
               SendMessage(self->hPlusButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
               DeleteObject(self->hFontCtm);
               self->hFontCtm = NULL;
               self->initialized = FALSE;
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnSliderPrc WM_NCDESTROY");
               if(!::RemoveWindowSubclass(hWnd, self->pfnSliderPrc, uIdSubclass))
                   DPTD(" failed");
               break;
               return FALSE;
            }
         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      })

   , pfnEditPrc([](
      HWND hWnd
      , UINT uMsg
      , WPARAM wParam
      , LPARAM lParam
      , UINT_PTR uIdSubclass
      , DWORD_PTR dwRefData)->LRESULT{

         auto* self = reinterpret_cast<SliderEditControl*>(dwRefData);

         switch(uMsg){
            case WM_PAINT:
            {
               DPTD("pfnEditPrc WM_PAINT");
               break;
            }

            case WM_SIZE:
            {
               DPTD("pfnEditPrc WM_SIZE");
            }
            break;

            case WM_GETDLGCODE:
            {
               DPFTD("pfnEditPrc WM_GETDLGCODE");
               return DLGC_WANTALLKEYS;
            }
            case WM_CHAR:
            {
               DPTD("pfnEditPrc WM_CHAR");
               switch(wParam){
                  case  L'\r':
                  {
                     DPTD("pfnEditPrc L\'\\r\'");
                     return 1;
                     break;
                  }
                  case L'\t':
                  {
                     DPTD("pfnEditPrc L\'\\t\'");
                     return 0;
                     break;
                  }
                  case 0x08:// バックスペース
                  {
                     DPTD("pfnEditPrc 0x08 Back Space");
                     break;
                  }

                  default:
                  {
                     break;
                  }
               }
               ::PostMessage(self->hEdit, WM_ADJUSTVALUE, NULL, NULL);
               break;
            }

            case WM_MOUSEMOVE:
               DPTD("pfnEditPrc WM_MOUSEMOVE");
               break;

            case WM_MOUSEHOVER:
               DPTD("pfnEditPrc WM_MOUSEHOVER");
               break;

            case WM_MOUSELEAVE:
               DPTD("pfnEditPrc WM_MOUSELEAVE");
               self->Hovered = FALSE;
               break;

            case WM_KEYDOWN:
               DPTD("pfnEditPrc WM_KEYDOWN");
               switch(wParam){
                  case VK_RETURN:
                  case VK_TAB:
                  {
                     DPTD("VpfnEditPrc K_RETURN VK_TAB");
                     self->SwitchEditShowState(FALSE);
                     return 0;
                  }

                  case VK_UP:
                  {
                     DPTD("pfnEditPrc VK_UP");
                     int Value;
                     self->GetValue(Value);
                     ++Value;
                     self->SetValue(Value);
                     int len = ::GetWindowTextLength(hWnd);
                     ::SendMessage(hWnd, EM_SETSEL, len, len);
                     return 0;
                  }

                  case VK_DOWN:
                  {
                     DPTD("pfnEditPrc VK_DOWN");
                     int Value;
                     self->GetValue(Value);
                     --Value;
                     self->SetValue(Value);
                     int len = ::GetWindowTextLength(hWnd);
                     ::SendMessage(hWnd, EM_SETSEL, len, len);
                     return 0;
                  }
                  case VK_BACK:
                  {
                     DPTD("pfnEditPrc VK_BACK");
                     break;
                  }
                  case VK_DELETE:
                  {
                     DPTD("pfnEditPrc VK_DELETE");
                     break;
                  }
                  default: {}
               }
               break;

            case WM_LBUTTONDOWN:
            {
               DPTD("pfnEditPrc WM_LBUTTONDOWN");
               POINT pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

               RECT rcLocal;
               GetWindowRect(hWnd, &rcLocal);
               MapWindowRect(NULL, hWnd, &rcLocal);
               if(!PtInRect(&rcLocal, pt)){
                  if(GetCapture() == hWnd)
                     ReleaseCapture();
                  self->SwitchEditShowState(FALSE);
                  POINT ptDlg = pt;
                  MapWindowPoints(hWnd, self->hDlgParent, &ptDlg, 1);

                  HWND hChild = ChildWindowFromPointEx(
                     self->hDlgParent
                     , ptDlg
                     , CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
                  if(hChild){
                     SetFocus(hChild);
                     POINT ptOther = ptDlg;
                     MapWindowPoints(self->hDlgParent, hChild, &ptOther, 1);
                     PostMessageW(hChild, WM_LBUTTONDOWN, wParam, MAKELPARAM(ptOther.x, ptOther.y));
                  }
               }
               break;
            }

            case WM_LBUTTONUP:
            {
               DPTD("pfnEditPrc WM_LBUTTONUP");
               break;
            }

            case WM_CAPTURECHANGED:
            {
               DPTD("pfnEditPrc WM_CAPTURECHANGED");
               HWND hWnd = GetCapture();
               DPTD(string("Handle Is ") + to_string((UINT_PTR)hWnd));
               DPTD(string("CtlHandle is ") + to_string((UINT_PTR)self->hWnd));
               break;
            }

            case WM_SETFOCUS:
            {
               DPTD("pfnEditPrc WM_SETFOCUS");
               break;
            }

            case WM_KILLFOCUS:
            {
               DPTD("pfnEditPrc WM_KILLFOCUS");
               {
                  if(GetCapture() == hWnd)
                     ReleaseCapture();

                  self->SwitchEditShowState(FALSE);
                  return 0;
               }
               break;
            }

            case WM_ADJUSTVALUE:
            {
               DPTD("pfnEditPrc WM_ADJUSTVALUE");
               int value;
               self->GetValue(value);
               self->SetValue(value);
               InvalidateRect(self->hEdit, NULL, TRUE);
               return 0;
            }

            case WM_DESTROY:
            {
               DPTD("pfnEditPrc WM_DESTROY");
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnEditPrc WM_NCDESTROY");
               if(!::RemoveWindowSubclass(hWnd, self->pfnEditPrc, uIdSubclass))
                   DPTD(" failed");
               break;
            }
         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      })

      , pfnButtonPrc([](
      HWND hWnd
      , UINT uMsg
      , WPARAM wParam
      , LPARAM lParam
      , UINT_PTR uIdSubclass
      , DWORD_PTR dwRefData)->LRESULT{

         SliderEditControl* self = reinterpret_cast<SliderEditControl*>(dwRefData);
         switch(uMsg){
            case WM_GETDLGCODE:
            {
               return DLGC_WANTALLKEYS;
            }

            case WM_PAINT:
            {
               DPTD("pfnButtonPrc WM_PAINT");
               return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }


            case WM_LBUTTONDOWN:
            {
               DPTD("pfnButtonPrc WM_LBUTTONDOWN");
               SetCapture(hWnd);
               self->isInitialDelayPassed = FALSE;
               if(!SetTimer(hWnd, TIMER_ID_AUTOREPEAT, AUTOREPEAT_INITIAL_DELAY, NULL)){
                  DPLE;
               }
               return 0;
            }
            case WM_LBUTTONUP:
            {
               DPTD("pfnButtonPrc WM_LBUTTONUP");

               ReleaseCapture();
               KillTimer(hWnd, TIMER_ID_AUTOREPEAT);
               if(self->isInitialDelayPassed){
                  self->isInitialDelayPassed = FALSE;
               } else{
                  SendMessage(self->hWnd, WM_COMMAND,
                              MAKEWPARAM(uIdSubclass, BN_CLICKED),
                              (LPARAM)hWnd);
               }
               break;
            }

            case WM_TIMER:
            {
               DPTD("pfnButtonPrc WM_TIMER");
               if(wParam == TIMER_ID_AUTOREPEAT){
                  if(!self->isInitialDelayPassed){
                     self->isInitialDelayPassed = TRUE;
                     SetTimer(hWnd, TIMER_ID_AUTOREPEAT, AUTOREPEAT_INTERVAL, NULL);
                  }

                  SendMessage(self->hWnd, WM_COMMAND,
                              MAKEWPARAM(GetDlgCtrlID(hWnd), BN_CLICKED),
                              (LPARAM)hWnd);
               }
               break;
            }

            case WM_CAPTURECHANGED:
            {
               DPTD("pfnButtonPrc WM_CAPTURECHANGED");
               break;
            }

            case WM_SETFOCUS:
            {
               DPTD("pfnButtonPrc WM_SETFOCUS");
               SetFocus(self->hWnd);
               break;
            }

            case WM_KILLFOCUS:
            {
               DPTD("pfnButtonPrc WM_KILLFOCUS");
               break;
            }

            case WM_DESTROY:
            {
               DPTD("pfnButtonPrc WM_DESTROY");
               break;
            }

            case WM_NCDESTROY:
            {
               DPTD("pfnButtonPrc WM_NCDESTROY");
               if(!::RemoveWindowSubclass(hWnd, self->pfnButtonPrc, uIdSubclass))
                  DPTD(" failed");
               break;
            }
         }


         return DefSubclassProc(hWnd, uMsg, wParam, lParam);

      })

{
   WNDCLASSEXW wndcls{};
   if(::GetClassInfoExW(hInst, ClassName.c_str(), &wndcls))
      return;

   wndcls.cbSize = sizeof(WNDCLASSEXW);
   wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
   wndcls.lpfnWndProc = ::DefWindowProc;
   wndcls.cbClsExtra = 0;
   wndcls.cbWndExtra = DLGWINDOWEXTRA;
   wndcls.hInstance = hInst;
   wndcls.hIcon = NULL;
   wndcls.hCursor = ::LoadCursorW(hInst, IDC_ARROW);
   wndcls.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
   wndcls.lpszMenuName = NULL;
   wndcls.lpszClassName = ClassName.c_str();

   if(!(atmCtmCtl = RegisterClassExW(&wndcls))){
      DPLE;
   }
   return;
}

SliderEditControl::~SliderEditControl(){
   UnregisterClassW(ClassName.c_str(), hInst);
}

BOOL SliderEditControl::SetWindowSubclass(HWND hParentDlg, int InctlID){
   hDlgParent = hParentDlg;
   if(initialized) return FALSE;
   nIDDlgItem = InctlID;


   hWnd = GetDlgItem(hDlgParent,nIDDlgItem);

   if(!::SetWindowSubclass(
      hWnd
      , pfnSliderPrc
      , (UINT_PTR)hWnd
      , (DWORD_PTR)this)){
      return FALSE;
   };

   UpdateLayoutRects();
   hFontCtm = GetFontAdjusted(hWnd, &rcAll);

   if(!(hEdit = CreateWindowExW(
      WS_EX_CLIENTEDGE
      , L"EDIT"
      , L""
      , WS_CHILD | WS_BORDER
      |  ES_CENTER | ES_NUMBER
      , rcSlider.left
      , rcSlider.top
      , rcSlider.right - rcSlider.left
      , rcSlider.bottom - rcSlider.top
      , hWnd
      , (HMENU)IDC_EDIT
      , hInst
      , this
   ))){
      DPFTD("hEdit = CreateWindowExW failed");
      return FALSE;
   }

   if(!::SetWindowSubclass(
      hEdit
      , pfnEditPrc
      , (UINT_PTR)hEdit
      , (DWORD_PTR)this)){
      return FALSE;
   };

   SendMessage(hEdit, WM_SETFONT, (WPARAM)hFontCtm, TRUE);

   if(!(hMinusButton = CreateWindowExW(
      0
      , L"BUTTON"
      , L"-"
      , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER
      , rcMinus.left
      , rcMinus.top
      , rcMinus.right - rcMinus.left
      , rcMinus.bottom - rcMinus.top
      , hWnd
      , (HMENU)IDC_BUTTON_MINUS
      , hInst
      , NULL
   ))){
      DPFTD("hMinusButton = CreateWindowExW failed");
      return FALSE;
   }

   if(!::SetWindowSubclass(
      hMinusButton
      , pfnButtonPrc
      , (UINT_PTR)IDC_BUTTON_MINUS
      , (DWORD_PTR)this)){
      return FALSE;
   };

   if(!(hPlusButton = CreateWindowExW(
      0
      , L"BUTTON"
      , L"+"
      , WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
      , rcPlus.left
      , rcPlus.top
      , rcPlus.right - rcPlus.left
      , rcPlus.bottom - rcPlus.top
      , hWnd
      , (HMENU)IDC_BUTTON_PLUS
      , hInst
      , NULL
   ))){
      DPFTD("hPlusButton = CreateWindowExW failed");
      return FALSE;
   }

   if(!::SetWindowSubclass(
      hPlusButton
      , pfnButtonPrc
      , (UINT_PTR)IDC_BUTTON_PLUS
      , (DWORD_PTR)this)){
      return FALSE;
   };

   initialized = TRUE;
   return TRUE;
}

BOOL SliderEditControl::SetValue(int Inval){
   int val = AdjustValueRange(Inval);
   wstring wstr{ to_wstring(val) };
   if(!::SetWindowTextW(hEdit, wstr.c_str())){
      DPLE;
      return FALSE;
   }
   ::SendMessage(hEdit, EM_SETSEL, wstr.size(), wstr.size());
   return TRUE;
}

BOOL SliderEditControl::GetValue(int& val){
   wstring wstr(32, L'\0');
   if(int num; !(num = ::GetWindowTextW(hEdit, wstr.data(), (int)wstr.size()))){
      DPLE;
      return FALSE;
   } else{
      wstr.resize(num);
      val = stoi(wstr);
      return TRUE;
   }
}

BOOL SliderEditControl::SetRange(int min, int max){
   if(min > max){
      DPTD("An incorrect value was specified.");
      return FALSE;
   }

   int newpos;
   if(!GetValue(newpos)){
      return FALSE;
   }
   this->min = min;
   this->max = max;
   newpos = AdjustValueRange(newpos);
   if(!SetValue(newpos)){
      return FALSE;
   }
   return TRUE;
}


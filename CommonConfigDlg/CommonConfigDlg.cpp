/**
 * @file CommonConfigDlg.cpp
 * @brief CommonConfigDlg�̎���
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * ���̃t�@�C�����̂��ׂẴR�[�h�́A���ɖ��L����Ă��Ȃ�����AMIT���C�Z���X�ɏ]���܂��B
 */
#include "CommonConfigDlg.h"
#include "resource.h"
#include "../DebugFnc/DebugFnc.h"
#include "..\CommonLib\CommonLib.h"
#include "..\UtilDefAviUtl2\UtilDefAviUtl2.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../SliderEditControl/" _CRT_STRINGIZE($CONFIGURATION) "/SliderEditControl-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../UtilDefAviUtl2/" _CRT_STRINGIZE($CONFIGURATION) "/UtilDefAviUtl2-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

#pragma once
using namespace std;
using namespace UtilDefAviUtl2;
using namespace CommonLib;

extern "C" IMAGE_DOS_HEADER __ImageBase;

CommonConfigDlg::CommonConfigDlg():
   hInst(reinterpret_cast<HMODULE>(&__ImageBase))
   , pfnSubClass([](
      HWND hWnd
      , UINT uMsg
      , WPARAM wParam
      , LPARAM lParam
      , UINT_PTR uIdSubclass
      , DWORD_PTR dwRefData)->LRESULT{
         CommonConfigDlg* self = (CommonConfigDlg*)dwRefData;
         switch(uMsg){
            case WM_COMMAND:
            {
               DPTD("pfnSubClass WM_COMMAND");
               HWND hwndCtrl = (HWND)lParam;
               WORD id = LOWORD(wParam);
               WORD code = HIWORD(wParam);

               switch(id){
                  case IDOK:
                     if(code == BN_CLICKED){
                        self->bUpdate = TRUE;
                        EndDialog(hWnd, id);
                        return TRUE;
                     }
                  case IDCANCEL:
                  {
                     if(code == BN_CLICKED){
                        EndDialog(hWnd, id);
                        return TRUE;
                     }
                     break;
                  }
                  case IDC_EDIT_RATE:
                  case IDC_EDIT_SCALE:
                     switch(HIWORD(wParam)){
                        case EN_CHANGE:
                        {
                           /// FPS�ݒ�
                           {
                              int iRate, iScale;
                              self->InsSERate.GetValue(iRate);
                              self->InsSEScale.GetValue(iScale);
                              SendMessageW(GetDlgItem(hWnd, IDC_STATIC_FPS), WM_SETTEXT, 0, (LPARAM)CalcFPStoStringW(iRate, iScale).c_str());
                           }
                           break;
                        }
                     }
                     break;
               }
               break;
            }

            case WM_NOTIFY:
            {
               DPTD("pfnSubClass WM_NOTIFY");
               switch(((LPNMHDR)lParam)->idFrom){
                  case IDC_LIST_FILES:
                  {
                     DPTD("pfnSubClass IDC_LIST_FILES");
                     switch(((LPNMITEMACTIVATE)lParam)->hdr.code){
                        case NM_CLICK:
                        {
                           DPTD("pfnSubClass NM_CLICK");
                           LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)lParam;
                           if(pia->iItem >= 0 && pia->iSubItem == 0){
                              // �u�ݒ�v��̃A�C�e�����N���b�N���ꂽ
                              self->LunchFileConfigDlg(
                                 hWnd
                                 , pia->hdr.hwndFrom
                                 , pia->iItem
                              );
                           }
                           break;
                        }
                        case NM_DBLCLK:
                        {
                           DPTD("pfnSubClass NM_DBLCLK");
                           LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)lParam;
                           if(pia->iItem >= 0 /*&& pia->iSubItem == 0*/){
                              // �u�ݒ�v��̃A�C�e�����N���b�N���ꂽ
                              self->LunchFileConfigDlg(
                                 hWnd
                                 , pia->hdr.hwndFrom
                                 , pia->iItem
                              );
                           }
                           break;
                        }
                     }
                     break;
                  }
                  break;
               }
               break;
            }

            case WM_LBUTTONDOWN:
            {
               SetFocus(hWnd);
               break;
            }

            case WM_DESTROY:
            {
               if(self->bUpdate){
                  /// �f�t�H���g�ݒ�̍X�V
                  map<wstring, wstring>mapUpdate;
                  {
                     int value;
                     self->InsSEDot.GetValue(value);
                     mapUpdate.insert({ L"N_DOTS",to_wstring(value) });
                  }
                  {
                     int value;
                     self->InsSERate.GetValue(value);
                     mapUpdate.insert({ L"RATE",to_wstring(value) });
                  }
                  {
                     int value;
                     self->InsSEScale.GetValue(value);
                     mapUpdate.insert({ L"SCALE",to_wstring(value) });
                  }
                  WriteMapToIniW(
                     L"DEFAULT_INITIAL_FILE_SETTINGS"
                     , mapUpdate
                     , MakeIniPathNameW(self->hTargetModule));

                  /// ���X�g�̍X�V
                  int numItem = (int)::SendMessageW(GetDlgItem(hWnd, IDC_LIST_FILES), LVM_GETITEMCOUNT, 0, 0L);
                  for(int i = 0; i < numItem; ++i){
                     map OneItem = self->GetOneLineItemOfListViewW(GetDlgItem(hWnd, IDC_LIST_FILES), i);
                     wstring binPathFile = OneItem[L"FILE_PATH_NAME"];
                     wstring B64PathFile = BinToB64W(WtoBin(binPathFile));
                     OneItem[L"FILE_PATH_NAME"] = B64PathFile;
                     WriteMapToIniW(
                        B64PathFile
                        , OneItem
                        , MakeIniPathNameW(self->hTargetModule));
                  }
               }
               break;
            }
            case WM_NCDESTROY:
            {
               if(self->bUpdate){
               }
               break;
            }
         }
         return DefSubclassProc(hWnd, uMsg, wParam, lParam);
      })
   , InsSEDot(hInst, SLIDER_EDIT_CLASS_NAME)
   , InsSERate(hInst, SLIDER_EDIT_CLASS_NAME)
   , InsSEScale(hInst, SLIDER_EDIT_CLASS_NAME)
{
   INITCOMMONCONTROLSEX initctlex{
      sizeof(INITCOMMONCONTROLSEX)
   , ICC_WIN95_CLASSES };
   InitCommonControlsEx(&initctlex);
}

INT_PTR CommonConfigDlg::DialogBoxParamW(HWND  hWndParent, HMODULE hTarget){
   hTargetModule = hTarget;
   INT_PTR result = ::DialogBoxParamW(
      hInst
      , MAKEINTRESOURCEW(IDD_DLG_CONFIG_MAIN)
      , hWndParent
      , [](
         HWND hWnd
         , UINT uMsg
         , WPARAM wParam
         , LPARAM lParam)->INT_PTR{
            CommonConfigDlg* self = reinterpret_cast<CommonConfigDlg*>(lParam);
            switch(uMsg){
               case WM_INITDIALOG:
                  DPTD("WM_INITDIALOG");

                  /// �����r���ŃG���[���o����_�C�A���O�����
                  unique_ptr<remove_pointer_t<HWND>, void(*)(HWND)> CloseIfErr
                  { hWnd,[](HWND h){EndDialog(h, IDCANCEL); } };

                  /// ���X�g�r���[�����ݒ�
                  {
                     HWND hListView = GetDlgItem(hWnd, IDC_LIST_FILES);
                      /// �w�b�_�[�����ݒ�
                     {
                        HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
                        HBITMAP hBmp = LoadPngFromResource(self->hInst, IDR_GEARPNG);
                        if(hBmp){
                           int index = ImageList_Add(hImageList, hBmp, NULL);
                           DeleteObject(hBmp); // �R�s�[�ς݂Ȃ̂Ŕj������OK
                        }
                        ListView_SetImageList(hListView, hImageList, LVSIL_SMALL);
                        ListView_SetExtendedListViewStyle(
                           hListView
                           , ListView_GetExtendedListViewStyle(hListView) | LVS_EX_FULLROWSELECT);
                        //�_�~�[
                        int IndexCol = 0;
                        LVCOLUMNW lvc = {};
                        lvc.mask = LVCF_WIDTH;
                        lvc.cx = 0;  // ��0�Ō����Ȃ�����
                        if((IndexCol = ListView_InsertColumn(hListView, IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                         //�ݒ�
                        lvc = {};
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 60;
                        lvc.pszText = const_cast<LPWSTR>(L"�ݒ�");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        // �t�@�C�����i���͌�Œ�������̂ŉ��l�j
                        lvc = {};
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 0;
                        lvc.pszText = const_cast<LPWSTR>(L"�t�@�C����");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        // �T�C�Y
                        lvc = {};
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 80;
                        lvc.pszText = const_cast<LPWSTR>(L"�h�b�g��");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        // Rate
                        lvc = {};
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 60;
                        lvc.pszText = const_cast<LPWSTR>(L" Rate ");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        // Scale
                        lvc = {};
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 60;
                        lvc.pszText = const_cast<LPWSTR>(L" Scale ");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        // FPS
                        lvc = {};
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 60;
                        lvc.pszText = const_cast<LPWSTR>(L" FPS ");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        //�_�~�[����
                        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                        lvc.fmt = LVCFMT_CENTER;
                        lvc.cx = 60;
                        lvc.pszText = const_cast<LPWSTR>(L"");
                        if((IndexCol = ListView_InsertColumn(hListView, ++IndexCol, &lvc)); IndexCol == -1){
                           DPT("ListView Col Error");
                           return FALSE;
                        }

                        //�擪�_�~�[�폜
                        ListView_DeleteColumn(hListView, 0);

                        int colCount = Header_GetItemCount(ListView_GetHeader(hListView));

                        // �e����w�b�_�[���Ƀt�B�b�g
                        for(int i = 0; i < colCount; i++){
                           ListView_SetColumnWidth(hListView, i, LVSCW_AUTOSIZE_USEHEADER);
                        }

                        //�����_�~�[���擾
                        int Width = ListView_GetColumnWidth(hListView, colCount - 1);
                        //�����_�~�[�폜
                        ListView_DeleteColumn(hListView, colCount - 1);
                        //�t�@�C�������ɉ��Z
                        Width += ListView_GetColumnWidth(hListView, COL_FILENAME);
                        // �t�@�C������̕���ύX
                        ListView_SetColumnWidth(hListView, COL_FILENAME, Width);
                     }

                     /// @brief �A�C�e���ǉ�
                     {
                        unique_ptr<WCHAR[]> buffer(new(nothrow) WCHAR[SECTION_NAME_BUFFER_SIZE]); // �Z�N�V�������ꗗ���i�[����o�b�t�@
                        if(!buffer){
                           DPT("Allocation failed.");
                           ::MessageBoxA(hWnd, "Allocation failed.", "Error", MB_ICONEXCLAMATION | MB_OK);
                           return TRUE;
                        }

                        CleanInvalidSectionsW(IgnoreList, MakeIniPathNameW(self->hTargetModule));
                        DWORD len = ::GetPrivateProfileSectionNamesW(
                           buffer.get()
                           , SECTION_NAME_BUFFER_SIZE
                           , MakeIniPathNameW(self->hTargetModule).c_str());
                        if(len == 0){
                           DPT("INI file could not be read.");
                           ::MessageBoxA(hWnd, "INI file could not be read.", "Error", MB_ICONEXCLAMATION | MB_OK);
                           return FALSE; // �ǂݍ��ݎ��s
                        }
                        const WCHAR* p = buffer.get();

                        while(*p){
                           wstring sectionName = p;
                           p += sectionName.size() + 1;

                            //ignoreList �Ɋ܂܂��Z�N�V�����̓X�L�b�v
                           if(find(IgnoreList.begin(), IgnoreList.end(), sectionName) != IgnoreList.end())
                              continue;

                           map sectionMap = ReadIniToMapW(sectionName, MakeIniPathNameW(self->hTargetModule));
                           {
                              // 0�s�ڂɃA�C�e����ǉ������
                              LVITEMW lvi = {};
                              lvi.mask = LVIF_TEXT | LVIF_IMAGE;  // �e�L�X�g�ƃC���[�W��ݒ�
                              lvi.iItem = ListView_GetItemCount(hListView);                      // �s�ԍ�
                              lvi.iSubItem = 0;                   // ��ԍ��i�ݒ��j
                              lvi.pszText = const_cast<LPWSTR>(L"");
                              lvi.iImage = 0;                     // ImageList_AddIcon �Œǉ������ŏ��̃A�C�R���̓C���f�b�N�X0
                              int rowIndex = ListView_InsertItem(hListView, &lvi);

                              // 2��ځi�t�@�C������j
                              wstring wstr;
                              wstr = BinToW(B64WtoBin(sectionMap[self->ColString.at(COL_FILENAME)]));
                              ListView_SetItemText(
                                 hListView
                                 , rowIndex
                                 , COL_FILENAME
                                 , wstr.data());

                              // 3��ځi��ӂ̃h�b�g���j
                              wstr = sectionMap[self->ColString.at(COL_N_DOTS)];
                              ListView_SetItemText(
                                 hListView
                                 , rowIndex
                                 , COL_N_DOTS
                                 , wstr.data());

                              // 4��ځiRate�j
                              wstr = sectionMap[self->ColString.at(COL_RATE)];
                              ListView_SetItemText(
                                 hListView
                                 , rowIndex
                                 , COL_RATE
                                 , wstr.data());

                              // 5��ځiScale�j
                              wstr = sectionMap[self->ColString.at(COL_SCALE)];
                              ListView_SetItemText(
                                 hListView
                                 , rowIndex
                                 , COL_SCALE
                                 , wstr.data());

                              // 6��ځiFPS�j
                              wstr = CalcFPStoStringW(stoi(sectionMap[L"RATE"]), stoi(sectionMap[L"SCALE"]));
                              ListView_SetItemText(
                                 hListView
                                 , rowIndex
                                 , COL_FPS
                                 , wstr.data());
                           }
                        }
                     }
                  }

                  /// �X���C�_�[�G�f�B�b�g�ݒ�
                  {
                     map<wstring, wstring> DefaultSetteingsMap =
                        ReadIniToMapW(L"DEFAULT_INITIAL_FILE_SETTINGS", MakeIniPathNameW(self->hTargetModule));
                     self->InsSEDot.SetWindowSubclass(hWnd, IDC_EDIT_NUM_DOTS);
                     self->InsSEDot.SetValue(stoi(DefaultSetteingsMap[L"N_DOTS"]));
                     self->InsSEDot.SetRange(DOT_RANGE_LOW, DOT_RANGE_HIGH);
                     self->InsSERate.SetWindowSubclass(hWnd, IDC_EDIT_RATE);
                     self->InsSERate.SetValue(stoi(DefaultSetteingsMap[L"RATE"]));
                     self->InsSERate.SetRange(RATE_RANGE_LOW, RATE_RANGE_HIGH);
                     self->InsSEScale.SetWindowSubclass(hWnd, IDC_EDIT_SCALE);
                     self->InsSEScale.SetValue(stoi(DefaultSetteingsMap[L"SCALE"]));
                     self->InsSEScale.SetRange(SCALE_RANGE_LOW, SCALE_RANGE_HIGH);
                  }

                  /// FPS�ݒ�
                  {
                     int iRate, iScale;
                     self->InsSERate.GetValue(iRate);
                     self->InsSEScale.GetValue(iScale);
                     SendMessageW(GetDlgItem(hWnd, IDC_STATIC_FPS), WM_SETTEXT, 0, (LPARAM)CalcFPStoStringW(iRate, iScale).c_str());
                  }

                  if(!SetWindowSubclass(
                     hWnd
                     , self->pfnSubClass
                     , (UINT_PTR)self
                     , (DWORD_PTR)self)){
                     DPT("SetWindowSubclass failed");
                     return FALSE;
                  }
                  /// �G���[�����������̂Ń����[�X
                  CloseIfErr.release();
                  return TRUE;
            }
            return 0; }
   , (INT_PTR)this);

   if(result == -1){
      DPLE;
   };
   return result;

}

BOOL CommonConfigDlg::LunchFileConfigDlg(
   HWND hWndParent
   , HWND hWndListView
   , int iItem){
   map ConfigLine = GetOneLineItemOfListViewW(hWndListView, iItem);

   if(int result = (int)InsFileCfg.DialogBoxParamW(hWndParent, ConfigLine); result != 1){
      DPT(format("DialogBoxParamW Error.{}", result));
      return FALSE;
   }

   map update = InsFileCfg.GetConfigW();
   UpdateOneLineItemOfListViewW(hWndListView, iItem, update);
   /// FPS�X�V
   {
      wstring buf = CalcFPStoStringW(stod(update[L"RATE"]), stod(update[L"SCALE"]));
      LVITEM lvi = {};
      lvi.iSubItem = COL_FPS;//��w��
      lvi.pszText = buf.data();
      lvi.cchTextMax = (int)buf.size() + 1;
      // �e�L�X�g���X�V
      if(!SendMessage(
         hWndListView
         , LVM_SETITEMTEXT
         , (WPARAM)iItem
         , (LPARAM)&lvi)){
         DPLE;
         return FALSE;
      }
   }
   return TRUE;
}

map<wstring, wstring>
CommonConfigDlg::GetOneLineItemOfListViewW(HWND hWndListView,int iItem) const{
   map<wstring, wstring> ConfigLine;

   for(int i = COL_FILENAME; COL_SCALE >= i; ++i){
      wstring buf(MAX_PATH, L'\0');
      LVITEM lvi = {};
      lvi.iSubItem = i;//��w��
      lvi.pszText = buf.data();
      lvi.cchTextMax = (int)buf.capacity();
      // �e�L�X�g���擾
      buf.resize((int)SendMessage(
         hWndListView
         , LVM_GETITEMTEXT
         , (WPARAM)iItem
         , (LPARAM)&lvi));
      ConfigLine.insert({ ColString.at(i), buf });
   }
   return ConfigLine;
}

BOOL CommonConfigDlg::UpdateOneLineItemOfListViewW(
   HWND hWndListView
   , int iItem
   , const std::map<std::wstring, std::wstring> mapLine){
   for(int i = COL_FILENAME; COL_SCALE >= i; ++i){
      wstring buf = mapLine.at(ColString.at(i));
      LVITEM lvi = {};
      lvi.iSubItem = i;//��w��
      lvi.pszText = buf.data();
      lvi.cchTextMax = (int)buf.size() + 1;
      // �e�L�X�g���X�V
      SendMessage(
         hWndListView
         , LVM_SETITEMTEXT
         , (WPARAM)iItem
         , (LPARAM)&lvi);
   }
   return 0;
}

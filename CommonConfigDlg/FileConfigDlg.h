/**
 * @file FileConfigDlg.h
 * @brief FileConfigDlg�̐錾
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * ���̃t�@�C�����̂��ׂẴR�[�h�́A���ɖ��L����Ă��Ȃ�����AMIT���C�Z���X�ɏ]���܂��B
 */
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <cmath>
#include <iomanip>
#include <uxtheme.h>
#include "../BitManip256/m256.h"
#include "../SliderEditControl/SliderEditControl.h"
#include "../PreviewWindow/PreviewWindow.h"

class FileConfigDialog{

   static constexpr UINT DOT_RANGE_LOW = 1;
   static constexpr UINT DOT_RANGE_HIGH = 1024;
   static constexpr UINT RATE_RANGE_LOW = 1;
   static constexpr UINT RATE_RANGE_HIGH = 30000;
   static constexpr UINT SCALE_RANGE_LOW = 1;
   static constexpr UINT SCALE_RANGE_HIGH = 1001;
   static constexpr WCHAR CUSTOM_CONTROL_CLASS_NAME[] = L"SliderEditClass";
public:
   const std::set<std::wstring> requiredKeys {
      L"FILE_PATH_NAME"
      ,L"N_DOTS"
      , L"RATE"
      , L"SCALE"
   };
private:
   HINSTANCE hInst;
   HWND hWnd{};
   int IDDlg{};
   BOOL UpdateConfig{};
   SUBCLASSPROC pfnSubClass;
   SliderEditControl EditNumOfDots;
   SliderEditControl EditRate;
   SliderEditControl EditScale;
   PreviewWindow PrevWnd;
   /// @brief ���o�[�V�o�C�i���[�t�H�[�}�b�g
   std::vector<bit_manip::m256> vRBF;
   int vRBFIndex{}; 
   std::map<std::wstring, std::wstring> Config;

public:
   FileConfigDialog();
   FileConfigDialog(const FileConfigDialog&) = delete;
   FileConfigDialog(FileConfigDialog&&) = delete;
   FileConfigDialog& operator =(const FileConfigDialog&) = delete;
   FileConfigDialog& operator =(FileConfigDialog&&) = delete;
   virtual ~FileConfigDialog();
   /// @brief ���[�_��ConfigDialog��\������B
   /// ���̌�AGetConfig�ɂ��ݒ肳�ꂽ�ݒ�l�𓾂�B
   /// @param hWndParent �e�E�C���h�E�n���h���B
   /// @param InIDDlg �_�C�A���OID
   /// @param InCfg �_�C�A���O�ɃZ�b�g����ݒ�l���i�[���ꂽmap
   /// @return 1:�����A
   /// 0:�e�E�C���h�E�n���h���������B
   /// -2:map�ݒ�l�̗v�f������Ă��Ȃ��B
   /// -1:���̑��̗��R�Ŏ��s�BGetLastError�ŃG���[�l���擾�\�B
   [[nodiscard]]INT_PTR DialogBoxParamW(
      HWND  hWndParent
      ,const std::map<std::wstring, std::wstring>& InCfg);
public:
   /// @brief �_�C�A���O������Config�𓾂�B
   /// @return std::map<std::string, std::string>�`����Config�B
   std::map<std::wstring, std::wstring> GetConfigW();
};


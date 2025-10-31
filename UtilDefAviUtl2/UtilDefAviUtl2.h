/**
 * @file UtilDefAviUtl2.h
 * @brief UtilDefAviUtl2��p�̒�`�y�сA�֐��̐錾
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
#include <windows.h>
#include <string>
#include <vector>
#include <map>

/// @brief AviUtl2�p�̃��[�e�B���e�B
namespace UtilDefAviUtl2{


   /// @brief m256�t�H�[�}�b�g�̔Ֆʂ�DIB�ɕ`�悷��B
   /// @param InBoard �{�[�h�Ֆ�
   /// @param hInBmp �`�悷��DIB�n���h���B
   /// @return ����ɕ`�悪����������TRUE�B����ȊOFALSE�B
   BOOL DrawBoardDIB(__m256i InBoard, HBITMAP hInBmp);

   /// @brief �悭�o�Ă���Z�N�V���������`
   static constexpr WCHAR STR_DEFAULT_INITIAL_FILE_SETTINGS[] = L"DEFAULT_INITIAL_FILE_SETTINGS";

   /// @brief INI�t�@�C�����쐬�����Ƃ���
   /// STR_DEFAULT_INITIAL_FILE_SETTINGS�Z�N�V�����̏����ݒ�
   static const std::map<std::wstring, std::wstring> default_initial_setteings = {
   {L"N_DOTS",  L"512"}
   ,{L"RATE",L"1"}
   ,{L"SCALE",L"1"}
   ,{L"FILE_PATH_NAME",L""}
   };

   /// @brief �t�@�C���l�[���Z�N�V������ǂލۂɖ�������Z�N�V�������X�g�B
   static const std::vector<std::wstring> IgnoreList =
   {
      {L"COMMON"}
      ,{L"DEFAULT_SETTINGS"}
      ,{L"META"}
      ,{L"DEFAULT_INITIAL_FILE_SETTINGS"} };

   BOOL MakeIniFile(HMODULE hModule, int BuildNumber);

   double CalcFPS(const double dRate, const double dScale);
   std::string CalcFPStoStringA(const double dRate, const double dScale);
   std::wstring CalcFPStoStringW(const double dRate, const double dScale);
}
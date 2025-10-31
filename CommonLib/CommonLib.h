/**
 * @file CommonLib.h
 * @brief CommonLib�̐錾
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

namespace CommonLib{
   std::u8string WtoU8(const std::wstring& wstr);
   std::string WtoA(const std::wstring& wstr);
   std::map<std::u8string, std::u8string> WtoU8(const std::map<std::wstring, std::wstring>& Inwmap);
   std::wstring U8toW(const std::u8string& str);
   std::wstring AtoW(const std::string& str);
   std::string U8toA(const std::u8string& str);
   std::u8string AtoU8(const std::string& str);
   std::map<std::wstring, std::wstring> U8toW(const std::map<std::u8string, std::u8string>& Inmap);
   /// @brief base64�ŃG���R�[�h���ꂽ��������f�R�[�h����B
   /// @param strB64 base64�ŃG���R�[�h���ꂽ������B
   /// @return �f�R�[�h���ꂽ�o�C�i�����܂ރf�[�^�B
   std::vector<BYTE> B64AtoBin(const std::string& strB64);
   std::vector<BYTE> B64U8toBin(const std::u8string& strB64);
   std::vector<BYTE> B64WtoBin(const std::wstring& strB64);
   std::wstring BinToW(const std::vector<BYTE>& bin);
   std::vector<BYTE> WtoBin(const std::wstring& wstr);

   std::u8string BinToU8(const std::vector<BYTE>& bin);
   std::vector<BYTE> U8toBin(const std::u8string& u8str);
   std::string BinToA(const std::vector<BYTE>& bin);
   std::vector<BYTE> AtoBin(const std::string& str);

   /// @brief �o�C�i�����܂ރf�[�^��base64�ɃG���R�[�h����B
   /// @param pbin �o�C�g�f�[�^�ւ̃|�C���^�B
   /// @param size �o�C�g�f�[�^�̐��B
   /// @return �G���R�[�h���ꂽ������B
   std::string BinToB64A(const std::vector<BYTE>& bin);
   std::wstring BinToB64W(const std::vector<BYTE>& bin);

   /// @brief �C���X�^���X�n���h�����炻�̃C���X�^���X���̃��W���[��
   /// �p�X���擾���A�g���q��INI�ɕς����p�X�l�[����Ԃ��B
   /// @param hInst ���W���[���n���h���B
   /// @return ���W���[���n���h���̃t���p�X������Ɋg���q��INI��
   /// �ς����������Ԃ��B
   std::wstring MakeIniPathNameW(HINSTANCE hInst);
   /// @brief �C���X�^���X�n���h�����炻�̃C���X�^���X���̃��W���[��
   /// �p�X���擾���A�g���q��INI�ɕς����p�X�l�[����Ԃ��B
   /// @param hInst ���W���[���n���h���B
   /// @return ���W���[���n���h���̃t���p�X������Ɋg���q��INI��
   /// �ς����������Ԃ��B
   std::u8string MakeIniPathNameU8(HINSTANCE hInst);

   /// @brief target�̑��݂��Ă���L�[�̒l��source�̒l�ōX�V����B
   /// @param target �X�V����Ώۂ�map
   /// @param source �X�V���f�[�^��map
   void UpdateExistingKeysU8(
      std::map<std::string, std::string>& target
      , const std::map<std::string, std::string>& source);
   void UpdateExistingKeysW(
      std::map<std::wstring, std::wstring>& target
      , const std::map<std::wstring, std::wstring>& source);
   /// @brief target�ɖ����L�[�̂݁Asource����L�[��value���R�s�[����B
   /// @param target Insert�Ώۂ�map�B
   /// @param source ��r�Ώۂ�map�B
   /// @return Insert����������TRUE�A����ȊOFALSE�B
   BOOL InsertOnlyNewKeysU8(
      std::map<std::string, std::string>& target,
      const std::map<std::string, std::string>& source);
   BOOL InsertOnlyNewKeysW(
      std::map<std::wstring, std::wstring>& target,
      const std::map<std::wstring, std::wstring>& source);

   /// @brief DDB,DIB���t�@�C���ɕۑ�����B
   /// @param hdc �f�o�C�X�R���e�L�X�g�n���h��
   /// @param hBitmap �r�b�g�}�b�v�n���h���B
   /// @param filename �t�@�C���p�X�l�C���B
   /// @return �����Ȃ�TRUE�B����ȊOFALSE�B
   BOOL SaveBitmapToFileW(HDC hdc, HBITMAP hBitmap, const std::wstring& filename);
   /// @brief �w�肳�ꂽ�Z�N�V�����̃L�[�ƃo�����[�̃y�A���擾����
   /// @param section �Z�N�V������
   /// @param filename INI�t�@�C���̃p�X�l�C���B
   /// @return �擾�����L�[�ƃo�����[��map�B
   std::map<std::wstring, std::wstring>
      ReadIniToMapW(const std::wstring& section, const std::wstring& filename);
      /// @brief INI�t�@�C���̃Z�N�V��������ݒ��map�ɓǂݍ���
      /// @param section �Z�N�V������
      /// @param filename INI�t�@�C���p�X
      /// @return �ǂݍ��܂ꂽmap
   std::map<std::u8string, std::u8string>
      ReadIniToMapU8(const std::u8string& section, const std::u8string& filename);
         /// @brief INI�t�@�C���̃Z�N�V������map�̐ݒ����������  
         /// @param section �Z�N�V������
         /// @param settings �Z�b�e�B���Omap
         /// @param iniPath INI�p�X
         /// @return ���������TRUE�A����ȊOFALSE
   BOOL WriteMapToIniW(
      const std::wstring& section
      , const std::map<std::wstring, std::wstring>& settings
      , const std::wstring& iniPath);
   /// @brief INI�t�@�C���̃Z�N�V������map�̐ݒ����������  
   /// @param section �Z�N�V������
   /// @param settings �Z�b�e�B���Omap
   /// @param iniPath INI�p�X
   /// @return ���������TRUE�A����ȊOFALSE
   BOOL WriteMapToIniU8(
      const std::u8string& section
      , const std::map<std::u8string, std::u8string>& settings
      , const std::u8string& iniPath);
   /// @brief INI�Z�N�V���������݂��邩���ׂ�B
   /// @param section �Z�N�V������
   /// @param path INI�t�@�C���p�X
   /// @return �Z�N�V���������݂��Ă����TRUE�A
   /// �t�@�C�������݂��Ă��Ȃ��ꍇFALSE�A
   /// ����ȊOFALSE�B
   BOOL IniSectionExistsW(const std::wstring& section, const std::wstring& path);
   /// @brief INI�Z�N�V���������݂��邩���ׂ�B
   /// @param section �Z�N�V������
   /// @param path INI�t�@�C���p�X
   /// @return �Z�N�V���������݂��Ă����TRUE�A
   /// �t�@�C�������݂��Ă��Ȃ��ꍇFALSE�A
   /// ����ȊOFALSE�B
   BOOL IniSectionExistsU8(const std::u8string& section, const std::u8string& path);
   /// @brief �}�b�v���}�[�W����B���̃}�b�v�͕ύX���Ȃ��B
   /// @param a �}�b�va�B
   /// @param b �}�b�vb�B
   /// @return �}�[�W���ꂽ�}�b�v�B
   std::map<std::string, std::string>
      MergeMapU8(
         const std::map<std::string, std::string>& a
         , const std::map < std::string, std::string >& b);
      /// @brief temp�f�B���N�g���ɍ쐬����e���v�t�@�C���p�X�X�g�����O���쐬����B
      /// @param InFileName �e���v�t�@�C���ɒǉ�����t�@�C�����B
      /// @param OutPathName �쐬�����e���v�t�@�C���̃p�X�ƃt�@�C����������string�B
      /// @return �쐬�����e���v�t�@�C���̕�����̃T�C�Y�B
   std::size_t MakeTempPathNameW(
      const std::wstring& InFileName
      , std::wstring& OutPathName);

   /// @brief ���C���E�B���h�E��T���B
   /// @param threadId �X���b�hID
   /// @return �E�B���h�E�n���h���B
   HWND FindMainWindowFromThread(DWORD threadId);
   /// @brief RECT����ʓ��Ɏ��܂�悤�ɒ�������B
   /// @param rc RECT�\���́B���̒l�͏�����������\��������܂��B
   /// @return ����:TRUE�A����ȊO:FALSE
   BOOL AdjustRectToFitScreenWithOverflow(RECT& rc);
   /// @brief GDI�I�u�W�F�X�g���R���X�g���N�^�ŃZ���N�g���āA�f�X�g���N�^�Ń��X�g�A����
   /// @tparam T GDI�I�u�W�F�N�g�n���h��
   template<typename T>
   class GdiSelectAndRestorer{
      HDC hdc;
      T hOldObj;
   public:
      GdiSelectAndRestorer() = delete;
      GdiSelectAndRestorer(HDC hdcIn, T hNewObj)
         : hdc(hdcIn){
         hOldObj = (T)::SelectObject(hdc, hNewObj);
      }
      ~GdiSelectAndRestorer(){
         ::SelectObject(hdc, hOldObj);
      }
   };

   template<typename T, T(*SelectFunc)(HDC, T)>
   class GdiSelectAndRestorerF{
      HDC hdc;
      T hOldObj;
   public:
      GdiSelectAndRestorerF() = delete;
      GdiSelectAndRestorerF(HDC hdcIn, T hNewObj)
         : hdc(hdcIn){
         hOldObj = SelectFunc(hdc, hNewObj);
      }
      ~GdiSelectAndRestorerF(){
         SelectFunc(hdc, hOldObj);
      }
   };

   class PaintDC{
      HWND hwnd;
      PAINTSTRUCT ps;
      HDC hdc;
   public:
      PaintDC() = delete;
      PaintDC(HWND hwndIn)
         : hwnd(hwndIn), hdc(BeginPaint(hwnd, &ps)){}
      ~PaintDC(){
         EndPaint(hwnd, &ps);
      }
      HDC get() const{ return hdc; }
      operator HDC() const{ return hdc; } // �Öٕϊ����\
   };

   class ScopedCOMInitializer{
      HRESULT hr;
   public:
      ScopedCOMInitializer(): hr(CoInitialize(nullptr)){}
      ~ScopedCOMInitializer(){
         if(SUCCEEDED(hr)) CoUninitialize();
      }
      bool succeeded() const{ return SUCCEEDED(hr); }
      HRESULT result() const{ return hr; }
   };

   /// @brief DDB����DIB�ɕϊ��B
   /// @param hdc �f�o�C�X�R���e�L�X�g
   /// @param hDDB DDB�n���h��
   /// @return DIB�n���h��
   HBITMAP DDBToDIB(HDC hdc, HBITMAP hDDB);
   /// @brief DIB�̕������쐬����B
   /// @param hSrcDIB �������B
   /// @return ���������n���h���B
   HBITMAP DupDIB(HBITMAP hSrcDIB);
   /// @brief �s�N�Z���̏�����������Ă��Ȃ�DIB���쐬����B
   /// @param Width ��
   /// @param Height ����
   /// @return DIB�n���h��
   HBITMAP CreateDIB24(int Width, int Height);

   HBITMAP CreateDIB24ppBits(int Width, int Height, void** ppBits);


   HBITMAP LoadPngFromResource(HINSTANCE hInst, int resID);
   /// @brief INI�t�@�C�����̃Z�N�V��������Base64��
   /// �f�R�[�h������������A�p�X�t�@�C�����Ƃ݂Ȃ��Ă���
   /// �p�X�t�@�C�����̃t�@�C�������݂��邩�m�F����B�B
   /// ���݂��Ă��Ȃ���΂��̃Z�N�V�������폜�B
   /// @param ignoreList ��������Z�N�V�������X�g�B
   /// @param iniFilePath INI�t�@�C���̃p�X�l�[���B
   /// @return �폜�����Z�N�V�����̐��B
   INT CleanInvalidSectionsW(const std::vector<std::wstring>& ignoreList, const std::wstring& iniFilePath);
   std::wstring FindFirstMatchingFile(const std::wstring& pattern);

}


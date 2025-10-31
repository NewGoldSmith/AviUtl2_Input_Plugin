/**
 * @file bit_manip256.cpp
 * @brief m256ビットマニピュレータの実装
 * @author Gold Smith
 * @date 2023 2024 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#include "bit_manip256.h"
#include <memory>
#include <ranges>

using namespace std;
using namespace bit_manip::AVX;


BOOL bit_manip::AVX::SaveM256ArrayW(const wstring& strPathFileName, const vector<m256>& in_vec){

   unique_ptr<remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hFile{
      [&]()->HANDLE{
         HANDLE hFile;
         if((hFile = ::CreateFileW(
            strPathFileName.c_str()
            , GENERIC_WRITE
            , 0
            , NULL
            , CREATE_ALWAYS
            , FILE_ATTRIBUTE_NORMAL
            , NULL)) == INVALID_HANDLE_VALUE){
            DPLE;
            return nullptr;
         };
         return hFile; }()
            , CloseHandle };

   if(!hFile) return FALSE;

   for(auto m : in_vec){
      DWORD writen{};
      if(!::WriteFile(
         hFile.get()
         , (LPCVOID)&m.m256i.m256i_u8[0]
         , (DWORD)sizeof(m)
         , &writen
         , NULL)){
         DPLE;
         return FALSE;
      }
      if(writen != sizeof(m)){
         MessageBoxA(NULL, DPT("Mismatch in number of writes.").c_str(), "Error", MB_ICONEXCLAMATION);
         return FALSE;
      }
   }
   return TRUE;
}


BOOL bit_manip::AVX::LoadM256ArrayW(const wstring& strPathFileName, vector<m256>& in_vec){

   unique_ptr<remove_pointer_t<HANDLE>, decltype(CloseHandle)*> hFile{
      [&]()->HANDLE{
         HANDLE hFile;
         if((hFile = ::CreateFileW(
            strPathFileName.c_str()
            , GENERIC_READ
            , FILE_SHARE_READ
            , NULL
            , OPEN_EXISTING
            , FILE_ATTRIBUTE_NORMAL
            , NULL)) == INVALID_HANDLE_VALUE){
            ::MessageBoxA(NULL, DPLE.c_str(), "Error", MB_ICONEXCLAMATION);
            return nullptr;}
         return hFile; }()
   ,CloseHandle };

   if(!hFile) return FALSE;

   in_vec.clear();
   for(;;){
      DWORD numRead{};
      m256 m;
      if(!::ReadFile(
         hFile.get()
         , (LPVOID)&m.m256i.m256i_u8[0]
         , sizeof(m)
         , &numRead
         , NULL)){
         DPLE;
         return FALSE;
      }
      if(numRead != sizeof(m)){
         if(numRead == 0) return TRUE;
         return FALSE;
      }
      in_vec.push_back(m);
   }
}

/**
 * @file CommonLib.cpp
 * @brief CommonLibの実装
 * @author Gold Smith
 * @date 2025
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#include "CommonLib.h"
#include <wincrypt.h>
#include <Shlwapi.h>
#include <PathCch.h>
#include <gdiplus.h>
#include <format>
#include <filesystem>
#include <cmath>
#include <fstream>
#include <memory>
#include "../DebugFnc/DebugFnc.h"
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;
using namespace Gdiplus;

u8string CommonLib::WtoU8(const wstring& wstr){
   if(wstr.empty()) return u8string();

   if(int size = WideCharToMultiByte(
      CP_UTF8
      , 0
      , wstr.data()
      , (int)wstr.size()
      , NULL
      , 0
      , NULL
      , NULL); !size){
      MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
      return u8string();
   } else{
      u8string result(size, u8'\0');
      if(int size2 = WideCharToMultiByte(
         CP_UTF8
         , 0
         , wstr.data()
         , (int)wstr.size()
         , (LPSTR)result.data()
         , size
         , NULL
         , NULL); !size2){
         MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
         return u8string();
      }
      else{
         result.resize(size2);
         return result;
      }
   }
}

std::string CommonLib::WtoA(const std::wstring& wstr){
   if(wstr.empty()) return string();

   if(int size = WideCharToMultiByte(
      CP_ACP
      , 0
      , wstr.data()
      , (int)wstr.size()
      , NULL
      , 0
      , NULL
      , NULL); !size){
      MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
      return string();
   } else{
      string result(size, '\0');
      if(int size2 = WideCharToMultiByte(
         CP_ACP
         , 0
         , wstr.data()
         , (int)wstr.size()
         , (LPSTR)result.data()
         , size
         , NULL
         , NULL); !size2){
         MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
         return string();
      }
      else{
         result.resize(size2);
         return result;
      }
   }
}

map<std::u8string, std::u8string> CommonLib::WtoU8(const map<std::wstring, std::wstring>& Inwmap){
   map<u8string, u8string> Outmap;
   for(const auto& [key, value] : Inwmap){
      Outmap[WtoU8(key)] = WtoU8(value);
   }
   return Outmap;
}

wstring CommonLib::U8toW(const u8string& str){
   if(str.empty()) return wstring();

   if(int size = MultiByteToWideChar(
      CP_UTF8
      , 0
      , (LPCCH)str.data()
      , (int)str.size()
      , NULL
      , 0);
      !size){
      MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
      return wstring();
   } else{
      wstring result(size, L'\0');
      if(int size2 = MultiByteToWideChar(
         CP_UTF8
         , 0
         , (LPCCH)str.data()
         , (int)str.size()
         , result.data()
         , size);
         !size2){
         MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
         return wstring();
      } else{
         result.resize(size2);
         return result;
      }
   }
}

std::wstring CommonLib::AtoW(const std::string& str){
   if(str.empty()) return wstring();

   if(int size = MultiByteToWideChar(
      CP_ACP
      , 0
      , (LPCCH)str.data()
      , (int)str.size()
      , NULL
      , 0);
      !size){
      MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
      return wstring();
   } else{
      wstring result(size, L'\0');
      if(int size2 = MultiByteToWideChar(
         CP_ACP
         , 0
         , (LPCCH)str.data()
         , (int)str.size()
         , result.data()
         , size);
         !size2){
         MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
         return wstring();
      } else{
         result.resize(size2);
         return result;
      }
   }
}

std::string CommonLib::U8toA(const std::u8string& str){
   return WtoA(U8toW(str));
}

std::u8string CommonLib::AtoU8(const std::string& str){
   return WtoU8(AtoW(str));
}

map<wstring, wstring> CommonLib::U8toW(const map<u8string, u8string>& Inmap){
   map<wstring, wstring> Outwmap;
   for(const auto& [key, value] : Inmap){
      Outwmap[U8toW(key)] = U8toW(value);
   }
   return Outwmap;
}

vector<BYTE> CommonLib::B64AtoBin(const string& strB64){
   if(strB64.empty()) return vector<BYTE>();

   if(DWORD binLen{};
      !CryptStringToBinaryA(
         strB64.data()
         , (DWORD)strB64.size()
         , CRYPT_STRING_BASE64
         , NULL
         , &binLen
         , NULL
         , NULL)){
      MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
      return vector<BYTE>();
   } else{
      vector<BYTE> bin(binLen, '\0');
      if(!CryptStringToBinaryA(
         strB64.data()
         , static_cast<DWORD>(strB64.size())
         , CRYPT_STRING_BASE64
         , (BYTE*)bin.data()
         , &binLen
         , NULL
         , NULL)){
         DPLE;
         MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
         return vector<BYTE>();
      }
      bin.resize(binLen);
      return bin;
   }
}

std::vector<BYTE> CommonLib::B64U8toBin(const std::u8string& strB64){
   string str = (char*)strB64.c_str();
   return B64AtoBin(str);
}

std::vector<BYTE> CommonLib::B64WtoBin(const std::wstring& strB64){
   string str = WtoA(strB64);
   return B64AtoBin(str);
}

wstring CommonLib::BinToW(const vector<BYTE>& bin){
    if (bin.size() % 2 != 0) {
        MessageBoxA(NULL,DPT("Invalid UTF - 16LE byte sequence length").c_str(),"Error",MB_ICONEXCLAMATION);
        return wstring();
    }

    wstring result;
    result.resize(bin.size() / 2);

    for (size_t i = 0; i < result.size(); ++i) {
        // リトルエンディアンなので下位バイトが先
        unsigned char lo = static_cast<unsigned char>(bin[2*i]);
        unsigned char hi = static_cast<unsigned char>(bin[2*i + 1]);
        result[i] = static_cast<wchar_t>((hi << 8) | lo);
    }

    return result;
}

std::vector<BYTE> CommonLib::WtoBin(const std::wstring& wstr){
    std::vector<BYTE> bin;
    bin.resize(wstr.size() * 2);

    for (size_t i = 0; i < wstr.size(); ++i) {
        wchar_t wc = wstr[i];
        // リトルエンディアン: 下位バイトが先
        bin[2*i]     = static_cast<BYTE>(wc & 0xFF);
        bin[2*i + 1] = static_cast<BYTE>((wc >> 8) & 0xFF);
    }

    return bin;
}

std::u8string CommonLib::BinToU8(const std::vector<BYTE>& bin){
    // バイト列をそのまま char8_t 配列としてコピー
    return std::u8string(
        reinterpret_cast<const char8_t*>(bin.data()),
        bin.size()
    );
}

std::vector<BYTE> CommonLib::U8toBin(const std::u8string& u8str){
   return std::vector<BYTE>(
      reinterpret_cast<const BYTE*>(u8str.data())
      ,reinterpret_cast<const BYTE*>(u8str.data()) + u8str.size()
   );
}

std::string CommonLib::BinToA(const std::vector<BYTE>& bin){
    // バイト列をそのまま char 配列としてコピー
    return std::string(
        reinterpret_cast<const char*>(bin.data()),
        bin.size()
    );
}

std::vector<BYTE> CommonLib::AtoBin(const std::string& str){
   return std::vector<BYTE>(
      reinterpret_cast<const BYTE*>(str.data())
      ,reinterpret_cast<const BYTE*>(str.data()) + str.size());
}

string CommonLib::BinToB64A(const std::vector<BYTE>& bin){
   DWORD Len{};
   if(DWORD b64Len{};
      !CryptBinaryToStringA(
         bin.data()
         , (DWORD)bin.size()
         , CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF
         , NULL
         , &b64Len)){
      MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
      return string();
   } else{
      string strB64(b64Len, '\0');
      if(!CryptBinaryToStringA(
         bin.data()
         , (DWORD)bin.size()
         , CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF
         , strB64.data()
         , &b64Len)){
         MessageBoxA(NULL, DPLE.c_str(), _CRT_STRINGIZE($APPNAME), MB_ICONEXCLAMATION | MB_OK);
         return string();
      }
      strB64.resize(b64Len);
      return strB64;
   }
}

std::wstring CommonLib::BinToB64W(const std::vector<BYTE>& bin){
   string str = BinToB64A(bin);
   return AtoW(str);
}

wstring CommonLib::MakeIniPathNameW(HINSTANCE hInst){
   WCHAR path[MAX_PATH + 1];
   if(!::GetModuleFileNameW(hInst, path, MAX_PATH))
      return L"";
   if(::PathCchRenameExtension(path, MAX_PATH,L"INI") != S_OK){
      return L"";
   }
   return path;
}

std::u8string CommonLib::MakeIniPathNameU8(HINSTANCE hInst){
   return WtoU8(MakeIniPathNameW(hInst));
}

void CommonLib::UpdateExistingKeysU8(
   map<string, string>& target
   , const map<string, string>& source){
   for(const auto& [key, value] : source){
      auto it = target.find(key);
      if(it != target.end()){
         it->second = value;
      }
   }
}

void CommonLib::UpdateExistingKeysW(map<wstring, wstring>& target, const map<wstring, wstring>& source){
   for(const auto& [key, value] : source){
      auto it = target.find(key);
      if(it != target.end()){
         it->second = value;
      }
   }
}

BOOL CommonLib::InsertOnlyNewKeysU8(std::map<string, string>& target, const map<string, string>& source){
   BOOL Update{};
   for(const auto& [key, value] : source){
      if(target.find(key) == target.end()){
         target[key] = value;  // 新規キーのみ追加
         Update = TRUE;
      }
   }
   return Update;
}

BOOL CommonLib::InsertOnlyNewKeysW(std::map<wstring, wstring>& target, const map<wstring, wstring>& source){
   BOOL Update{};
   for(const auto& [key, value] : source){
      if(target.find(key) == target.end()){
         target[key] = value;  // 新規キーのみ追加
         Update = TRUE;
      }
   }
   return Update;
}

BOOL CommonLib::SaveBitmapToFileW(HDC hdc, HBITMAP hBitmap, const wstring& filename){
   if(!hBitmap) return FALSE;

   BITMAP bmp = {};
   if(!GetObject(hBitmap, sizeof(BITMAP), &bmp))
      return FALSE;

  // 1. BITMAPINFOHEADER を初期化してフォーマットを取得
   BITMAPINFOHEADER bmih = {};
   bmih.biSize = sizeof(BITMAPINFOHEADER);

   if(!GetDIBits(hdc, hBitmap, 0, 0, NULL, (BITMAPINFO*)&bmih, DIB_RGB_COLORS))
      return FALSE;

  // 2. パレットサイズを計算（biClrUsed が優先）
   int colorCount = 0;
   if(bmih.biBitCount <= 8){
      colorCount = bmih.biClrUsed ? bmih.biClrUsed : (1 << bmih.biBitCount);
   }
   size_t paletteSize = colorCount * sizeof(RGBQUAD);

   // 3. イメージサイズを再計算（biSizeImage が 0 の場合もある）
   DWORD imageSize = bmih.biSizeImage;
   if(imageSize == 0){
      DWORD bytesPerLine = ((bmih.biWidth * bmih.biBitCount + 31) / 32) * 4;
      imageSize = bytesPerLine * abs(bmih.biHeight);
      bmih.biSizeImage = imageSize;
   }

   // 4. バッファ確保（ヘッダ＋パレット＋ピクセル）
   size_t totalSize = sizeof(BITMAPINFOHEADER) + paletteSize + imageSize;
   std::unique_ptr<BYTE[]> buffer(new(std::nothrow) BYTE[totalSize]);
   if(!buffer) return FALSE;

   BITMAPINFO* pInfo = reinterpret_cast<BITMAPINFO*>(buffer.get());
   pInfo->bmiHeader = bmih;

   // 5. ピクセルデータを取得
   if(!GetDIBits(hdc, hBitmap, 0, abs(bmih.biHeight),
                 buffer.get() + sizeof(BITMAPINFOHEADER) + paletteSize,
                 pInfo, DIB_RGB_COLORS)){
      return FALSE;
   }

   // 6. BITMAPFILEHEADER を作成
   BITMAPFILEHEADER bfh = {};
   bfh.bfType = 0x4D42; // 'BM'
   bfh.bfOffBits = DWORD(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize);
   bfh.bfSize = bfh.bfOffBits + imageSize;

   // 7. 書き込み
   std::ofstream ofs(filename, std::ios::binary);
   if(!ofs) return FALSE;

   ofs.write(reinterpret_cast<const char*>(&bfh), sizeof(bfh));
   ofs.write(reinterpret_cast<const char*>(&bmih), sizeof(bmih));
   if(paletteSize > 0){
      ofs.write(reinterpret_cast<const char*>(buffer.get() + sizeof(BITMAPINFOHEADER)), paletteSize);
   }
   ofs.write(reinterpret_cast<const char*>(buffer.get() + sizeof(BITMAPINFOHEADER) + paletteSize), imageSize);

   return ofs.good();
}

map<wstring, wstring> CommonLib::ReadIniToMapW(
   const wstring& section
   , const wstring& filename){

   map<wstring, wstring> result;

   constexpr DWORD bufferSize = 32 * 1024;
   vector<wchar_t> buffer(bufferSize);

   DWORD charsRead = 
      GetPrivateProfileSectionW(
         section.c_str()
         , buffer.data()
         , bufferSize
         , filename.c_str());
   if(charsRead == 0) return result;

   wchar_t* ptr = buffer.data();
   while(*ptr){
      wstring entry(ptr);
      size_t pos = entry.find(L'=');
      if(pos != wstring::npos){
         wstring key = entry.substr(0, pos);
         wstring value = entry.substr(pos + 1);
         result[key] = value;
      }
      ptr += entry.size() + 1;
   }

   return result;
}

map<u8string, u8string> CommonLib::ReadIniToMapU8(const u8string& section, const u8string& filename){
   return WtoU8(ReadIniToMapW(U8toW(section), U8toW(filename)));
}

BOOL CommonLib::WriteMapToIniW(
   const std::wstring& section
   , const std::map<std::wstring, std::wstring>& settings
   , const std::wstring& iniPath) {

   for(const auto& [key, value] : settings){
      if(BOOL result = ::WritePrivateProfileStringW(
         section.c_str()
         , key.c_str()
         , value.c_str()
         , iniPath.c_str());
         !result){
         DPLE;
         return FALSE;
      };
   }
   return TRUE;
}

BOOL CommonLib::WriteMapToIniU8(
   const u8string& section
   , const map<u8string, u8string>& settings
   , const u8string& iniPath) {

   return WriteMapToIniW(U8toW(section), U8toW(settings), U8toW(iniPath));
}

BOOL CommonLib::IniSectionExistsW(const wstring& section, const wstring& path){
   constexpr DWORD bufferSize = 32767;
   vector<wchar_t> buffer(bufferSize);

   DWORD charsRead = GetPrivateProfileSectionNamesW(buffer.data(), bufferSize, path.c_str());
   if(charsRead == 0) return FALSE;

   const wchar_t* p = buffer.data();
   while(*p){
      if(_wcsicmp(p, section.c_str()) == 0){
         return TRUE;
      }
      p += wcslen(p) + 1;
   }
   return FALSE;
}

BOOL CommonLib::IniSectionExistsU8(const u8string& section, const u8string& path){
   return IniSectionExistsW(U8toW(section),U8toW(path));
}

map<string, string> CommonLib::MergeMapU8(
   const map<string, string>& a
   , const map<string, string>& b){
   map<string, string> result = a;
   for(const auto& [key, value] : b){
      result[key] = value;
   }
   return result;
}

size_t CommonLib::MakeTempPathNameW(const wstring& InFileName, wstring& OutPathName){
    // 1. Tempディレクトリ取得
   wchar_t tempPath[MAX_PATH] = {};
   DWORD len = GetTempPathW(MAX_PATH, tempPath);
   if(len == 0 || len > MAX_PATH) return 0;

   // 2. 元ファイル名と拡張子を分離
   wchar_t fname[MAX_PATH] = {};
   wchar_t ext[MAX_PATH] = {};
   _wsplitpath_s(InFileName.c_str(), nullptr, 0, nullptr, 0, fname, MAX_PATH, ext, MAX_PATH);

   // 3. GUID生成
   GUID guid;
   if(CoCreateGuid(&guid) != S_OK) return 0;

   wstringstream guidStr;
   guidStr << hex << setfill(L'0')
      << setw(8) << guid.Data1 << L"-"
      << setw(4) << guid.Data2 << L"-"
      << setw(4) << guid.Data3 << L"-";

   for(int i = 0; i < 2; ++i)
      guidStr << setw(2) << (int)guid.Data4[i];
   guidStr << L"-";
   for(int i = 2; i < 8; ++i)
      guidStr << setw(2) << (int)guid.Data4[i];

   wstring guidPart = guidStr.str();

   // 4. フルパス構築
   wstringstream oss;
   oss << tempPath << fname << L"_" << guidPart << ext;
   OutPathName = oss.str();

   // 5. MAX_PATH を超えたら GUID 部分を切り詰める
   if(OutPathName.size() >= MAX_PATH){
      size_t maxGuidLen = MAX_PATH - wcslen(tempPath) - wcslen(fname) - wcslen(ext) - 2; // "_" + null
      if(maxGuidLen < 8) return 0; // GUIDが短すぎると一意性が危険

      guidPart = guidPart.substr(0, maxGuidLen);
      wstringstream oss2;
      oss2 << tempPath << fname << L"_" << guidPart << ext;
      OutPathName = oss2.str();
   }

   return OutPathName.size();
}

HWND CommonLib::FindMainWindowFromThread(DWORD threadId){
   struct st1{
      HWND hwnd = nullptr;
   } data;

   ::EnumThreadWindows(
      threadId
      , [](HWND hwnd, LPARAM lParam) -> BOOL {
         auto& data = *reinterpret_cast<st1*>(lParam);

         // メインウィンドウらしさの判定
         if(::IsWindowVisible(hwnd) && ::GetWindow(hwnd, GW_OWNER) == nullptr){
            data.hwnd = hwnd;
            return FALSE; // 見つかったので列挙終了
         }
         return TRUE;
      }
   , reinterpret_cast<LPARAM>(&data));

   return data.hwnd;
}

BOOL CommonLib::AdjustRectToFitScreenWithOverflow(RECT& rc){
   RECT rcWorkArea = {};
   if(!SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWorkArea, 0))
      return false;

   int width = rc.right - rc.left;
   int height = rc.bottom - rc.top;

   // 幅が画面より大きい → 左端を0に固定（右はみ出しは許容）
   if(width >= (rcWorkArea.right - rcWorkArea.left)){
      rc.left = rcWorkArea.left;
      rc.right = rc.left + width;
   }

   // 高さが画面より大きい → 上端を0に固定（下はみ出しは許容）
   if(height >= (rcWorkArea.bottom - rcWorkArea.top)){
      rc.top = rcWorkArea.top;
      rc.bottom = rc.top + height;
   }

   // 通常のはみ出し → はみ出した量だけ逆方向にずらす
   if(rc.right > rcWorkArea.right){
      int overflow = rc.right - rcWorkArea.right;
      rc.left -= overflow;
      rc.right -= overflow;
   }
   if(rc.left < rcWorkArea.left){
      int overflow = rcWorkArea.left - rc.left;
      rc.left += overflow;
      rc.right += overflow;
   }

   if(rc.bottom > rcWorkArea.bottom){
      int overflow = rc.bottom - rcWorkArea.bottom;
      rc.top -= overflow;
      rc.bottom -= overflow;
   }
   if(rc.top < rcWorkArea.top){
      int overflow = rcWorkArea.top - rc.top;
      rc.top += overflow;
      rc.bottom += overflow;
   }

   return true;
}

HBITMAP CommonLib::DDBToDIB(HDC hdc, HBITMAP hDDB){
    if (!hdc || !hDDB)
        return NULL;

    // 1. ヘッダー取得
    BITMAPINFOHEADER bmih = {};
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    if(!GetDIBits(
       hdc
       , hDDB
       , 0
       , 0
       , NULL
       , (BITMAPINFO*)&bmih
       , DIB_RGB_COLORS)){
       return NULL;
    }

    // 2. パレットサイズ（8bpp以下）
    int cbPalette = (bmih.biBitCount <= 8)
       ? (1i64 << bmih.biBitCount) * sizeof(RGBQUAD)
       : 0;

   // 3. ピクセルサイズ
    DWORD cbImage = GDI_DIBSIZE(bmih);

    // 4. 一括メモリ確保（BITMAPINFO + パレット + ピクセル）
    size_t cbTotal = sizeof(BITMAPINFOHEADER) + cbPalette + cbImage;
    unique_ptr<BYTE, void(*)(BYTE*)> pBuffer
    { new(nothrow) BYTE[cbTotal] ,[](BYTE* pb) { delete[] pb; } };
    if(!pBuffer){
       DPT("Allocation failed.");
       return NULL;
    }

    // 5. ポインタ分割
    BITMAPINFO* pbmi = (BITMAPINFO*)pBuffer.get();
    pbmi->bmiHeader = bmih;
    BYTE* pBits = pBuffer.get() + sizeof(BITMAPINFOHEADER) + cbPalette;

    // 6. ピクセル取得
    const int nHeight = std::abs(bmih.biHeight);
    if(!GetDIBits(
       hdc
       , hDDB
       , 0
       , nHeight
       , pBits
       , pbmi
       , DIB_RGB_COLORS)){
       DPLE;
       return NULL;
    }

    // 7. 新しい DIB を作成
    void* pNewBits = NULL;
    HBITMAP hDIB;
    if(hDIB =
       CreateDIBSection(
          hdc
          , pbmi
          , DIB_RGB_COLORS
          , &pNewBits
          , NULL
          , 0);
       !hDIB || !pNewBits){
       DPLE;
       return NULL;
    }

   // 8. ピクセルコピー
    memcpy_s(pNewBits, cbImage, pBits, cbImage);

    // 9. 完了
    return hDIB;
}

HBITMAP CommonLib::DupDIB(HBITMAP hSrcDIB){
    if (!hSrcDIB) return NULL;

    unique_ptr<remove_pointer_t< HDC>, void(*)(HDC hdc)> hDC
    { GetDC(NULL),[](HDC hdc){ReleaseDC(NULL,hdc); } };

    // 1. ヘッダー取得
    BITMAPINFOHEADER bih = { sizeof(BITMAPINFOHEADER) };
    if(!GetDIBits(
       hDC.get()
       , hSrcDIB
       , 0
       , 0
       , NULL
       , (BITMAPINFO*)&bih
       , DIB_RGB_COLORS)){
       return NULL;
    }

    // 2. パレットサイズ（8bpp以下）
    int paletteSize = (bih.biBitCount <= 8) ? (1i64 << bih.biBitCount) * sizeof(RGBQUAD) : 0;

    // 3. ピクセルサイズ
    DWORD imageSize = GDI_DIBSIZE(bih);

    // 4. 一括メモリ確保
    size_t totalSize = sizeof(BITMAPINFOHEADER) + paletteSize + imageSize;
    unique_ptr< BYTE, void(*)(BYTE* pb)> buffer
    { new(nothrow) BYTE[totalSize],[](BYTE* pb){delete[]pb; } };
    if(!buffer) return NULL;

    // 5. ポインタ分割
    BITMAPINFO* pbmi = (BITMAPINFO*)buffer.get();
    pbmi->bmiHeader = bih;
    BYTE* pBits = buffer.get() + sizeof(BITMAPINFOHEADER) + paletteSize;

    // 6. ピクセル取得
    if (!GetDIBits(hDC.get(), hSrcDIB, 0, bih.biHeight, pBits, pbmi, DIB_RGB_COLORS)){
        return NULL;
    }

    // 7. 新しい DIB を作成
    void* pNewBits{};
    HBITMAP hNewBitmap = CreateDIBSection(hDC.get(), pbmi, DIB_RGB_COLORS, &pNewBits, nullptr, 0);
    if (!hNewBitmap || !pNewBits) {
       return NULL;
    }

    // 8. ピクセルコピー
    memcpy_s(pNewBits, imageSize, pBits, imageSize);

    // 9. 後始末
    return hNewBitmap;
}

HBITMAP CommonLib::CreateDIB24(int Width, int Height){
   if(Width <= 0 || Height <= 0)
      return NULL;

   BITMAPINFO bmi = {};
   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi.bmiHeader.biWidth = Width;
   bmi.bmiHeader.biHeight = -Height; // トップダウンDIB
   bmi.bmiHeader.biPlanes = 1;
   bmi.bmiHeader.biBitCount = 24;
   bmi.bmiHeader.biCompression = BI_RGB;

   void* pBits{};
   
   if(HBITMAP hBmpResult = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0); !hBmpResult){
      DPLE;
      return NULL;
   }else{
      return hBmpResult;
   }
}

HBITMAP CommonLib::CreateDIB24ppBits(int Width, int Height, void** ppBits){
   if (Width <= 0 || Height <= 0 || !ppBits)
      return NULL;

   BITMAPINFO bmi = {};
   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi.bmiHeader.biWidth = Width;
   bmi.bmiHeader.biHeight = -Height; // トップダウンDIB
   bmi.bmiHeader.biPlanes = 1;
   bmi.bmiHeader.biBitCount = 24;
   bmi.bmiHeader.biCompression = BI_RGB;

   HBITMAP hBmpResult = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, ppBits, NULL, 0);
   if (!hBmpResult) {
      DPLE;
      return NULL;
   }

   return hBmpResult;

}

HBITMAP CommonLib::LoadPngFromResource(HINSTANCE hInst, int resID){
    // リソースを探す
   HRSRC hRes;
   if(!(hRes = FindResourceW(hInst, MAKEINTRESOURCE(resID), L"PNG"))){
      DPLE;
      return NULL;
   }
   DWORD size;
   if(!(size = SizeofResource(hInst, hRes))){
      DPLE;
      return NULL;
   }
   HGLOBAL hMem;
   if(!(hMem = LoadResource(hInst, hRes))){
      DPLE;
      return NULL;
   }
   void* pMem;
   if(!(pMem = LockResource(hMem))){
      DPLE;
      return NULL;
   }

   // HGLOBAL を IStream に変換
   HGLOBAL hBuffer;
   if(!(hBuffer = GlobalAlloc(GMEM_MOVEABLE, size))){
      DPLE;
      return NULL;
   }
   void* pBuffer;
   if(!(pBuffer = GlobalLock(hBuffer))){
      DPLE;
      return NULL;
   }
   memcpy_s(pBuffer, size, pMem, size);
   GlobalUnlock(hBuffer);

   IStream* pStream = nullptr;
   HRESULT hr = ::CreateStreamOnHGlobal(hBuffer, TRUE, &pStream);
   if(FAILED(hr) || !pStream){
      GlobalFree(hBuffer); // 自分で解放する必要あり
      return NULL;
   }

   Gdiplus::GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR gdiplusToken;
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

   // GDI+ Bitmap として読み込み
   Bitmap* bmp = Gdiplus::Bitmap::FromStream(pStream);
   HBITMAP hBmp = NULL;
   if(bmp){
      bmp->GetHBITMAP(Color(0, 0, 0, 0), &hBmp); // 透過色を指定
      delete bmp;
   }
   pStream->Release();
   GdiplusShutdown(gdiplusToken);
   return hBmp;
}

INT CommonLib::CleanInvalidSectionsW(const vector<wstring>& ignoreList, const wstring& iniFilePath){
   std::vector<WCHAR> buffer(1892); // セクション名一覧を格納するバッファ
   DWORD len = GetPrivateProfileSectionNamesW(buffer.data(), (DWORD)buffer.size(), iniFilePath.c_str());
   if(len == 0) return -1; // 読み込み失敗

   int deletedCount = 0;
   const WCHAR* p = buffer.data();
   while(*p){
      wstring sectionName = p;
      p += sectionName.size() + 1;

      // ignoreList に含まれるセクションはスキップ
      if(find(ignoreList.begin(), ignoreList.end(), sectionName) != ignoreList.end())
         continue;

     // Base64 デコードしてファイルパスを得る
      vector<BYTE> filePath = B64WtoBin(sectionName);
      wstring wfilePath = BinToW(filePath);

      // ファイル存在確認
      DWORD attr = GetFileAttributesW(wfilePath.c_str());
      if(attr == INVALID_FILE_ATTRIBUTES){
          // 存在しなければセクション削除
         if(WritePrivateProfileStringW(sectionName.c_str(), NULL, NULL, iniFilePath.c_str())){
            ++deletedCount;
         }
      }
   }

   return deletedCount;
}

std::wstring CommonLib::FindFirstMatchingFile(const std::wstring& pattern){
   WIN32_FIND_DATAW fd;
   HANDLE hFind = ::FindFirstFileW(pattern.c_str(), &fd);
   if(hFind == INVALID_HANDLE_VALUE){
      return L""; // 見つからなかった
   }

   // パターンからディレクトリ部分を抽出
   size_t lastSlash = pattern.find_last_of(L"\\/"); // 最後の \ または /
   std::wstring directory;
   if(lastSlash != std::wstring::npos){
      directory = pattern.substr(0, lastSlash + 1); // スラッシュ込みで切り出す
   }

   std::wstring result = directory + fd.cFileName; // フルパスを構築
   FindClose(hFind);
   return result;
}


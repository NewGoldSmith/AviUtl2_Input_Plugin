/**
 * @file CommonLib.h
 * @brief CommonLibの宣言
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
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
   /// @brief base64でエンコードされた文字列をデコードする。
   /// @param strB64 base64でエンコードされた文字列。
   /// @return デコードされたバイナリも含むデータ。
   std::vector<BYTE> B64AtoBin(const std::string& strB64);
   std::vector<BYTE> B64U8toBin(const std::u8string& strB64);
   std::vector<BYTE> B64WtoBin(const std::wstring& strB64);
   std::wstring BinToW(const std::vector<BYTE>& bin);
   std::vector<BYTE> WtoBin(const std::wstring& wstr);

   std::u8string BinToU8(const std::vector<BYTE>& bin);
   std::vector<BYTE> U8toBin(const std::u8string& u8str);
   std::string BinToA(const std::vector<BYTE>& bin);
   std::vector<BYTE> AtoBin(const std::string& str);

   /// @brief バイナリも含むデータをbase64にエンコードする。
   /// @param pbin バイトデータへのポインタ。
   /// @param size バイトデータの数。
   /// @return エンコードされた文字列。
   std::string BinToB64A(const std::vector<BYTE>& bin);
   std::wstring BinToB64W(const std::vector<BYTE>& bin);

   /// @brief インスタンスハンドルからそのインスタンス元のモジュール
   /// パスを取得し、拡張子をINIに変えたパスネームを返す。
   /// @param hInst モジュールハンドル。
   /// @return モジュールハンドルのフルパス文字列に拡張子をINIに
   /// 変えた文字列を返す。
   std::wstring MakeIniPathNameW(HINSTANCE hInst);
   /// @brief インスタンスハンドルからそのインスタンス元のモジュール
   /// パスを取得し、拡張子をINIに変えたパスネームを返す。
   /// @param hInst モジュールハンドル。
   /// @return モジュールハンドルのフルパス文字列に拡張子をINIに
   /// 変えた文字列を返す。
   std::u8string MakeIniPathNameU8(HINSTANCE hInst);

   /// @brief targetの存在しているキーの値をsourceの値で更新する。
   /// @param target 更新する対象のmap
   /// @param source 更新元データのmap
   void UpdateExistingKeysU8(
      std::map<std::string, std::string>& target
      , const std::map<std::string, std::string>& source);
   void UpdateExistingKeysW(
      std::map<std::wstring, std::wstring>& target
      , const std::map<std::wstring, std::wstring>& source);
   /// @brief targetに無いキーのみ、sourceからキーとvalueをコピーする。
   /// @param target Insert対象のmap。
   /// @param source 比較対象のmap。
   /// @return InsertがあったらTRUE、それ以外FALSE。
   BOOL InsertOnlyNewKeysU8(
      std::map<std::string, std::string>& target,
      const std::map<std::string, std::string>& source);
   BOOL InsertOnlyNewKeysW(
      std::map<std::wstring, std::wstring>& target,
      const std::map<std::wstring, std::wstring>& source);

   /// @brief DDB,DIBをファイルに保存する。
   /// @param hdc デバイスコンテキストハンドル
   /// @param hBitmap ビットマップハンドル。
   /// @param filename ファイルパスネイム。
   /// @return 成功ならTRUE。それ以外FALSE。
   BOOL SaveBitmapToFileW(HDC hdc, HBITMAP hBitmap, const std::wstring& filename);
   /// @brief 指定されたセクションのキーとバリューのペアを取得する
   /// @param section セクション名
   /// @param filename INIファイルのパスネイム。
   /// @return 取得したキーとバリューのmap。
   std::map<std::wstring, std::wstring>
      ReadIniToMapW(const std::wstring& section, const std::wstring& filename);
      /// @brief INIファイルのセクションから設定をmapに読み込む
      /// @param section セクション名
      /// @param filename INIファイルパス
      /// @return 読み込まれたmap
   std::map<std::u8string, std::u8string>
      ReadIniToMapU8(const std::u8string& section, const std::u8string& filename);
         /// @brief INIファイルのセクションにmapの設定を書き込む  
         /// @param section セクション名
         /// @param settings セッティングmap
         /// @param iniPath INIパス
         /// @return 成功すればTRUE、それ以外FALSE
   BOOL WriteMapToIniW(
      const std::wstring& section
      , const std::map<std::wstring, std::wstring>& settings
      , const std::wstring& iniPath);
   /// @brief INIファイルのセクションにmapの設定を書き込む  
   /// @param section セクション名
   /// @param settings セッティングmap
   /// @param iniPath INIパス
   /// @return 成功すればTRUE、それ以外FALSE
   BOOL WriteMapToIniU8(
      const std::u8string& section
      , const std::map<std::u8string, std::u8string>& settings
      , const std::u8string& iniPath);
   /// @brief INIセクションが存在するか調べる。
   /// @param section セクション名
   /// @param path INIファイルパス
   /// @return セクションが存在していればTRUE、
   /// ファイルが存在していない場合FALSE、
   /// それ以外FALSE。
   BOOL IniSectionExistsW(const std::wstring& section, const std::wstring& path);
   /// @brief INIセクションが存在するか調べる。
   /// @param section セクション名
   /// @param path INIファイルパス
   /// @return セクションが存在していればTRUE、
   /// ファイルが存在していない場合FALSE、
   /// それ以外FALSE。
   BOOL IniSectionExistsU8(const std::u8string& section, const std::u8string& path);
   /// @brief マップをマージする。元のマップは変更しない。
   /// @param a マップa。
   /// @param b マップb。
   /// @return マージされたマップ。
   std::map<std::string, std::string>
      MergeMapU8(
         const std::map<std::string, std::string>& a
         , const std::map < std::string, std::string >& b);
      /// @brief tempディレクトリに作成するテンプファイルパスストリングを作成する。
      /// @param InFileName テンプファイルに追加するファイル名。
      /// @param OutPathName 作成したテンプファイルのパスとファイル名を入れるstring。
      /// @return 作成したテンプファイルの文字列のサイズ。
   std::size_t MakeTempPathNameW(
      const std::wstring& InFileName
      , std::wstring& OutPathName);

   /// @brief メインウィンドウを探す。
   /// @param threadId スレッドID
   /// @return ウィンドウハンドル。
   HWND FindMainWindowFromThread(DWORD threadId);
   /// @brief RECTが画面内に収まるように調整する。
   /// @param rc RECT構造体。この値は書き換えられる可能性があります。
   /// @return 成功:TRUE、それ以外:FALSE
   BOOL AdjustRectToFitScreenWithOverflow(RECT& rc);
   /// @brief GDIオブジェストをコンストラクタでセレクトして、デストラクタでリストアする
   /// @tparam T GDIオブジェクトハンドル
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
      operator HDC() const{ return hdc; } // 暗黙変換も可能
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

   /// @brief DDBからDIBに変換。
   /// @param hdc デバイスコンテキスト
   /// @param hDDB DDBハンドル
   /// @return DIBハンドル
   HBITMAP DDBToDIB(HDC hdc, HBITMAP hDDB);
   /// @brief DIBの複製を作成する。
   /// @param hSrcDIB 複製元。
   /// @return 複製したハンドル。
   HBITMAP DupDIB(HBITMAP hSrcDIB);
   /// @brief ピクセルの初期化がされていないDIBを作成する。
   /// @param Width 幅
   /// @param Height 高さ
   /// @return DIBハンドル
   HBITMAP CreateDIB24(int Width, int Height);

   HBITMAP CreateDIB24ppBits(int Width, int Height, void** ppBits);


   HBITMAP LoadPngFromResource(HINSTANCE hInst, int resID);
   /// @brief INIファイル中のセクション名をBase64で
   /// デコードした文字列を、パスファイル名とみなしてその
   /// パスファイル名のファイルが存在するか確認する。。
   /// 存在していなければそのセクションを削除。
   /// @param ignoreList 無視するセクションリスト。
   /// @param iniFilePath INIファイルのパスネーム。
   /// @return 削除したセクションの数。
   INT CleanInvalidSectionsW(const std::vector<std::wstring>& ignoreList, const std::wstring& iniFilePath);
   std::wstring FindFirstMatchingFile(const std::wstring& pattern);

}


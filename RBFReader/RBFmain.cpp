/**
 * @file RBFmain.cpp
 * @brief ReversiBinaryFormatファイルの読み込み実装
 * @author ＫＥＮくん
 * @author web site <https: //spring-fragrance.mints.ne.jp/aviutl/>
 * @date 2025 Original
 * @author Gold Smith
 * @date 2025 modified
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#pragma once

#define NOMINMAX
#include "build_info.h"
#include <Windows.h>
#include "..\aviutl2_sdk\input2.h"
#include "RBFmain.h"
#include <format>
#include <shlwapi.h>
#include <pathcch.h>
#include <wrl/client.h>
#include "ST_FOR_INST.h"
#include "..\CommonLib\CommonLib.h"
#include "..\BitManip256\bit_manip256.h"
#include "..\UtilDefAviUtl2\UtilDefAviUtl2.h"
#include "..\CommonConfigDlg\PublicCfgDlgInterface.h"
#include "resource.h"


#pragma comment(lib, "vfw32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib,  "../CommonLib/" _CRT_STRINGIZE($CONFIGURATION) "/CommonLib-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../BitManip256/" _CRT_STRINGIZE($CONFIGURATION) "/BitManip256-" _CRT_STRINGIZE($CONFIGURATION) ".lib")
#pragma comment(lib,  "../UtilDefAviUtl2/" _CRT_STRINGIZE($CONFIGURATION) "/UtilDefAviUtl2-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;
using namespace Microsoft::WRL;
using namespace UtilDefAviUtl2;
using namespace bit_manip;
using namespace bit_manip::AVX;
using namespace CommonLib;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace{
#pragma push_macro("WIDEN2")
#undef WIDEN2 
#pragma push_macro("WIDEN")
#undef WIDEN
#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
   /// @brief コンパイル年月日文字列を作成する関数を定義
   /// @return コンパイルされた年月日文字列
   wstring MakeDateStringW(){
      map<wstring, int> month_map =
      {
         {L"Jan", 1}, {L"Feb", 2}, {L"Mar", 3}, {L"Apr", 4}
         ,{L"May", 5}, {L"Jun", 6}, {L"Jul", 7}, {L"Aug", 8}
         ,{L"Sep", 9}, {L"Oct", 10}, {L"Nov", 11}, {L"Dec", 12}
      };

      wstring month_str(WIDEN(__DATE__), 3);
      int month = month_map[month_str];

      int day = stoi(wstring(WIDEN(__DATE__) + 4, 2));
      int year = stoi(wstring(WIDEN(__DATE__) + 7, 4));

      return std::format(L"{:04}{:02}{:02}", year, month, day);
   }
#pragma pop_macro("WIDEN2")
#pragma pop_macro("WIDEN")
}


//---------------------------------------------------------------------
//	入力プラグイン構造体定義
//---------------------------------------------------------------------
INPUT_PLUGIN_TABLE input_plugin_table = {
   INPUT_PLUGIN_TABLE::FLAG_VIDEO /*| INPUT_PLUGIN_TABLE::FLAG_AUDIO*/
   ,L"Riversi Board Format Reader (sample)"
   ,L"RbfFile (*.rbf)\0*.rbf\0"
   ,L"Reversi Board Format Reader version 0.01"
   ,[](LPCWSTR lpcwstrzInFilePathName)->INPUT_HANDLE{
   // 入力ファイルをオープンする関数へのポインタ
   // file		: ファイル名
   // 戻り値	: TRUEなら入力ファイルハンドル
   /// @brief func_open
   DPTD("func_open");

   /// @detail pInstをINPUT_HANDLEにキャストして返す。
   /// 返す前に初期設定をする。途中でエラーがあった場合、
   /// デフォルトデリーターでデリートされるトラップを設定。
   unique_ptr<STRUCTURE_FOR_INSTANCE> 
      pInst(new(nothrow) STRUCTURE_FOR_INSTANCE((HMODULE)&__ImageBase));
   if(!pInst){
      DPT("Allocation failed.");
      return NULL;
   };

   // ファイルパス名をコピー
   pInst->FilePathNameW = lpcwstrzInFilePathName;

   pInst->flag = INPUT_PLUGIN_TABLE::FLAG_VIDEO;

   /// @brief INIファイルが無ければ、作成し、初期設定を書き込む
   if(!(::PathFileExistsW(MakeIniPathNameW(pInst->hModule).c_str()))){
      MakeIniFile(pInst->hModule, BUILD_NUMBER);
   }

   /// @brief METAセクションLAST_UPDATE_INIの書き込み
   if(!WriteMapToIniW(
      L"META"
      , { {L"LAST_BUILD_DATE",MakeDateStringW()},{L"BUILD_NUMBER",to_wstring(BUILD_NUMBER)} }
      , MakeIniPathNameW(pInst->hModule))){
      DPT("META section write error.");
      return NULL;
   }

   /// @brief コモンセクション読み込み
   map cfg_common = ReadIniToMapW(
      L"COMMON"
      , MakeIniPathNameW(pInst->hModule));

   /// @brief 入力ファイルのセクションがあるか確認し、
   /// 無ければセクションを作成し、デフォルト設定を書き込む。

   if(!IniSectionExistsW(
      BinToB64W(WtoBin(pInst->FilePathNameW))
      , MakeIniPathNameW(pInst->hModule))){

      /// @brief ファイルごとのデフォルト初期設定値読み込み
      map<wstring, wstring> deault_file_cfg = ReadIniToMapW(
         STR_DEFAULT_INITIAL_FILE_SETTINGS
         , MakeIniPathNameW(pInst->hModule));

      /// @brief ファイルパスとファイル名をbase64にエンコード
      deault_file_cfg[L"FILE_PATH_NAME"] = BinToB64W(
         WtoBin(pInst->FilePathNameW));

      /// @brief ファイルパスネームをbase64にエンコードしたものをセクション名に設定
      WriteMapToIniW(
         BinToB64W(WtoBin(pInst->FilePathNameW))
         , deault_file_cfg
         , MakeIniPathNameW(pInst->hModule));
   } else{
      /// 既にセクションがある場合も必要なキーが揃っていなければ、そのキーを初期値で更新。
      map<wstring, wstring> deault_file_cfg = ReadIniToMapW(
         STR_DEFAULT_INITIAL_FILE_SETTINGS
         , MakeIniPathNameW(pInst->hModule));
      map a_file_cfg = ReadIniToMapW(
         BinToB64W(WtoBin(pInst->FilePathNameW))
         , MakeIniPathNameW(pInst->hModule));
      /// @brief キーが揃っているか確認し、揃ってなければそのキーをInsert。
      if(InsertOnlyNewKeysW(a_file_cfg, deault_file_cfg)){
         WriteMapToIniW(
            BinToB64W(WtoBin(pInst->FilePathNameW))
            , a_file_cfg
            , MakeIniPathNameW(pInst->hModule));
      };
   }

   /// @brief 個別ファイル設定読み込み
   map a_file_cfg = ReadIniToMapW(
      BinToB64W(WtoBin(pInst->FilePathNameW))
      , MakeIniPathNameW(pInst->hModule));
   pInst->bmih = {
      /*biSize;         */ sizeof(BITMAPINFOHEADER)
      /*biWidth;        */ ,stoi(a_file_cfg[L"N_DOTS"])
      /*biHeight;       */ ,stoi(a_file_cfg[L"N_DOTS"])
      /*biPlanes;       */ ,1
      /*biBitCount;     */ ,24
      /*biCompression;  */ ,BI_RGB
      /*biSizeImage;    */ ,((stoi(a_file_cfg[L"N_DOTS"]) * 3 + 3) & ~3) * (DWORD)stoi(a_file_cfg[L"N_DOTS"])
      /*biYPelsPerMeter;*/
      /*biXPelsPerMeter;*/
      /*biClrUsed;      */
      /*biClrImportant; */
   };

   pInst->vrbf.clear();
   LoadM256ArrayW(pInst->FilePathNameW, pInst->vrbf);

   pInst->video_stream_info = {
   /*fccType              */streamtypeVIDEO
   /*fccHandler           */,0
   /*dwFlags              */,0
   /*dwCaps               */,0
   /*wPriority            */,0
   /*wLanguage            */,0
   /*dwScale              */,(DWORD)stoi(a_file_cfg[L"SCALE"]) // 1秒あたりの分母
   /*dwRate               */,(DWORD)stoi(a_file_cfg[L"RATE"]) // 1秒あたりの分子（1/1=1fps）
   /*dwStart              */,0
   /*dwLength             */,(DWORD)pInst->vrbf.size()// フレーム数（1フレーム）
   /*dwInitialFrames      */,0
   /*dwSuggestedBufferSize*/,pInst->bmih.biSizeImage
   /*dwQuality            */
   /*dwSampleSize         */
   /*rcFrame              */
   /*dwEditCount          */
   /*dwFormatChangeCount  */
   /*szName[64]           */
   };

   SetRect(&pInst->video_stream_info.rcFrame, 0, 0, stoi(a_file_cfg[L"N_DOTS"]), stoi(a_file_cfg[L"N_DOTS"]));
   INPUT_PLUGIN_TABLE* pipt = GetInputPluginTable();
   wcscpy_s(pInst->video_stream_info.szName, pipt->information);

   /// @brief releaseでトラップ解除。
   return pInst.release();
   }

   , [](INPUT_HANDLE ih)->bool{
   // 入力ファイルをクローズする関数へのポインタ
   // ih		: 入力ファイルハンドル
   // 戻り値	: TRUEなら成功
   // @brief func_close
      DPTD("func_close");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;
      delete pInst;
      return true;
      }

      , [](INPUT_HANDLE ih, INPUT_INFO* iip)->bool{
            // @brief 入力ファイルの情報を取得する関数へのポインタ
            // ih		: 入力ファイルハンドル
            // iip		: 入力ファイル情報構造体へのポインタ
            // 戻り値	: TRUEなら成功
            //func_info_get
      DPTD("func_info_get");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;
      iip->flag = 0;
      if(pInst->flag & INPUT_PLUGIN_TABLE::FLAG_VIDEO){
         iip->flag |= INPUT_INFO::FLAG_VIDEO | INPUT_INFO::FLAG_TIME_TO_FRAME;
         iip->rate = pInst->video_stream_info.dwRate;
         iip->scale = pInst->video_stream_info.dwScale;
         iip->n = pInst->video_stream_info.dwLength;
         iip->format = (BITMAPINFOHEADER*)&pInst->bmih;
         iip->format_size = pInst->bmih.biSize;
      }

      if(pInst->flag & INPUT_PLUGIN_TABLE::FLAG_AUDIO){
         iip->flag |= INPUT_INFO::FLAG_AUDIO;
         iip->audio_n = pInst->audio_stream_info.dwLength;
         iip->audio_format = (WAVEFORMATEX*)pInst->audioformat;
         iip->audio_format_size = pInst->audioformatsize;
      }

      return true;
      }

      , [](INPUT_HANDLE ih, int frame, void* buf)->int{
      //func_read_video
      // 画像データを読み込む関数へのポインタ
      // ih		: 入力ファイルハンドル
      // frame	: 読み込むフレーム番号
      // buf		: データを読み込むバッファへのポインタ
      // 戻り値	: 読み込んだデータサイズ

      DPTD("func_read_video");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;

      unique_ptr<remove_pointer_t<HBITMAP>, decltype(DeleteObject)*> hBitmap=
      { CreateDIB24(pInst->bmih.biWidth, pInst->bmih.biHeight),DeleteObject };

      DrawBoardDIB(pInst->vrbf[frame], hBitmap.get());

      unique_ptr<remove_pointer_t<HDC>, void(*)(HDC hDC)> hDC
      { GetDC(NULL),[](HDC hDC){ReleaseDC(NULL,hDC); } };

      ::GetDIBits(hDC.get(), hBitmap.get(), 0, pInst->bmih.biHeight, buf, (BITMAPINFO*)&pInst->bmih, DIB_RGB_COLORS);
      return pInst->bmih.biSizeImage;
      }

      , [](INPUT_HANDLE ih, int start, int length, void* buf)->int{
      ///func_read_audio
      DPTD("func_read_audio");
      STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;
      LONG size{};
      int samplesize{};

      samplesize = ((WAVEFORMATEX*)pInst->audioformat)->nBlockAlign;
      if(AVIStreamRead(pInst->paudio, start, length, buf, samplesize * length, NULL, &size) != S_OK) return 0;
      return size; }

      , [](HWND hwnd, HINSTANCE dll_hinst)->bool{
      // 入力設定のダイアログを要求された時に呼ばれる関数へのポインタ (nullptrなら呼ばれません)
      // hwnd			: ウィンドウハンドル
      // dll_hinst	: インスタンスハンドル
      // 戻り値		: TRUEなら成功
       //func_config
      DPTD("func_config");

      /// @brief INIファイルが無ければ、作成し、初期設定を書き込む
      if(!(::PathFileExistsW(MakeIniPathNameW(dll_hinst).c_str()))){
         MakeIniFile(dll_hinst, BUILD_NUMBER);
      }

      /// @brief METAセクションLAST_BUILD_DATE、BUILD_NUMBERの書き込み
      if(!WriteMapToIniW(
         L"META"
         , { {L"LAST_BUILD_DATE",MakeDateStringW()},{L"BUILD_NUMBER",to_wstring(BUILD_NUMBER)} }
         , MakeIniPathNameW(dll_hinst))){
         DPT("META section write error.");
         return 0;
      }

         /// モジュールファイルを探しロードし、関数ポインタからCoCreateInstanceを実行。
      WCHAR path[MAX_PATH + 1];
      if(!::GetModuleFileNameW(dll_hinst, path, MAX_PATH))
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
         ::MessageBoxA(hwnd
                     ,"CommonConfigDlg*.dllが見つかりません。プラグインと同じディレクトリに配置してください。"
                     ,"Error"
                     ,MB_OK | MB_ICONEXCLAMATION);
         return FALSE;
      }

      unique_ptr<remove_pointer_t<HMODULE>, decltype(FreeLibrary)*>hDll
      { ::LoadLibraryW(strfound.c_str()),FreeLibrary };
      if(!hDll){
         DPLE;
         return FALSE;
      }

      using PFN_CO_CREATE_INSTANCE =
         HRESULT(WINAPI*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);

      PFN_CO_CREATE_INSTANCE pfnCoCreateInstance =
         (PFN_CO_CREATE_INSTANCE)::GetProcAddress(hDll.get(), CO_CREATE_INSTANCE_STRING);
      if(!pfnCoCreateInstance){
         DPLE;
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
         pDlg->DialogBoxParamW(hwnd, dll_hinst, result);
      }

      // DLLを開放されても設定が残るように保存しておいてください
      return true; }

   , [](INPUT_HANDLE ih, int type, int index)->int{
   // 入力ファイルの読み込み対象トラックを設定する関数へのポインタ (FLAG_MULTI_TRACKが有効の時のみ呼ばれます)
	// func_open()の直後にトラック数取得、トラック番号設定が呼ばれます。※オープン直後の設定以降は呼ばれません
	// ih		: 入力ファイルハンドル
	// type		: メディア種別 ( 0 = 映像 / 1 = 音声 )
	// index	: トラック番号 ( -1 が指定された場合はトラック数の取得 )
	// 戻り値	: 設定したトラック番号 (失敗した場合は -1 を返却)
	//			  トラック数の取得の場合は設定可能なトラックの数 (メディアが無い場合は 0 を返却)
        // func_set_track
      DPTD("func_set_track");
      return -1;
      }

      , [](INPUT_HANDLE ih, double time)->int{
      // 映像の時間から該当フレーム番号を算出する時に呼ばれる関数へのポインタ (FLAG_TIME_TO_FRAMEが有効の時のみ呼ばれます)
      // 画像データを読み込む前に呼び出され、結果のフレーム番号で読み込むようになります。
      // ※FLAG_TIME_TO_FRAMEを利用する場合のINPUT_INFOのrate,scale情報は平均フレームレートを表す値を設定してください
      // ih		: 入力ファイルハンドル
      // time		: 映像の時間(秒)
      // 戻り値	: 映像の時間に対応するフレーム番号
            // func_time_to_frame
         DPTD("func_time_to_frame");
         STRUCTURE_FOR_INSTANCE* pInst = (STRUCTURE_FOR_INSTANCE*)ih;

         double fps = (double)pInst->video_stream_info.dwRate
            / (double)pInst->video_stream_info.dwScale;
         return (int)(time * fps); // 切り捨て
         }
};

//---------------------------------------------------------------------
//	初期化
//---------------------------------------------------------------------
void func_init( HMODULE h ) {
   AVIFileInit();
}

//---------------------------------------------------------------------
//	終了
//---------------------------------------------------------------------
void func_exit( void ) {
   AVIFileExit();
}

//---------------------------------------------------------------------
//	入力プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport)constexpr INPUT_PLUGIN_TABLE* GetInputPluginTable(void)
{
   return &input_plugin_table;
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
   switch(ul_reason_for_call){
      case DLL_PROCESS_ATTACH:
         DPTD("DLL_PROCESS_ATTACH");
         func_init(hModule);
         break;

      case DLL_PROCESS_DETACH:
         DPTD("DLL_PROCESS_DETACH");
         if(lpReserved == nullptr){
            func_exit();
         }
         break;

      case DLL_THREAD_ATTACH:
         DPTD("DLL_THREAD_ATTACH");
         break;
      case DLL_THREAD_DETACH:
         DPTD("DLL_THREAD_DETACH");
         break;
   }
   return TRUE;
}




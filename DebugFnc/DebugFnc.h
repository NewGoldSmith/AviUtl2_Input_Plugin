/**
 * @file DebugFnc.h
 * @brief デバッグ関数の宣言
 * @author Gold Smith
 * @date 2023 2024 2025
 *
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 * @detail なるべく直接実装ファイル(.cppファイル等)に
 *includeされることを推奨します。
 */
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <minwindef.h>

/// @detail DEBUG_LOG_LEVELの使い方
/// デバッグ出力レベルを設定する場合に使います。
/// DPTDLマクロを使う時、適切な設定をすることにより、出力レベルが変わります。
/// DPTDLマクロを使う前に次を定義します。
/// #pragma push_macro("DEBUG_LOG_LEVEL")
/// #undef DEBUG_LOG_LEVEL
/// #define DEBUG_LOG_LEVEL num
/// numに適切な数値を設定します。
/// 使い終わったら
/// #pragma pop_macro("DEBUG_LOG_LEVEL")
/// と、定義します。
/// 使い方の例
/// /*ファイル冒頭で*/
/// #pragma push_macro("DEBUG_LOG_LEVEL")
/// #undef DEBUG_LOG_LEVEL
/// #define DEBUG_LOG_LEVEL 2
/// 
/// /*ファイルのどこかで*/
/// DPTDL("IDOK or IDCANCEL", 1);
/// 
/// /*ファイルの終わりで*/
/// #pragma pop_macro("DEBUG_LOG_LEVEL")
/// 
/// この場合、レベルに達していませんのでデバッグ出力に出力されません。

#define DEBUG_LOG_LEVEL 0L

namespace DebugFnc {

   std::string binary_to_string(const uint8_t b);
   /// @brief ボードの文字列を作成。８行に渡る。
   /// @param p プレーヤーボード
   /// @param o 相手ボード
   /// @param m ムーブ位置
   /// @param cp プレーヤー文字コード
   /// @param co 相手文字コード
   /// @param cm ムーブ文字コード
   /// @param cv 空の文字コード
   /// @return ８行の文字列
   std::vector<std::string> BinaryBoardsToString(
      const uint64_t p
      ,const uint64_t o
      ,const uint64_t m
      ,const char cp = 'X'
      ,const char co = 'C'
      ,const char cm = 'M'
      ,const char cv = '#');
   void print_binary(const uint8_t b);
   void dout_binary(const uint8_t b);
   std::string binary_to_string(const uint16_t b);
   void print_binary(const uint16_t b);
   void dout_binary(const uint16_t b);
   std::string binary_to_string(const uint64_t b);
   std::vector<std::string> GetStrings_b2s(const uint64_t b);
   void print_binary(const uint64_t b);
   /// @brief デバッグ出力
   /// @tparam T 算術可能型または、テキスト、及びstd::string 
   /// @param strFile ファイルテキスト
   /// @param strLine 行テキスト
   /// @param v 評価パラメータ
   /// @return 生成されデバッグ出力された文字列と同じ文字列。
   template <typename T>
   inline std::string dout(const std::string& strFile, const std::string& strLine, const T& v){

      // std::to_stringを使用できない型の場合に対応
      std::string valueStr;
      if constexpr(std::is_same_v<T,bool>){
         valueStr = v != 0 ? "true":"false";
      } else if constexpr(std::is_arithmetic_v<T>){
         valueStr = std::to_string(v);
      } else if constexpr( std::is_same_v<T, const char*> || std::is_same_v<T, char*> ){
         valueStr.assign(v);
      } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const std::string&>){
         valueStr = v;
      } else if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>){
         valueStr.assign(v);
      } else{
         valueStr = "<non-arithmetic type>";
      }

      // デバッグ文字列の作成
      std::string dstr = strFile + "(" + strLine + "):" + " type:" + typeid(v).name() + " value:" + valueStr + "\r\n";
      ::OutputDebugStringA(dstr.c_str());
      return dstr;
   }

   inline std::string dout_string(const std::string& strFile, const std::string& strLine, const std::string& v){
      // デバッグ文字列の作成
      std::string dstr = strFile + "(" + strLine + "):" + v + "\r\n";
      ::OutputDebugStringA(dstr.c_str());
      return dstr;
   }

   inline std::string dout_string_level(const std::string& file, const std::string& line, const std::string& msg, LONG defLevel, LONG level){
      if(level < defLevel) return "";

      std::string dstr = file + "(" + line + "):" + "[" + std::to_string(defLevel) + "] " + ": " + msg + "\r\n";
      ::OutputDebugStringA(dstr.c_str());
      return dstr;
   }

   void dout_binary(const uint64_t b);
   std::string GetErrString(const DWORD dw);
   const std::string ErrOut_(
      const DWORD dw
      , LPCSTR const lpcszFile
      , const DWORD dwLine
      , LPCSTR const lpcszFunction
      , const std::string& lpszOpMessage = "");

   
/// @brief デバッグ・プリント・ラスト・エラー
/// @brief ラストエラーの内容をデバッグ出力に出力する。
/// @return ラストエラーの内容をstd::stringで返す。
#define DPLE DebugFnc::ErrOut_(::GetLastError(),__FILE__,__LINE__,__FUNCTION__)
/// @brief デバッグ・プリント・ラスト・エラー・ユーザーメッセージ
/// @param cv 空の文字コード
/// @return ８行の文字列
#define DPLEM(mes) DebugFnc::ErrOut_(::GetLastError(),__FILE__,__LINE__,__FUNCTION__,mes)
/// @brief デバッグ・プリント・エラー・ナンバー
/// @param err_num エラーコード
/// @return デバッグ出力と同じ文字列
#define DPEN(err_num) DebugFnc::ErrOut_(err_num,__FILE__,__LINE__,__FUNCTION__)
/// @brief デバッグ・プリント・エラー・ナンバー・ユーザーメッセージ
/// @param err_num エラーコード
/// @param mes ユーザーメッセージ
/// @return デバッグ出力と同じ文字列
#define DPENM(err_num,mes) DebugFnc::ErrOut_(err_num,__FILE__,__LINE__,__FUNCTION__,mes)
/// @brief デバッグ・プリント・タイプ・アンド・値
#define DPTV(s) DebugFnc::dout(__FILE__, _CRT_STRINGIZE(__LINE__),s)
/// @brief デバッグ・プリント・テキスト
/// @brief デバッグ出力にテキストを出力する。
/// @param s 入力std::string。
/// @return 入力と同じ内容のstd::string。
#define DPT(s) DebugFnc::dout_string(__FILE__,_CRT_STRINGIZE(__LINE__),s)
/// @brief プリント・テキスト
#define PT(s_) ([&](){std::stringstream ss_; ss_ << __FILE__ << "(" << __LINE__ << "): " << s_ << std::endl; std::cout << ss_.str(); return ss_.str();}())
/// @brief デバッグ・プリント・何か
#define DPA(s) DebugFnc::dout(__FILE__,_CRT_STRINGIZE(__LINE__),s)
/// @brief デバッグ・プリント・バイナリー
#define DPB(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::dout_binary(b);}
/// @brief デバッグ・プリント・m256・ステータス
#define DP256S(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::DebugPrintBB256Satus(b);}

#ifdef _DEBUG
/// @brief デバッグ・プリント・タイプ・アンド・値・デバッグビルド時
#define DPTVD(s) DebugFnc::dout(__FILE__, _CRT_STRINGIZE(__LINE__),s)
/// @brief デバッグ・プリント・何でも・デバッグビルド時
#define DPAD(s) DebugFnc::dout(__FILE__,_CRT_STRINGIZE(__LINE__),s)
/// @brief デバッグ・プリント・テキスト・デバッグビルド時
#define DPTD(s) DebugFnc::dout_string(__FILE__,_CRT_STRINGIZE(__LINE__),s)
/// @brief デバッグ・プリント・テキスト・デバッグビルド時・レベル
#define DPTDL(s,l) DebugFnc::dout_string_level(__FILE__,_CRT_STRINGIZE(__LINE__),s,DEBUG_LOG_LEVEL,l)
/// @brief デバッグ・プリント・ファンクション名・テキスト・デバッグビルド時
#define DPFTD(s) DebugFnc::dout_string(__FILE__,_CRT_STRINGIZE(__LINE__),std::string(__FUNCTION__) + s)
/// @brief デバッグ・プリント・バイナリー・デバッグビルド時
#define DPBD(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::dout_binary(b);}
/// @brief デバッグ・プリント・m256・ステータス・デバッグビルド時
#define DP256SD(b){::OutputDebugStringA(__FILE__ "(" _CRT_STRINGIZE(__LINE__)"):" #b "\r\n");DebugFnc::DebugPrintBB256Satus(b);}
#else
#define DPTVD(s) __noop
#define DPAD(s) __noop
#define DPTD(s) __noop
#define DPTDL(s,l) __noop
#define DPFTD(s) __noop
#define DPBD(b) __noop
#define DP256SD(b) __noop
#endif 
   // _DEBUG
}

/**
 * @file DebugFnc.cpp
 * @brief デバッグ関数の実装
 * @author Gold Smith
 * @date 2023,2025
 *
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 */
#include "DebugFnc.h"

std::string DebugFnc::binary_to_string(const uint8_t b)
{
   std::stringstream ss;
   for (int j = 7; j >= 0; --j) {
      ss << (b >> j & 1) << " ";
   }
   return ss.str().c_str();
}

 std::vector<std::string> DebugFnc::BinaryBoardsToString(
   const uint64_t p
   ,const uint64_t o
   ,const uint64_t m
   ,const char cp
   ,const char co
   ,const char cm
   ,const char cv){
   std::vector<std::string> result(8);
   for( int i = 63; i >= 0; --i ){
      if( p & (1ULL << i) ){
         result[7-(i / 8)] += cp;
      } else if( o & (1ULL << i) ){
         result[7-(i / 8)] += co;
      } else if( m & (1ULL << i) ){
         result[7-(i / 8)] += cm;
      } else{
         result[7-(i / 8)] += cv;
      }
      if( i % 8 != 0 ){
         result[7-(i / 8)] += ' ';
      }
   }
   return result;
}

void DebugFnc::print_binary(const uint8_t b)
{
   std::cout << binary_to_string(b) << std::endl;
}

void DebugFnc::dout_binary(const uint8_t b)
{
   OutputDebugStringA((binary_to_string(b) + "\r\n").c_str());
}

std::string DebugFnc::binary_to_string(const uint16_t b)
{
   std::stringstream ss;
   ss << binary_to_string(((uint8_t*)&b)[1]);
   ss << std::endl;
   ss << binary_to_string(((uint8_t*)&b)[0]);
   return ss.str().c_str();
}

void DebugFnc::print_binary(const uint16_t b)
{
   std::cout << binary_to_string(b) << std::endl;
}

void DebugFnc::dout_binary(const uint16_t b)
{
   OutputDebugStringA((binary_to_string(b) + "\r\n").c_str());
}

std::string DebugFnc::binary_to_string(const uint64_t b)
{
   std::stringstream ss;
   for (int j = 7; j >= 0; --j) {
      ss << binary_to_string(((uint8_t*)&b)[j]);
      if (j == 0) {
         break;
      }
      ss << std::endl;
   }
   return ss.str().c_str();
}

std::vector<std::string> DebugFnc::GetStrings_b2s(const uint64_t b){
   std::stringstream ss;
   std::vector<std::string>vs(8);
   for( int j = 7; j >= 0; --j ){
      vs[7-j] = binary_to_string(((uint8_t*)&b)[j]);
      if( j == 0 )
         break;
   }
   return vs;
}

void DebugFnc::print_binary(const uint64_t b)
{
   std::cout << binary_to_string(b) << std::endl;
}

void DebugFnc::dout_binary(const uint64_t b)
{
   ::OutputDebugStringA((binary_to_string(b) + "\r\n").c_str());
}

//std::string DebugFnc::dout(const std::string& str){
//	::OutputDebugStringA((str + "\r\n").c_str());
//	return str;
//}

std::string DebugFnc::GetErrString(const DWORD dw)
{
      LPVOID lpMsgBuf;

      ::FormatMessageA(
         FORMAT_MESSAGE_ALLOCATE_BUFFER
         | FORMAT_MESSAGE_FROM_SYSTEM
         | FORMAT_MESSAGE_IGNORE_INSERTS
         | FORMAT_MESSAGE_MAX_WIDTH_MASK
         , NULL
         , dw
         , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
         , (LPSTR)&lpMsgBuf
         , 0, NULL);
      std::string str=(char*)lpMsgBuf;
      ::LocalFree(lpMsgBuf);
      return str;
}

const std::string DebugFnc::ErrOut_(const DWORD dw, LPCSTR const lpcszFile, const DWORD dwLine, LPCSTR const lpcszFunction, const std::string &lpszOpMessage) {
   const std::string strOpMessage = lpszOpMessage.empty() ?
      "" :
      "User Message:\"" + lpszOpMessage + "\"";
   std::stringstream ss;
   ss << lpcszFile << "(" << dwLine << "):error C" << dw << ":"\
      << GetErrString(dw)
      << "function name:" << lpcszFunction
      << " " << strOpMessage << "\r\n";
   ::OutputDebugStringA(ss.str().c_str());
   return ss.str();
}

/**
 * @file DllMain.cpp
 * @brief DllMainの実装
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#include "DllMain.h"
#include "../DebugFnc/DebugFnc.h"
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

void func_init(HMODULE hModule){

}

void func_exit(){

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
      case DLL_THREAD_DETACH:
         break;
   }
   return TRUE;
}

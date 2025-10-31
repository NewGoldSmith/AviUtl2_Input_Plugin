/**
 * @file WinMain.cpp
 * @brief Main関数の実装
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */

#include "WinMain.h"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int APIENTRY wWinMain
(_In_ HINSTANCE hInstance
 , _In_opt_ HINSTANCE hPrevInstance
 , _In_ LPWSTR    lpCmdLine
 , _In_ int       nCmdShow){
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   int result;
   {
      MainDialog Dlg(hInstance);
      result = (int)Dlg.DialogBoxParamW(NULL);
   }
   return result;
}

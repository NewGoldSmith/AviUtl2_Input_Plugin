/**
 * @file PublicCfgDlgInterface.h
 * @brief PublicCfgDlgInterfaceの定義
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 * @detail このファイルをインターフェースを使う側の.cppにもincludeする。
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */

#pragma once
#define NOMINMAX
#include <Windows.h>

static constexpr WCHAR FIND_MODULE_STRING[] = L"CommonConfigDlg*.dll";
static constexpr CHAR  CO_CREATE_INSTANCE_STRING[] = "CoCreateInstance";

/// クラスIDとして利用
// {B088CEA3-DA9E-4A35-849E-819B7B61D2BF}
static constexpr  IID CLSID_CFG_DLG = 
{ 0xb088cea3, 0xda9e, 0x4a35, { 0x84, 0x9e, 0x81, 0x9b, 0x7b, 0x61, 0xd2, 0xbf } };
static constexpr WCHAR CLSID_CFG_DLG_STRING[] = L"{B088CEA3-DA9E-4A35-849E-819B7B61D2BF}";

/// インターフェースIDとして利用
// {CD78F70E-65B3-4934-A8B9-1AF5ADD1F69A}
static constexpr IID IID_ICfgDlg = { 0xCD78F70E, 0x65B3, 0x4934, {0xA8, 0xB9, 0x1A, 0xF5, 0xAD, 0xD1, 0xF6, 0x9A} };

// {CD78F70E-65B3-4934-A8B9-1AF5ADD1F69A}
MIDL_INTERFACE("CD78F70E-65B3-4934-A8B9-1AF5ADD1F69A")
ICfgDlg : public IUnknown
{
public:
   virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
   virtual ULONG STDMETHODCALLTYPE AddRef();
   virtual ULONG STDMETHODCALLTYPE Release();
   virtual HRESULT STDMETHODCALLTYPE DialogBoxParamW(
      HWND hParentWnd,
      HMODULE hTargetModule,
      LONGLONG& result) = 0;
};



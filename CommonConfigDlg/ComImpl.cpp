/**
 * @file ComImpl.cpp
 * @brief ComImplの定義
 * @author Gold Smith
 * @date 2025
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 * @detail インターフェースの実装派生クラス
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */

#define NOMINMAX
#include <Windows.h>
#include <new>
#include "PublicCfgDlgInterface.h"
#include "CommonConfigDlg.h"
#include "..\DebugFnc\DebugFnc.h"
#pragma comment(lib,  "../DebugFnc/" _CRT_STRINGIZE($CONFIGURATION) "/DebugFnc-" _CRT_STRINGIZE($CONFIGURATION) ".lib")

using namespace std;

namespace{
   extern "C" IMAGE_DOS_HEADER __ImageBase;
   LONG g_lockCount;
   LONG g_objectCount;

   class ImpCfgDlg : public ICfgDlg{
      LONG m_refCount;
      CommonConfigDlg InsCmnCfgDlg;
   public:
      ImpCfgDlg(): m_refCount(1){}

     // IUnknown
      HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override{
         if(!ppv) return E_POINTER;
         *ppv = nullptr;

         GUID clsid;
         HRESULT hr = CLSIDFromString(CLSID_CFG_DLG_STRING, &clsid);
         if(FAILED(hr)) return hr;

         if((riid == __uuidof(ICfgDlg)) || (riid == IID_IUnknown)){
            *ppv = static_cast<ICfgDlg*>(this);
            AddRef();
            return S_OK;
         }
         return E_NOINTERFACE;
      }

      ULONG STDMETHODCALLTYPE AddRef() override{
         return InterlockedIncrement(&m_refCount);
      }

      ULONG STDMETHODCALLTYPE Release() override{
         ULONG res = InterlockedDecrement(&m_refCount);
         DPTD(format("ImpCfgDlg m_refCount:{}", m_refCount));
         if(res == 0){
            delete this;
         }
         return res;
      }

      // ICfgDlg
      HRESULT STDMETHODCALLTYPE DialogBoxParamW(
         HWND hParentWnd
         , HMODULE hTargetModule
         , LONGLONG& result) override{
         result = InsCmnCfgDlg.DialogBoxParamW(hParentWnd, hTargetModule);
         return S_OK;
      }
   };

   namespace{
      class ImpClassFactory : public IClassFactory{
      public:
          // IUnknown
         STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override{
            if(!ppv) return E_POINTER;
            if(riid == IID_IUnknown || riid == IID_IClassFactory){
               *ppv = static_cast<IClassFactory*>(this);
               AddRef();
               return S_OK;
            }
            *ppv = nullptr;
            return E_NOINTERFACE;
         };

         STDMETHODIMP_(ULONG) AddRef() override{ return InterlockedIncrement(&m_refCount); };

         STDMETHODIMP_(ULONG) Release() override{
            ULONG count = InterlockedDecrement(&m_refCount);
            DPT(format("ImpClassFactory m_refCount:{}", m_refCount));
            if(count == 0) delete this;
            return count;
         };

          // IClassFactory
         STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override{
            if(!ppvObject) return E_POINTER;
            *ppvObject = nullptr;

            if(pUnkOuter != nullptr) return CLASS_E_NOAGGREGATION;

            ImpCfgDlg* pObj = new (nothrow) ImpCfgDlg();
            if(!pObj) return E_OUTOFMEMORY;

            HRESULT hr = pObj->QueryInterface(riid, ppvObject);
            pObj->Release();  // 初期参照カウント調整
            return hr;

         };

         // IClassFactory
         STDMETHODIMP LockServer(BOOL fLock) override{
            if(fLock)
               InterlockedIncrement(&g_lockCount);
            else
               InterlockedDecrement(&g_lockCount);
            return S_OK;

         };

         ImpClassFactory(): m_refCount(1){}
      private:
         LONG m_refCount;
      };
   }

   /// @brief システムが呼び出す。
   extern "C"
      _Check_return_
      STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv){

      if(rclsid == CLSID_CFG_DLG){
         ImpClassFactory* factory = new (std::nothrow) ImpClassFactory();
         if(!factory) return E_OUTOFMEMORY;
         HRESULT hr = factory->QueryInterface(riid, ppv);
         factory->Release();  // 初期参照カウント調整
         return hr;
      }
      return CLASS_E_CLASSNOTAVAILABLE;
   }

   /// @brief システムが呼び出す。
   __control_entrypoint(DllExport)
      STDAPI  DllCanUnloadNow(void){
      return (g_objectCount == 0 && g_lockCount == 0) ? S_OK : S_FALSE;
   }

   extern  "C" __declspec(dllexport) HRESULT WINAPI DllRegisterServer(){
      HRESULT hr = S_OK;

      // 1. CLSID を文字列に変換
      LPOLESTR clsidString = nullptr;
      hr = StringFromCLSID(CLSID_CFG_DLG, &clsidString);
      if(FAILED(hr)) return hr;

      // 2. レジストリキーの構築
      wchar_t keyPath[256];
      swprintf_s(keyPath, L"Software\\Classes\\CLSID\\%s", clsidString);

      // 3. CLSID キーの作成
      HKEY hKey = nullptr;
      if(RegCreateKeyExW(HKEY_CURRENT_USER, keyPath, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS){
         CoTaskMemFree(clsidString);
         return REGDB_E_CLASSNOTREG;
      }

      // 4. 説明の設定（任意）
      RegSetValueExW(hKey, nullptr, 0, REG_SZ, (const BYTE*)L"Config Dialog COM Class", sizeof(L"Config Dialog COM Class"));

      // 5. InprocServer32 サブキーの作成
      HKEY hSubKey = nullptr;
      if(RegCreateKeyExW(hKey, L"InprocServer32", 0, nullptr, 0, KEY_WRITE, nullptr, &hSubKey, nullptr) == ERROR_SUCCESS){
         wchar_t modulePath[MAX_PATH];
         GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH);

         RegSetValueExW(hSubKey, nullptr, 0, REG_SZ, (const BYTE*)modulePath, (DWORD)(wcslen(modulePath) + 1) * sizeof(wchar_t));
         RegSetValueExW(hSubKey, L"ThreadingModel", 0, REG_SZ, (const BYTE*)L"Apartment", sizeof(L"Apartment"));
         RegCloseKey(hSubKey);
      }

      RegCloseKey(hKey);
      CoTaskMemFree(clsidString);
      return S_OK;
   }

   extern "C" __declspec(dllexport) HRESULT WINAPI DllUnregisterServer(){
      LPOLESTR clsidString = nullptr;
      HRESULT hr = StringFromCLSID(CLSID_CFG_DLG, &clsidString);
      if(FAILED(hr)) return hr;

      wchar_t keyPath[256];
      swprintf_s(keyPath, L"Software\\Classes\\CLSID\\%s", clsidString);

      // 再帰的にキーを削除
      LONG result = ::RegDeleteTreeW(HKEY_CURRENT_USER, keyPath);

      ::CoTaskMemFree(clsidString);
      return (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) ? S_OK : E_FAIL;
   }

   EXTERN_C __declspec(dllexport) HRESULT CoCreateInstance(
      REFCLSID  rclsid
      , LPUNKNOWN pUnkOuter
      , DWORD     dwClsContext
      , REFIID    riid
      , LPVOID* ppv
   ){
      if(!ppv) return E_POINTER;
      *ppv = nullptr;

      if(rclsid != CLSID_CFG_DLG) return CLASS_E_CLASSNOTAVAILABLE;
      if(pUnkOuter != NULL) return CLASS_E_NOAGGREGATION;

      // インプロシサーバー以外は拒否
      if((dwClsContext & CLSCTX_INPROC_SERVER) == 0){
         return CLASS_E_CLASSNOTAVAILABLE;
      }

      IClassFactory* pIClassFactory = NULL;
      HRESULT hr = ::DllGetClassObject(rclsid, IID_IClassFactory, (LPVOID*)&pIClassFactory);
      if(FAILED(hr)) return hr;

      hr = pIClassFactory->CreateInstance(pUnkOuter, riid, ppv);
      pIClassFactory->Release();
      return hr;
   }
}

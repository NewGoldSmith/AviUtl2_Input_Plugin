#include <Windows.h>
#include "../PreviewWindow/PreviewWindow.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int)
{
    // 1. テスト用のビットマップを作成（赤い矩形）
    const int width = 200;
    const int height = 150;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HDC hDC = CreateCompatibleDC(NULL);
    HBITMAP hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    DeleteDC(hDC);

    if (hBitmap && pBits) {
        // 赤で塗りつぶす
        DWORD* pixels = static_cast<DWORD*>(pBits);
        for (int i = 0; i < width * height; ++i)
            pixels[i] = 0x00'00'00'FF; // BGRA: 赤
    }

    // 2. PreviewWindow を生成して表示
    PreviewWindow preview(hInst);
    HWND hPreview = preview.CreatePrevWindow(NULL);
    if (hPreview) {
        preview.DrawBitmap(hBitmap);
    }

    // 3. メッセージループ
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // 4. 後始末
    DeleteObject(hBitmap);
    return 0;
}


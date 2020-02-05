#include "SplashScreenManager.h"

#define SSM SplashScreenManager::

IStream* SSM CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
    IStream* ipStream = NULL;

    HRSRC hrsrc = FindResource(NULL, lpName, lpType);
    if (hrsrc == NULL) {
        goto Return;
    }

    DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
    HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
    if (hglbImage == NULL) {
        goto Return;
    }

    LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == NULL) {
        goto Return;
    }

    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == NULL) {
        goto Return;
    }

    LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == NULL) {
        goto FreeData;
    }

    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
    GlobalUnlock(hgblResourceData);

    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream))) {
        goto Return;
    }
FreeData:
    GlobalFree(hgblResourceData);

Return:
    return ipStream;
}

IWICBitmapSource* SSM LoadBitmapFromStream(IStream* ipImageStream) {
    IWICBitmapSource* ipBitmap = NULL;
    IWICBitmapDecoder* ipDecoder = NULL;

    if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder)))) {
        goto Return;
    }

    if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad))) {
        goto ReleaseDecoder;
    }

    UINT nFrameCount = 0;

    if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1) {
        goto ReleaseDecoder;
    }
    IWICBitmapFrameDecode* ipFrame = NULL;

    if (FAILED(ipDecoder->GetFrame(0, &ipFrame))) {
        goto ReleaseDecoder;
    }

    WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
    ipFrame->Release();

ReleaseDecoder:
    ipDecoder->Release();

Return:
    return ipBitmap;
}

HBITMAP SSM CreateHBITMAP(IWICBitmapSource* ipBitmap) {
    HBITMAP hbmp = NULL;

    UINT width = 0;
    UINT height = 0;
    
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0) {
        goto Return;
    }

    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG)height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    void* pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);

    if (hbmp == NULL) {
        goto Return;
    }

    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE*>(pvImageBits)))) {
        DeleteObject(hbmp);
        hbmp = NULL;
    }

Return:
    return hbmp;
}

HBITMAP SSM LoadSplashImage()
{
    HBITMAP hbmpSplash = NULL;

    IStream* ipImageStream = SSM CreateStreamOnResource(MAKEINTRESOURCE(IDI_SPLASHIMAGE), _T("BMP"));
    if (ipImageStream == NULL) {
        goto Return;
    }


    IWICBitmapSource* ipBitmap = SSM LoadBitmapFromStream(ipImageStream);
    if (ipBitmap == NULL) {
        goto ReleaseStream;
    }

    // create a HBITMAP containing the image

    hbmpSplash = CreateHBITMAP(ipBitmap);
    ipBitmap->Release();

ReleaseStream:
    ipImageStream->Release();

Return:
    return hbmpSplash;
}

HWND SSM CreateSplashWindow(HINSTANCE hInstance, const wchar_t wcClassName[])
{
    HWND hwndOwner = CreateWindow(wcClassName, NULL, WS_POPUP,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    return CreateWindowEx(WS_EX_LAYERED, wcClassName, NULL, WS_POPUP | WS_VISIBLE,
        0, 0, 0, 0, hwndOwner, NULL, hInstance, NULL);
}

void SSM SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash) {
    BITMAP bm;
    GetObject(hbmpSplash, sizeof(bm), &bm);
    SIZE sizeSplash = { bm.bmWidth, bm.bmHeight };

    POINT ptZero = { 0 };
    HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorinfo = { 0 };
    monitorinfo.cbSize = sizeof(monitorinfo);
    GetMonitorInfo(hmonPrimary, &monitorinfo);

    const RECT& rcWork = monitorinfo.rcWork;
    POINT ptOrigin;
    ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - sizeSplash.cx) / 2;
    ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - sizeSplash.cy) / 2;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpSplash);

    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash, hdcMem, &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA);

    SelectObject(hdcMem, hbmpOld);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}
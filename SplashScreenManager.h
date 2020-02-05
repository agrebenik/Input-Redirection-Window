
#include <Windows.h>
#include <Wincodec.h>
#include "resource.h"
#ifndef UNICODE
#define UNICODE
#endif
#pragma once

class SplashScreenManager
{
	IDB_BITMAP1;
	HBITMAP CreateHBITMAP(IWICBitmapSource*);
	HBITMAP LoadSplashImage();
	IWICBitmapSource* LoadBitmapFromStream(IStream*);
	IStream* CreateStreamOnResource(LPCTSTR, LPCTSTR);
	HWND CreateSplashWindow(HINSTANCE, const wchar_t wcClassName[]);
	void SetSplashImage(HWND, HBITMAP);
};


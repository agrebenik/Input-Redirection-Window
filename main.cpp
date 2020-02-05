#ifndef UNICODE
#define UNICODE
#endif 

#include <stdio.h>
#include <cstdio>
#include <cwchar>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash);

const wchar_t g_CLASS_NAME[] = L"Sample Window Class";

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    
    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateSplashWindow(hInstance);
        
    /* CreateWindowEx(
        0, g_CLASS_NAME, L"Learn to Program Windows", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL
    );*/

    if (hwnd == NULL) {
        return 0;
    }



    ShowWindow(hwnd, nCmdShow);
    
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    wchar_t msg[32];
    switch (uMsg) {
        case WM_SYSKEYDOWN: {
            swprintf_s(msg, L"WM_SYSKEYDOWN: 0x%x\n", wParam);
            OutputDebugString(msg);
            break;
        }

        case WM_SYSCHAR: {
            swprintf_s(msg, L"WM_SYSCHAR: %c\n", (wchar_t)wParam);
            OutputDebugString(msg);
            break;
        }

        case WM_SYSKEYUP: {
            swprintf_s(msg, L"WM_SYSKEYUP: 0x%x\n", wParam);
            OutputDebugString(msg);
            break;
        }

        case WM_KEYDOWN: {
            swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam);
            OutputDebugString(msg);
            break;
        }

        case WM_KEYUP: {
            swprintf_s(msg, L"WM_KEYUP: 0x%x\n", wParam);
            OutputDebugString(msg);
            break;
        }

        case WM_CHAR: {
            swprintf_s(msg, L"WM_CHAR: %c\n", (wchar_t)wParam);
            OutputDebugString(msg);
            break;
        }
    
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
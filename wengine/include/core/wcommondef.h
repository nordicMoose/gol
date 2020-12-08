#ifndef WCOMMONDEF_H
#define WCOMMONDEF_H
#include <windows.h>

//define custom message to resize window
#define WM_W_RESIZE (WM_USER+0)
#define WM_W_RENDERMODE (WM_USER+1)
#define WM_W_FORCEPAINT (WM_USER+3)

#define W_WINDOWSTYLE (WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX)
#define W_WINDOWSTYLEX (WS_EX_APPWINDOW)

#endif
#pragma once

INT WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR cmdLine,INT cmdShow);
LRESULT WINAPI WinProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

BOOL WinCreate(HWND hWnd);
BOOL WinDestroy(HWND hWnd);
BOOL WinClose(HWND hWnd);
BOOL WinCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL WinKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL WinContextMenu(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL WinButtonDown(HWND hWnd,UINT button,LONG mx,LONG my);
BOOL WinSize(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL WinPaint(HWND hWnd);
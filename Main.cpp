#include "Globals.h"
#include "Main.h"
#include "Transform.h"

CImage gImage[2];
CImage gTransformedImage[2];

VOID FrequencyBandpassFilter(COMPLEX_NUMBER** data,long nx,long ny,long cutoffl,long cutoffh)
{
	for(long y = 0; y < ny/2; ++y)
	{
		for(long x = 0; x < nx/2; ++x)
		{
			if((x < cutoffl && y < cutoffl) || (x > cutoffh && y > cutoffh))
			{
				data[y][x].real = 0;
				data[y][x].imag = 0;
			}
		}

		for(long x = nx/2; x < nx; ++x)
		{
			if((x - nx/2 < cutoffl && y < cutoffl) || (x > cutoffh && y > cutoffh))
			{
				data[y][x].real = 0;
				data[y][x].imag = 0;
			}
		}
	}

	for(long y = ny/2; y < ny; ++y)
	{
		for(long x = 0; x < nx/2; ++x)
		{
			if((x < cutoffl && y - ny/2 < cutoffl) || (x > cutoffh && y - ny/2 > cutoffh))
			{
				data[y][x].real = 0;
				data[y][x].imag = 0;
			}
		}

		for(long x = nx/2; x < nx; ++x)
		{
			if((x - nx/2 < cutoffl && y - ny/2 < cutoffl) || (x > cutoffh && y - ny/2 > cutoffh))
			{
				data[y][x].real = 0;
				data[y][x].imag = 0;
			}
		}
	}
}

VOID AmplitudeBandpassFilter(COMPLEX_NUMBER** data,long nx,long ny,long cutoffl,long cutoffh)
{
	for(long y = 0; y < ny; ++y)
	{
		for(long x = 0; x < nx; ++x)
		{
			float pwr = sqrtf(data[y][x].real * data[y][x].real + data[y][x].imag * data[y][x].imag);

			if(pwr < cutoffl || pwr > cutoffh)
			{
				data[y][x].real = 0;
				data[y][x].imag = 0;
			}
		}
	}
}

BOOL TransformImage(const CImage& original,CImage& transformed,CImage& originalProcessed,CImage& transformedProcessed)
{
	COMPLEX_NUMBER** data = (COMPLEX_NUMBER**)malloc(sizeof(COMPLEX_NUMBER*)*original.GetHeight());

	for(long y = 0; y < original.GetHeight(); ++y)
	{
		data[y] = (COMPLEX_NUMBER*)malloc(sizeof(COMPLEX_NUMBER)*original.GetWidth());

		for(long x = 0; x < original.GetWidth(); ++x)
		{
			data[y][x].real = ((LONG)GetRValue(original.GetPixel(x,y)) + (LONG)GetGValue(original.GetPixel(x,y)) + (LONG)GetBValue(original.GetPixel(x,y))) / 3.0f;
			data[y][x].imag = 0;
		}
	}

	FFT2D(data,original.GetWidth(),original.GetHeight(),1);

	if(!transformed.Create(original.GetWidth(),original.GetHeight(),24))
	{
		for(long y = 0; y < original.GetHeight(); ++y)
			free(data[y]);

		free(data);

		return FALSE;
	}

	for(long y = 0; y < original.GetHeight(); ++y)
	{
		for(long x = 0; x < original.GetWidth(); ++x)
		{
			FLOAT power = sqrtf(data[y][x].real * data[y][x].real + data[y][x].imag * data[y][x].imag);

			transformed.SetPixel(x < original.GetWidth()/2 ? original.GetWidth()/2 + x : x - original.GetWidth()/2,y < original.GetHeight()/2 ? original.GetHeight()/2 + y : y - original.GetHeight()/2,RGB(power*128,power*128,power*128));
		}
	}

	///

	AmplitudeBandpassFilter(data,original.GetWidth(),original.GetHeight(),0/128.0f,150/128.0f);
	//FrequencyBandpassFilter(data,original.GetWidth(),original.GetHeight(),50,100);

	///

	if(!transformedProcessed.Create(original.GetWidth(),original.GetHeight(),24))
	{
		for(long y = 0; y < original.GetHeight(); ++y)
			free(data[y]);

		free(data);

		return FALSE;
	}

	for(long y = 0; y < original.GetHeight(); ++y)
	{
		for(long x = 0; x < original.GetWidth(); ++x)
		{
			FLOAT power = sqrtf(data[y][x].real * data[y][x].real + data[y][x].imag * data[y][x].imag);

			transformedProcessed.SetPixel(x < original.GetWidth()/2 ? original.GetWidth()/2 + x : x - original.GetWidth()/2,y < original.GetHeight()/2 ? original.GetHeight()/2 + y : y - original.GetHeight()/2,RGB(power*128,power*128,power*128));
		}
	}

	FFT2D(data,original.GetWidth(),original.GetHeight(),-1);

	if(!originalProcessed.Create(original.GetWidth(),original.GetHeight(),24))
	{
		for(long y = 0; y < original.GetHeight(); ++y)
			free(data[y]);

		free(data);

		return FALSE;
	}

	for(long y = 0; y < original.GetHeight(); ++y)
	{
		for(long x = 0; x < original.GetWidth(); ++x)
		{
			FLOAT power = sqrt(data[y][x].real * data[y][x].real + data[y][x].imag * data[y][x].imag);

			originalProcessed.SetPixel(x,y,RGB(power,power,power));
		}
	}

	for(long y = 0; y < original.GetHeight(); ++y)
		free(data[y]);

	free(data);

	return TRUE;
}

INT WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR cmdLine,INT cmdShow)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex,sizeof(wcex));
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_CLASSDC|CS_DBLCLKS;
	wcex.lpfnWndProc	= WinProcedure;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wcex.lpszClassName	= TEXT("Transform");

	if(!RegisterClassEx(&wcex))
		return 1;

	HWND hWnd = CreateWindowEx(NULL,wcex.lpszClassName,TEXT("Transform"),WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,550,300,NULL,NULL,GetModuleHandle(NULL),NULL);
	if(!hWnd)
		return 1;

	MSG msg;
	ZeroMemory(&msg,sizeof(msg));

	while(GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hWnd);
	UnregisterClass(wcex.lpszClassName,GetModuleHandle(NULL));

	return (INT)msg.wParam;
}

LRESULT WINAPI WinProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
			return WinCreate(hWnd) ? 0 : -1;

		case WM_DESTROY:
			return WinDestroy(hWnd) ? 0 : -1;

		case WM_CLOSE:
			return WinClose(hWnd) ? 0 : -1;

		case WM_COMMAND:
			return WinCommand(hWnd,wParam,lParam) ? 0 : -1;

		case WM_CONTEXTMENU:
			return WinContextMenu(hWnd,wParam,lParam) ? 0 : -1;

		case WM_LBUTTONDOWN:
			return WinButtonDown(hWnd,msg,LOWORD(lParam),HIWORD(lParam)) ? 0 : -1;

		case WM_KEYDOWN:
			return WinKeyDown(hWnd,wParam,lParam) ? 0 : -1;

		case WM_SIZE:
			return WinSize(hWnd,msg,wParam,lParam) ? 0 : -1;

		case WM_PAINT:
			return WinPaint(hWnd) ? 0 : -1;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}
BOOL WinCreate(HWND hWnd)
{
	if(FAILED(gImage[0].Load(TEXT("Sample.jpg"))))
	{
		MessageBox(hWnd,TEXT("Failed to load image Sample.jpg"),TEXT("Error"),MB_OK|MB_ICONEXCLAMATION);

		return FALSE;
	}

	TransformImage(gImage[0],gTransformedImage[0],gImage[1],gTransformedImage[1]);

	RECT rect = {0,0,gImage[0].GetWidth()*4 + 5 + 5 + 10 + 10 + 10,gImage[0].GetHeight() + 10};
	AdjustWindowRect(&rect,GetWindowLong(hWnd,GWL_STYLE),FALSE);
	SetWindowPos(hWnd,NULL,NULL,NULL,rect.right - rect.left,rect.bottom - rect.top,SWP_NOMOVE);

	return TRUE;
}

BOOL WinDestroy(HWND hWnd)
{
	return TRUE;
}

BOOL WinClose(HWND hWnd)
{
	PostQuitMessage(NULL);

	return TRUE;
}

BOOL WinCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

BOOL WinContextMenu(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

BOOL WinButtonDown(HWND hWnd,UINT button,LONG mx,LONG my)
{
	return TRUE;
}

BOOL WinSize(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	InvalidateRect(hWnd,NULL,FALSE);

	return TRUE;
}

BOOL WinKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}

	return TRUE;
}

BOOL WinPaint(HWND hWnd)
{
	RECT client;
	GetClientRect(hWnd,&client);

	PAINTSTRUCT paint;
	HDC hDC = BeginPaint(hWnd,&paint);

	SetBkMode(hDC,TRANSPARENT);
	DeleteObject(SelectObject(hDC,GetStockObject(DEFAULT_GUI_FONT)));

	HBRUSH brush = CreateSolidBrush(RGB(255,255,255));
	FillRect(hDC,&client,brush);
	DeleteObject(brush);

	if(!gImage[0].IsNull() && !gTransformedImage[0].IsNull() && !gImage[1].IsNull() && !gTransformedImage[1].IsNull())
	{
		gImage[0].BitBlt(hDC,5,5);
		gTransformedImage[0].BitBlt(hDC,5 + gImage[0].GetWidth() + 10,5);
		gTransformedImage[1].BitBlt(hDC,5 + gImage[0].GetWidth() + 10 + gTransformedImage[0].GetWidth() + 10,5);
		gImage[1].BitBlt(hDC,5 + gImage[0].GetWidth() + 10 + gTransformedImage[0].GetWidth() + 10 + gTransformedImage[1].GetWidth() + 10,5);
	}

	EndPaint(hWnd,&paint);

	return TRUE;
}
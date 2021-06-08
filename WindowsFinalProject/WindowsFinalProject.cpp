#include <windows.h>
#include "WindowsFinalProject.h"
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD	tPre, tNow;
HDC mainDC,bufDC,bgDC,uiDC,tmpDC;
// mainDC	最终绘图DC
// bufDC	mainDC的缓冲DC
// bgDC		保存背景图的DC
// uiDC		保存UI图片的DC
// tmpDC	首次导入图片使用的临时DC

void DrawFrame();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static LPCWSTR szAppName = L"HelloWin";
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, L"This program requires Windows NT!",
			szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName,
		L"The Windows Program",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,//锁定窗口大小
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		512,//宽
		512,//高
		NULL,
		NULL,
		hInstance,
		NULL);
	RECT windowRect,clientRect;
	GetWindowRect(hwnd, &windowRect);
	GetClientRect(hwnd, &clientRect);
	MoveWindow(hwnd, 100, 100, 512 + (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left), 512 + (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top), FALSE);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	GetClientRect(hwnd, &clientRect);
	//全局变量初始化
	mainDC = GetDC(hwnd);
	HBITMAP tmpBitmap = CreateCompatibleBitmap(mainDC, 512, 512);
	bufDC = CreateCompatibleDC(mainDC);
	SelectObject(bufDC,tmpBitmap);
	DeleteObject(tmpBitmap);
	tmpBitmap = CreateCompatibleBitmap(mainDC, 512, 768);
	bgDC = CreateCompatibleDC(mainDC);
	SelectObject(bgDC, tmpBitmap);
	DeleteObject(tmpBitmap);
	uiDC = CreateCompatibleDC(mainDC);
	tmpDC = CreateCompatibleDC(mainDC);
	//绘制背景bgDC
	HBITMAP bgHBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	SelectObject(tmpDC, bgHBitmap);
	BitBlt(bgDC, 0, 0, 512, 512, tmpDC, 0, 0, SRCCOPY);
	BitBlt(bgDC, 256, 0, 256, 256, tmpDC, 0, 0, SRCCOPY);
	BitBlt(bgDC, 0, 256, 256, 256, tmpDC, 0, 0, SRCCOPY);
	BitBlt(bgDC, 256, 256, 256, 256, tmpDC, 0, 0, SRCCOPY);
	BitBlt(bgDC, 0, 512, 256, 256, tmpDC, 0, 0, SRCCOPY);
	BitBlt(bgDC, 256, 512, 256, 256, tmpDC, 0, 0, SRCCOPY);
	DeleteObject(bgHBitmap);
	//背景bgDC绘制结束
	//全局变量初始化结束
	GetMessage(&msg, NULL, 0, 0);
	while ( msg.message!= WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		tNow = GetTickCount();
		if (tNow - tPre >= 40)
			DrawFrame();
	}
	return msg.wParam;
}

void DrawFrame() {
	BitBlt(mainDC, 0, 0, 512, 512, bgDC, 0, 0, SRCCOPY);//test,背景绘制
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	/*case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetWindowRect(hWnd, &rect);

		EndPaint(hWnd, &ps);
	}
	break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}









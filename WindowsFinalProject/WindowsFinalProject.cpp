#include "framework.h"
#include "WindowsFinalProject.h"
#include "MyHelperFunc.h"

using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

ULONG_PTR m_gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

DWORD tPre, tNow;
HDC mainDC, bufDC, bgDC, tmpDC;
// mainDC	最终绘图DC
// bufDC	mainDC的缓冲DC
// bgDC		保存背景图的DC
// tmpDC	首次导入图片使用的临时DC

void DrawFrame(HINSTANCE);

//兼容MFC，使用wWinMain
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    szCmdLine,
    _In_ int       iCmdShow)
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
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, //锁定窗口大小
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        512, //宽
        512, //高
        NULL,
        NULL,
        hInstance,
        NULL);
    RECT windowRect, clientRect;
    GetWindowRect(hwnd, &windowRect);
    GetClientRect(hwnd, &clientRect);
    MoveWindow(hwnd, 100, 100, 512 + (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left), 512 + (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top), FALSE);
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);
    GetClientRect(hwnd, &clientRect);
    //全局变量初始化
    //GDI+初始化
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    //GDI+初始化结束
    //DC初始化
    mainDC = GetDC(hwnd);
    HBITMAP tmpBitmap = CreateCompatibleBitmap(mainDC, 512, 512);
    bufDC = CreateCompatibleDC(mainDC);
    SelectObject(bufDC, tmpBitmap);
    DeleteObject(tmpBitmap);
    tmpBitmap = CreateCompatibleBitmap(mainDC, 512, 768);
    bgDC = CreateCompatibleDC(mainDC);
    SelectObject(bgDC, tmpBitmap);
    DeleteObject(tmpBitmap);
    tmpDC = CreateCompatibleDC(mainDC);
    //DC初始化结束
    //绘制背景bgDC
    HBITMAP bgHBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
    SelectObject(tmpDC, bgHBitmap);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            StretchBlt(bgDC, i * 128, j * 128, 128, 128, tmpDC, 0, 0, 256, 256, SRCCOPY);
        }
    }
    DeleteObject(bgHBitmap);
    //背景bgDC绘制结束
    //全局变量初始化结束
    GetMessage(&msg, NULL, 0, 0);
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        tNow = GetTickCount();
        if (tNow - tPre >= 40)
        {
            tPre = tNow;
            DrawFrame(hInstance);
        }
    }
    GdiplusShutdown(m_gdiplusToken);//关闭GDI+
    //关闭所有DC
    DeleteDC(bufDC);
    DeleteDC(bgDC);
    DeleteDC(tmpDC);
    return msg.wParam;
}

void DrawFrame(HINSTANCE hInstance)
{
    static int speed = 1;
    static int bgPos = 0;
    bgPos += speed;
    bgPos %= 256;
    BitBlt(bufDC, 0, 0, 512, 512, bgDC, 0, bgPos, SRCCOPY); //绘制背景
    Graphics bufMem(bufDC);// bufDC的Graphics
    Image* pimage; // Construct an image
    ImageFromIDResource(hInstance, IDB_PNG1, _T("PNG"), pimage);
    bufMem.DrawImage(pimage, 0, 0, pimage->GetWidth()/2, pimage->GetHeight()/2);
    BitBlt(mainDC, 0, 0, 512, 512, bufDC, 0, 0, SRCCOPY); //最终绘制
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

#include "framework.h"
#include "MyHelperFunc.h"

using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

ULONG_PTR m_gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

ULONGLONG tPre, tNow;
HDC mainDC, bufDC, bgDC, tmpDC;
// mainDC	最终绘图DC
// bufDC	mainDC的缓冲DC
// bgDC		保存背景图的DC
// tmpDC	首次导入图片使用的临时DC
Graphics* bufMem;
int status =3;
double intenalSpeed = BASESPEED;

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
    SetBkMode(bufDC, TRANSPARENT);//设置文字背景透明
    SelectObject(bufDC, tmpBitmap);
    DeleteObject(tmpBitmap);
    tmpBitmap = CreateCompatibleBitmap(mainDC, 768, 640);
    bgDC = CreateCompatibleDC(mainDC);
    SelectObject(bgDC, tmpBitmap);
    DeleteObject(tmpBitmap);
    tmpDC = CreateCompatibleDC(mainDC);
    //DC初始化结束
    bufMem = new Graphics(bufDC); //GDI+ Graphics init
    //绘制背景bgDC
    HBITMAP bgHBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
    SelectObject(tmpDC, bgHBitmap);
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 5; j++)
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
        tNow = GetTickCount64();
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
    static long frameCount = 0;
    frameCount++;
    int horizontalSpeed =0;
    int verticalSpeed =0;
    static int bgHorizontalPos = 128;
    static int bgPos = 0;
    static int haveTraveled = 0;
    switch (status)// calc horizontalSpeed and verticalSpeed
    {
    case 6:
    case 0: {
        horizontalSpeed = 0;
        verticalSpeed = 0;
    }break;
    case 1: { //左偏45度
        horizontalSpeed = (int)(-sin(getRadian(45)) * intenalSpeed - 0.5);
        verticalSpeed = (int)(cos(getRadian(45)) * intenalSpeed+0.5);
    }break;
    case 2: { //左偏30度
        horizontalSpeed = (int)(-sin(getRadian(30)) * intenalSpeed - 0.5);
        verticalSpeed = (int)(cos(getRadian(30)) * intenalSpeed+0.5);
    }break;
    case 9:
    case 3: { //直行
        horizontalSpeed = 0;
        verticalSpeed = (int)(intenalSpeed+0.5);
    }break;

    case 4: { //右偏30度
        horizontalSpeed = (int)(sin(getRadian(30)) * intenalSpeed+0.5);
        verticalSpeed = (int)(cos(getRadian(30)) * intenalSpeed+0.5);
    }break;
    case 5: { //右偏45度
        horizontalSpeed = (int)(sin(getRadian(45)) * intenalSpeed+0.5);
        verticalSpeed = (int)(cos(getRadian(45)) * intenalSpeed+0.5);
    }break;
    default:
        break;
    }
    haveTraveled += verticalSpeed;
    bgPos += verticalSpeed;
    bgHorizontalPos += horizontalSpeed;
    // 背景溢出补偿
    bgPos %= 128;
    if (bgHorizontalPos < 0) {
        bgHorizontalPos += 128;
    }
    else if (bgHorizontalPos>256) {
        bgHorizontalPos -= 128;
    }
    // 背景溢出补偿结束
    BitBlt(bufDC, 0, 0, 512, 512, bgDC, bgHorizontalPos, bgPos, SRCCOPY); //绘制背景

    //最后绘制角色
    DrawSurfboard(hInstance, bufMem, status, frameCount%3);
    DrawPlayer(hInstance, bufMem, status);
    // draw score
    WCHAR scoreBuffer[256];
    swprintf(scoreBuffer, sizeof(scoreBuffer) / sizeof(*scoreBuffer), L"%d米", haveTraveled/ BASESPEED);
    RECT scoreRect{0,0,512,40};
    DrawText(bufDC, scoreBuffer, lstrlenW(scoreBuffer), &scoreRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    BitBlt(mainDC, 0, 0, 512, 512, bufDC, 0, 0, SRCCOPY); //最终显示到屏幕上
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
    case WM_KEYDOWN:
    {
        switch (wParam) {
        case VK_UP: {
            intenalSpeed = 0;
            status = 0;
        }break;
        case VK_LEFT: {
            if (status==0 || status == 1) {
                //啥都不做
            }
            else if (status!=2) {
                status = 2;
            }
            else {
                status = 1;
            }
        }break;

        case VK_RIGHT: {
            if (status == 0 || status == 5) {
                //啥都不做
            }
            else if (status != 4) {
                status = 4;
            }
            else {
                status = 5;
            }
        }break;
        case VK_DOWN: {
            status = 3;
            if ((int)intenalSpeed == 0) {
                intenalSpeed = BASESPEED;
            }
        }break;
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

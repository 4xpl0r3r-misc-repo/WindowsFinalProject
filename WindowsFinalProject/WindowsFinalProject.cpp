#include "framework.h"
#include "DrawingFunc.h"
#include "LogicFunc.h"

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
// Global Variables
int status =3;
double internalSpeed = 0;
boolean started = false;
ULONGLONG frameCount = 0;
int bgHorizontalPos = 128;
int bgPos = 0;
int haveTraveled = 0;
vector<physicalObj> obstacles;
int horizontalSpeed = 0;
int verticalSpeed = 0;
bool replayFlag;
int xMoved = 0;
vector<operateRecord> records;

void DrawFrame(HINSTANCE);


void moveLogic() {
    if (internalSpeed < 32)internalSpeed += 1.0 / 25;//每帧速度+1/25
    switch (status)// calc horizontalSpeed and verticalSpeed
    {
    case 6:
    case 0: {
        horizontalSpeed = 0;
        verticalSpeed = 0;
    }break;
    case 1: { //左偏45度
        horizontalSpeed = (int)(-sin(getRadian(45)) * internalSpeed - 0.5);
        verticalSpeed = (int)(cos(getRadian(45)) * internalSpeed + 0.5);
    }break;
    case 2: { //左偏30度
        horizontalSpeed = (int)(-sin(getRadian(30)) * internalSpeed - 0.5);
        verticalSpeed = (int)(cos(getRadian(30)) * internalSpeed + 0.5);
    }break;
    case 9:
    case 3: { //直行
        horizontalSpeed = 0;
        verticalSpeed = (int)(internalSpeed + 0.5);
    }break;

    case 4: { //右偏30度
        horizontalSpeed = (int)(sin(getRadian(30)) * internalSpeed + 0.5);
        verticalSpeed = (int)(cos(getRadian(30)) * internalSpeed + 0.5);
    }break;
    case 5: { //右偏45度
        horizontalSpeed = (int)(sin(getRadian(45)) * internalSpeed + 0.5);
        verticalSpeed = (int)(cos(getRadian(45)) * internalSpeed + 0.5);
    }break;
    default:
        break;
    }
    // calc speed for horizon and vertical finished
    haveTraveled += verticalSpeed;
    xMoved += horizontalSpeed;
    bgPos += verticalSpeed;
    bgHorizontalPos += horizontalSpeed;
    for (auto it = obstacles.begin(); it != obstacles.end(); it++)
    {
        it->y -= verticalSpeed;
        it->x -= horizontalSpeed;
    }
    // 背景溢出补偿
    bgPos %= 128;
    if (bgHorizontalPos < 0) {
        bgHorizontalPos += 128;
    }
    else if (bgHorizontalPos > 256) {
        bgHorizontalPos -= 128;
    }
    // 背景溢出补偿结束
}

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
    wndclass.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSFINALPROJECT);
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, L"This program requires Windows NT!",
            szAppName, MB_ICONERROR);
        return 0;
    }
    hwnd = CreateWindow(szAppName,
        L"冲浪",
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
    srand((unsigned)time(NULL));
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
    obstacles = vector<physicalObj>(); // vector init
    generateObstacles(obstacles, bgHorizontalPos -128 + 512 / 2, 512 / 2 + 512 / 4);//绘制初始地图
    static long nextGeneratePos = 512 / 2 + 512 / 4;
    records = vector<operateRecord>();
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
            frameCount++;
            // Logic Here
            if (started) {
                if (haveTraveled >= nextGeneratePos) {
                    nextGeneratePos += 512;
                    static bool displayFlag;
                    static int count;
                    if (!displayFlag) {
                        displayFlag = true;
                    }
                    else {
                        for (int i = count-8; i < count; i++)
                        {
                            obstacles[i].displayFlag = true;
                        }
                    }
                    for (int i = 0; i < 8; i++)
                    {
                        obstacles[count++].invalidFlag = true;
                    }
                    generateObstacles(obstacles, bgHorizontalPos -128 +512/2, 512 / 2 + 512 / 4);
                }
                moveLogic();
                //检查碰撞
                physicalObj player = { 256, 128 ,16};
                for (auto it = obstacles.begin(); it != obstacles.end(); it++)
                {
                    if (!it->displayFlag && checkCollision(*it, player)) { // 正方形空间
                        status = 6;
                        static bool onceFlag;
                        if (!onceFlag) {
                            onceFlag = true;
                            records.push_back(operateRecord{ frameCount,6 });
                        }
                        break;
                    }
                }
            }
            else if (replayFlag) {
                static bool resetOnceFlag;
                static int haveTraveledConst = haveTraveled;
                if (!resetOnceFlag) {
                    resetOnceFlag = true;
                    // 还原障碍物们
                    for (auto it = obstacles.begin(); it != obstacles.end(); it++)
                    {
                        it->y += haveTraveled;
                        it->x += xMoved;
                        //默认全部关闭
                        it->displayFlag = true;
                        it->invalidFlag = true;
                    }
                    haveTraveled = 0;
                    for (int i = 0; i < 8; i++)
                    {
                        obstacles[i].displayFlag = false;
                        obstacles[i].invalidFlag = false;
                    }
                    status = 3;
                    internalSpeed = BASESPEED;
                }
                //画出需要展示的障碍物即可，无需检查碰撞，使用record即可
                int enabled = haveTraveled / 512;
                for (int i = 0; i < 8; i++)
                {
                    obstacles[(enabled + 1) * 8 + i].displayFlag = false;
                    if (enabled - 2 >= 0) {
                        obstacles[(enabled - 2) * 8 + i].displayFlag = true;
                    }
                }
                moveLogic();
                static int index = 0;
                if (records[index].frameId==frameCount) {
                    status = records[index].status;
                    index++;
                    switch (status)
                    {
                    case 0:
                        internalSpeed = 0;
                        break;
                    case 3:
                        if ((int)internalSpeed == 0) {
                            internalSpeed = BASESPEED;
                        }
                        break;
                    default:
                        break;
                    }
                    if (status == 6) {
                        replayFlag = 0;
                        haveTraveled = haveTraveledConst;
                    }
                }
            }
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
    BitBlt(bufDC, 0, 0, 512, 512, bgDC, bgHorizontalPos, bgPos, SRCCOPY); //绘制背景
    DrawObstacles(hInstance, bufMem, obstacles);//绘制障碍物
    //最后绘制角色
    DrawSurfboard(hInstance, bufMem, status, frameCount%3);
    DrawPlayer(hInstance, bufMem, status);
    // draw score
    WCHAR scoreBuffer[256];
    swprintf(scoreBuffer, sizeof(scoreBuffer) / sizeof(*scoreBuffer), L"%d米", haveTraveled/ BASESPEED);
    RECT scoreRect{0,0,512,40};
    HFONT hF = (HFONT)GetStockObject(SYSTEM_FONT);
    SelectObject(bufDC, hF);
    DrawText(bufDC, scoreBuffer, lstrlenW(scoreBuffer), &scoreRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    DeleteObject(hF);
    if (status ==6 && !replayFlag) {
        scoreRect = {0,256-40,512,256+40};
        HFONT endFont = CreateFont(40, 20, 0, 0, FW_HEAVY, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"等线");
        SelectObject(bufDC, endFont);
        DrawText(bufDC, L"游戏结束", lstrlenW(L"游戏结束"), &scoreRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        DeleteObject(endFont);
    }
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
        case ID_PLAT: {
            frameCount = 0;
            started = true;
            internalSpeed = BASESPEED;
        }break;
        case ID_REPLAY: {
            if (status!=6) {
                MessageBox(hWnd, L"尚未进行过游戏，请先玩一局游戏再看回放",L"提示", MB_OK);
            }
            started = 0;//防止进入游戏逻辑
            replayFlag = true;
            frameCount = 0;
        }break;
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
        if (!started||status ==6)break;
        switch (wParam) {
        case VK_UP: {
            internalSpeed = 0;
            status = 0;
            records.push_back(operateRecord{ frameCount,0 });
        }break;
        case VK_LEFT: {
            if (status==0 || status == 1) {
                //啥都不做
            }
            else if (status!=2) {
                status = 2;
                records.push_back(operateRecord{ frameCount,2 });
            }
            else {
                status = 1;
                records.push_back(operateRecord{ frameCount,1 });
            }
        }break;

        case VK_RIGHT: {
            if (status == 0 || status == 5) {
                //啥都不做
            }
            else if (status != 4) {
                status = 4;
                records.push_back(operateRecord{ frameCount,4 });
            }
            else {
                status = 5;
                records.push_back(operateRecord{ frameCount,5 });
            }
        }break;
        case VK_DOWN: {
            status = 3;
            records.push_back(operateRecord{ frameCount,3 });
            if ((int)internalSpeed == 0) {
                internalSpeed = BASESPEED;
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

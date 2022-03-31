#pragma comment (lib, "Msimg32.lib")

#include "LabWinApi.h"
#include "resource.h"

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterSquareClass(HINSTANCE hInstance);
ATOM                MyRegisterLayerClass(HINSTANCE hInstance);

BOOL                InitInstance(HINSTANCE, int, int);
BOOL                InitInstanceSquare(HINSTANCE, int, HWND, int);
BOOL                InitInstanceLayer(HINSTANCE, int, HWND, int);

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    SquareProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    LayerProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LABWINAPI, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_SQUARE_TITLE, szSquareTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SQUARE, szSquareClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_LAYER_TITLE, szLayerTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAYER, szLayerClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);
    MyRegisterSquareClass(hInstance);
    MyRegisterLayerClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow, PRIMARY_WND) 
        || !InitInstanceSquare(hInst, nCmdShow, g_hMainWindows[PRIMARY_WND], PRIMARY_WND)
        || !InitInstanceLayer(hInst, nCmdShow, g_hMainWindows[PRIMARY_WND], PRIMARY_WND))
    {
        return FALSE;
    }

    if (!InitInstance(hInstance, nCmdShow, SECONDARY_WND) 
        || !InitInstanceSquare(hInst, nCmdShow, g_hMainWindows[SECONDARY_WND], SECONDARY_WND)
        || !InitInstanceLayer(hInst, nCmdShow, g_hMainWindows[SECONDARY_WND], SECONDARY_WND))
    {
        return FALSE;
    }

    InitBoard();
    UpdateAllSquares();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

// Registers the window class.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2048));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_LABWINAPI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2048));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterSquareClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = SquareProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2048));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szSquareClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2048));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterLayerClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = LayerProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2048));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szLayerClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2048));

    return RegisterClassExW(&wcex);
}


// Saves instance handle and creates main window
// It is assumed that PRIMARY_WND is created ALWAYS before the SECONDARY_WND
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, int wndId)
{
   hInst = hInstance; // Store instance handle in our global variable

   int wndWidth, wndHeight;
   GetMainWndSize(&wndWidth, &wndHeight);
   g_hMainWindows[wndId] = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
      CW_USEDEFAULT, 0, wndWidth, wndHeight, nullptr, nullptr, hInstance, nullptr);
   if (wndId == SECONDARY_WND)
   {
       SetWindowLongPtrW(g_hMainWindows[wndId], GWL_EXSTYLE, GetWindowLongW(g_hMainWindows[wndId], GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
       POINT screenCenter{ GetSystemMetrics(SM_CXFULLSCREEN) / 2 , GetSystemMetrics(SM_CYFULLSCREEN) / 2 };
       ReflectWindows(screenCenter, g_hMainWindows[PRIMARY_WND], g_hMainWindows[SECONDARY_WND]);
   }
       
   if (!g_hMainWindows[wndId])
   {
      return FALSE;
   }

   ShowWindow(g_hMainWindows[wndId], nCmdShow);
   UpdateWindow(g_hMainWindows[wndId]);

   return TRUE;
}

BOOL InitInstanceSquare(HINSTANCE hInstance, int nCmdShow, HWND hWnd, int mainWndId)
{
    static int chldId = 0;

    // Creating windows
    g_hScore[mainWndId] = CreateWindowW(szSquareClass, szSquareTitle, WS_CHILD,
        g_gapSize, g_gapSize, 
        g_squareSdLen * 4 + g_gapSize * 3, g_squareSdLen,
        g_hMainWindows[mainWndId], (HMENU)chldId, hInst, nullptr);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            g_hSquares[mainWndId][j][i] = CreateWindowW(szSquareClass, szSquareTitle, WS_CHILD,
                (g_squareSdLen + g_gapSize) * i + g_gapSize, 
                (g_squareSdLen + g_gapSize) * j + 2 * g_gapSize + g_squareSdLen, 
                g_squareSdLen, g_squareSdLen, g_hMainWindows[mainWndId], 
                (HMENU)chldId, hInst, nullptr);
            chldId++;
        }
    }

    // Displaying windows
    ShowWindow(g_hScore[mainWndId], nCmdShow);
    UpdateWindow(g_hScore[mainWndId]);


    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            ShowWindow(g_hSquares[mainWndId][i][j], nCmdShow);
            UpdateWindow(g_hSquares[mainWndId][i][j]);
        }
    }
    
    return TRUE;
}

BOOL InitInstanceLayer(HINSTANCE hInstance, int nCmdShow, HWND hWnd, int mainWndId)
{
    static int chldId = 20;

    RECT rc;
    GetClientRect(g_hMainWindows[mainWndId], &rc);
    int wndWidth = rc.right - rc.left;
    int wndHeight = rc.bottom - rc.top;
    g_hLayers[mainWndId] = CreateWindowW(szLayerClass, szLayerTitle, WS_CHILD,
        0, 0, wndWidth, wndHeight, g_hMainWindows[mainWndId], (HMENU)chldId, hInst, nullptr);
    //assert(g_hLayers[mainWndId]);
    chldId++;
    if (!g_hLayers[mainWndId])
    {
        return FALSE;
    }

    
    ShowWindow(g_hLayers[mainWndId], SW_SHOW);
    
    //SetWindowPos(g_hLayers[mainWndId], HWND_BOTTOM, 0, 0, wndWidth, wndHeight, SWP_SHOWWINDOW);
    //UpdateWindow(g_hLayers[mainWndId]);
    //UpdateAllSquares(); // TODO

    return TRUE;
}

//  PURPOSE: Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            HMENU hMenu = GetMenu(hWnd);
            MENUITEMINFO menuItems[4]{};

            for (int i = 0; i < 4; i++)
            {
                menuItems[i].cbSize = sizeof(MENUITEMINFO);
                menuItems[i].fMask = MIIM_STATE;
            }
            GetMenuItemInfo(hMenu, ID_GOAL_8, FALSE, &menuItems[0]);
            GetMenuItemInfo(hMenu, ID_GOAL_16, FALSE, &menuItems[1]);
            GetMenuItemInfo(hMenu, ID_GOAL_64, FALSE, &menuItems[2]);
            GetMenuItemInfo(hMenu, ID_GOAL_2048, FALSE, &menuItems[3]);
            switch (wmId)
            {
            case IDM_LEFT:
                UpdateBoard(Direction::left);
                break;
            case IDM_RIGHT:
                UpdateBoard(Direction::right);
                break;
            case IDM_UP:
                UpdateBoard(Direction::up);
                break;
            case IDM_DOWN:
                UpdateBoard(Direction::down);
                break;
            // Player chooses the goal
            

            case ID_GOAL_8:
            {
                g_goal = 8;

                if (menuItems[0].fState == MFS_CHECKED) 
                {
                    menuItems[0].fState = MFS_UNCHECKED;
                    menuItems[1].fState = MFS_CHECKED;
                    menuItems[2].fState = MFS_CHECKED;
                    menuItems[3].fState = MFS_CHECKED;
                }
                else 
                {
                    menuItems[0].fState = MFS_CHECKED;
                    menuItems[1].fState = MFS_UNCHECKED;
                    menuItems[2].fState = MFS_UNCHECKED;
                    menuItems[3].fState = MFS_UNCHECKED;
                }
                SetMenuItemInfo(hMenu, ID_GOAL_8, FALSE, &menuItems[0]);
                SetMenuItemInfo(hMenu, ID_GOAL_16, FALSE, &menuItems[1]);
                SetMenuItemInfo(hMenu, ID_GOAL_64, FALSE, &menuItems[2]);
                SetMenuItemInfo(hMenu, ID_GOAL_2048, FALSE, &menuItems[3]);
            }
                break;
            case ID_GOAL_16:
            {
                g_goal = 16;

                if (menuItems[1].fState == MFS_CHECKED)
                {
                    menuItems[0].fState = MFS_CHECKED;
                    menuItems[1].fState = MFS_UNCHECKED;
                    menuItems[2].fState = MFS_CHECKED;
                    menuItems[3].fState = MFS_CHECKED;
                    
                }
                else
                {
                    menuItems[0].fState = MFS_UNCHECKED;
                    menuItems[1].fState = MFS_CHECKED;
                    menuItems[2].fState = MFS_UNCHECKED;
                    menuItems[3].fState = MFS_UNCHECKED;
                }
                SetMenuItemInfo(hMenu, ID_GOAL_8, FALSE, &menuItems[0]);
                SetMenuItemInfo(hMenu, ID_GOAL_16, FALSE, &menuItems[1]);
                SetMenuItemInfo(hMenu, ID_GOAL_64, FALSE, &menuItems[2]);
                SetMenuItemInfo(hMenu, ID_GOAL_2048, FALSE, &menuItems[3]);
            }
                break; 
            case ID_GOAL_64:
            {
                g_goal = 64;

                if (menuItems[2].fState == MFS_CHECKED)
                {
                    menuItems[0].fState = MFS_CHECKED;
                    menuItems[1].fState = MFS_CHECKED;
                    menuItems[2].fState = MFS_UNCHECKED;
                    menuItems[3].fState = MFS_CHECKED;
                }
                else
                {
                    menuItems[0].fState = MFS_UNCHECKED;
                    menuItems[1].fState = MFS_UNCHECKED;
                    menuItems[2].fState = MFS_CHECKED;
                    menuItems[3].fState = MFS_UNCHECKED;
                }
                SetMenuItemInfo(hMenu, ID_GOAL_8, FALSE, &menuItems[0]);
                SetMenuItemInfo(hMenu, ID_GOAL_16, FALSE, &menuItems[1]);
                SetMenuItemInfo(hMenu, ID_GOAL_64, FALSE, &menuItems[2]);
                SetMenuItemInfo(hMenu, ID_GOAL_2048, FALSE, &menuItems[3]);
            }
                
                break;
            case ID_GOAL_2048:
            {
                g_goal = 2048;

                if (menuItems[3].fState == MFS_CHECKED)
                {
                    menuItems[0].fState = MFS_CHECKED;
                    menuItems[1].fState = MFS_CHECKED;
                    menuItems[2].fState = MFS_CHECKED;
                    menuItems[3].fState = MFS_UNCHECKED;
                }
                else
                {
                    menuItems[0].fState = MFS_UNCHECKED;
                    menuItems[1].fState = MFS_UNCHECKED;
                    menuItems[2].fState = MFS_UNCHECKED;
                    menuItems[3].fState = MFS_CHECKED;
                }
                SetMenuItemInfo(hMenu, ID_GOAL_8, FALSE, &menuItems[0]);
                SetMenuItemInfo(hMenu, ID_GOAL_16, FALSE, &menuItems[1]);
                SetMenuItemInfo(hMenu, ID_GOAL_64, FALSE, &menuItems[2]);
                SetMenuItemInfo(hMenu, ID_GOAL_2048, FALSE, &menuItems[3]);
            }
                break;
            case ID_NEW_GAME:
                ResetBoard();
                InitBoard();
                g_scoreVal = 0;
                g_status = GameStatus::inProgress;
                UpdateAllSquares();
                RedrawWindow(g_hLayers[PRIMARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
                RedrawWindow(g_hLayers[SECONDARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rc;
            HDC hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rc);
            HBRUSH brush = CreateSolidBrush(COLOR_MAINWND);
            FillRect(hdc, &rc, brush);
            
            EndPaint(hWnd, &ps);
            DeleteObject(brush);
        }
        break;
    case WM_CREATE:
    {
        std::ifstream fin("save.dat", std::ios::binary);
        if (!fin)
            break;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                fin.read(reinterpret_cast<char*>(&g_squaresVals[i][j]), sizeof(unsigned short));
            }
        }
        fin.read(reinterpret_cast<char*>(&g_scoreVal), sizeof(int));
        fin.read(reinterpret_cast<char*>(&g_lastScoreVal), sizeof(int));
        fin.read(reinterpret_cast<char*>(&g_goal), sizeof(int));
        fin.read(reinterpret_cast<char*>(&g_status), sizeof(GameStatus));
    }
    break;
    case WM_CLOSE:
    {
        /*if (MessageBox(hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hWnd);
        }*/
        // TODO
        std::ofstream fout("save.dat", std::ios::trunc | std::ios::binary);
        if (!fout)
        {
            MessageBox(hWnd, L"Game couldn't be saved!", L"Save error", MB_OK);
            DestroyWindow(hWnd);
        }
        fout.write(reinterpret_cast<char*>(g_squaresVals), sizeof g_squaresVals);
        fout.write(reinterpret_cast<char*>(&g_scoreVal), sizeof g_scoreVal);
        fout.write(reinterpret_cast<char*>(&g_lastScoreVal), sizeof g_lastScoreVal);
        fout.write(reinterpret_cast<char*>(&g_goal), sizeof g_goal);
        fout.write(reinterpret_cast<char*>(&g_status), sizeof g_status);
        fout.close();

        DestroyWindow(hWnd);
    }
    break;
    case WM_MOVING:
    {
        HWND activeWnd, idleWnd;
        if (GetActiveWindow() == g_hMainWindows[PRIMARY_WND])
        {
            activeWnd = g_hMainWindows[PRIMARY_WND];
            idleWnd = g_hMainWindows[SECONDARY_WND];
        }
        else
        {
            activeWnd = g_hMainWindows[SECONDARY_WND];
            idleWnd = g_hMainWindows[PRIMARY_WND];
        }

        // Reflection about the center of the screen
        POINT screenCenter{ GetSystemMetrics(SM_CXFULLSCREEN) / 2 , GetSystemMetrics(SM_CYFULLSCREEN) / 2 };
        ReflectWindows(screenCenter, activeWnd, idleWnd);

        // Make one window semitransparent when two windows overlap
        RECT rcIntersect;
        RECT rcPrimary, rcSecondary;
        GetWindowRect(g_hMainWindows[PRIMARY_WND], &rcPrimary);
        GetWindowRect(g_hMainWindows[SECONDARY_WND], &rcSecondary);
        if (IntersectRect(&rcIntersect, &rcPrimary, &rcSecondary)) // rectangles intersect
        {
            SetWindowLong(g_hMainWindows[SECONDARY_WND], GWL_EXSTYLE, GetWindowLong(idleWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(g_hMainWindows[SECONDARY_WND], 0, (255 * 50) / 100, LWA_ALPHA);
        }
        else
        {
            SetWindowLong(g_hMainWindows[SECONDARY_WND], GWL_EXSTYLE, GetWindowLong(idleWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
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

LRESULT CALLBACK SquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rc;
        HDC hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);

        // Rounded corners;
        SetWindowRgn(hWnd, CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 20, 20), TRUE);

        // Set background color
        HBRUSH brush;
        bool isScoreCounter = false;
        int squareValue = GetSquareValue(hWnd);
        switch (squareValue)
        {
        case 2:
            brush = CreateSolidBrush(COLOR_2);
            SetBkColor(hdc, COLOR_2);
            break;
        case 4:
            brush = CreateSolidBrush(COLOR_4);
            SetBkColor(hdc, COLOR_4);
            break;
        case 8:
            brush = CreateSolidBrush(COLOR_8);
            SetBkColor(hdc, COLOR_8);
            break;
        case 16:
            brush = CreateSolidBrush(COLOR_16);
            SetBkColor(hdc, COLOR_16);
            break;
        case 32:
            brush = CreateSolidBrush(COLOR_32);
            SetBkColor(hdc, COLOR_32);
            break;
        case 64:
            brush = CreateSolidBrush(COLOR_64);
            SetBkColor(hdc, COLOR_64);
            break;
        case 128:
            brush = CreateSolidBrush(COLOR_128);
            SetBkColor(hdc, COLOR_128);
            break;
        case 256:
            brush = CreateSolidBrush(COLOR_256);
            SetBkColor(hdc, COLOR_256);
            break;
        case 512:
            brush = CreateSolidBrush(COLOR_512);
            SetBkColor(hdc, COLOR_512);
            break;
        case 1024:
            brush = CreateSolidBrush(COLOR_1024);
            SetBkColor(hdc, COLOR_1024);
            break;
        case 2048:
            brush = CreateSolidBrush(COLOR_2048);
            SetBkColor(hdc, COLOR_2048);
            break;
        default:
            brush = CreateSolidBrush(COLOR_0);
            SetBkColor(hdc, COLOR_0);
        }
        FillRect(hdc, &rc, brush);

        // Set text
        std::wstring str = std::to_wstring(abs(squareValue));
        HFONT font = CreateFont(-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72),
            0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Verdana"));
        HFONT oldFont = (HFONT)SelectObject(hdc, font);
        SetTextColor(hdc, COLOR_TEXT);
        if (squareValue != 0 || hWnd == g_hScore[PRIMARY_WND] || hWnd == g_hScore[SECONDARY_WND])
        {
            DrawText(hdc, str.c_str(), wcslen(str.c_str()), &rc,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }


        SelectObject(hdc, oldFont);
        DeleteObject(brush);
        DeleteObject(font);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CREATE:
    {

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

LRESULT CALLBACK LayerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
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
    case WM_CREATE:
        
        break;
    case WM_PAINT:
    {
        
        PAINTSTRUCT ps;
        RECT rc;
        HDC hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        switch (g_status)
        {
        case GameStatus::win:
        {
            ShowWindow(hWnd, SW_SHOWNORMAL);
            PaintRect(hdc, rc, COLOR_WIN, 127);

            std::wstring str{ L"WIN!" };
            SetBkColor(hdc, COLOR_WIN);
            SetBkMode(hdc, TRANSPARENT);
            HFONT font = CreateFont(-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Verdana"));
            HFONT oldFont = (HFONT)SelectObject(hdc, font);
            SetTextColor(hdc, COLOR_TEXT);
            DrawText(hdc, str.c_str(), wcslen(str.c_str()), &rc,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, oldFont);
            DeleteObject(font);
        }
            
            
            break;
        case GameStatus::defeat:
        {
            ShowWindow(hWnd, SW_SHOWNORMAL);
            PaintRect(hdc, rc, COLOR_GAME_OVER, 127);

            std::wstring str{ L"GAME OVER :(" };
            SetBkColor(hdc, COLOR_GAME_OVER);
            SetBkMode(hdc, TRANSPARENT);
            HFONT font = CreateFont(-MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Verdana"));
            HFONT oldFont = (HFONT)SelectObject(hdc, font);
            SetTextColor(hdc, COLOR_TEXT);
            DrawText(hdc, str.c_str(), wcslen(str.c_str()), &rc,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, oldFont);
            DeleteObject(font);
        }
            
            break;
        default:
        {
            HBRUSH brush = CreateSolidBrush(COLOR_MAINWND);
            FillRect(hdc, &rc, brush);
            UpdateAllSquares();
            RedrawWindow(g_hScore[PRIMARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
            RedrawWindow(g_hScore[SECONDARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
            DeleteObject(brush);
        }
            
            ;
        }
        
            
        
        EndPaint(hWnd, &ps);

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


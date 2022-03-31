#include "LabWinApi.h"


// Global Variables definitions:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szSquareTitle[MAX_LOADSTRING];
WCHAR szSquareClass[MAX_LOADSTRING];
WCHAR szLayerTitle[MAX_LOADSTRING];
WCHAR szLayerClass[MAX_LOADSTRING];
HWND g_hMainWindows[2];
HWND g_hSquares[2][4][4];
HWND g_hScore[2];
HWND g_hLayers[2];
unsigned short g_squaresVals[4][4];
int g_scoreVal = 0;
int g_lastScoreVal = -1;
const int g_squareSdLen = 60;
const int g_gapSize = 10;
int g_goal = 2048;
GameStatus g_status = GameStatus::inProgress;

VOID GetMainWndSize(int* width, int* height)
{
    *height = 5 * g_squareSdLen + 6 * g_gapSize
        + GetSystemMetrics(SM_CYCAPTION)
        + GetSystemMetrics(SM_CYMENU)
        + 2 * GetSystemMetrics(SM_CYFRAME)
        + 2 * GetSystemMetrics(SM_CXPADDEDBORDER);
    *width = 4 * g_squareSdLen + 5 * g_gapSize
        + 2 * GetSystemMetrics(SM_CYFRAME)
        + 2 * GetSystemMetrics(SM_CXPADDEDBORDER);
}

VOID ReflectWindows(POINT p, HWND referenceWnd, HWND reflectedWnd)
{
    RECT rcReference;
    GetWindowRect(referenceWnd, &rcReference);
    MoveWindow(reflectedWnd, 2 * p.x - rcReference.right, 2 * p.y - rcReference.bottom,
        rcReference.right - rcReference.left, rcReference.bottom - rcReference.top, TRUE);
}

VOID ResetBoard()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            g_squaresVals[i][j] = 0;
        }
    }
}

VOID InitBoard()
{
    int randRow = RANDOM(0, 3);
    int randCol = RANDOM(0, 3);
    g_squaresVals[randRow][randCol] = 2;
}

VOID UpdateBoard(Direction d)
{
    if (g_status == GameStatus::win || g_status == GameStatus::defeat)
        return;
    switch (d)
    {
    case Direction::left:
    {
        for (int i = 0; i < 4; i++)
        {
            std::queue<int> emptySlots;
            for (int j = 0; j < 4; j++)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(j);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[i][slot] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(j);
                    }
                }
            }
        }
        for (int i = 0; i < 4; i++)
        {
            for (int j = 1; j < 4; j++)
            {
                if (g_squaresVals[i][j] == g_squaresVals[i][j - 1])
                {
                    g_squaresVals[i][j - 1] *= 2;
                    g_squaresVals[i][j] = 0;
                    g_scoreVal += g_squaresVals[i][j - 1];
                }
            }
        }
        for (int i = 0; i < 4; i++)
        {
            std::queue<int> emptySlots;
            for (int j = 0; j < 4; j++)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(j);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[i][slot] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(j);
                    }
                }
            }
        }


    }
    break;
    case Direction::right:
    {

        for (int i = 0; i < 4; i++)
        {
            std::queue<int> emptySlots;
            for (int j = 3; j >= 0; j--)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(j);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[i][slot] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(j);
                    }
                }
            }
        }
        for (int i = 0; i < 4; i++)
        {
            for (int j = 2; j >= 0; j--)
            {
                if (g_squaresVals[i][j] == g_squaresVals[i][j + 1])
                {
                    g_squaresVals[i][j + 1] *= 2;
                    g_squaresVals[i][j] = 0;
                    g_scoreVal += g_squaresVals[i][j + 1];
                }
            }
        }
        for (int i = 0; i < 4; i++)
        {
            std::queue<int> emptySlots;
            for (int j = 3; j >= 0; j--)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(j);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[i][slot] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(j);
                    }
                }
            }
        }

    }
    break;
    case Direction::up:
    {
        for (int j = 0; j < 4; j++)
        {
            std::queue<int> emptySlots;
            for (int i = 0; i < 4; i++)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(i);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[slot][j] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(i);
                    }
                }
            }
        }
        for (int j = 0; j < 4; j++)
        {
            for (int i = 1; i < 4; i++)
            {
                if (g_squaresVals[i][j] == g_squaresVals[i - 1][j])
                {
                    g_squaresVals[i - 1][j] *= 2;
                    g_squaresVals[i][j] = 0;
                    g_scoreVal += g_squaresVals[i - 1][j];
                }
            }
        }
        for (int j = 0; j < 4; j++)
        {
            std::queue<int> emptySlots;
            for (int i = 0; i < 4; i++)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(i);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[slot][j] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(i);
                    }
                }
            }
        }


    }
    break;
    case Direction::down:
    {
        for (int j = 0; j < 4; j++)
        {
            std::queue<int> emptySlots;
            for (int i = 3; i >= 0; i--)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(i);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[slot][j] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(i);
                    }
                }
            }
        }
        for (int j = 0; j < 4; j++)
        {
            for (int i = 2; i >= 0; i--)
            {
                if (g_squaresVals[i][j] == g_squaresVals[i + 1][j])
                {
                    g_squaresVals[i + 1][j] *= 2;
                    g_squaresVals[i][j] = 0;
                    g_scoreVal += g_squaresVals[i + 1][j];
                }
            }
        }
        for (int j = 0; j < 4; j++)
        {
            std::queue<int> emptySlots;
            for (int i = 3; i >= 0; i--)
            {
                if (g_squaresVals[i][j] == 0)
                    emptySlots.push(i);
                else
                {
                    if (!emptySlots.empty())
                    {
                        int slot = emptySlots.front();
                        emptySlots.pop();
                        g_squaresVals[slot][j] = g_squaresVals[i][j];
                        g_squaresVals[i][j] = 0;
                        emptySlots.push(i);
                    }
                }
            }
        }

    }
    break;
    }

    g_status = CheckGameStatus();
    if (g_status == GameStatus::win || g_status == GameStatus::defeat)
    {
        UpdateAllSquares();
        RedrawWindow(g_hLayers[PRIMARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(g_hLayers[SECONDARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
    }
    else // game in progress
    {
        // Generate a random "2"
        std::vector<int> availableSquares;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (g_squaresVals[i][j] == 0)
                    availableSquares.push_back(i * 4 + j);
            }
        }

        if (availableSquares.size() != 0)
        {
            int rndIndex = RANDOM(0, availableSquares.size() - 1);
            int rndSquareRow = availableSquares[rndIndex] / 4;
            int rndSquareCol = availableSquares[rndIndex] % 4;
            g_squaresVals[rndSquareRow][rndSquareCol] = 2;
        }

        UpdateAllSquares();
    }
}

VOID UpdateAllSquares()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            RedrawWindow(g_hSquares[PRIMARY_WND][i][j], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
            RedrawWindow(g_hSquares[SECONDARY_WND][i][j], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
        }

    }
    if (g_scoreVal != g_lastScoreVal)
    {
        RedrawWindow(g_hScore[PRIMARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(g_hScore[SECONDARY_WND], 0, 0, RDW_INVALIDATE | RDW_UPDATENOW);
        g_lastScoreVal = g_scoreVal;
    }
}

// Returns the value on a square with window handle hWnd and the negated value stored in the score counter
int GetSquareValue(HWND hWnd)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (hWnd == g_hSquares[PRIMARY_WND][i][j]
                || hWnd == g_hSquares[SECONDARY_WND][i][j])
            {
                return g_squaresVals[i][j];
            }

        }
    }
    if (hWnd == g_hScore[PRIMARY_WND]
        || hWnd == g_hScore[SECONDARY_WND])
    {
        return -g_scoreVal;
    }
    return -1; // never reached

}

GameStatus CheckGameStatus()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (g_squaresVals[i][j] == g_goal)
                return GameStatus::win;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (g_squaresVals[i][j] == 0)
                return GameStatus::inProgress;
        }
    }

    // full board doesn't imply a defeat
    // check if there are incident squares with the same value
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j < 4; j++)
        {
            if (g_squaresVals[i][j] == g_squaresVals[i][j - 1])
                return GameStatus::inProgress;
        }
    }
    for (int j = 0; j < 4; j++)
    {
        for (int i = 1; i < 4; i++)
        {
            if (g_squaresVals[i][j] == g_squaresVals[i - 1][j])
                return GameStatus::inProgress;
        }
    }
    return GameStatus::defeat;
}

// As suggested by Tony here: https://stackoverflow.com/a/10975603/15767108 
// Details: https://docs.microsoft.com/en-us/windows/win32/gdi/alpha-blending-a-bitmap
bool PaintRect(HDC hdc, RECT dim, COLORREF brushCol, unsigned int opacity)
{
    HDC tempHdc = CreateCompatibleDC(hdc);
    BLENDFUNCTION blend = { AC_SRC_OVER, static_cast<BYTE>(0), opacity, static_cast<BYTE>(0) };

    HBITMAP hbitmap;       // bitmap handle 
    BITMAPINFO bmi;        // bitmap header 
    VOID* pvBits;
    // zero the memory for the bitmap info 
    ZeroMemory(&bmi, sizeof(BITMAPINFO));

    // setup bitmap info  
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = dim.right - dim.left;
    bmi.bmiHeader.biHeight = dim.bottom - dim.top;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = (dim.right - dim.left) * (dim.bottom - dim.top) * 4;

    // create our DIB section and select the bitmap into the dc
    hbitmap = CreateDIBSection(tempHdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
    if (!hbitmap)
    {
        DeleteDC(tempHdc);
        return false;
    }
    SelectObject(tempHdc, hbitmap);

    SetDCBrushColor(tempHdc, brushCol);
    FillRect(tempHdc, &dim, CreateSolidBrush(brushCol));



    AlphaBlend(hdc, dim.left, dim.top, dim.right, dim.bottom, tempHdc, dim.left, dim.top, dim.right, dim.bottom, blend);

    DeleteDC(tempHdc);
    DeleteObject(hbitmap);

    return true;
}
#pragma once

#include "resource.h"
#include "framework.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <queue>
#include <vector>
#include <fstream>

#define MAX_LOADSTRING 100
// Main windows identifiers
#define PRIMARY_WND 0
#define SECONDARY_WND 1
// Colors corresponding to square values
#define COLOR_0 RGB(204, 192, 174)
#define COLOR_2 RGB(238, 228, 198)
#define COLOR_4 RGB(239, 225, 218)
#define COLOR_8 RGB(243, 179, 124)
#define COLOR_16 RGB(246, 153, 100)
#define COLOR_32 RGB(246, 125, 98)
#define COLOR_64 RGB(247, 93, 60)
#define COLOR_128 RGB(237, 206, 116)
#define COLOR_256 RGB(239, 204, 98)
#define COLOR_512 RGB(243, 201, 85)
#define COLOR_1024 RGB(238, 200, 72)
#define COLOR_2048 RGB(239, 192, 47)
#define COLOR_MAINWND RGB(250, 247, 238)
#define COLOR_TEXT RGB(255, 255, 255)
#define COLOR_WIN RGB(3, 252, 73)
#define COLOR_GAME_OVER RGB(245, 84, 59)
// helper macros
#define RANDOM(min, max) (std::rand() % ((max) - (min) + 1) + (min))
// Directions available to the player
enum class Direction { left, right, up, down };
enum class GameStatus { defeat, win, inProgress };

// Global Variables:
extern HINSTANCE hInst;                                // current instance
extern WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
extern WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
extern WCHAR szSquareTitle[MAX_LOADSTRING];
extern WCHAR szSquareClass[MAX_LOADSTRING];
extern WCHAR szLayerTitle[MAX_LOADSTRING];
extern WCHAR szLayerClass[MAX_LOADSTRING];
extern HWND g_hMainWindows[2];
extern HWND g_hSquares[2][4][4];
extern HWND g_hScore[2];
extern HWND g_hLayers[2];
extern unsigned short g_squaresVals[4][4];
extern int g_scoreVal;
extern int g_lastScoreVal;
extern const int g_squareSdLen;
extern const int g_gapSize;
extern int g_goal;
extern GameStatus g_status;

VOID                GetMainWndSize(int*, int*);
VOID				ReflectWindows(POINT, HWND, HWND);
VOID                InitBoard();
VOID                UpdateBoard(Direction);
VOID                UpdateAllSquares();
VOID                ResetBoard();
int                 GetSquareValue(HWND hWnd);
GameStatus          CheckGameStatus();
bool                PaintRect(HDC hdc, RECT dim, COLORREF brushCol, unsigned int opacity);

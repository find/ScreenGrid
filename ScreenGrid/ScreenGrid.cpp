// ScreenGrid.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "ScreenGrid.h"
#include <commdlg.h>
#include <sstream>

#define MAX_LOADSTRING 100
const int MAX_GRID_SPACING = 256;
const int MIN_GRID_SPACING = 10;
const COLORREF TRANSPARENT_COLOR = RGB(255, 255, 0);

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND  hMainWnd = NULL;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    GridSizeDlg(HWND, UINT, WPARAM, LPARAM);

int spacing = 64;
bool ontop = false;
COLORREF color = RGB(128, 128, 128);
// Grid Offset
bool  offsetting = false;
int64_t offsetX = 0, offsetY = 0;
int64_t offsetAnchorX = 0, offsetAnchorY = 0;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCREENGRID, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCREENGRID));

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

    return (int)msg.wParam;
}

void updateTitle()
{
    std::stringstream ss;
    ss << "ScreenGrid (" << spacing << "px)";
    if (ontop)
        ss << "  [TOP]";
    ::SetWindowTextA(hMainWnd, ss.str().c_str());
}

void toggleTopmost(HWND hWnd)
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    if (!ontop) {
        ::SetWindowPos(hWnd,        // handle to window
            HWND_TOPMOST,           // placement-order handle
            rect.left,              // horizontal position
            rect.top,               // vertical position
            rect.right - rect.left, // width
            rect.bottom - rect.top, // height
            SWP_SHOWWINDOW);        // window-positioning options
        ontop = true;
    }
    else {
        ::SetWindowPos(hWnd,        // handle to window
            HWND_NOTOPMOST,         // placement-order handle
            rect.left,              // horizontal position
            rect.top,               // vertical position
            rect.right - rect.left, // width
            rect.bottom - rect.top, // height
            SWP_SHOWWINDOW); // window-positioning options
        ontop = false;
    }
    updateTitle();
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCREENGRID));
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName  = nullptr; // MAKEINTRESOURCEW(IDC_SCREENGRID);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, 
                  szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
                  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    SetLayeredWindowAttributes(hWnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
    HMENU pSysMenu = ::GetSystemMenu(hWnd, FALSE);
    if (pSysMenu)
    {
        ::InsertMenu(pSysMenu, 0, MF_BYPOSITION | MF_STRING, IDM_ABOUT, TEXT("&About ..."));
        ::InsertMenu(pSysMenu, 1, MF_BYPOSITION | MF_STRING, IDM_GRIDSIZEDLG, TEXT("Set &Grid Size..."));
        ::InsertMenu(pSysMenu, 2, MF_BYPOSITION | MF_STRING, IDM_CHOOSECOLOR, TEXT("Choose &Color..."));
        ::InsertMenu(pSysMenu, 3, MF_BYPOSITION | MF_STRING, IDM_STAYTOP, TEXT("Toggle Stay On &Top"));
        ::InsertMenu(pSysMenu, 4, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    hMainWnd = hWnd;
    toggleTopmost(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static UPDATELAYEREDWINDOWINFO updateinfo = { 0 };
    updateinfo.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
    updateinfo.hdcDst = NULL;
    updateinfo.pptDst = NULL;
    updateinfo.psize = NULL;
    updateinfo.hdcSrc = NULL;
    updateinfo.pptSrc = NULL;
    updateinfo.crKey = TRANSPARENT_COLOR;
    updateinfo.pblend = NULL;
    updateinfo.dwFlags = ULW_COLORKEY;
    updateinfo.prcDirty = NULL;

    switch (message)
    {
    case WM_COMMAND:
    case WM_SYSCOMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_GRIDSIZEDLG:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_GRIDSIZE), hWnd, GridSizeDlg);
            break;
        case IDM_CHOOSECOLOR:
        {
            CHOOSECOLOR cc = { 0 };
            static COLORREF custClr[16] = { 0 };
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hMainWnd;
            cc.rgbResult = color;
            cc.lpCustColors = custClr;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColor(&cc)) {
                color = cc.rgbResult;
                if (color == TRANSPARENT_COLOR)
                    color = TRANSPARENT_COLOR + 1;
                InvalidateRect(hMainWnd, NULL, FALSE);
            }
        }
        break;
        case IDM_STAYTOP:
            toggleTopmost(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT clientRect = { 0 };
        HDC hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &clientRect);
        auto brush = CreateSolidBrush(TRANSPARENT_COLOR);
        SelectObject(hdc, brush);
        Rectangle(hdc, 0, 0, clientRect.right, clientRect.bottom);
        auto dotPen = CreatePen(PS_DOT, 1, color);
        auto thickPen = CreatePen(PS_SOLID, 1, color);
        SelectObject(hdc, dotPen);
        SetBkMode(hdc, TRANSPARENT);
        for (int i = (spacing - 1 + offsetX) % spacing; i < clientRect.right; i += spacing) {
            MoveToEx(hdc, i, 0, nullptr);
            LineTo(hdc, i, clientRect.bottom);
        }
        for (int i = (spacing - 1 + offsetY) % spacing; i < clientRect.bottom; i += spacing) {
            MoveToEx(hdc, 0, i, nullptr);
            LineTo(hdc, clientRect.right, i);
        }
        SelectObject(hdc, thickPen);
        for (int i = (spacing - 1 + spacing * 3 + offsetX) % (4 * spacing); i < clientRect.right; i += spacing * 4) {
            MoveToEx(hdc, i, 0, nullptr);
            LineTo(hdc, i, clientRect.bottom);
        }
        for (int i = (spacing - 1 + spacing * 3 + offsetY) % (4 * spacing); i < clientRect.right; i += spacing * 4) {
            MoveToEx(hdc, 0, i, nullptr);
            LineTo(hdc, clientRect.right, i);
        }
        DeleteObject(brush);
        DeleteObject(dotPen);
        DeleteObject(thickPen);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        int newSpacing = max(MIN_GRID_SPACING, min(MAX_GRID_SPACING, spacing + zDelta / abs(zDelta)));
        if (newSpacing != spacing) {
            // offset+gridpos*spacing = center
            // newOffset+gridpos*newSpacing == center
            int64_t centerX = LOWORD(lParam);
            int64_t centerY = HIWORD(lParam);
            float gridposX = float(centerX - offsetX) / spacing;
            float gridposY = float(centerY - offsetY) / spacing;
            offsetX = WORD(centerX - gridposX * newSpacing);
            offsetY = WORD(centerY - gridposY * newSpacing);
            InvalidateRect(hWnd, NULL, FALSE);
            spacing = newSpacing;
            updateTitle();
        }
    }
    break;
    case WM_LBUTTONDOWN:
        offsetting = true;
        offsetAnchorX = LOWORD(lParam) - offsetX;
        offsetAnchorY = HIWORD(lParam) - offsetY;
        ::SetCapture(hWnd);
        break;
    case WM_MOUSEMOVE:
        if (offsetting) {
            offsetX = LOWORD(lParam) - offsetAnchorX;
            offsetY = HIWORD(lParam) - offsetAnchorY;
            InvalidateRect(hWnd, NULL, FALSE);
            //std::string msg = "offsetting: " + std::to_string(offsetX) + ", " + std::to_string(offsetY) + "\n";
            //OutputDebugStringA(msg.c_str());
        }
        break;
    case WM_LBUTTONUP:
    case WM_MOUSELEAVE:
    case WM_KILLFOCUS:
        offsetting = false;
        ::ReleaseCapture();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for grid size setting box.
INT_PTR CALLBACK GridSizeDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        std::stringstream ss;
        ss << spacing;
        SetDlgItemTextA(hDlg, IDC_GRIDSIZE_EDIT, ss.str().c_str());
    }
    return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            if (LOWORD(wParam) == IDOK) {
                char buf[512] = { 0 };
                GetDlgItemTextA(hDlg, IDC_GRIDSIZE_EDIT, buf, 512);
                spacing = max(MIN_GRID_SPACING, min(MAX_GRID_SPACING, atoi(buf)));
                updateTitle();
                InvalidateRect(hMainWnd, NULL, FALSE);
            }

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// black magic ...
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
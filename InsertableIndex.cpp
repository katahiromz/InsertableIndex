#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cassert>

HWND g_hList = NULL;
HIMAGELIST g_hImageList = NULL;
HWND g_hStatus = NULL;
WNDPROC g_fnOldListViewWndProc = NULL;
HPEN g_hPenBlack = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
HPEN g_hPenRed = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
HPEN g_hPenGreen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
HPEN g_hPenBlue = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));

INT FindInsertableIndex(HWND hwndListView, POINT pt)
{
    RECT rcBound, rcIcon, rcLabel;
    INT i, nCount = ListView_GetItemCount(hwndListView);
    INT xOld, yOld;
    BOOL bSmall = ((GetWindowStyle(hwndListView) & LVS_TYPEMASK) != LVS_ICON);

    pt.x += GetScrollPos(hwndListView, SB_HORZ);
    pt.y += GetScrollPos(hwndListView, SB_VERT);


    if (GetWindowStyle(hwndListView) & LVS_ALIGNLEFT)
    {
        // vertically
        for (i = 0; i < nCount; ++i)
        {
            ListView_GetItemRect(hwndListView, i, &rcBound, LVIR_BOUNDS);
            ListView_GetItemRect(hwndListView, i, &rcIcon, LVIR_ICON);
            ListView_GetItemRect(hwndListView, i, &rcLabel, LVIR_LABEL);
            {
                HDC hDC = GetDC(hwndListView);
                SelectObject(hDC, GetStockObject(NULL_BRUSH));
                SelectObject(hDC, g_hPenBlack);
                Rectangle(hDC, rcBound.left, rcBound.top, rcBound.right, rcBound.bottom);
                SelectObject(hDC, g_hPenRed);
                Rectangle(hDC, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom);
                SelectObject(hDC, g_hPenGreen);
                Rectangle(hDC, rcLabel.left, rcLabel.top, rcLabel.right, rcLabel.bottom);
                ReleaseDC(hwndListView, hDC);
            }
            if (pt.x < rcBound.right && pt.y < (rcIcon.top + rcIcon.bottom) / 2)
            {
                return i;
            }
        }
        for (i = nCount - 1; i >= 0; --i)
        {
            DWORD dw = ListView_GetItemSpacing(hwndListView, bSmall);
            ListView_GetItemRect(hwndListView, i, &rcBound, LVIR_BOUNDS);
            ListView_GetItemRect(hwndListView, i, &rcIcon, LVIR_ICON);
            INT x = LOWORD(dw);
            INT y = HIWORD(dw);
            {
                HDC hDC = GetDC(hwndListView);
                SelectObject(hDC, GetStockObject(NULL_BRUSH));
                SelectObject(hDC, g_hPenBlue);
                Rectangle(hDC, rcBound.left, rcBound.top, rcBound.left + x, rcBound.top + y);
                ReleaseDC(hwndListView, hDC);
            }
        }
    }
    else
    {
        // horizontally
        for (i = 0; i < nCount; ++i)
        {
            ListView_GetItemRect(hwndListView, i, &rcBound, LVIR_BOUNDS);
            ListView_GetItemRect(hwndListView, i, &rcLabel, LVIR_LABEL);
            {
                HDC hDC = GetDC(hwndListView);
                SelectObject(hDC, GetStockObject(NULL_BRUSH));
                SelectObject(hDC, g_hPenBlack);
                Rectangle(hDC, rcBound.left, rcBound.top, rcBound.right, rcBound.bottom);
                SelectObject(hDC, g_hPenRed);
                Rectangle(hDC, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom);
                SelectObject(hDC, g_hPenGreen);
                Rectangle(hDC, rcLabel.left, rcLabel.top, rcLabel.right, rcLabel.bottom);
                ReleaseDC(hwndListView, hDC);
            }
            if (pt.x < (rcIcon.left + rcIcon.right) / 2 && pt.y < rcBound.bottom)
            {
                return i;
            }
        }
        for (i = nCount - 1; i >= 0; --i)
        {
            DWORD dw = ListView_GetItemSpacing(hwndListView, bSmall);
            ListView_GetItemRect(hwndListView, i, &rcBound, LVIR_BOUNDS);
            ListView_GetItemRect(hwndListView, i, &rcIcon, LVIR_ICON);
            INT x = LOWORD(dw);
            INT y = HIWORD(dw);
            {
                HDC hDC = GetDC(hwndListView);
                SelectObject(hDC, GetStockObject(NULL_BRUSH));
                SelectObject(hDC, g_hPenBlue);
                Rectangle(hDC, rcBound.left, rcBound.top, rcBound.left + x, rcBound.top + y);
                ReleaseDC(hwndListView, hDC);
            }
        }
    }

    return nCount;
}

LRESULT CALLBACK
ListViewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    TCHAR szText[64];
    INT iInsertable;
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        iInsertable = FindInsertableIndex(hwnd, pt);
        wsprintf(szText, TEXT("(%d, %d), insertable:%d"), pt.x, pt.y, iInsertable);
        SendMessage(g_hStatus, SB_SETTEXT, 0, (LPARAM)szText);
    }
    return CallWindowProc(g_fnOldListViewWndProc, hwnd, uMsg, wParam, lParam);
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    g_hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE, NULL, hwnd, 2);
    if (g_hStatus == NULL)
        return FALSE;

    RECT rc;
    GetClientRect(hwnd, &rc);

    // NOTE: Choose one:
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_ICON;
    DWORD style = WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_ALIGNLEFT;
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_SMALLICON;
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_SMALLICON | LVS_ALIGNLEFT;
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_LIST;
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_LIST | LVS_ALIGNLEFT;
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_REPORT;
    //DWORD style = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_ALIGNLEFT;

    g_hList = CreateWindow(WC_LISTVIEW, NULL, style,
        0, 0, 0, 0, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
    if (g_hList == NULL)
        return FALSE;

    g_fnOldListViewWndProc = SubclassWindow(g_hList, ListViewWndProc);

    //SetWindowLong(g_hList, GWL_STYLE, LVS_AUTOARRANGE | GetWindowStyle(g_hList));

    if ((style & LVS_TYPEMASK) == LVS_ICON)
    {
        g_hImageList = ImageList_Create(32, 32, ILC_COLOR4 | ILC_MASK, 1, 1);
        if (g_hImageList == NULL)
            return FALSE;

        ImageList_AddIcon(g_hImageList, LoadIcon(NULL, IDI_ASTERISK));
        ListView_SetImageList(g_hList, g_hImageList, LVSIL_NORMAL);
    }
    else
    {
        g_hImageList = ImageList_Create(16, 16, ILC_COLOR4 | ILC_MASK, 1, 1);
        if (g_hImageList == NULL)
            return FALSE;

        ImageList_AddIcon(g_hImageList, LoadIcon(NULL, IDI_ASTERISK));
        ListView_SetImageList(g_hList, g_hImageList, LVSIL_SMALL);
    }

    LV_COLUMN col;
    ZeroMemory(&col, sizeof(col));
    col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    col.fmt = LVCFMT_LEFT;
    col.cx = 100;
    col.pszText = TEXT("Name");
    ListView_InsertColumn(g_hList, 0, &col);

    LV_ITEM item;
    ZeroMemory(&item, sizeof(item));
    item.mask = LVIF_TEXT | LVIF_IMAGE;
    item.iImage = 0;
    for (INT i = 0; i < 36; i++)
    {
        TCHAR szText[64];
        wsprintf(szText, TEXT("Item %d"), i);
        item.pszText = szText;
        item.iItem = i;
        ListView_InsertItem(g_hList, &item);
    }

    SetFocus(g_hList);

    PostMessage(hwnd, WM_SIZE, 0, 0);

    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rc, rcStatus;
    GetClientRect(hwnd, &rc);

    SendMessage(g_hStatus, WM_SIZE, 0, 0);
    GetWindowRect(g_hStatus, &rcStatus);

    MoveWindow(g_hList, 0, 0, rc.right, rc.bottom - (rcStatus.bottom - rcStatus.top), TRUE);
}

void OnDestroy(HWND hwnd)
{
    DestroyWindow(g_hList);
    DestroyWindow(g_hStatus);
    PostQuitMessage(0);
}

LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SIZE, OnSize);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    InitCommonControls();

    WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpszClassName = TEXT("InsertableIndex");
    if (!RegisterClass(&wc))
    {
        MessageBoxA(NULL, "RegisterClass failed", NULL, MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindow(TEXT("InsertableIndex"), TEXT("InsertableIndex"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
        NULL, NULL, hInstance, NULL);
    if (!hwnd)
    {
        MessageBoxA(NULL, "CreateWindow failed", NULL, MB_ICONERROR);
        return -2;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

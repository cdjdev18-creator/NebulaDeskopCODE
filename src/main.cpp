#include <windows.h>
#include <commctrl.h>
#include <cwchar>

#define ID_FILE_TREE      1001
#define ID_PROJECT_TREE   1002
#define ID_AGENT_CONV     1003
#define ID_AGENT_INPUT    1004
#define ID_SEND_BTN       1005
#define ID_BTN_CONFIG     1010
#define ID_BTN_AGENTES    1011
#define ID_BTN_ADD_PROJ   1012

#define ID_SPLITTER_LR    2001
#define ID_SPLITTER_LTB   2002
#define ID_SPLITTER_CT    2003

static const COLORREF CLR_BG        = RGB(30, 30, 30);
static const COLORREF CLR_PANEL     = RGB(35, 35, 35);
static const COLORREF CLR_HEADER    = RGB(45, 45, 45);
static const COLORREF CLR_TEXT      = RGB(220, 220, 220);
static const COLORREF CLR_SPLITTER  = RGB(60, 60, 60);

static int g_cx = 900, g_cy = 600;
static int g_leftW   = 250;
static int g_centerW = 400;
static int g_rightW  = 250;
static int g_leftSplit  = 220;
static int g_centerSplit = 320;
static const int SPLIT = 4;

static bool g_drag = false;
static int g_dragWhich = 0;
static int g_dragStartX = 0;
static int g_dragStartY = 0;

static HWND g_hFileTree   = nullptr;
static HWND g_hProjTree   = nullptr;
static HWND g_hAgentConv  = nullptr;
static HWND g_hAgentInput = nullptr;
static HWND g_hSendBtn    = nullptr;
static HWND g_hBtnConfig  = nullptr;
static HWND g_hBtnAgentes = nullptr;
static HWND g_hBtnAddProj = nullptr;
static HWND g_hSplitLR    = nullptr;
static HWND g_hSplitLTB   = nullptr;
static HWND g_hSplitCT    = nullptr;

static HFONT g_hFont      = nullptr;
static HFONT g_hHeaderFnt = nullptr;

static void AddTreeRoot(HWND hTree, const wchar_t* name) {
    TVINSERTSTRUCTW tis = {};
    tis.item.mask = TVIF_TEXT | TVIF_STATE;
    tis.item.state = TVIS_EXPANDED;
    tis.item.stateMask = TVIS_EXPANDED;
    tis.item.pszText = const_cast<wchar_t*>(name);
    TreeView_InsertItem(hTree, &tis);
}

static void AddTreeChild(HWND hTree, HTREEITEM hParent, const wchar_t* name) {
    TVINSERTSTRUCTW tis = {};
    tis.hParent = hParent;
    tis.item.mask = TVIF_TEXT;
    tis.item.pszText = const_cast<wchar_t*>(name);
    TreeView_InsertItem(hTree, &tis);
}

static void FillFileTree(HWND hTree) {
    HTREEITEM root = TreeView_GetRoot(hTree);
    if (!root) {
        AddTreeRoot(hTree, L"NebulaDeskopCODE");
        root = TreeView_GetRoot(hTree);
    }
    AddTreeChild(hTree, root, L"src");
    AddTreeChild(hTree, root, L"EXE");
    AddTreeChild(hTree, root, L".kilo");
    AddTreeChild(hTree, root, L"build.bat");
    AddTreeChild(hTree, root, L"LICENSE");
    AddTreeChild(hTree, root, L"README.md");
    HTREEITEM src = TreeView_GetChild(hTree, root);
    if (src) {
        AddTreeChild(hTree, src, L"main.cpp");
    }
}

static void FillProjectTree(HWND hTree) {
    HTREEITEM root = TreeView_GetRoot(hTree);
    if (!root) {
        AddTreeRoot(hTree, L"Proyectos Nebula");
        root = TreeView_GetRoot(hTree);
    }
    AddTreeChild(hTree, root, L"Mi Proyecto");
    AddTreeChild(hTree, root, L"Web App");
    AddTreeChild(hTree, root, L"Consola");
}

static void LayoutControls(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    g_cx = rc.right - rc.left;
    g_cy = rc.bottom - rc.top;

    int minW = 300;
    if (g_cx < minW) g_cx = minW;

    int totalSplit = SPLIT * 2;
    int availW = g_cx - totalSplit;

    int leftW   = g_leftW;
    int centerW = g_centerW;
    int rightW  = g_rightW;
    int sum = leftW + centerW + rightW;
    if (sum > availW) {
        float scale = (float)availW / (float)sum;
        leftW   = (int)(leftW   * scale);
        centerW = (int)(centerW * scale);
        rightW  = availW - leftW - centerW;
    }
    if (leftW < 100)   leftW   = 100;
    if (centerW < 150) centerW = 150;
    if (rightW < 100)  rightW  = 100;

    int availH = g_cy;

    int leftSplitH   = g_leftSplit;
    int centerSplitH = g_centerSplit;

    int leftBottomH = availH - leftSplitH - SPLIT;
    if (leftBottomH < 60) {
        leftBottomH = 60;
        leftSplitH  = availH - SPLIT - 60;
    }
    if (leftSplitH < 60) {
        leftSplitH = 60;
        leftBottomH = availH - SPLIT - 60;
    }

    int centerBottomH = availH - centerSplitH - SPLIT;
    if (centerBottomH < 60) {
        centerBottomH = 60;
        centerSplitH  = availH - SPLIT - 60;
    }
    if (centerSplitH < 100) {
        centerSplitH = 100;
        centerBottomH = availH - SPLIT - 100;
    }

    g_leftW   = leftW;
    g_centerW = centerW;
    g_rightW  = rightW;
    g_leftSplit  = leftSplitH;
    g_centerSplit = centerSplitH;

    int xLeft   = 0;
    int xSplit1 = xLeft + leftW;
    int xCenter = xSplit1 + SPLIT;
    int xSplit2 = xCenter + centerW;
    int xRight  = xSplit2 + SPLIT;

    int ySplitL = g_leftSplit;
    int yBottomL = ySplitL + SPLIT;

    int ySplitC = g_centerSplit;
    int yBottomC = ySplitC + SPLIT;

    MoveWindow(g_hFileTree,   xLeft + 4, 4, leftW - 8, ySplitL - 8, TRUE);
    MoveWindow(g_hSplitLTB,   xLeft, ySplitL, leftW, SPLIT, TRUE);

    int btnH = 26;
    int btnGap = 6;
    int btnAreaH = btnH * 3 + btnGap * 2;
    int treeH = availH - yBottomL - btnAreaH - 10;
    if (treeH < 60) treeH = 60;
    MoveWindow(g_hProjTree,   xLeft + 4, yBottomL + 4, leftW - 8, treeH, TRUE);

    int btnY = yBottomL + 4 + treeH + btnGap;
    MoveWindow(g_hBtnConfig,  xLeft + 8, btnY, leftW - 16, btnH, TRUE);
    MoveWindow(g_hBtnAgentes, xLeft + 8, btnY + btnH + btnGap, leftW - 16, btnH, TRUE);
    MoveWindow(g_hBtnAddProj, xLeft + 8, btnY + (btnH + btnGap) * 2, leftW - 16, btnH, TRUE);

    MoveWindow(g_hSplitLR,    xSplit1, 0, SPLIT, availH, TRUE);

    MoveWindow(g_hAgentConv,  xRight + 4, 44, rightW - 8, availH - 44 - 40 - 8, TRUE);
    MoveWindow(g_hAgentInput, xRight + 4, availH - 36, rightW - 80 - 8, 28, TRUE);
    MoveWindow(g_hSendBtn,    xRight + rightW - 76, availH - 36, 72, 28, TRUE);

    MoveWindow(g_hSplitCT,    xCenter, ySplitC, centerW, SPLIT, TRUE);

    InvalidateRect(hwnd, nullptr, FALSE);
}

static void DrawHeader(HWND hwnd, HDC hdc, int x, int y, int w, int h, const wchar_t* text) {
    RECT rc = {x, y, x + w, y + h};
    HBRUSH hBrush = CreateSolidBrush(CLR_HEADER);
    FillRect(hdc, &rc, hBrush);
    DeleteObject(hBrush);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, CLR_TEXT);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void DrawPanelBorder(HWND hwnd, HDC hdc, int x, int y, int w, int h) {
    RECT rc = {x, y, x + w, y + h};
    HBRUSH hBrush = CreateSolidBrush(CLR_PANEL);
    FillRect(hdc, &rc, hBrush);
    DeleteObject(hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, CLR_SPLITTER);
    HPEN hOld = (HPEN)SelectObject(hdc, hPen);
    Rectangle(hdc, x, y, x + w, y + h);
    SelectObject(hdc, hOld);
    DeleteObject(hPen);
}

static void DrawAgentHeader(HWND hwnd, HDC hdc, int x, int y, int w) {
    RECT rc = {x, y, x + w, y + 40};
    HBRUSH hBrush = CreateSolidBrush(CLR_HEADER);
    FillRect(hdc, &rc, hBrush);
    DeleteObject(hBrush);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, CLR_TEXT);
    RECT txt = {x + 8, y, x + w - 8, y + 40};
    DrawTextW(hdc, L"Nebula Agent", -1, &txt, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            InitCommonControls();

            g_hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hHeaderFnt = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

            g_hFileTree = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, L"",
                                          WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
                                          0, 0, 100, 100, hwnd, (HMENU)ID_FILE_TREE, nullptr, nullptr);
            SendMessageW(g_hFileTree, WM_SETFONT, (WPARAM)g_hFont, 0);
            FillFileTree(g_hFileTree);

            g_hProjTree = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, L"",
                                          WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
                                          0, 0, 100, 100, hwnd, (HMENU)ID_PROJECT_TREE, nullptr, nullptr);
            SendMessageW(g_hProjTree, WM_SETFONT, (WPARAM)g_hFont, 0);
            FillProjectTree(g_hProjTree);

            g_hSplitLR = CreateWindowW(WC_STATIC, L"", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
                                       0, 0, SPLIT, 100, hwnd, (HMENU)ID_SPLITTER_LR, nullptr, nullptr);
            g_hSplitLTB = CreateWindowW(WC_STATIC, L"", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
                                        0, 0, 100, SPLIT, hwnd, (HMENU)ID_SPLITTER_LTB, nullptr, nullptr);
            g_hSplitCT = CreateWindowW(WC_STATIC, L"", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
                                       0, 0, 100, SPLIT, hwnd, (HMENU)ID_SPLITTER_CT, nullptr, nullptr);

            g_hAgentConv = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDITW, L"",
                                           WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_WANTRETURN,
                                           0, 0, 200, 200, hwnd, (HMENU)ID_AGENT_CONV, nullptr, nullptr);
            SendMessageW(g_hAgentConv, WM_SETFONT, (WPARAM)g_hFont, 0);
            SetWindowTextW(g_hAgentConv, L"Nebula Agent\nConversaciones apareceran aqui.\n");

            g_hAgentInput = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDITW, L"",
                                            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                            0, 0, 150, 28, hwnd, (HMENU)ID_AGENT_INPUT, nullptr, nullptr);
            SendMessageW(g_hAgentInput, WM_SETFONT, (WPARAM)g_hFont, 0);

            g_hSendBtn = CreateWindowW(WC_BUTTON, L"Send",
                                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       0, 0, 72, 28, hwnd, (HMENU)ID_SEND_BTN, nullptr, nullptr);
            SendMessageW(g_hSendBtn, WM_SETFONT, (WPARAM)g_hFont, 0);

            g_hBtnConfig = CreateWindowW(WC_BUTTON, L"Configuracion",
                                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                         0, 0, 100, 26, hwnd, (HMENU)ID_BTN_CONFIG, nullptr, nullptr);
            SendMessageW(g_hBtnConfig, WM_SETFONT, (WPARAM)g_hFont, 0);

            g_hBtnAgentes = CreateWindowW(WC_BUTTON, L"Agentes",
                                          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                          0, 0, 100, 26, hwnd, (HMENU)ID_BTN_AGENTES, nullptr, nullptr);
            SendMessageW(g_hBtnAgentes, WM_SETFONT, (WPARAM)g_hFont, 0);

            g_hBtnAddProj = CreateWindowW(WC_BUTTON, L"Agregar proyecto +",
                                          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                          0, 0, 100, 26, hwnd, (HMENU)ID_BTN_ADD_PROJ, nullptr, nullptr);
            SendMessageW(g_hBtnAddProj, WM_SETFONT, (WPARAM)g_hFont, 0);

            LayoutControls(hwnd);
            return 0;
        }
        case WM_SIZE:
            LayoutControls(hwnd);
            return 0;
        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = 700;
            mmi->ptMinTrackSize.y = 500;
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH hBg = CreateSolidBrush(CLR_BG);
            FillRect(hdc, &rc, hBg);
            DeleteObject(hBg);

            int xLeft   = 0;
            int xSplit1 = xLeft + g_leftW;
            int xCenter = xSplit1 + SPLIT;
            int xSplit2 = xCenter + g_centerW;
            int xRight  = xSplit2 + SPLIT;

            int ySplitL = g_leftSplit;
            int yBottomL = ySplitL + SPLIT;
            int ySplitC = g_centerSplit;
            int yBottomC = ySplitC + SPLIT;

            DrawPanelBorder(hwnd, hdc, xLeft, 0, g_leftW, ySplitL);
            DrawHeader(hwnd, hdc, xLeft, 0, g_leftW, 28, L"Explorador de archivos");

            DrawPanelBorder(hwnd, hdc, xLeft, yBottomL, g_leftW, g_cy - yBottomL);
            DrawHeader(hwnd, hdc, xLeft, yBottomL, g_leftW, 28, L"Proyectos Nebula");

            DrawPanelBorder(hwnd, hdc, xCenter, 0, g_centerW, ySplitC);
            DrawHeader(hwnd, hdc, xCenter, 0, g_centerW, 28, L"Editor");

            DrawPanelBorder(hwnd, hdc, xCenter, yBottomC, g_centerW, g_cy - yBottomC);
            DrawHeader(hwnd, hdc, xCenter, yBottomC, g_centerW, 28, L"Terminal");

            DrawPanelBorder(hwnd, hdc, xRight, 0, g_rightW, g_cy);
            DrawAgentHeader(hwnd, hdc, xRight, 0, g_rightW);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            SetBkColor(hdc, CLR_PANEL);
            SetTextColor(hdc, CLR_TEXT);
            return (LRESULT)CreateSolidBrush(CLR_PANEL);
        }
        case WM_CTLCOLORBTN: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, CLR_PANEL);
            SetTextColor(hdc, CLR_TEXT);
            return (LRESULT)CreateSolidBrush(CLR_PANEL);
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            if (dis->CtlType == ODT_STATIC) {
                HDC hdc = dis->hDC;
                RECT rc = dis->rcItem;
                HBRUSH hBrush = CreateSolidBrush(CLR_SPLITTER);
                FillRect(hdc, &rc, hBrush);
                DeleteObject(hBrush);
                return TRUE;
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            POINT pt = {LOWORD(lParam), HIWORD(lParam)};
            HWND hHit = ChildWindowFromPoint(hwnd, pt);
            int id = GetDlgCtrlID(hHit);
            if (id == ID_SPLITTER_LR || id == ID_SPLITTER_LTB || id == ID_SPLITTER_CT) {
                g_drag = true;
                g_dragWhich = id;
                g_dragStartX = pt.x;
                g_dragStartY = pt.y;
                SetCapture(hwnd);
                SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                return 0;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (!g_drag) break;
            POINT pt = {LOWORD(lParam), HIWORD(lParam)};
            int dx = pt.x - g_dragStartX;
            int dy = pt.y - g_dragStartY;

            if (g_dragWhich == ID_SPLITTER_LR) {
                int newLeft = g_leftW + dx;
                int newRight = g_rightW - dx;
                int avail = g_cx - SPLIT * 2;
                if (newLeft < 100) newLeft = 100;
                if (newRight < 100) newRight = 100;
                int center = avail - newLeft - newRight;
                if (center < 150) {
                    int diff = 150 - center;
                    newRight += diff;
                    if (newRight < 100) {
                        newRight = 100;
                        newLeft = avail - 150 - 100;
                        if (newLeft < 100) newLeft = 100;
                    }
                }
                g_leftW = newLeft;
                g_rightW = newRight;
            } else if (g_dragWhich == ID_SPLITTER_LTB) {
                int newTop = g_leftSplit + dy;
                int avail = g_cy - SPLIT;
                if (newTop < 60) newTop = 60;
                if (avail - newTop < 60) newTop = avail - 60;
                g_leftSplit = newTop;
            } else if (g_dragWhich == ID_SPLITTER_CT) {
                int newTop = g_centerSplit + dy;
                int avail = g_cy - SPLIT;
                if (newTop < 100) newTop = 100;
                if (avail - newTop < 60) newTop = avail - 60;
                g_centerSplit = newTop;
            }
            g_dragStartX = pt.x;
            g_dragStartY = pt.y;
            LayoutControls(hwnd);
            return 0;
        }
        case WM_LBUTTONUP:
            if (g_drag) {
                g_drag = false;
                ReleaseCapture();
                SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            }
            return 0;
        case WM_SETCURSOR: {
            if (g_drag) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                return TRUE;
            }
            break;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_SEND_BTN) {
                wchar_t buf[512] = {};
                GetWindowTextW(g_hAgentInput, buf, 512);
                if (wcslen(buf) > 0) {
                    wchar_t conv[4096] = {};
                    GetWindowTextW(g_hAgentConv, conv, 4096);
                    size_t len = wcslen(conv);
                    swprintf(conv + len, 4096 - len, L"\nTu: %s", buf);
                    SetWindowTextW(g_hAgentConv, conv);
                    SetWindowTextW(g_hAgentInput, L"");
                    SendMessageW(g_hAgentConv, EM_LINESCROLL, 0, 0x7FFF);
                }
                return 0;
            }
            if (LOWORD(wParam) == ID_BTN_CONFIG ||
                LOWORD(wParam) == ID_BTN_AGENTES ||
                LOWORD(wParam) == ID_BTN_ADD_PROJ) {
                return 0;
            }
            break;
        case WM_KEYDOWN:
            if (wParam == VK_RETURN) {
                PostQuitMessage(0);
                return 0;
            }
            if (wParam == 0x43) {
                if (OpenClipboard(hwnd)) {
                    EmptyClipboard();
                    const wchar_t* text = L"NebulaDeskopCODE";
                    size_t size = (wcslen(text) + 1) * sizeof(wchar_t);
                    HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, size);
                    if (hData) {
                        wchar_t* pData = (wchar_t*)GlobalLock(hData);
                        if (pData) {
                            memcpy(pData, text, size);
                            GlobalUnlock(hData);
                            SetClipboardData(CF_UNICODETEXT, hData);
                        }
                    }
                    CloseClipboard();
                }
                return 0;
            }
            break;
        case WM_DESTROY:
            if (g_hFont) DeleteObject(g_hFont);
            if (g_hHeaderFnt) DeleteObject(g_hHeaderFnt);
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"NebulaDeskopCODE";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(CLR_BG);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"NebulaDeskopCODE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr) {
        return 0;
    }

    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DeleteObject(wc.hbrBackground);
    return 0;
}

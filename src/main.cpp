#include <windows.h>
#include <cstring>

static const wchar_t* VISIBLE_TEXT = L"NebulaDeskopCODE";

static void CopyTextToClipboard(HWND hwnd, const wchar_t* text) {
    if (text == nullptr) {
        return;
    }
    size_t length = wcslen(text);
    if (length == 0) {
        return;
    }
    size_t size = (length + 1) * sizeof(wchar_t);
    if (!OpenClipboard(hwnd)) {
        return;
    }
    EmptyClipboard();
    HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hData != nullptr) {
        wchar_t* pData = static_cast<wchar_t*>(GlobalLock(hData));
        if (pData != nullptr) {
            memcpy(pData, text, size);
            GlobalUnlock(hData);
            SetClipboardData(CF_UNICODETEXT, hData);
        }
    }
    CloseClipboard();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(220, 220, 220));
            DrawTextW(hdc, VISIBLE_TEXT, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_KEYDOWN:
            if (wParam == VK_RETURN) {
                PostQuitMessage(0);
                return 0;
            }
            if (wParam == 0x43) {
                CopyTextToClipboard(hwnd, VISIBLE_TEXT);
                return 0;
            }
            break;
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
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));

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

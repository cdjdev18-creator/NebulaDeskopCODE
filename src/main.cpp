#include <windows.h>
#include <commctrl.h>
#include <cwchar>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <shlwapi.h>
#include <combaseapi.h>
#include <objbase.h>
#include "WebView2.h"

static HINSTANCE g_hInstance = nullptr;
static HWND g_hwnd = nullptr;

static ICoreWebView2Environment* g_webviewEnv = nullptr;
static ICoreWebView2Controller* g_webviewController = nullptr;
static ICoreWebView2* g_webview = nullptr;

static int g_panelLeft = 280;
static int g_panelRight = 350;
static int g_panelCenter = 600;
static int g_panelCenterTop = 400;
static int g_panelCenterBottom = 160;
static bool g_yamaVisible = true;

static const wchar_t CONFIG_FILE[] = L"confis\\confiUI.txt";

static const wchar_t OBFUSCATE_SET[] =
    L"\u0410\u0411\u0412\u0413\u0414\u0415\u0416\u0417\u0418\u0419\u041a\u041b\u041c\u041d\u041e\u041f\u0420\u0421\u0422\u0423\u0424\u0425\u0426\u0427\u0428\u0429"
    L"\u042b\u042d\u042e\u042f\u0430\u0431\u0432\u0433\u0434\u0435\u0436\u0437\u0438\u0439\u043a\u043b\u043c\u043d\u043e\u043f\u0440\u0441\u0442\u0443\u0444\u0445\u0446\u0447\u0448\u0449"
    L"\u044b\u044d\u044e\u044f\u0627\u0628\u062a\u062b\u062c\u062d\u062e\u062f\u0630\u0631\u0632\u0633\u0634\u0635\u0636\u0637\u0638\u0639\u063a\u0641\u0642\u0643\u0644\u0645\u0647"
    L"\u0648\u064a\u0660\u0661\u0662\u0663\u0664\u0665\u0666\u0667\u0668\u0669\u2605\u2606\u2660\u2661\u2662\u2663\u2665\u2666\u2667\u2668\u2669\u266A\u266B\u266C\u266D\u266E\u266F"
    L"\u2713\u2717\u271D\u2721\u2725\u2729\u272D\u2731\u2735\u2739\u273D\u2741\u2745\u2749\u274D\u2751\u2755\u2759\u275D\u2761\u2765\u2769\u276D\u2771\u2775\u2779\u277D"
    L"\u2781\u2785\u2789\u278D\u2791\u2795\u2799\u279D\u27A1\u27A5\u27A9\u27AD\u27B1\u27B5\u27B9\u27BD\u27C1\u27C5\u27C9\u27CD\u27D1\u27D5\u27D9\u27DD\u27E1\u27E5"
    L"\u27E9\u27ED\u27F1\u27F5\u27F9\u27FD\u3000\u3001\u3002\u3003\u3004\u3005\u3006\u3007\u3008\u3009\u300A\u300B\u300C\u300D\u300E\u300F\u3010\u3011\u3012\u3013"
    L"\u3014\u3015\u3016\u3017\u3018\u3019\u301A\u301B\u301C\u301D\u301E\u301F\u3020\u3021\u3022\u3023\u3024\u3025\u3026\u3027\u3028\u3029\u302A\u302B\u302C\u302D"
    L"\u302E\u302F\u3030\u3031\u3032\u3033\u3034\u3035\u3036\u3037\u3038\u3039\u303A\u303B\u303C\u303D\u303E\u303F\u30FB\uFF01\uFF02\uFF03\uFF04\uFF05\uFF06"
    L"\uFF07\uFF08\uFF09\uFF0A\uFF0B\uFF0C\uFF0D\uFF0E\uFF0F\uFF10\uFF11\uFF12\uFF13\uFF14\uFF15\uFF16\uFF17\uFF18\uFF19\uFF1A\uFF1B\uFF1C\uFF1D\uFF1E\uFF1F"
    L"\uFF20\uFF21\uFF22\uFF23\uFF24\uFF25\uFF26\uFF27\uFF28\uFF29\uFF2A\uFF2B\uFF2C\uFF2D\uFF2E\uFF2F\uFF30\uFF31\uFF32\uFF33\uFF34\uFF35\uFF36\uFF37\uFF38"
    L"\uFF39\uFF3A\uFF3B\uFF3C\uFF3D\uFF3E\uFF3F\uFF40\uFF41\uFF42\uFF43\uFF44\uFF45\uFF46\uFF47\uFF48\uFF49\uFF4A\uFF4B\uFF4C\uFF4D\uFF4E\uFF4F\uFF50\uFF51"
    L"\uFF52\uFF53\uFF54\uFF55\uFF56\uFF57\uFF58\uFF59\uFF5A\uFF5B\uFF5C\uFF5D\uFF5E\uFF5F\uFF60\uFF61\uFF62\uFF63\uFF64\uFF65\uFF66\uFF67\uFF68\uFF69\uFF6A"
    L"\uFF6B\uFF6C\uFF6D\uFF6E\uFF6F\uFF70\uFF71\uFF72\uFF73\uFF74\uFF75\uFF76\uFF77\uFF78\uFF79\uFF7A\uFF7B\uFF7C\uFF7D\uFF7E\uFF7F\uFF80\uFF81\uFF82\uFF83"
    L"\uFF84\uFF85\uFF86\uFF87\uFF88\uFF89\uFF8A\uFF8B\uFF8C\uFF8D\uFF8E\uFF8F\uFF90\uFF91\uFF92\uFF93\uFF94\uFF95\uFF96\uFF97\uFF98\uFF99\uFF9A\uFF9B\uFF9C"
    L"\uFF9D\uFF9E\uFF9F\uFFA0\uFFA1\uFFA2\uFFA3\uFFA4\uFFA5\uFFA6\uFFA7\uFFA8\uFFA9\uFFAB\uFFAC\uFFAD\uFFAE\uFFAC";

static wchar_t OBFUSCATE_MAP[256];

static void BuildObfuscateMap() {
    int len = (int)wcslen(OBFUSCATE_SET);
    for (int i = 0; i < 256; i++) {
        OBFUSCATE_MAP[i] = OBFUSCATE_SET[i % len];
    }
}

static std::wstring Obfuscate(const std::string& input) {
    std::wstring result;
    result.reserve(input.size());
    for (size_t i = 0; i < input.size(); i++) {
        unsigned char b = (unsigned char)input[i];
        unsigned char obf = b ^ 0xA5;
        result.push_back(OBFUSCATE_MAP[obf]);
    }
    return result;
}

static std::string Deobfuscate(const std::wstring& input) {
    std::string result;
    result.reserve(input.size());
    for (size_t i = 0; i < input.size(); i++) {
        wchar_t wc = input[i];
        unsigned char obf = 0;
        for (int j = 0; j < 256; j++) {
            if (OBFUSCATE_MAP[j] == wc) { obf = (unsigned char)j; break; }
        }
        result.push_back((char)(obf ^ 0xA5));
    }
    return result;
}

static std::wstring GetExeDir() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    wchar_t* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) *lastSlash = L'\0';
    return std::wstring(path);
}

static std::string SerializeConfig() {
    std::ostringstream oss;
    oss << "leftW=" << g_panelLeft << ";"
        << "rightW=" << g_panelRight << ";"
        << "termH=" << g_panelCenterBottom << ";"
        << "mainW=" << g_panelCenter << ";"
        << "yamaVisible=" << (g_yamaVisible ? 1 : 0) << ";"
        << "centerTop=" << g_panelCenterTop << ";";
    return oss.str();
}

static void DeserializeConfig(const std::string& data) {
    std::istringstream iss(data);
    std::string token;
    while (std::getline(iss, token, ';')) {
        size_t eq = token.find('=');
        if (eq == std::string::npos) continue;
        std::string key = token.substr(0, eq);
        std::string val = token.substr(eq + 1);
        if (key == "leftW") g_panelLeft = std::stoi(val);
        else if (key == "rightW") g_panelRight = std::stoi(val);
        else if (key == "termH") g_panelCenterBottom = std::stoi(val);
        else if (key == "mainW") g_panelCenter = std::stoi(val);
        else if (key == "yamaVisible") g_yamaVisible = (std::stoi(val) != 0);
        else if (key == "centerTop") g_panelCenterTop = std::stoi(val);
    }
}

static void SaveConfig() {
    std::wstring exeDir = GetExeDir();
    std::wstring dir = exeDir + L"\\confis";
    CreateDirectoryW(dir.c_str(), nullptr);
    std::wstring path = exeDir + L"\\" + CONFIG_FILE;

    std::string raw = SerializeConfig();
    std::wstring obf = Obfuscate(raw);

    std::wofstream ofs(path.c_str(), std::ios::binary | std::ios::trunc);
    if (ofs) {
        ofs << obf;
    }
}

static bool LoadConfig() {
    std::wstring exeDir = GetExeDir();
    std::wstring path = exeDir + L"\\" + CONFIG_FILE;

    std::wifstream ifs(path.c_str(), std::ios::binary);
    if (!ifs) return false;

    std::wstringstream wss;
    wss << ifs.rdbuf();
    std::wstring obf = wss.str();
    if (obf.empty()) return false;

    std::string raw = Deobfuscate(obf);
    if (raw.empty()) return false;

    DeserializeConfig(raw);
    return true;
}

static std::wstring GetUiFileUrl() {
    std::wstring dir = GetExeDir();
    std::wstring path = dir + L"\\ui\\index.html";
    return L"file:///" + path;
}

class WebMessageHandler;

class EnvironmentHandler : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
public:
    explicit EnvironmentHandler(HWND hwnd) : m_hwnd(hwnd), m_refCount(1) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) return E_POINTER;
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() {
        ULONG ref = InterlockedDecrement(&m_refCount);
        if (ref == 0) delete this;
        return ref;
    }

    STDMETHODIMP Invoke(HRESULT result, ICoreWebView2Environment* env) {
        if (SUCCEEDED(result) && env) {
            g_webviewEnv = env;
            env->AddRef();
            PostMessageW(m_hwnd, WM_APP + 1, 0, 0);
        }
        return S_OK;
    }

private:
    HWND m_hwnd;
    LONG m_refCount;
};

class WebMessageHandler : public ICoreWebView2WebMessageReceivedEventHandler {
public:
    explicit WebMessageHandler(HWND hwnd) : m_hwnd(hwnd), m_refCount(1) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) return E_POINTER;
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() {
        ULONG ref = InterlockedDecrement(&m_refCount);
        if (ref == 0) delete this;
        return ref;
    }

    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) {
        LPWSTR message = nullptr;
        if (SUCCEEDED(args->TryGetWebMessageAsString(&message))) {
            std::wstring msg(message);
            CoTaskMemFree(message);

            if (msg.find(L"SAVE_CONFIG") == 0) {
                SaveConfig();
                PostMessageW(m_hwnd, WM_APP + 3, 0, 0);
            }
        }
        return S_OK;
    }

    void SendMessage(const std::wstring& message) {
        if (g_webview) {
            g_webview->PostWebMessageAsString(message.c_str());
        }
    }

private:
    HWND m_hwnd;
    LONG m_refCount;
};

class ControllerHandler : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
public:
    explicit ControllerHandler(HWND hwnd) : m_hwnd(hwnd), m_refCount(1) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
        if (!ppvObject) return E_POINTER;
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() {
        ULONG ref = InterlockedDecrement(&m_refCount);
        if (ref == 0) delete this;
        return ref;
    }

    STDMETHODIMP Invoke(HRESULT result, ICoreWebView2Controller* controller) {
        if (SUCCEEDED(result) && controller) {
            g_webviewController = controller;
            controller->AddRef();
            controller->get_CoreWebView2(&g_webview);

            WebMessageHandler* msgHandler = new WebMessageHandler(m_hwnd);
            EventRegistrationToken token;
            g_webview->add_WebMessageReceived(msgHandler, &token);

            g_webviewController->put_IsVisible(TRUE);
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            g_webviewController->put_Bounds(rc);

            std::wstring url = GetUiFileUrl();
            g_webview->Navigate(url.c_str());

            PostMessageW(m_hwnd, WM_APP + 2, 0, 0);
        }
        return S_OK;
    }

private:
    HWND m_hwnd;
    LONG m_refCount;
};

static WebMessageHandler* g_msgHandler = nullptr;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            g_hwnd = hwnd;
            g_hInstance = ((LPCREATESTRUCTW)lParam)->hInstance;

            InitCommonControls();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

            BuildObfuscateMap();
            if (!LoadConfig()) {
                g_panelLeft = 280;
                g_panelRight = 350;
                g_panelCenter = 600;
                g_panelCenterTop = 400;
                g_panelCenterBottom = 160;
                g_yamaVisible = true;
            }

            EnvironmentHandler* envHandler = new EnvironmentHandler(hwnd);
            HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
                nullptr, nullptr, nullptr, envHandler);
            if (FAILED(hr)) {
                OutputDebugStringW(L"Failed to create WebView2 environment\n");
            }
            return 0;
        }
        case WM_APP + 1: {
            ControllerHandler* ctrlHandler = new ControllerHandler(hwnd);
            g_webviewEnv->CreateCoreWebView2Controller(hwnd, ctrlHandler);
            return 0;
        }
        case WM_APP + 2: {
            g_msgHandler = new WebMessageHandler(hwnd);
            return 0;
        }
        case WM_APP + 3: {
            return 0;
        }
        case WM_SIZE: {
            if (g_webviewController) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                g_webviewController->put_Bounds(rc);
            }
            return 0;
        }
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
            SaveConfig();
            if (g_webviewController) {
                g_webviewController->put_IsVisible(FALSE);
                g_webviewController->Release();
                g_webviewController = nullptr;
            }
            if (g_webviewEnv) {
                g_webviewEnv->Release();
                g_webviewEnv = nullptr;
            }
            if (g_msgHandler) {
                g_msgHandler->Release();
                g_msgHandler = nullptr;
            }
            CoUninitialize();
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
    wc.hbrBackground = CreateSolidBrush(RGB(20, 20, 20));

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"NebulaDeskopCODE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800,
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

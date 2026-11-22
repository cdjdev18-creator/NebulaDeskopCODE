#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wincrypt.h>
#include <combaseapi.h>
#include <objbase.h>
#include <cwchar>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <psapi.h>
#include <tlhelp32.h>
#include "WebView2.h"

static HINSTANCE g_hInstance = nullptr;
static HWND g_hwndMain = nullptr;

static ICoreWebView2Environment* g_webviewEnv = nullptr;
static ICoreWebView2Controller* g_mainController = nullptr;
static ICoreWebView2* g_mainWebview = nullptr;

static int g_panelLeft = 280;
static int g_panelRight = 350;
static int g_panelCenter = 600;
static int g_panelCenterTop = 400;
static int g_panelCenterBottom = 160;
static bool g_yamaVisible = true;

static int g_windowMode = 1;
static int g_customWidth = 1200;
static int g_customHeight = 800;

static std::vector<std::wstring> g_projects;
static std::wstring g_currentProject;

static const wchar_t CONFIG_FILE[] = L"confis\\confiUI.txt";
static const wchar_t PROJECTS_FILE[] = L"confis\\projects.txt";
static const wchar_t AGENTS_DIR[] = L"confis\\agents";

static std::map<HWND, std::wstring> g_secondaryUrls;

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

static bool fileExists(const std::wstring& path);

// ============================================================================
// SISTEMA DE LOG (UTF-8, rotacion, respaldo en LocalAppData, thread-safe)
// ============================================================================

class Logger {
public:
    static Logger& Instance() {
        static Logger inst;
        return inst;
    }

    void Log(const char* level, const char* message) {
        EnterCriticalSection(&m_cs);
        if (!m_ready) {
            LeaveCriticalSection(&m_cs);
            return;
        }
        std::string line = Timestamp() + " [" + std::string(level) + "] " + std::string(message) + "\n";
        WriteLine(line);
        LeaveCriticalSection(&m_cs);
    }

    void LogWinError(const char* level, const char* context) {
        DWORD err = GetLastError();
        std::ostringstream oss;
        oss << context << " (codigo de error Windows: " << err << ")";
        Log(level, oss.str().c_str());
    }

private:
    Logger() : m_ready(false) {
        InitializeCriticalSection(&m_cs);
        m_logPath = ResolveLogPath();
        if (!m_logPath.empty()) {
            RotateIfNeeded();
            m_ready = true;
            Log("INFO", "Sistema de log inicializado.");
        }
    }

    ~Logger() {
        if (m_ready) {
            Log("INFO", "Sistema de log cerrado.");
        }
        DeleteCriticalSection(&m_cs);
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::wstring ResolveLogPath() {
        std::wstring exeDir = GetExeDir();
        std::wstring primary = exeDir + L"\\log.txt";
        if (IsPathWritable(primary)) return primary;

        wchar_t localAppData[MAX_PATH] = {};
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, localAppData))) {
            std::wstring fallbackDir = std::wstring(localAppData) + L"\\NebulaDeskopCODE";
            CreateDirectoryW(fallbackDir.c_str(), nullptr);
            std::wstring fallback = fallbackDir + L"\\log.txt";
            if (IsPathWritable(fallback)) return fallback;
        }
        return L"";
    }

    bool IsPathWritable(const std::wstring& path) {
        HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) return false;
        CloseHandle(h);
        return true;
    }

    void RotateIfNeeded() {
        if (m_logPath.empty()) return;
        WIN32_FILE_ATTRIBUTE_DATA attr;
        if (!GetFileAttributesExW(m_logPath.c_str(), GetFileExInfoStandard, &attr)) return;
        ULARGE_INTEGER size;
        size.LowPart = attr.nFileSizeLow;
        size.HighPart = attr.nFileSizeHigh;
        const ULONGLONG MAX_SIZE = 5ULL * 1024ULL * 1024ULL;
        if (size.QuadPart < MAX_SIZE) return;

        for (int i = 5; i >= 1; i--) {
            std::wostringstream cur, nxt;
            cur << m_logPath << L"." << i;
            nxt << m_logPath << L"." << (i + 1);
            if (i == 5) DeleteFileW(cur.str().c_str());
            else {
                if (fileExists(cur.str())) MoveFileW(cur.str().c_str(), nxt.str().c_str());
            }
        }
        MoveFileW(m_logPath.c_str(), (m_logPath + L".1").c_str());
    }

    void WriteLine(const std::string& line) {
        if (m_logPath.empty()) return;
        HANDLE h = CreateFileW(m_logPath.c_str(), FILE_APPEND_DATA,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                               OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD written = 0;
        WriteFile(h, line.c_str(), (DWORD)line.size(), &written, nullptr);
        CloseHandle(h);
    }

    std::string Timestamp() {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&timeT);
        std::ostringstream oss;
        oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::wstring m_logPath;
    CRITICAL_SECTION m_cs;
    bool m_ready;
};

#define LOG_INFO(msg)  Logger::Instance().Log("INFO", msg)
#define LOG_WARN(msg)  Logger::Instance().Log("WARNING", msg)
#define LOG_WARNING(msg) Logger::Instance().Log("WARNING", msg)
#define LOG_ERROR(msg) Logger::Instance().Log("ERROR", msg)
#define LOG_CRIT(msg)  Logger::Instance().Log("CRITICAL", msg)
#define LOG_WINERR(level, ctx) Logger::Instance().LogWinError(level, ctx)

// ============================================================================
// SISTEMA DE INSTANCIA UNICA
// ============================================================================

static const wchar_t SINGLE_INSTANCE_MUTEX[] = L"Global\\NebulaDeskopCODE_SingleInstanceMutex_v1";
static HANDLE g_singleInstanceMutex = nullptr;
static bool g_weOwnMutex = false;
static DWORD g_previousPid = 0;

static std::wstring GetProcessExePath(DWORD pid) {
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return L"";
    wchar_t path[MAX_PATH] = {};
    DWORD size = MAX_PATH;
    if (!QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        CloseHandle(hProc);
        return L"";
    }
    CloseHandle(hProc);
    return std::wstring(path);
}

static HWND FindPreviousInstanceWindow() {
    return FindWindowW(L"NebulaDeskopCODE", nullptr);
}

static bool IsSameExecutable(DWORD pid) {
    std::wstring otherPath = GetProcessExePath(pid);
    if (otherPath.empty()) return false;
    std::wstring ourPath;
    {
        wchar_t buf[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, buf, MAX_PATH);
        ourPath = std::wstring(buf);
    }
    return _wcsicmp(otherPath.c_str(), ourPath.c_str()) == 0;
}

#ifndef PROCESS_SYNCHRONIZE
#define PROCESS_SYNCHRONIZE 0x00020000
#endif

static bool WaitForProcessEnd(DWORD pid, DWORD timeoutMs) {
    HANDLE hProc = OpenProcess(PROCESS_SYNCHRONIZE, FALSE, pid);
    if (!hProc) return true;
    DWORD result = WaitForSingleObject(hProc, timeoutMs);
    CloseHandle(hProc);
    return result == WAIT_OBJECT_0;
}

static bool ForceTerminateProcess(DWORD pid) {
    HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!hProc) return false;
    BOOL result = TerminateProcess(hProc, 1);
    CloseHandle(hProc);
    return result != FALSE;
}

static bool CheckSingleInstance() {
    // Primero intentar abrir un mutex existente (instancia previa)
    HANDLE existingMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, SINGLE_INSTANCE_MUTEX);
    if (existingMutex) {
        CloseHandle(existingMutex);
        LOG_INFO("Se detecto una instancia anterior de NebulaDeskopCODE.");

        HWND prevWnd = FindPreviousInstanceWindow();
        if (prevWnd) {
            DWORD prevPid = 0;
            GetWindowThreadProcessId(prevWnd, &prevPid);
            if (prevPid != 0 && IsSameExecutable(prevPid)) {
                g_previousPid = prevPid;
                LOG_INFO("Solicitando cierre controlado a la instancia anterior.");

                if (!PostMessageW(prevWnd, WM_CLOSE, 0, 0)) {
                    LOG_WINERR("WARNING", "No se pudo enviar WM_CLOSE a la instancia anterior");
                }

                const DWORD GRACEFUL_WAIT_MS = 5000;
                if (WaitForProcessEnd(prevPid, GRACEFUL_WAIT_MS)) {
                    LOG_INFO("La instancia anterior cerro correctamente.");
                } else {
                    LOG_WARN("La instancia anterior no responde tras el cierre controlado.");
                    LOG_WARN("Intentando cierre forzado de la instancia anterior.");
                    if (ForceTerminateProcess(prevPid)) {
                        LOG_CRIT("Cierre forzado de la instancia anterior. Pueden perderse cambios no guardados.");
                        WaitForProcessEnd(prevPid, 3000);
                    } else {
                        LOG_WINERR("ERROR", "No se pudo finalizar forzosamente la instancia anterior");
                        return false;
                    }
                }
            } else {
                LOG_WARNING("Ventana encontrada pero no corresponde a NebulaDeskopCODE.");
                return true;
            }
        } else {
            LOG_WARN("Mutex activo pero no se encontro ventana de la instancia anterior.");
        }

        // Crear nuestro propio mutex tras cerrar la instancia previa
        g_singleInstanceMutex = CreateMutexW(nullptr, FALSE, SINGLE_INSTANCE_MUTEX);
        if (!g_singleInstanceMutex) {
            LOG_WINERR("ERROR", "No se pudo crear el mutex de instancia unica");
            return true;
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            LOG_ERROR("Otra instancia se activo mientras esperabamos.");
            CloseHandle(g_singleInstanceMutex);
            g_singleInstanceMutex = nullptr;
            return false;
        }
        g_weOwnMutex = true;
        LOG_INFO("Mutex de instancia unica obtenido tras cierre de instancia previa.");
        return true;
    }

    // No existe mutex previo: somos la primera instancia
    g_singleInstanceMutex = CreateMutexW(nullptr, FALSE, SINGLE_INSTANCE_MUTEX);
    if (!g_singleInstanceMutex) {
        LOG_WINERR("ERROR", "No se pudo crear el mutex de instancia unica");
        return true;
    }
    g_weOwnMutex = true;
    LOG_INFO("No se encontraron instancias anteriores de NebulaDeskopCODE.");
    return true;
}

static void ReleaseSingleInstance() {
    if (g_singleInstanceMutex) {
        CloseHandle(g_singleInstanceMutex);
        g_singleInstanceMutex = nullptr;
    }
    g_weOwnMutex = false;
}

// ============================================================================
// FILTRO DE EXCEPCIONES NO CONTROLADAS
// ============================================================================

static LONG WINAPI NebulaUnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo) {
    std::ostringstream oss;
    oss << "Excepcion no controlada (codigo: 0x" << std::hex
        << pExceptionInfo->ExceptionRecord->ExceptionCode << ")";
    LOG_CRIT(oss.str().c_str());
    return EXCEPTION_EXECUTE_HANDLER;
}

static std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &result[0], size);
    return result;
}

static std::string WideToUtf8(const std::wstring& str) {
    if (str.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.size(), &result[0], size, nullptr, nullptr);
    return result;
}

static std::string SerializeConfig() {
    std::ostringstream oss;
    oss << "leftW=" << g_panelLeft << ";"
        << "rightW=" << g_panelRight << ";"
        << "termH=" << g_panelCenterBottom << ";"
        << "mainW=" << g_panelCenter << ";"
        << "yamaVisible=" << (g_yamaVisible ? 1 : 0) << ";"
        << "centerTop=" << g_panelCenterTop << ";"
        << "windowMode=" << g_windowMode << ";"
        << "customW=" << g_customWidth << ";"
        << "customH=" << g_customHeight << ";";
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
        else if (key == "windowMode") g_windowMode = std::stoi(val);
        else if (key == "customW") g_customWidth = std::stoi(val);
        else if (key == "customH") g_customHeight = std::stoi(val);
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
        LOG_INFO("Configuracion guardada.");
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

static void SaveProjects() {
    std::wstring exeDir = GetExeDir();
    std::wstring dir = exeDir + L"\\confis";
    CreateDirectoryW(dir.c_str(), nullptr);
    std::wstring path = exeDir + L"\\" + PROJECTS_FILE;

    std::wofstream ofs(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!ofs) return;

    for (size_t i = 0; i < g_projects.size(); i++) {
        ofs << g_projects[i] << L"\n";
    }
}

static bool LoadProjects() {
    std::wstring exeDir = GetExeDir();
    std::wstring path = exeDir + L"\\" + PROJECTS_FILE;

    std::wifstream ifs(path.c_str(), std::ios::binary);
    if (!ifs) return false;

    g_projects.clear();
    std::wstring line;
    while (std::getline(ifs, line)) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty()) g_projects.push_back(line);
    }
    return true;
}

static std::string readFileUtf8(const std::wstring& path) {
    std::ifstream ifs(path.c_str(), std::ios::binary);
    if (!ifs) return std::string();
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

static bool writeFileUtf8(const std::wstring& path, const std::string& content) {
    std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!ofs) return false;
    ofs.write(content.c_str(), (std::streamsize)content.size());
    return ofs.good();
}

static bool directoryExists(const std::wstring& path) {
    DWORD dwAttr = GetFileAttributesW(path.c_str());
    return (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

static bool fileExists(const std::wstring& path) {
    DWORD dwAttr = GetFileAttributesW(path.c_str());
    return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

static void enumerateDirectory(HWND hwnd, ICoreWebView2* webview, const std::wstring& path) {
    std::wstring searchPath = path;
    if (searchPath.back() != L'\\') searchPath += L"\\*";

    WIN32_FIND_DATAW ffd;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        if (webview) {
            std::wstring msg = L"EVENT:DIR_LIST|" + path + L"|[]";
            webview->PostWebMessageAsString(msg.c_str());
        }
        return;
    }

    std::vector<std::pair<bool, std::wstring>> items;
    do {
        if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0) continue;
        bool isDir = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        items.push_back({isDir, ffd.cFileName});
    } while (FindNextFileW(hFind, &ffd) != 0);
    FindClose(hFind);

    std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
        if (a.first != b.first) return a.first;
        return a.second < b.second;
    });

    std::wstring json = L"[";
    for (size_t i = 0; i < items.size(); i++) {
        std::wstring name = items[i].second;
        std::replace(name.begin(), name.end(), L'"', L'\\');
        json += L"{\"name\":\"" + name + L"\",\"type\":\"" + (items[i].first ? L"dir" : L"file") + L"\"}";
        if (i + 1 < items.size()) json += L",";
    }
    json += L"]";

    if (webview) {
        std::wstring msg = L"EVENT:DIR_LIST|" + path + L"|" + json;
        webview->PostWebMessageAsString(msg.c_str());
    }
}

static void pickFolder(HWND hwnd, ICoreWebView2* webview) {
    BROWSEINFOW bi = {};
    bi.hwndOwner = hwnd;
    bi.lpszTitle = L"Seleccionar carpeta de proyecto";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
    if (pidl != nullptr) {
        wchar_t path[MAX_PATH] = {};
        if (SHGetPathFromIDListW(pidl, path)) {
            if (webview) {
                std::wstring msg = L"EVENT:FOLDER_PICKED|" + std::wstring(path);
                webview->PostWebMessageAsString(msg.c_str());
            }
        }
        IMalloc* pMalloc = nullptr;
        if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
            pMalloc->Free(pidl);
            pMalloc->Release();
        }
    } else {
        if (webview) {
            webview->PostWebMessageAsString(L"EVENT:FOLDER_CANCELLED");
        }
    }
}

static void addProject(const std::wstring& path, ICoreWebView2* webview) {
    for (size_t i = 0; i < g_projects.size(); i++) {
        if (g_projects[i] == path) return;
    }
    g_projects.push_back(path);
    SaveProjects();

    if (webview) {
        std::wstring json = L"[";
        for (size_t i = 0; i < g_projects.size(); i++) {
            std::wstring name = g_projects[i];
            size_t lastSlash = name.find_last_of(L"\\/");
            if (lastSlash != std::wstring::npos) name = name.substr(lastSlash + 1);
            std::replace(name.begin(), name.end(), L'"', L'\\');
            json += L"{\"path\":\"" + g_projects[i] + L"\",\"name\":\"" + name + L"\"}";
            if (i + 1 < g_projects.size()) json += L",";
        }
        json += L"]";
        std::wstring msg = L"EVENT:PROJECTS_LIST|" + json;
        webview->PostWebMessageAsString(msg.c_str());
    }
}

static void removeProject(const std::wstring& path, ICoreWebView2* webview) {
    g_projects.erase(std::remove(g_projects.begin(), g_projects.end(), path), g_projects.end());
    SaveProjects();

    if (g_currentProject == path) {
        g_currentProject.clear();
    }

    if (webview) {
        std::wstring json = L"[";
        for (size_t i = 0; i < g_projects.size(); i++) {
            std::wstring name = g_projects[i];
            size_t lastSlash = name.find_last_of(L"\\/");
            if (lastSlash != std::wstring::npos) name = name.substr(lastSlash + 1);
            std::replace(name.begin(), name.end(), L'"', L'\\');
            json += L"{\"path\":\"" + g_projects[i] + L"\",\"name\":\"" + name + L"\"}";
            if (i + 1 < g_projects.size()) json += L",";
        }
        json += L"]";
        std::wstring msg = L"EVENT:PROJECTS_LIST|" + json;
        webview->PostWebMessageAsString(msg.c_str());
    }
}

static void setWindowMode(int mode, int w, int h) {
    g_windowMode = mode;
    if (w > 0) g_customWidth = w;
    if (h > 0) g_customHeight = h;

    if (g_hwndMain) {
        switch (mode) {
            case 0:
                ShowWindow(g_hwndMain, SW_RESTORE);
                break;
            case 1:
                ShowWindow(g_hwndMain, SW_MAXIMIZE);
                break;
            case 2:
                ShowWindow(g_hwndMain, SW_MAXIMIZE);
                SetWindowLongW(g_hwndMain, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                SetWindowPos(g_hwndMain, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                             GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                break;
            case 3:
                ShowWindow(g_hwndMain, SW_RESTORE);
                SetWindowPos(g_hwndMain, HWND_TOP, 0, 0, g_customWidth, g_customHeight,
                             SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                break;
        }
    }
    SaveConfig();
}

static void listAgents(ICoreWebView2* webview) {
    std::wstring exeDir = GetExeDir();
    std::wstring agentsPath = exeDir + L"\\" + AGENTS_DIR;

    std::wstring json = L"[";
    bool first = true;

    if (directoryExists(agentsPath)) {
        std::wstring searchPath = agentsPath + L"\\*";
        WIN32_FIND_DATAW ffd;
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &ffd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0) continue;
                if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) continue;

                std::wstring agentDir = agentsPath + L"\\" + ffd.cFileName;
                std::wstring agentJsonPath = agentDir + L"\\agent.json";

                std::string agentName = WideToUtf8(ffd.cFileName);
                std::string agentDesc = "";
                std::string agentVersion = "1.0.0";
                std::string agentAuthor = "Desconocido";
                std::string agentLicense = "";

                if (fileExists(agentJsonPath)) {
                    std::string content = readFileUtf8(agentJsonPath);
                    auto extractField = [&](const std::string& key, std::string& out) {
                        size_t pos = content.find("\"" + key + "\"");
                        if (pos != std::string::npos) {
                            size_t colon = content.find(":", pos);
                            if (colon != std::string::npos) {
                                size_t q1 = content.find("\"", colon + 1);
                                size_t q2 = content.find("\"", q1 + 1);
                                if (q1 != std::string::npos && q2 != std::string::npos) {
                                    out = content.substr(q1 + 1, q2 - q1 - 1);
                                }
                            }
                        }
                    };
                    extractField("nombre", agentName);
                    if (agentName.empty()) agentName = WideToUtf8(ffd.cFileName);
                    extractField("descripcion", agentDesc);
                    extractField("version", agentVersion);
                    extractField("autor", agentAuthor);
                    extractField("proveedor", agentAuthor);
                    extractField("licencia", agentLicense);
                }

                if (!first) json += L",";
                first = false;

                std::wstring wName = Utf8ToWide(agentName);
                std::wstring wDesc = Utf8ToWide(agentDesc);
                std::wstring wVersion = Utf8ToWide(agentVersion);
                std::wstring wAuthor = Utf8ToWide(agentAuthor);
                std::wstring wLicense = Utf8ToWide(agentLicense);
                std::replace(wName.begin(), wName.end(), L'"', L'\\');
                std::replace(wDesc.begin(), wDesc.end(), L'"', L'\\');
                std::replace(wVersion.begin(), wVersion.end(), L'"', L'\\');
                std::replace(wAuthor.begin(), wAuthor.end(), L'"', L'\\');
                std::replace(wLicense.begin(), wLicense.end(), L'"', L'\\');

                json += L"{\"name\":\"" + wName + L"\",\"description\":\"" + wDesc +
                        L"\",\"version\":\"" + wVersion + L"\",\"author\":\"" + wAuthor +
                        L"\",\"license\":\"" + wLicense + L"\",\"installed\":true}";
            } while (FindNextFileW(hFind, &ffd) != 0);
            FindClose(hFind);
        }
    }
    json += L"]";

    if (webview) {
        std::wstring msg = L"EVENT:AGENT_LIST|" + json;
        webview->PostWebMessageAsString(msg.c_str());
    }
}

static void uninstallAgent(const std::wstring& name, ICoreWebView2* webview) {
    std::wstring exeDir = GetExeDir();
    std::wstring agentPath = exeDir + L"\\" + AGENTS_DIR + L"\\" + name;

    if (directoryExists(agentPath)) {
        std::wstring jsonPath = agentPath + L"\\agent.json";
        DeleteFileW(jsonPath.c_str());
        RemoveDirectoryW(agentPath.c_str());
    }

    if (webview) {
        listAgents(webview);
    }
}

static void handleWebMessage(HWND hwnd, ICoreWebView2* webview, const std::wstring& msg);
static void openSettingsWindow();
static void openAgentManagerWindow();

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
    WebMessageHandler(HWND hwnd, ICoreWebView2* webview)
        : m_hwnd(hwnd), m_webview(webview), m_refCount(1) {}

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
            handleWebMessage(m_hwnd, m_webview, msg);
        }
        return S_OK;
    }

    void SendMessage(const std::wstring& message) {
        if (m_webview) {
            m_webview->PostWebMessageAsString(message.c_str());
        }
    }

private:
    HWND m_hwnd;
    ICoreWebView2* m_webview;
    LONG m_refCount;
};

class ControllerHandlerMain : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
public:
    explicit ControllerHandlerMain(HWND hwnd) : m_hwnd(hwnd), m_refCount(1) {}

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
            g_mainController = controller;
            controller->AddRef();
            controller->get_CoreWebView2(&g_mainWebview);

            WebMessageHandler* msgHandler = new WebMessageHandler(m_hwnd, g_mainWebview);
            EventRegistrationToken token;
            g_mainWebview->add_WebMessageReceived(msgHandler, &token);

            controller->put_IsVisible(TRUE);
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            controller->put_Bounds(rc);

            std::wstring url = L"file:///" + GetExeDir() + L"\\ui\\index.html";
            g_mainWebview->Navigate(url.c_str());

            PostMessageW(m_hwnd, WM_APP + 2, 0, 0);
        }
        return S_OK;
    }

private:
    HWND m_hwnd;
    LONG m_refCount;
};

class ControllerHandlerSecondary : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
public:
    ControllerHandlerSecondary(HWND hwnd, const std::wstring& url)
        : m_hwnd(hwnd), m_url(url), m_refCount(1) {}

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
            controller->AddRef();
            ICoreWebView2* webview = nullptr;
            controller->get_CoreWebView2(&webview);

            WebMessageHandler* msgHandler = new WebMessageHandler(m_hwnd, webview);
            EventRegistrationToken token;
            webview->add_WebMessageReceived(msgHandler, &token);

            controller->put_IsVisible(TRUE);
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            controller->put_Bounds(rc);

            webview->Navigate(m_url.c_str());

            webview->Release();
            controller->Release();
        }
        return S_OK;
    }

private:
    HWND m_hwnd;
    std::wstring m_url;
    LONG m_refCount;
};

static void handleWebMessage(HWND hwnd, ICoreWebView2* webview, const std::wstring& msg) {
    if (msg.find(L"SAVE_CONFIG") == 0) {
        SaveConfig();
        return;
    }
    if (msg.find(L"READ_DIR") == 0) {
        std::wstring path = msg.substr(8);
        enumerateDirectory(hwnd, webview, path);
        return;
    }
    if (msg.find(L"READ_FILE") == 0) {
        std::wstring path = msg.substr(9);
        std::string content = readFileUtf8(path);
        DWORD encodeLen = 0;
        CryptBinaryToStringA((const BYTE*)content.data(), (DWORD)content.size(),
                             CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &encodeLen);
        std::string b64(encodeLen > 0 ? encodeLen - 1 : 0, 0);
        if (encodeLen > 0) {
            CryptBinaryToStringA((const BYTE*)content.data(), (DWORD)content.size(),
                                 CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &b64[0], &encodeLen);
            b64.resize(encodeLen - 1);
        }
        if (webview) {
            std::wstring wideEncoded = Utf8ToWide(b64);
            std::wstring wmsg = L"EVENT:FILE_CONTENT|" + path + L"|" + wideEncoded;
            webview->PostWebMessageAsString(wmsg.c_str());
        }
        return;
    }
    if (msg.find(L"WRITE_FILE") == 0) {
        size_t sep = msg.find(L'|', 10);
        if (sep != std::wstring::npos) {
            std::wstring path = msg.substr(10, sep - 10);
            std::wstring encodedW = msg.substr(sep + 1);
            std::string encoded = WideToUtf8(encodedW);
            DWORD outLen = 0;
            CryptStringToBinaryA(encoded.c_str(), (DWORD)encoded.size(), CRYPT_STRING_BASE64,
                                 nullptr, &outLen, nullptr, nullptr);
            std::string decoded(outLen, 0);
            CryptStringToBinaryA(encoded.c_str(), (DWORD)encoded.size(), CRYPT_STRING_BASE64,
                                 (BYTE*)decoded.data(), &outLen, nullptr, nullptr);
            decoded.resize(outLen);
            if (writeFileUtf8(path, decoded)) {
                if (webview) {
                    webview->PostWebMessageAsString((L"EVENT:FILE_SAVED|" + path).c_str());
                }
            } else {
                if (webview) {
                    webview->PostWebMessageAsString((L"EVENT:FILE_ERROR|" + path + L"|write_failed").c_str());
                }
            }
        }
        return;
    }
    if (msg == L"PICK_FOLDER") {
        pickFolder(hwnd, webview);
        return;
    }
    if (msg.find(L"ADD_PROJECT") == 0) {
        std::wstring path = msg.substr(10);
        addProject(path, webview);
        return;
    }
    if (msg.find(L"REMOVE_PROJECT") == 0) {
        std::wstring path = msg.substr(14);
        removeProject(path, webview);
        return;
    }
    if (msg == L"LIST_PROJECTS") {
        if (webview) {
            std::wstring json = L"[";
            for (size_t i = 0; i < g_projects.size(); i++) {
                std::wstring name = g_projects[i];
                size_t lastSlash = name.find_last_of(L"\\/");
                if (lastSlash != std::wstring::npos) name = name.substr(lastSlash + 1);
                std::replace(name.begin(), name.end(), L'"', L'\\');
                json += L"{\"path\":\"" + g_projects[i] + L"\",\"name\":\"" + name + L"\"}";
                if (i + 1 < g_projects.size()) json += L",";
            }
            json += L"]";
            std::wstring wmsg = L"EVENT:PROJECTS_LIST|" + json;
            webview->PostWebMessageAsString(wmsg.c_str());
        }
        return;
    }
    if (msg.find(L"SET_CURRENT_PROJECT") == 0) {
        std::wstring path = msg.substr(19);
        g_currentProject = path;
        SaveConfig();
        return;
    }
    if (msg.find(L"SET_WINDOW") == 0) {
        std::wstring rest = msg.substr(9);
        size_t sep1 = rest.find(L'|');
        int mode = 1;
        int w = 0, h = 0;
        if (sep1 != std::wstring::npos) {
            mode = std::stoi(rest.substr(0, sep1));
            std::wstring sizePart = rest.substr(sep1 + 1);
            size_t sep2 = sizePart.find(L'|');
            if (sep2 != std::wstring::npos) {
                w = std::stoi(sizePart.substr(0, sep2));
                h = std::stoi(sizePart.substr(sep2 + 1));
            }
        } else {
            mode = std::stoi(rest);
        }
        setWindowMode(mode, w, h);
        return;
    }
    if (msg == L"AGENT_LIST") {
        listAgents(webview);
        return;
    }
    if (msg.find(L"AGENT_UNINSTALL") == 0) {
        std::wstring name = msg.substr(16);
        uninstallAgent(name, webview);
        return;
    }
    if (msg == L"OPEN_SETTINGS") {
        openSettingsWindow();
        return;
    }
    if (msg == L"OPEN_AGENT_MANAGER") {
        openAgentManagerWindow();
        return;
    }
}

static LRESULT CALLBACK WindowProcMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            g_hwndMain = hwnd;
            g_hInstance = ((LPCREATESTRUCTW)lParam)->hInstance;

            InitCommonControls();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

            BuildObfuscateMap();
            if (!LoadConfig()) {
                LOG_WARNING("Archivo de configuracion no encontrado. Usando valores por defecto.");
                g_panelLeft = 280; g_panelRight = 350;
                g_panelCenter = 600; g_panelCenterTop = 400;
                g_panelCenterBottom = 160; g_yamaVisible = true;
                g_windowMode = 1; g_customWidth = 1200; g_customHeight = 800;
            } else {
                LOG_INFO("Configuracion cargada.");
            }
            LoadProjects();
            LOG_INFO("Proyectos cargados.");

            if (g_windowMode == 2) {
                ShowWindow(hwnd, SW_MAXIMIZE);
                SetWindowLongW(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                             GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_SHOWWINDOW);
            } else if (g_windowMode == 3) {
                ShowWindow(hwnd, SW_RESTORE);
                SetWindowPos(hwnd, HWND_TOP, 0, 0, g_customWidth, g_customHeight,
                             SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
            } else if (g_windowMode == 0) {
                ShowWindow(hwnd, SW_RESTORE);
            } else {
                ShowWindow(hwnd, SW_MAXIMIZE);
            }

            EnvironmentHandler* envHandler = new EnvironmentHandler(hwnd);
            HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
                nullptr, nullptr, nullptr, envHandler);
            if (FAILED(hr)) {
                LOG_WINERR("ERROR", "No se pudo crear el entorno de WebView2");
                OutputDebugStringW(L"Failed to create WebView2 environment\n");
            } else {
                LOG_INFO("Entorno de WebView2 inicializado.");
            }
            return 0;
        }
        case WM_APP + 1: {
            ControllerHandlerMain* ctrlHandler = new ControllerHandlerMain(hwnd);
            g_webviewEnv->CreateCoreWebView2Controller(hwnd, ctrlHandler);
            LOG_INFO("Controlador de WebView2 creado.");
            return 0;
        }
        case WM_APP + 2: {
            WebMessageHandler* msgHandler = new WebMessageHandler(hwnd, g_mainWebview);
            EventRegistrationToken token;
            g_mainWebview->add_WebMessageReceived(msgHandler, &token);
            return 0;
        }
        case WM_APP + 3: {
            return 0;
        }
        case WM_SIZE: {
            if (g_mainController) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                g_mainController->put_Bounds(rc);
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
            SaveProjects();
            LOG_INFO("Configuracion y proyectos guardados.");
            if (g_mainController) {
                g_mainController->put_IsVisible(FALSE);
                g_mainController->Release();
                g_mainController = nullptr;
            }
            if (g_mainWebview) {
                g_mainWebview->Release();
                g_mainWebview = nullptr;
            }
            if (g_webviewEnv) {
                g_webviewEnv->Release();
                g_webviewEnv = nullptr;
            }
            CoUninitialize();
            LOG_INFO("Ventana principal cerrada.");
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK WindowProcSecondary(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            g_hInstance = ((LPCREATESTRUCTW)lParam)->hInstance;
            InitCommonControls();
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            BuildObfuscateMap();
            LOG_INFO("Ventana secundaria creada.");

            EnvironmentHandler* envHandler = new EnvironmentHandler(hwnd);
            HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
                nullptr, nullptr, nullptr, envHandler);
            if (FAILED(hr)) {
                LOG_WINERR("ERROR", "No se pudo crear el entorno de WebView2 (secundario)");
                OutputDebugStringW(L"Failed to create WebView2 environment (secondary)\n");
            }
            return 0;
        }
        case WM_APP + 1: {
            auto it = g_secondaryUrls.find(hwnd);
            std::wstring url;
            if (it != g_secondaryUrls.end()) {
                url = it->second;
                g_secondaryUrls.erase(it);
            }
            ControllerHandlerSecondary* ctrlHandler = new ControllerHandlerSecondary(hwnd, url);
            g_webviewEnv->CreateCoreWebView2Controller(hwnd, ctrlHandler);
            return 0;
        }
        case WM_SIZE:
            return 0;
        case WM_DESTROY:
            LOG_INFO("Ventana secundaria cerrada.");
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static void createSecondaryWindow(const wchar_t* className, const wchar_t* title,
                                   const std::wstring& url, int w, int h) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProcSecondary;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(20, 20, 20));
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, className, title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,
        nullptr, nullptr, g_hInstance, nullptr
    );
    if (hwnd) {
        g_secondaryUrls[hwnd] = url;
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}

static void openSettingsWindow() {
    LOG_INFO("Abriendo ventana de configuracion.");
    createSecondaryWindow(L"NebulaSettingsClass", L"Nebula Settings",
                          L"file:///" + GetExeDir() + L"\\ui\\settings.html",
                          500, 450);
}

static void openAgentManagerWindow() {
    LOG_INFO("Abriendo administrador de agentes.");
    createSecondaryWindow(L"NebulaAgentManagerClass", L"Nebula Agent Manager",
                          L"file:///" + GetExeDir() + L"\\ui\\agentmanager.html",
                          700, 500);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SetUnhandledExceptionFilter(NebulaUnhandledExceptionFilter);

    if (!CheckSingleInstance()) {
        LOG_ERROR("No se pudo obtener el mutex de instancia unica. Saliendo.");
        return 1;
    }

    LOG_INFO("NebulaDeskopCODE iniciando.");
    LOG_INFO("Comprobando instancia unica.");
    LOG_INFO("Inicializando interfaz.");

    const wchar_t CLASS_NAME[] = L"NebulaDeskopCODE";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProcMain;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(20, 20, 20));

    if (!RegisterClassW(&wc)) {
        LOG_WINERR("CRITICAL", "No se pudo registrar la clase de ventana principal");
        ReleaseSingleInstance();
        return 1;
    }
    LOG_INFO("Clase de ventana principal registrada.");

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"NebulaDeskopCODE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd == nullptr) {
        LOG_WINERR("CRITICAL", "No se pudo crear la ventana principal");
        ReleaseSingleInstance();
        return 1;
    }

    LOG_INFO("Ventana principal creada.");
    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    LOG_INFO("Aplicacion cerrada correctamente.");
    ReleaseSingleInstance();

    DeleteObject(wc.hbrBackground);
    return 0;
}

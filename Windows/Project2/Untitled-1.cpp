// Project2.cpp

#include "framework.h"
#include "Project2.h"

#include <oleacc.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cwctype>

#pragma comment(lib, "Oleacc.lib")

#define MAX_LOADSTRING 100

// ---------------- Layout constants ----------------
#define WINDOW_HEIGHT   50
#define WINDOW_MAX_WIDTH 500

#define BTN_WIDTH   80
#define BTN_HEIGHT  28
#define BTN_MARGIN  5

#define BTN_BASE_ID 4000
#define WM_UPDATE_CARET (WM_APP + 1)
#define WM_UPDATE_SUGGESTIONS (WM_APP + 2)

// ---------------- Globals ----------------
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND g_hMainWnd = nullptr;
HWND g_hScrollWnd = nullptr;
HWND g_lastEditorWnd = nullptr;

HWINEVENTHOOK g_hCaretHook = nullptr;
HHOOK g_hKeyboardHook = nullptr;
RECT g_caretRect = { 0 };

int g_scrollX = 0;
int g_contentWidth = 0;

std::vector<std::wstring> g_dictionary =
{
    L"the", L"be", L"to", L"of", L"and", L"a", L"in", L"that", L"have", L"I",
    L"it", L"for", L"not", L"on", L"with", L"he", L"as", L"you", L"do", L"at",
};

std::unordered_map<int, std::wstring> g_buttonMap;
std::vector<HWND> g_buttons;
std::wstring g_inputBuffer;

// ---------------- Forward declarations ----------------
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK CaretWinEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);

void SendTextToEditor(const wchar_t* text);
void ClampToWorkArea(int& x, int& y, int w, int h);

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void RebuildButtons(const std::vector<std::wstring>& words);
std::vector<std::wstring> GetSuggestions(const std::wstring& prefix);

// =====================================================
// Entry point
// =====================================================
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT2, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    CoInitialize(nullptr);

    g_hCaretHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,
        EVENT_OBJECT_LOCATIONCHANGE,
        nullptr,
        CaretWinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    // Install low-level keyboard hook to capture typed characters and update suggestions.
    g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hCaretHook) UnhookWinEvent(g_hCaretHook);
    if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
    CoUninitialize();
    return (int)msg.wParam;
}

// =====================================================
// Register window class
// =====================================================
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = szWindowClass;
    return RegisterClassExW(&wcex);
}

// =====================================================
// Create window + dynamic buttons
// =====================================================
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    g_hMainWnd = CreateWindowW(
        szWindowClass, szTitle,
        WS_POPUP | WS_BORDER,
        0, 0, 300, WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr);

    if (!g_hMainWnd) return FALSE;

    // Scroll container (kept for potential scrollbar visual; buttons are created as children of main window
    // so WM_COMMAND arrives in main window)
    g_hScrollWnd = CreateWindowW(
        L"STATIC", nullptr,
        WS_CHILD | WS_VISIBLE | WS_HSCROLL,
        0, 0, 300, WINDOW_HEIGHT,
        g_hMainWnd, nullptr, hInstance, nullptr);

    // Initially build buttons from full dictionary
    RebuildButtons(g_dictionary);

    ShowWindow(g_hMainWnd, nCmdShow);
    return TRUE;
}

// =====================================================
// Window procedure
// =====================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_UPDATE_CARET:
    {
        int width = min(g_contentWidth, WINDOW_MAX_WIDTH);
        int x = g_caretRect.left;
        int y = g_caretRect.bottom + 6;

        ClampToWorkArea(x, y, width, WINDOW_HEIGHT);

        SetWindowPos(hWnd, HWND_TOPMOST,
            x, y, width, WINDOW_HEIGHT,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);

        SetWindowPos(g_hScrollWnd, nullptr,
            0, 0, width, WINDOW_HEIGHT,
            SWP_NOZORDER);
    }
    break;

    case WM_UPDATE_SUGGESTIONS:
    {
        // Generate suggestions based on g_inputBuffer and rebuild buttons.
        std::vector<std::wstring> suggestions = GetSuggestions(g_inputBuffer);
        RebuildButtons(suggestions);
    }
    break;

    case WM_HSCROLL:
    {
        int code = LOWORD(wParam);
        SCROLLINFO si{ sizeof(si), SIF_ALL };
        GetScrollInfo(g_hScrollWnd, SB_HORZ, &si);

        if (code == SB_THUMBTRACK)
            g_scrollX = si.nTrackPos;
        else if (code == SB_LINELEFT)
            g_scrollX -= 20;
        else if (code == SB_LINERIGHT)
            g_scrollX += 20;

        SetScrollPos(g_hScrollWnd, SB_HORZ, g_scrollX, TRUE);
        // ScrollWindow was targeting g_hScrollWnd previously where buttons were children.
        // Since buttons are now children of the main window, scroll the main window so buttons visually move.
        ScrollWindow(hWnd, -g_scrollX, 0, nullptr, nullptr);
        UpdateWindow(hWnd);
    }
    break;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        if (g_buttonMap.count(id))
            SendTextToEditor((g_buttonMap[id] + L" ").c_str());
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// =====================================================
// Caret tracking
// =====================================================
void CALLBACK CaretWinEventProc(
    HWINEVENTHOOK, DWORD event, HWND hwnd,
    LONG idObject, LONG, DWORD, DWORD)
{
    if (event != EVENT_OBJECT_LOCATIONCHANGE || idObject != OBJID_CARET)
        return;

    // When caret changes, reset input buffer if the target window changed.
    if (g_lastEditorWnd != hwnd)
        g_inputBuffer.clear();

    g_lastEditorWnd = hwnd;

    IAccessible* acc = nullptr;
    VARIANT var{};
    if (FAILED(AccessibleObjectFromEvent(hwnd, idObject, 0, &acc, &var)))
        return;

    long x, y, w, h;
    if (SUCCEEDED(acc->accLocation(&x, &y, &w, &h, var)))
    {
        g_caretRect = { x, y, x + w, y + h };
        PostMessage(g_hMainWnd, WM_UPDATE_CARET, 0, 0);
    }
    acc->Release();
}

// =====================================================
// Low-level keyboard hook - capture typed characters and build a small prefix buffer.
// =====================================================
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        // Only gather input if we have a tracked editor caret.
        if (g_lastEditorWnd)
        {
            KBDLLHOOKSTRUCT* ks = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vk = ks->vkCode;

            // Handle backspace
            if (vk == VK_BACK)
            {
                if (!g_inputBuffer.empty())
                {
                    g_inputBuffer.pop_back();
                    PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
                }
            }
            // Clear buffer on space/enter/tab (user finished a word)
            else if (vk == VK_SPACE || vk == VK_RETURN || vk == VK_TAB)
            {
                if (!g_inputBuffer.empty())
                {
                    g_inputBuffer.clear();
                    PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
                }
            }
            else
            {
                // Try to translate VK to unicode character (respecting current keyboard state)
                BYTE keyState[256];
                if (GetKeyboardState(keyState))
                {
                    wchar_t buf[8] = {};
                    // Use ToUnicodeEx to get the character
                    HKL layout = GetKeyboardLayout(0);
                    int rc = ToUnicodeEx((UINT)vk, ks->scanCode, keyState, buf, _countof(buf), 0, layout);
                    if (rc > 0)
                    {
                        // Append only printable characters (letters/digits/punct)
                        wchar_t ch = buf[0];
                        if (!iswcntrl(ch))
                        {
                            g_inputBuffer.push_back(ch);
                            // Convert buffer to lowercase for consistent suggestion behavior
                            std::transform(g_inputBuffer.begin(), g_inputBuffer.end(), g_inputBuffer.begin(), ::towlower);
                            PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
                        }
                    }
                }
            }
        }
    }

    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

// =====================================================
// Build / rebuild buttons for given words.
// Destroys previous buttons and creates new ones as children of g_hMainWnd.
// =====================================================
void RebuildButtons(const std::vector<std::wstring>& words)
{
    // Destroy old buttons
    for (HWND b : g_buttons)
    {
        if (IsWindow(b))
            DestroyWindow(b);
    }
    g_buttons.clear();
    g_buttonMap.clear();

    int x = BTN_MARGIN;
    int y = BTN_MARGIN;

    for (size_t i = 0; i < words.size(); ++i)
    {
        int id = BTN_BASE_ID + (int)i;
        HWND btn = CreateWindowW(
            L"BUTTON",
            words[i].c_str(),
            WS_CHILD | WS_VISIBLE,
            x, y, BTN_WIDTH, BTN_HEIGHT,
            g_hMainWnd,
            (HMENU)id,
            hInst, nullptr);

        if (btn)
        {
            g_buttons.push_back(btn);
            g_buttonMap[id] = words[i];
        }

        x += BTN_WIDTH + BTN_MARGIN;
    }

    g_contentWidth = x;

    // Update scroll info on g_hScrollWnd
    SCROLLINFO si{};
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = g_contentWidth;
    si.nPage = WINDOW_MAX_WIDTH;
    SetScrollInfo(g_hScrollWnd, SB_HORZ, &si, TRUE);

    // Force a layout/paint
    InvalidateRect(g_hMainWnd, nullptr, TRUE);
    UpdateWindow(g_hMainWnd);
}

// =====================================================
// Suggestion matching (case-insensitive prefix match).
// Returns up to 16 matches (tunable).
// =====================================================
std::vector<std::wstring> GetSuggestions(const std::wstring& prefix)
{
    std::vector<std::wstring> results;

    if (prefix.empty())
    {
        // When no prefix, return the full dictionary (or a limited set).
        results = g_dictionary;
        return results;
    }

    std::wstring lowerPrefix = prefix;
    std::transform(lowerPrefix.begin(), lowerPrefix.end(), lowerPrefix.begin(), ::towlower);

    for (const auto& w : g_dictionary)
    {
        std::wstring lw = w;
        std::transform(lw.begin(), lw.end(), lw.begin(), ::towlower);
        if (lw.size() >= lowerPrefix.size() && std::equal(lowerPrefix.begin(), lowerPrefix.end(), lw.begin()))
        {
            results.push_back(w);
            if (results.size() >= 16) break;
        }
    }

    // If no prefix matches, optionally return best-effort matches (substring)
    if (results.empty())
    {
        for (const auto& w : g_dictionary)
        {
            std::wstring lw = w;
            std::transform(lw.begin(), lw.end(), lw.begin(), ::towlower);
            if (lw.find(lowerPrefix) != std::wstring::npos)
            {
                results.push_back(w);
                if (results.size() >= 16) break;
            }
        }
    }

    return results;
}

// =====================================================
// Send text to editor
// =====================================================
void SendTextToEditor(const wchar_t* text)
{
    if (!g_lastEditorWnd || !text || !*text)
        return;

    // Allow foreground switch
    AllowSetForegroundWindow(ASFW_ANY);

    DWORD targetThread = GetWindowThreadProcessId(g_lastEditorWnd, nullptr);
    DWORD thisThread = GetCurrentThreadId();

    AttachThreadInput(thisThread, targetThread, TRUE);
    SetForegroundWindow(g_lastEditorWnd);
    SetFocus(g_lastEditorWnd);

    std::vector<INPUT> inputs;
    inputs.reserve(wcslen(text) * 2);

    for (const wchar_t* p = text; *p; ++p)
    {
        INPUT down{};
        down.type = INPUT_KEYBOARD;
        down.ki.wVk = 0;
        down.ki.wScan = *p;
        down.ki.dwFlags = KEYEVENTF_UNICODE;

        INPUT up = down;
        up.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs.push_back(down);
        inputs.push_back(up);
    }

    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));

    AttachThreadInput(thisThread, targetThread, FALSE);

    // After inserting a suggestion, clear input buffer (word completed)
    g_inputBuffer.clear();
    PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
}

// =====================================================
// Clamp window to screen
// =====================================================
void ClampToWorkArea(int& x, int& y, int w, int h)
{
    HMONITOR m = MonitorFromPoint({ x, y }, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfo(m, &mi);

    RECT r = mi.rcWork;
    if (x < r.left) x = r.left;
    if (y < r.top) y = r.top;
    if (x + w > r.right) x = r.right - w;
    if (y + h > r.bottom) y = r.bottom - h;
}
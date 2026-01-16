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
#define WINDOW_MAX_WIDTH 600

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
    L"this", L"but", L"his", L"by", L"from", L"they", L"we", L"say", L"her", L"she",
    L"or", L"an", L"will", L"my", L"one", L"all", L"would", L"there", L"their", L"what",
    L"so", L"up", L"out", L"if", L"about", L"who", L"get", L"which", L"go", L"me",
    L"when", L"make", L"can", L"like", L"time", L"no", L"just", L"him", L"know", L"take",
    L"people", L"into", L"year", L"your", L"good", L"some", L"could", L"them", L"see", L"other",
    L"than", L"then", L"now", L"look", L"only", L"come", L"its", L"over", L"think", L"also",
    L"back", L"after", L"use", L"two", L"how", L"our", L"work", L"first", L"well", L"way",
    L"even", L"new", L"want", L"because", L"any", L"these", L"give", L"day", L"most", L"us",

    L"house", L"book", L"car", L"tree", L"computer", L"phone", L"window", L"door", L"table", L"chair",
    L"water", L"food", L"music", L"movie", L"city", L"country", L"school", L"student", L"teacher", L"friend",
    L"family", L"child", L"mother", L"father", L"sister", L"brother", L"dog", L"cat", L"animal", L"sun",
    L"moon", L"star", L"sky", L"earth", L"sea", L"river", L"mountain", L"road", L"street", L"shop",
    L"market", L"store", L"price", L"money", L"bank", L"office", L"company", L"business", L"job", L"workday",
    L"project", L"program", L"code", L"developer", L"engineer", L"design", L"idea", L"plan", L"system", L"engine",
    L"network", L"signal", L"data", L"file", L"folder", L"image", L"picture", L"sound", L"voice", L"language",
    L"word", L"sentence", L"paragraph", L"article", L"report", L"research", L"science", L"history", L"nature", L"art",
    L"culture", L"travel", L"holiday", L"vacation", L"hotel", L"airport", L"ticket", L"journey", L"adventure", L"map",
    L"guide", L"story", L"author", L"reader", L"page", L"index", L"chapter", L"title", L"subject", L"topic",

    L"question", L"answer", L"help", L"support", L"service", L"product", L"feature", L"update", L"release", L"version",
    L"install", L"setup", L"config", L"option", L"preference", L"setting", L"control", L"button", L"menu", L"dialog",
    L"window", L"windowed", L"fullscreen", L"display", L"monitor", L"keyboard", L"mouse", L"pointer", L"cursor", L"selection",
    L"highlight", L"search", L"find", L"replace", L"paste", L"copy", L"cut", L"undo", L"redo", L"save",
    L"load", L"open", L"close", L"start", L"stop", L"run", L"execute", L"build", L"compile", L"link",
    L"deploy", L"test", L"debug", L"trace", L"log", L"error", L"exception", L"crash", L"fix", L"patch",
    L"release", L"versioning", L"ci", L"cd", L"pipeline", L"agent", L"runner", L"orchestrator", L"task", L"workflow",
    L"user", L"account", L"profile", L"login", L"logout", L"password", L"token", L"session", L"permission", L"access",
    L"security", L"privacy", L"encrypt", L"decrypt", L"key", L"certificate", L"firewall", L"virus", L"malware", L"backup",
    L"restore", L"archive", L"compress", L"extract", L"transfer", L"upload", L"download", L"sync", L"synchronize", L"share",

    L"connect", L"disconnect", L"online", L"offline", L"remote", L"local", L"cloud", L"server", L"client", L"host",
    L"port", L"protocol", L"http", L"https", L"ftp", L"smtp", L"pop", L"dns", L"ip", L"address",
    L"route", L"gateway", L"subnet", L"mask", L"broadcast", L"packet", L"socket", L"stream", L"buffer", L"cache",
    L"memory", L"cpu", L"processor", L"core", L"thread", L"process", L"task", L"queue", L"stack", L"heap",
    L"pointer", L"reference", L"variable", L"constant", L"macro", L"template", L"class", L"object", L"struct", L"enum",
    L"interface", L"method", L"function", L"callback", L"event", L"handler", L"delegate", L"lambda", L"operator", L"expression",
    L"condition", L"loop", L"iteration", L"recursion", L"algorithm", L"complexity", L"optimize", L"performance", L"latency", L"throughput",
    L"bandwidth", L"frequency", L"noise", L"filter", L"sensor", L"actuator", L"device", L"hardware", L"firmware", L"driver",
    L"usb", L"bluetooth", L"wifi", L"ethernet", L"wireless", L"wired", L"battery", L"power", L"charge", L"voltage",

    L"current", L"circuit", L"resist", L"capacitor", L"inductor", L"transistor", L"diode", L"led", L"displaypanel", L"graphics",
    L"video", L"render", L"shader", L"texture", L"pixel", L"resolution", L"aspect", L"ratio", L"color", L"red",
    L"green", L"blue", L"alpha", L"contrast", L"brightness", L"saturation", L"tone", L"gradient", L"shadow", L"border",
    L"margin", L"padding", L"layout", L"grid", L"column", L"row", L"align", L"center", L"justify", L"bold",
    L"italic", L"underline", L"font", L"type", L"size", L"scale", L"zoom", L"rotate", L"translate", L"move",
    L"drag", L"drop", L"click", L"doubleclick", L"rightclick", L"hover", L"focus", L"blur", L"active", L"inactive",
    L"visible", L"hidden", L"transparent", L"opaque", L"animate", L"transition", L"transform", L"duration", L"delay", L"loop",

    L"loopback", L"benchmark", L"throughputtest", L"stress", L"load", L"concurrency", L"parallel", L"synchronous", L"asynchronous", L"callbackhell",
    L"promise", L"future", L"await", L"async", L"threadpool", L"mutex", L"semaphore", L"lock", L"deadlock", L"starvation",
    L"priority", L"scheduler", L"timer", L"tick", L"heartbeat", L"monitorproc", L"profiler", L"tracer", L"inspector", L"debugger",
    L"breakpoint", L"watch", L"variableview", L"stacktrace", L"dump", L"corefile", L"regression", L"unit", L"integration", L"functional",
    L"acceptance", L"smoke", L"sanity", L"cihelper", L"cdbuild", L"pipelineagent", L"buildbot", L"agent", L"runner", L"orchestrator",
    L"repository", L"branch", L"commit", L"merge", L"rebase", L"pull", L"push", L"clone", L"fork", L"issue",
    L"ticket", L"milestone", L"releaseplan", L"sprint", L"kanban", L"scrum", L"retrospective", L"standup", L"planning", L"grooming",
    L"roadmap", L"vision", L"strategy", L"objective", L"keyresult", L"okr", L"stakeholder", L"owner", L"manager", L"lead",
    L"director", L"vp", L"cto", L"ceo", L"founder", L"entrepreneur", L"startup", L"scaleup", L"incubator", L"accelerator",

    L"mentor", L"coach", L"consultant", L"advisor", L"analyst", L"researcher", L"scientist", L"professor", L"lecturer", L"instructor",
    L"candidate", L"applicant", L"interview", L"resume", L"cv", L"portfolio", L"reference", L"recommendation", L"endorsement", L"credential",
    L"certificate", L"degree", L"diploma", L"certification", L"accreditation", L"license", L"permit", L"policy", L"procedure", L"protocol",
    L"guideline", L"standard", L"compliance", L"audit", L"quality", L"assurance", L"control", L"risk", L"mitigation", L"governance",
    L"ethics", L"privacypolicy", L"terms", L"agreement", L"contract", L"nda", L"licensing", L"trademark", L"patent", L"copyright",
    L"licensee", L"licensor", L"vendor", L"supplier", L"partner", L"distributor", L"reseller", L"channel", L"marketplace", L"ecosystem",
    L"community", L"forum", L"discussion", L"thread", L"comment", L"reply", L"mention", L"notification", L"subscription", L"feed",

    L"streaming", L"broadcast", L"podcast", L"webinar", L"meeting", L"conference", L"workshop", L"seminar", L"training", L"course",
    L"module", L"lesson", L"tutorial", L"guidebook", L"handbook", L"manual", L"documentation", L"docs", L"api", L"sdk",
    L"cli", L"gui", L"ux", L"ui", L"researchpaper", L"preprint", L"journal", L"conferencepaper", L"abstract", L"citation",
    L"referencebook", L"encyclopedia", L"dictionary", L"thesaurus", L"indexing", L"searchengine", L"crawler", L"spider", L"bot", L"agent",
    L"automation", L"automate", L"script", L"scriptable", L"macroable", L"plugin", L"extension", L"addon", L"integration", L"connector",
    L"adapter", L"bridge", L"gateway", L"proxy", L"middleware", L"orchestration", L"pipeline", L"workflowengine", L"scheduler", L"cron",
    L"jobrunner", L"executor", L"taskqueue", L"messagebus", L"messaging", L"pubsub", L"eventhub", L"streamprocessor", L"eventstore", L"eventlog",

    L"metrics", L"telemetry", L"observability", L"monitoring", L"alerting", L"dashboard", L"visualization", L"chart", L"graph", L"histogram",
    L"gauge", L"counter", L"sampling", L"tracing", L"otel", L"prometheus", L"grafana", L"zabbix", L"newrelic", L"datadog",
    L"elastic", L"elasticsearch", L"logstash", L"kibana", L"efk", L"elk", L"fluentd", L"collector", L"agentless", L"sidecar",
    L"container", L"docker", L"kubernetes", L"pod", L"service", L"deployment", L"replica", L"scaling", L"autoscale", L"horizontal",
    L"vertical", L"namespace", L"context", L"label", L"selector", L"ingress", L"egress", L"servicemesh", L"istio", L"linkerd",
    L"cni", L"helm", L"chart", L"operator", L"controller", L"stateful", L"stateless", L"daemonset", L"job", L"cronjob",

    L"database", L"sql", L"nosql", L"relational", L"keyvalue", L"document", L"column", L"row", L"table", L"index",
    L"schema", L"migration", L"query", L"transaction", L"isolation", L"consistency", L"availability", L"partition", L"replication", L"shard",
    L"leader", L"follower", L"primary", L"secondary", L"backup", L"restore", L"snapshot", L"wal", L"logship", L"binlog",
    L"orm", L"entity", L"dao", L"repository", L"connection", L"pool", L"cursor", L"stmt", L"prepared", L"bind",
    L"execute", L"fetch", L"commit", L"rollback", L"deadlockdetect", L"queryplan", L"optimizer", L"explain", L"analyze", L"vacuum",

    L"indexer", L"searchindex", L"fulltext", L"nlp", L"tokenize", L"stem", L"lemmatize", L"synonym", L"spellcheck", L"autocomplete",
    L"suggestion", L"classifier", L"regression", L"classification", L"clustering", L"prediction", L"model", L"training", L"inference", L"dataset",
    L"feature", L"label", L"score", L"accuracy", L"precision", L"recall", L"f1", L"crossvalidation", L"epoch", L"batch",
    L"gradient", L"descent", L"optimizer", L"loss", L"metric", L"overfitting", L"underfitting", L"regularization", L"dropout", L"embedding",

    L"neural", L"network", L"cnn", L"rnn", L"transformer", L"attention", L"encoder", L"decoder", L"bert", L"gpt",
    L"llm", L"chatbot", L"assistant", L"agentai", L"pipelineai", L"modelserver", L"inferenceengine", L"serving", L"onnx", L"tensorflow",
    L"pytorch", L"keras", L"sklearn", L"scipy", L"numpy", L"pandas", L"matplotlib", L"seaborn", L"plot", L"visual",
    L"analysis", L"eda", L"statistics", L"probability", L"bayes", L"montecarlo", L"simulation", L"optimization", L"linear", L"algebra",

    L"matrix", L"vector", L"tensor", L"calculus", L"differential", L"integral", L"derivative", L"functionmath", L"equation", L"systemmath",
    L"graphalgo", L"dfs", L"bfs", L"dijkstra", L"astar", L"bellman", L"ford", L"kruskal", L"prims", L"unionfind",
    L"hash", L"map", L"set", L"multiset", L"queue", L"deque", L"stack", L"heapdata", L"priorityqueue", L"treemap",
    L"bst", L"avl", L"redblack", L"trie", L"suffix", L"prefix", L"kmp", L"boyermoore", L"rabinkarp", L"rollinghash",

    L"encryption", L"aes", L"rsa", L"ecc", L"sha", L"md5", L"hmac", L"jwt", L"oauth", L"openid",
    L"authorization", L"authentication", L"sspi", L"kerberos", L"ldap", L"active", L"directory", L"cas", L"saml", L"mfa",
    L"otp", L"sms", L"email", L"push", L"webauthn", L"biometric", L"fingerprint", L"facial", L"retina", L"voiceid",

    L"localization", L"l10n", L"internationalization", L"i18n", L"translation", L"plural", L"currency", L"timezone", L"locale", L"utf8",
    L"unicode", L"encoding", L"charset", L"bom", L"newline", L"crlf", L"lf", L"eol", L"mime", L"httpheader",
    L"contenttype", L"json", L"xml", L"yaml", L"csv", L"toml", L"ini", L"properties", L"protobuf", L"avro",
    L"thrift", L"graphql", L"rest", L"rpc", L"grpc", L"soap", L"ws", L"wsdl", L"swagger", L"openapi",

    L"mock", L"stub", L"fixture", L"assert", L"expect", L"verify", L"spy", L"coverage", L"report", L"badge",
    L"changelog", L"readme", L"license", L"contributing", L"codeofconduct", L"styleguide", L"formatter", L"linter", L"precommit", L"hook",
    L"gitignore", L"dockerignore", L"env", L"dotenv", L"secrets", L"vault", L"kms", L"iam", L"rbac", L"abac",
    L"policyengine", L"opa", L"regulation", L"gdpr", L"ccpa", L"hipaa", L"pci", L"sox", L"complianceaudit", L"certify"
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
// Create window (hidden at first!)
// =====================================================
BOOL InitInstance(HINSTANCE hInstance, int)
{
    hInst = hInstance;

    g_hMainWnd = CreateWindowW(
        szWindowClass, szTitle,
        WS_POPUP | WS_BORDER,
        0, 0, 300, WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr);

    if (!g_hMainWnd)
        return FALSE;

    // Do NOT show window now — keep hidden until typing begins
    ShowWindow(g_hMainWnd, SW_HIDE);

    // Build initial full dictionary buttons (but window is hidden)
    RebuildButtons(g_dictionary);

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
        // If no typing yet → remain hidden
        if (g_inputBuffer.empty())
        {
            ShowWindow(g_hMainWnd, SW_HIDE);
            break;
        }

        int width = min(g_contentWidth, WINDOW_MAX_WIDTH);

        int x = g_caretRect.left;
        int y = g_caretRect.bottom + 6;
        ClampToWorkArea(x, y, width, WINDOW_HEIGHT);

        SetWindowPos(hWnd, HWND_TOPMOST,
            x, y, width, WINDOW_HEIGHT,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    break;

    case WM_UPDATE_SUGGESTIONS:
    {
        std::vector<std::wstring> suggestions = GetSuggestions(g_inputBuffer);
        RebuildButtons(suggestions);

        if (g_inputBuffer.empty())
            ShowWindow(g_hMainWnd, SW_HIDE);
        else
            ShowWindow(g_hMainWnd, SW_SHOWNOACTIVATE);
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
// Keyboard hook: build prefix buffer
// =====================================================
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        if (g_lastEditorWnd)
        {
            KBDLLHOOKSTRUCT* ks = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vk = ks->vkCode;

            if (vk == VK_BACK)
            {
                if (!g_inputBuffer.empty())
                {
                    g_inputBuffer.pop_back();
                    PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
                }
            }
            else if (vk == VK_SPACE || vk == VK_RETURN || vk == VK_TAB)
            {
                g_inputBuffer.clear();
                PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
            }
            else
            {
                BYTE keyState[256];
                if (GetKeyboardState(keyState))
                {
                    wchar_t buf[8] = {};
                    HKL layout = GetKeyboardLayout(0);
                    int rc = ToUnicodeEx(vk, ks->scanCode, keyState, buf, _countof(buf), 0, layout);

                    if (rc > 0)
                    {
                        wchar_t ch = buf[0];
                        if (!iswcntrl(ch))
                        {
                            g_inputBuffer.push_back(ch);
                            std::transform(g_inputBuffer.begin(), g_inputBuffer.end(),
                                g_inputBuffer.begin(), ::towlower);
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
// Build buttons
// =====================================================
void RebuildButtons(const std::vector<std::wstring>& words)
{
    for (HWND b : g_buttons)
    {
        if (IsWindow(b))
            DestroyWindow(b);
    }
    g_buttons.clear();
    g_buttonMap.clear();

    int x = BTN_MARGIN;
    int y = BTN_MARGIN;

    if(words.empty())
    {
        g_contentWidth = 0;
        InvalidateRect(g_hMainWnd, nullptr, TRUE);
        UpdateWindow(g_hMainWnd);
		ShowWindow(g_hMainWnd, SW_HIDE);
        return;
	}

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

    InvalidateRect(g_hMainWnd, nullptr, TRUE);
    UpdateWindow(g_hMainWnd);
}

// =====================================================
// Suggestion logic
// =====================================================
std::vector<std::wstring> GetSuggestions(const std::wstring& prefix)
{
    std::vector<std::wstring> results;

    if (prefix.empty())
        return {};

    std::wstring lowerPrefix = prefix;
    std::transform(lowerPrefix.begin(), lowerPrefix.end(),
        lowerPrefix.begin(), ::towlower);

    for (const auto& w : g_dictionary)
    {
        std::wstring lw = w;
        std::transform(lw.begin(), lw.end(), lw.begin(), ::towlower);

        if (lw.rfind(lowerPrefix, 0) == 0)
        {
            results.push_back(w);
            if (results.size() >= 16)
                break;
        }
    }
    return results;
}

// =====================================================
// Send selected suggestion to editor
// =====================================================
void SendTextToEditor(const wchar_t* text)
{
    if (!g_lastEditorWnd || !text || !*text)
        return;

    // Number of typed characters to remove
    int removeCount = (int)g_inputBuffer.size();

    AllowSetForegroundWindow(ASFW_ANY);

    DWORD targetThread = GetWindowThreadProcessId(g_lastEditorWnd, nullptr);
    DWORD thisThread = GetCurrentThreadId();

    // Attach input
    AttachThreadInput(thisThread, targetThread, TRUE);
    SetForegroundWindow(g_lastEditorWnd);
    SetFocus(g_lastEditorWnd);

    std::vector<INPUT> inputs;

    // ----------------------------------------
    // 1. REMOVE the already-typed prefix
    // ----------------------------------------
    for (int i = 0; i < removeCount; ++i)
    {
        INPUT bk1{};
        bk1.type = INPUT_KEYBOARD;
        bk1.ki.wVk = VK_BACK;

        INPUT bk2 = bk1;
        bk2.ki.dwFlags = KEYEVENTF_KEYUP;

        inputs.push_back(bk1);
        inputs.push_back(bk2);
    }

    // ----------------------------------------
    // 2. INSERT the full suggestion as Unicode
    // ----------------------------------------
    for (const wchar_t* p = text; *p; ++p)
    {
        INPUT down{};
        down.type = INPUT_KEYBOARD;
        down.ki.wScan = *p;
        down.ki.dwFlags = KEYEVENTF_UNICODE;

        INPUT up = down;
        up.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

        inputs.push_back(down);
        inputs.push_back(up);
    }

    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));

    // Detach
    AttachThreadInput(thisThread, targetThread, FALSE);

    // Clear buffer (word has been completed)
    g_inputBuffer.clear();
    PostMessage(g_hMainWnd, WM_UPDATE_SUGGESTIONS, 0, 0);
}
// =====================================================
// Clamp popup window
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

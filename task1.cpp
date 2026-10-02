#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>

#define ID_EDIT_X        101
#define ID_EDIT_Y        102
#define ID_BUTTON_CALC   103
#define ID_STATIC_RESULT 104

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"CalcWindowClass";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wc)) return 0;

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Лабораторна робота №3 — Розрахунок f(x, y) = x * y + 3",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 280,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditX, hEditY, hBtnCalc, hStaticRes;
    static HFONT hFont;

    switch (msg) {
    case WM_CREATE: {
        auto cs = reinterpret_cast<LPCREATESTRUCTW>(lParam);
        HINSTANCE hInst = cs->hInstance;

        hFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

        HWND hLabelX = CreateWindowExW(0, L"STATIC", L"Введіть x:",
            WS_CHILD | WS_VISIBLE, 40, 30, 90, 24, hwnd, nullptr, hInst, nullptr);

        hEditX = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_RIGHT,
            140, 28, 120, 26, hwnd, reinterpret_cast<HMENU>(ID_EDIT_X), hInst, nullptr);

        HWND hLabelY = CreateWindowExW(0, L"STATIC", L"Введіть y:",
            WS_CHILD | WS_VISIBLE, 40, 70, 90, 24, hwnd, nullptr, hInst, nullptr);

        hEditY = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_RIGHT,
            140, 68, 120, 26, hwnd, reinterpret_cast<HMENU>(ID_EDIT_Y), hInst, nullptr);

        hBtnCalc = CreateWindowExW(0, L"BUTTON", L"Розрахувати",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            140, 110, 120, 32, hwnd, reinterpret_cast<HMENU>(ID_BUTTON_CALC), hInst, nullptr);

        HWND hLabelRes = CreateWindowExW(0, L"STATIC", L"Результат:",
            WS_CHILD | WS_VISIBLE, 40, 165, 90, 24, hwnd, nullptr, hInst, nullptr);

        hStaticRes = CreateWindowExW(0, L"STATIC", L"3",
            WS_CHILD | WS_VISIBLE, 140, 165, 200, 24,
            hwnd, reinterpret_cast<HMENU>(ID_STATIC_RESULT), hInst, nullptr);

        HWND controls[] = { hLabelX, hEditX, hLabelY, hEditY, hBtnCalc, hLabelRes, hStaticRes };
        for (HWND ctrl : controls) {
            SendMessageW(ctrl, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        }
        break;
    }

    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_BUTTON_CALC) {
            wchar_t bufX[64]{}, bufY[64]{};
            GetWindowTextW(hEditX, bufX, 64);
            GetWindowTextW(hEditY, bufY, 64);

            try {
                size_t idxX = 0, idxY = 0;
                double x = std::stod(bufX, &idxX);
                double y = std::stod(bufY, &idxY);

                if (idxX != wcslen(bufX) || idxY != wcslen(bufY)) {
                    throw std::invalid_argument("trailing characters");
                }

                double result = x * y + 3.0;

                std::wstringstream ss;
                ss << std::fixed << std::setprecision(4) << result;
                std::wstring out = ss.str();
                out.erase(out.find_last_not_of(L'0') + 1, std::wstring::npos);
                if (out.back() == L'.') out.pop_back();

                SetWindowTextW(hStaticRes, out.c_str());
            } catch (...) {
                SetWindowTextW(hStaticRes, L"Помилка введення!");
            }
        }
        break;
    }

    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}
#include <windows.h>
#include <vector>

// Ідентифікатори пунктів меню
#define ID_MENU_IMAGE_DRAW       2001
#define ID_MENU_TRANSFORM_COLOR  2002
#define ID_MENU_TRANSFORM_REPLACE 2003
#define ID_MENU_INFO_ABOUT       2004
#define ID_MENU_INFO_EXIT        2005

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ==========================================
// КЛАСИ ГРАФІЧНИХ ОБ'ЄКТІВ
// ==========================================

class Field {
public:
    void show(HDC dc, int width, int height, bool isSunset) {
        COLORREF skyColor = isSunset ? RGB(255, 160, 122) : RGB(135, 206, 235);
        RECT skyRect = { 0, 0, width, height / 2 };
        HBRUSH skyBrush = CreateSolidBrush(skyColor);
        FillRect(dc, &skyRect, skyBrush);
        DeleteObject(skyBrush);

        COLORREF sunColor = isSunset ? RGB(255, 69, 0) : RGB(255, 220, 0);
        HBRUSH sunBrush = CreateSolidBrush(sunColor);
        HPEN nullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
        HGDIOBJ oldBrush = SelectObject(dc, sunBrush);
        HGDIOBJ oldPen = SelectObject(dc, nullPen);
        Ellipse(dc, width - 140, 30, width - 40, 130);

        COLORREF farColor = isSunset ? RGB(140, 130, 60) : RGB(120, 185, 90);
        HBRUSH hillFarBrush = CreateSolidBrush(farColor);
        SelectObject(dc, hillFarBrush);
        POINT hillFar[] = {
            { 0, height / 2 },
            { width / 3, height / 2 - 40 },
            { 2 * width / 3, height / 2 - 20 },
            { width, height / 2 - 50 },
            { width, height },
            { 0, height }
        };
        Polygon(dc, hillFar, 6);
        DeleteObject(hillFarBrush);

        COLORREF nearColor = isSunset ? RGB(110, 100, 40) : RGB(85, 160, 60);
        HBRUSH fieldBrush = CreateSolidBrush(nearColor);
        SelectObject(dc, fieldBrush);
        POINT hillNear[] = {
            { 0, height / 2 + 30 },
            { width / 2, height / 2 - 10 },
            { width, height / 2 + 40 },
            { width, height },
            { 0, height }
        };
        Polygon(dc, hillNear, 5);
        DeleteObject(fieldBrush);

        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        DeleteObject(sunBrush);
        DeleteObject(nullPen);
    }
};

class Tree {
public:
    void show(HDC dc, int x, int y, double scale = 1.0, bool isSunset = false) {
        int trunkW = static_cast<int>(16 * scale);
        int trunkH = static_cast<int>(60 * scale);
        int crownR = static_cast<int>(35 * scale);

        HBRUSH trunkBrush = CreateSolidBrush(RGB(105, 55, 25));
        HPEN contourPen = CreatePen(PS_SOLID, 1, RGB(40, 20, 10));
        HGDIOBJ oldBrush = SelectObject(dc, trunkBrush);
        HGDIOBJ oldPen = SelectObject(dc, contourPen);

        Rectangle(dc, x - trunkW / 2, y - trunkH, x + trunkW / 2, y);

        COLORREF crownColor = isSunset ? RGB(45, 90, 35) : RGB(34, 139, 34);
        HBRUSH crownBrush = CreateSolidBrush(crownColor);
        SelectObject(dc, crownBrush);

        Ellipse(dc, x - crownR, y - trunkH - crownR * 2 + 10, x + crownR, y - trunkH + 20);
        Ellipse(dc, x - crownR - 10, y - trunkH - crownR + 5, x, y - trunkH + 20);
        Ellipse(dc, x, y - trunkH - crownR + 5, x + crownR + 10, y - trunkH + 20);

        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        DeleteObject(trunkBrush);
        DeleteObject(crownBrush);
        DeleteObject(contourPen);
    }
};

class Tower {
public:
    void show(HDC dc, int x, int y, bool isSunset) {
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(50, 50, 60));
        HBRUSH stoneBrush = CreateSolidBrush(isSunset ? RGB(140, 130, 135) : RGB(160, 165, 175));
        HGDIOBJ oldPen = SelectObject(dc, borderPen);
        HGDIOBJ oldBrush = SelectObject(dc, stoneBrush);

        Rectangle(dc, x, y - 190, x + 70, y);

        for (int i = 0; i < 3; ++i) {
            Rectangle(dc, x + i * 25, y - 205, x + 15 + i * 25, y - 190);
        }

        POINT roof[] = {
            { x - 5, y - 190 },
            { x + 35, y - 245 },
            { x + 75, y - 190 }
        };
        HBRUSH roofBrush = CreateSolidBrush(isSunset ? RGB(139, 0, 0) : RGB(178, 34, 34));
        SelectObject(dc, roofBrush);
        Polygon(dc, roof, 3);
        DeleteObject(roofBrush);

        HBRUSH doorBrush = CreateSolidBrush(RGB(70, 40, 20));
        SelectObject(dc, doorBrush);
        RoundRect(dc, x + 23, y - 55, x + 47, y, 10, 10);
        DeleteObject(doorBrush);

        HBRUSH windowBrush = CreateSolidBrush(RGB(40, 40, 50));
        SelectObject(dc, windowBrush);
        RoundRect(dc, x + 27, y - 150, x + 43, y - 120, 6, 6);
        DeleteObject(windowBrush);

        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(borderPen);
        DeleteObject(stoneBrush);
    }
};

// Замінний об'єкт за варіантом 24 (заміна вежі на будинок)
class CottageHouse {
public:
    void show(HDC dc, int x, int y, bool isSunset) {
        HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(40, 30, 20));
        COLORREF wallColor = isSunset ? RGB(210, 170, 110) : RGB(230, 195, 140);
        HBRUSH wallBrush = CreateSolidBrush(wallColor);
        HGDIOBJ oldPen = SelectObject(dc, borderPen);
        HGDIOBJ oldBrush = SelectObject(dc, wallBrush);

        Rectangle(dc, x - 15, y - 100, x + 85, y);

        POINT roof[] = {
            { x - 25, y - 100 },
            { x + 35, y - 165 },
            { x + 95, y - 100 }
        };
        COLORREF roofColor = isSunset ? RGB(160, 40, 20) : RGB(200, 60, 30);
        HBRUSH roofBrush = CreateSolidBrush(roofColor);
        SelectObject(dc, roofBrush);
        Polygon(dc, roof, 3);
        DeleteObject(roofBrush);

        HBRUSH doorBrush = CreateSolidBrush(RGB(80, 40, 20));
        SelectObject(dc, doorBrush);
        Rectangle(dc, x + 45, y - 55, x + 72, y);
        DeleteObject(doorBrush);

        HBRUSH winBrush = CreateSolidBrush(isSunset ? RGB(255, 235, 130) : RGB(173, 216, 230));
        SelectObject(dc, winBrush);
        Rectangle(dc, x - 2, y - 75, x + 32, y - 35);
        DeleteObject(winBrush);

        MoveToEx(dc, x + 15, y - 75, nullptr);
        LineTo(dc, x + 15, y - 35);
        MoveToEx(dc, x - 2, y - 55, nullptr);
        LineTo(dc, x + 32, y - 55);

        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(borderPen);
        DeleteObject(wallBrush);
    }
};

class Mill {
public:
    void show(HDC dc, int x, int y, double scale = 1.0) {
        int baseW = static_cast<int>(70 * scale);
        int topW = static_cast<int>(45 * scale);
        int bodyH = static_cast<int>(110 * scale);
        int wingLen = static_cast<int>(65 * scale);

        HPEN bodyPen = CreatePen(PS_SOLID, 2, RGB(80, 50, 25));
        HBRUSH bodyBrush = CreateSolidBrush(RGB(215, 185, 140));
        HGDIOBJ oldPen = SelectObject(dc, bodyPen);
        HGDIOBJ oldBrush = SelectObject(dc, bodyBrush);

        POINT body[] = {
            { x - baseW / 2, y },
            { x - topW / 2, y - bodyH },
            { x + topW / 2, y - bodyH },
            { x + baseW / 2, y }
        };
        Polygon(dc, body, 4);

        HBRUSH capBrush = CreateSolidBrush(RGB(139, 69, 19));
        SelectObject(dc, capBrush);
        POINT cap[] = {
            { x - topW / 2 - 6, y - bodyH },
            { x, y - bodyH - static_cast<int>(30 * scale) },
            { x + topW / 2 + 6, y - bodyH }
        };
        Polygon(dc, cap, 3);
        DeleteObject(capBrush);

        int centerPivotY = y - bodyH + static_cast<int>(15 * scale);

        HPEN wingPen = CreatePen(PS_SOLID, static_cast<int>(3 * scale), RGB(60, 30, 10));
        SelectObject(dc, wingPen);

        MoveToEx(dc, x - wingLen, centerPivotY - wingLen, nullptr);
        LineTo(dc, x + wingLen, centerPivotY + wingLen);

        MoveToEx(dc, x - wingLen, centerPivotY + wingLen, nullptr);
        LineTo(dc, x + wingLen, centerPivotY - wingLen);

        HPEN crossPen = CreatePen(PS_SOLID, 1, RGB(90, 50, 20));
        SelectObject(dc, crossPen);
        for (int step = 20; step <= wingLen; step += 15) {
            int d = static_cast<int>(step * 0.707);
            MoveToEx(dc, x + d - 6, centerPivotY + d + 6, nullptr);
            LineTo(dc, x + d + 6, centerPivotY + d - 6);

            MoveToEx(dc, x - d - 6, centerPivotY - d + 6, nullptr);
            LineTo(dc, x - d + 6, centerPivotY - d - 6);

            MoveToEx(dc, x + d - 6, centerPivotY - d - 6, nullptr);
            LineTo(dc, x + d + 6, centerPivotY - d + 6);

            MoveToEx(dc, x - d - 6, centerPivotY + d - 6, nullptr);
            LineTo(dc, x - d + 6, centerPivotY + d + 6);
        }

        HBRUSH centerBrush = CreateSolidBrush(RGB(50, 25, 10));
        SelectObject(dc, centerBrush);
        Ellipse(dc, x - 5, centerPivotY - 5, x + 5, centerPivotY + 5);
        DeleteObject(centerBrush);

        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(bodyPen);
        DeleteObject(bodyBrush);
        DeleteObject(wingPen);
        DeleteObject(crossPen);
    }
};

// ==========================================
// ФУНКЦІЯ МАЛЮВАННЯ СЦЕНИ
// ==========================================

void DrawScene(HDC dc, int width, int height, bool showImage, bool isSunset, bool replaceObject) {
    if (!showImage) {
        RECT fullRect = { 0, 0, width, height };
        HBRUSH emptyBrush = CreateSolidBrush(RGB(245, 245, 245));
        FillRect(dc, &fullRect, emptyBrush);
        DeleteObject(emptyBrush);

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(110, 110, 110));
        RECT textRect = fullRect;
        DrawTextW(dc, L"Оберіть у меню: Зображення -> Рисунок...", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    Field field;
    field.show(dc, width, height, isSunset);

    if (replaceObject) {
        CottageHouse house;
        house.show(dc, width / 2 - 40, height / 2 + 100, isSunset);
    } else {
        Tower tower;
        tower.show(dc, width / 2 - 40, height / 2 + 100, isSunset);
    }

    Mill millFar;
    millFar.show(dc, width - 180, height / 2 + 50, 0.75);

    Mill millNear;
    millNear.show(dc, 180, height / 2 + 160, 1.15);

    Tree tree1;
    tree1.show(dc, 70, height / 2 + 140, 1.2, isSunset);

    Tree tree2;
    tree2.show(dc, 290, height / 2 + 180, 0.9, isSunset);

    Tree tree3;
    tree3.show(dc, width - 90, height / 2 + 130, 1.1, isSunset);

    Tree tree4;
    tree4.show(dc, width / 2 + 90, height / 2 + 120, 0.8, isSunset);
}

// Створення головного меню програми
void CreateAppMenu(HWND hwnd) {
    HMENU hMenuBar = CreateMenu();

    // 1. Меню "Зображення"
    HMENU hMenuImage = CreatePopupMenu();
    AppendMenuW(hMenuImage, MF_STRING, ID_MENU_IMAGE_DRAW, L"&Рисунок...\tCtrl+D");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuImage), L"&Зображення");

    // 2. Меню "Трансформації"
    HMENU hMenuTransform = CreatePopupMenu();
    AppendMenuW(hMenuTransform, MF_STRING, ID_MENU_TRANSFORM_COLOR, L"Зміна &кольору");
    AppendMenuW(hMenuTransform, MF_STRING, ID_MENU_TRANSFORM_REPLACE, L"&Заміна одного об'єкта");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuTransform), L"&Трансформації");

    // 3. Меню "Інформація"
    HMENU hMenuInfo = CreatePopupMenu();
    AppendMenuW(hMenuInfo, MF_STRING, ID_MENU_INFO_ABOUT, L"&Про програму");
    AppendMenuW(hMenuInfo, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenuInfo, MF_STRING, ID_MENU_INFO_EXIT, L"&Вихід\tAlt+F4");
    AppendMenuW(hMenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuInfo), L"&Інформація");

    SetMenu(hwnd, hMenuBar);
}

// ==========================================
// ТОЧКА ВХОДУ WINMAIN
// ==========================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MenuLandscapeWindowClass";

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
        L"Лабораторна робота №3 — Керування сценою через меню (Варіант 24)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 960, 640,
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

// ==========================================
// ОБРОБКА ПОВІДОМЛЕНЬ ВІКНА
// ==========================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static bool showImage = true;
    static bool isSunset = false;
    static bool replaceObject = false;

    switch (message) {
    case WM_CREATE:
        CreateAppMenu(hwnd);
        break;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case ID_MENU_IMAGE_DRAW:
            showImage = true;
            isSunset = false;
            replaceObject = false;
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_MENU_TRANSFORM_COLOR:
            if (!showImage) showImage = true;
            isSunset = !isSunset;
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_MENU_TRANSFORM_REPLACE:
            if (!showImage) showImage = true;
            replaceObject = !replaceObject;
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case ID_MENU_INFO_ABOUT:
            MessageBoxW(
                hwnd,
                L"Лабораторна робота №3\n"
                L"Тема: Елементи управління та меню Windows API\n"
                L"Варіант 24: Млин, дерево, вежа, поле\n"
                L"Індивідуальна дія: Заміна одного об'єкта (вежа -> будинок)",
                L"Про програму",
                MB_OK | MB_ICONINFORMATION
            );
            break;

        case ID_MENU_INFO_EXIT:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        DrawScene(hdc, rect.right, rect.bottom, showImage, isSunset, replaceObject);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_SIZE:
        InvalidateRect(hwnd, nullptr, TRUE);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return 0;
}
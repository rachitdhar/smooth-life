#include "helper.h"
#include "evolution_functions.h"
#include "source.h"

static float cell_state[GRID_WIDTH][GRID_HEIGHT] = { 0.0f };
static float state_diff[GRID_WIDTH][GRID_HEIGHT] = { 0.0f };
static bool is_paused = TRUE;
static is_reset_pressed = FALSE;
static RECT reset_btn_coords = { 0, 0, 0, 0 };

void handleTimeTick()
{
    calculateStateChange();
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            cell_state[i][j] += state_diff[i][j];
            if (cell_state[i][j] < MIN_CELL_DISPLAY_VALUE) cell_state[i][j] = 0.0f;
            else if (cell_state[i][j] > 1.0f) cell_state[i][j] = 1.0f;
        }
    }
}

void calculateStateChange()
{
    for (int center_x = 0; center_x < GRID_WIDTH; ++center_x) {
        for (int center_y = 0; center_y < GRID_HEIGHT; ++center_y) {
            float m = 0, n = 0;
            float M = 0, N = 0;

            for (int dx = -(R_A - 1); dx < R_A; ++dx) {
                for (int dy = -(R_A - 1); dy < R_A; ++dy) {
                    int x = emod(center_x + dx, GRID_WIDTH);
                    int y = emod(center_y + dy, GRID_HEIGHT);

                    if (dx * dx + dy * dy < R_I * R_I) {
                        m += cell_state[x][y];
                        M++;
                    }
                    else if (dx * dx + dy * dy < R_A * R_A) {
                        n += cell_state[x][y];
                        N++;
                    }
                }
            }
            m /= M;
            n /= N;
            state_diff[center_x][center_y] = DT * s(n, m);
        }
    }
}

void displayCells(HDC hdc)
{
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            drawAtLocation(hdc, i, j);
        }
    }
}

void displayBottomStrip(HDC hdc)
{
    Rectangle(hdc, 0, WIN_HEIGHT, WIN_WIDTH, WIN_HEIGHT + BOTTOM_STRIP_HEIGHT);
}

void displayButton(HWND hwnd, HDC hdc)
{
    HFONT hFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, BUTTON_TEXT_FONT);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    const char* str = "Reset";
    SIZE text_size = { 0 };
    GetTextExtentPoint32A(hdc, str, lstrlenA(str), &text_size);

    HBRUSH btnBrush = CreateSolidBrush(is_reset_pressed ? BUTTON_PRESSED_COLOR : BUTTON_NORMAL_COLOR);
    SelectObject(hdc, btnBrush);

    reset_btn_coords.left = WIN_WIDTH - 4 * CELL_SIZE - text_size.cx;
    reset_btn_coords.top = WIN_HEIGHT + 0.2 * BOTTOM_STRIP_HEIGHT;
    reset_btn_coords.right = WIN_WIDTH - 2 * CELL_SIZE;
    reset_btn_coords.bottom = WIN_HEIGHT + 0.8 * BOTTOM_STRIP_HEIGHT;

    Rectangle(hdc, reset_btn_coords.left, reset_btn_coords.top, reset_btn_coords.right, reset_btn_coords.bottom);
    DeleteObject(btnBrush);

    SetTextColor(hdc, TEXT_COLOR);
    SetBkMode(hdc, TRANSPARENT);
    TextOutA(
        hdc,
        reset_btn_coords.left + (reset_btn_coords.right - reset_btn_coords.left - text_size.cx) / 2,
        WIN_HEIGHT + 0.2 * BOTTOM_STRIP_HEIGHT + (reset_btn_coords.bottom - reset_btn_coords.top - text_size.cy) / 2,
        str,
        lstrlenA(str)
    );

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void displayText(HDC hdc)
{
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, TEXT_FONT);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    SetTextColor(hdc, TEXT_COLOR);
    SetBkMode(hdc, TRANSPARENT);

    // show whether paused or running
    const char* str_state = (is_paused) ? "-- PAUSED --" : "-- RUNNING --";
    TextOutA(hdc, 10, WIN_HEIGHT + 2, str_state, lstrlenA(str_state));

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

void drawAtLocation(HDC hdc, int x, int y)
{
    int grayscale = (int)(255 * cell_state[x][y]);
    if (grayscale > 255 || grayscale < 0) return;

    HBRUSH cellBrush = CreateSolidBrush(RGB(grayscale, grayscale, grayscale));
    SelectObject(hdc, cellBrush);
    Rectangle(hdc, x * CELL_SIZE, y * CELL_SIZE, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
    DeleteObject(cellBrush);
}

void initializeState()
{
    srand(time(NULL));
    for (int i = 0; i < GRID_WIDTH; ++i) {
        for (int j = 0; j < GRID_HEIGHT; ++j) {
            if (i > GRID_WIDTH / 4 && i < 3 * GRID_WIDTH / 4 && j > GRID_HEIGHT / 4 && j < 3 * GRID_HEIGHT / 4)
                cell_state[i][j] = (float)rand() / (float)RAND_MAX;
            else
                cell_state[i][j] = 0.0f;
        }
    }
}

void handleDrawing(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // display agent
    displayCells(hdc);

    // display bottom strip
    HBRUSH bottomStripBrush = CreateSolidBrush(BOTTOM_STRIP_COLOR);
    HBRUSH oldBrush = SelectObject(hdc, bottomStripBrush);
    displayBottomStrip(hdc);
    SelectObject(hdc, oldBrush);
    DeleteObject(bottomStripBrush);

    // display score
    displayText(hdc);

    // display reset button
    displayButton(hwnd, hdc);

    EndPaint(hwnd, &ps);
}

// Window procedure function to handle events
long long __stdcall WindowProc(HWND hwnd, UINT uMsg, unsigned long long wParam, long long lParam) {
    switch (uMsg) {
        case WM_PAINT:
            handleDrawing(hwnd);
            break;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE) is_paused = !is_paused;
            if (wParam == (int)'N') {
                memset(cell_state, 0, sizeof(cell_state));
                is_paused = TRUE;
            }
            if (is_paused) InvalidateRect(hwnd, NULL, TRUE);
            break;
        case WM_LBUTTONDOWN: {
            int xpos = LOWORD(lParam);
            int ypos = HIWORD(lParam);

            if (xpos > reset_btn_coords.left && xpos < reset_btn_coords.right && ypos > reset_btn_coords.top && ypos < reset_btn_coords.bottom) {
                is_reset_pressed = TRUE;
                initializeState();
                InvalidateRect(hwnd, NULL, TRUE);
                is_paused = TRUE;
            }
            break;
        }
        case WM_TIMER:
            if (!is_paused) {
                handleTimeTick();
                InvalidateRect(hwnd, NULL, TRUE);   // Request complete window redraw
            }
            if (is_reset_pressed) {
                is_reset_pressed = FALSE;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
        }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

// Entry point for Windows applications
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow) {
    initializeState();

    // setup the window
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "BasicWindowClass";

    RegisterClassA(&wc);

    RECT adjustedRect = { 0, 0, WIN_WIDTH, WIN_HEIGHT + BOTTOM_STRIP_HEIGHT };
    AdjustWindowRect(&adjustedRect, WS_OVERLAPPEDWINDOW, FALSE);

    int actualWidth = adjustedRect.right - adjustedRect.left;
    int actualHeight = adjustedRect.bottom - adjustedRect.top;

    // create a window (having no maximize/minimize, and being non-resizeable)
    HWND hwnd = CreateWindowExA(
        0, "BasicWindowClass", "Smooth Life",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        actualWidth,
        actualHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);
    SetTimer(hwnd, 1, 50, NULL);    // Timer event every 100 ms

    MSG msg = { 0 };
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}
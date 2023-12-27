#include <windows.h>
#include <vector>
#include <string>

struct Vector3D {
    float x, y, z;
};

struct Triangle3D {
    Vector3D vertices[3];
};

struct Mesh3D {
    Mesh3D(std::initializer_list<Triangle3D> initList) : triangles(initList) {}
    std::vector<Triangle3D> triangles;
};

Mesh3D meshCube = {

    // South Face
    { 0.f, 0.f, 0.f,   0.f, 1.f, 0.f,   1.f, 1.f, 0.f},
    { 0.f, 0.f, 0.f,   1.f, 1.f, 0.f,   1.f, 0.f, 0.f},

    // East face
    { 1.f, 0.f, 0.f,   1.f, 1.f, 0.f,   1.f, 1.f, 1.f },
    { 1.f, 0.f, 0.f,   1.f, 1.f, 1.f,   1.f, 0.f, 1.f },

    // North face
    { 1.f, 0.f, 1.f,   1.f, 1.f, 1.f,   0.f, 1.f, 1.f },
    { 1.f, 0.f, 1.f,   0.f, 1.f, 1.f,   0.f, 0.f, 1.f },

    // West face
    { 0.f, 0.f, 1.f,   0.f, 1.f, 1.f,   0.f, 1.f, 0.f },
    { 0.f, 0.f, 1.f,   0.f, 1.f, 0.f,   0.f, 0.f, 0.f },

    // Top face
    { 0.f, 1.f, 0.f,   0.f, 1.f, 1.f,   1.f, 1.f, 1.f },
    { 0.f, 1.f, 0.f,   1.f, 1.f, 1.f,   1.f, 1.f, 0.f },

    // Bottom face
    { 0.f, 0.f, 0.f,   0.f, 0.f, 1.f,   1.f, 0.f, 1.f },
    { 0.f, 0.f, 0.f,   1.f, 0.f, 1.f,   1.f, 0.f, 0.f }
};

struct Triangle2D {
    POINT points[3];
};

bool WindowProcess();
void WindowDraw(std::vector<Triangle2D> &triangles);

void rendererMain() {
    while(WindowProcess()) {
        std::vector<Triangle2D> triangles;
        for (const auto& triangle3D : meshCube.triangles) {
            Triangle2D triangle2D;
            for (int i = 0; i < 3; ++i) {
                triangle2D.points[i].x = static_cast<LONG>(triangle3D.vertices[i].x*4000/static_cast<LONG>(triangle3D.vertices[i].z+10.f));
                triangle2D.points[i].y = static_cast<LONG>(triangle3D.vertices[i].y*4000/static_cast<LONG>(triangle3D.vertices[i].z+10.f));
            }
            triangles.push_back(triangle2D);
        }
        WindowDraw(triangles);
    }
}


// --- WIN32 & GDI: WINDOW MANAGEMENT ---

// General window variables
std::string windowTitle = "Laurin's 3D Renderer";
COLORREF backgroundColor = RGB(0, 0, 0);
int initialWindowWidth = 1080;
int initialWindowHeight = 720;

int frames = 0;
POINT mousePos = { 0, 0 };
HWND hwnd = NULL;

// Declare a global off-screen buffer and its device context
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;
HBITMAP hOldBitmap = NULL;
int bufferWidth = 0;
int bufferHeight = 0;

void WindowDraw(std::vector<Triangle2D> &triangles) {
    // Clear screen with background color
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    HBRUSH hBackgroundBrush = CreateSolidBrush(backgroundColor);
    FillRect(hdcBuffer, &clientRect, hBackgroundBrush);
    DeleteObject(hBackgroundBrush);

    for (const auto& triangle : triangles) {
        // Create a region from the triangle's vertices
        HRGN hRgn = CreatePolygonRgn(triangle.points, 3, WINDING);

        // Fill the created region
        HBRUSH hBrush = CreateSolidBrush(RGB(20, 20, 20));
        HBRUSH hBrush1 = CreateSolidBrush(RGB(0, 0, 255));
        FillRgn(hdcBuffer, hRgn, hBrush);

        // Draw outline
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        SelectObject(hdcBuffer, hPen);
        FrameRgn(hdcBuffer, hRgn, hBrush1, 1, 1);

        DeleteObject(hRgn);
        DeleteObject(hBrush1);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }
    // Copy buffer to screen
    HDC hdc = GetDC(hwnd);
    BitBlt(hdc, 0, 0, bufferWidth, bufferHeight, hdcBuffer, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, hdc);
    frames++;
}

bool WindowProcess() {
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            return false;
    }
    return true;
}

// Window procedure to handle messages for the main window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEMOVE: {
            mousePos.x = LOWORD(lParam);
            mousePos.y = HIWORD(lParam);
            return 0;
        }
        case WM_TIMER: {
            const std::string title = windowTitle + " | FPS " + std::to_string(frames);
            SetWindowText(hwnd, title.c_str());
            frames = 0;
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "3DRendererWindowClass";
    RegisterClass(&wc);

    // Create the window
    hwnd = CreateWindowEx(
        0,                          // Optional window styles
        "3DRendererWindowClass",    // Window class
        windowTitle.c_str(),        // Window title
        WS_OVERLAPPEDWINDOW,        // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 1080, 720, // Size and position
        NULL, NULL, hInstance, NULL // Parent window, menu, instance handle, additional app data
    );
    ShowWindow(hwnd, nCmdShow);

    // Initialize buffer
    // todo: make sure to precisely understand, recreate on resize?
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    bufferWidth = clientRect.right - clientRect.left;
    bufferHeight = clientRect.bottom - clientRect.top;
    hdcBuffer = CreateCompatibleDC(NULL);
    hBitmap = CreateCompatibleBitmap(GetDC(hwnd), bufferWidth, bufferHeight);
    hOldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmap);

    // Set FPS Timer
    SetTimer(hwnd, 1, 1000, NULL);

    // Start main renderer process
    rendererMain();

    // Release Buffer
    SelectObject(hdcBuffer, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBuffer);
    DeleteObject(hdcBuffer);

    DeleteObject(hwnd);
    return 0;
}

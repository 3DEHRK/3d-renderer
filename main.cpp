#include <windows.h>
#include <vector>
#include <cstdlib>

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
void WindowDraw(std::vector<Triangle2D> &triangles, HWND &hwnd);

void rendererMain(HWND &hwnd) {
    while(WindowProcess()) {

        // accumulate triangle2ds to render
        std::vector<Triangle2D> triangles;
        for (const auto& triangle3D : meshCube.triangles) {
            Triangle2D triangle2D;
            for (int i = 0; i < 3; ++i) {
                // Create 2D points by removing the z-component
                triangle2D.points[i].x = static_cast<LONG>(triangle3D.vertices[i].x*4000/static_cast<LONG>(triangle3D.vertices[i].z+10.f));
                triangle2D.points[i].y = static_cast<LONG>(triangle3D.vertices[i].y*4000/static_cast<LONG>(triangle3D.vertices[i].z+10.f));
            }
            // Push back the generated 2D triangle
            triangles.push_back(triangle2D);
        }

        // render triangle2ds
        WindowDraw(triangles, hwnd);
    }
}


// --- LOW LEVEL WIN32 GDI WINDOW STUFF ---

POINT mousePos = { 0, 0 };
COLORREF backgroundColor = RGB(0, 0, 0);

// Declare a global off-screen buffer and its device context
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;
HBITMAP hOldBitmap = NULL;
int screenWidth = 0;
int screenHeight = 0;

void WindowDraw(std::vector<Triangle2D> &triangles, HWND &hwnd) {
    // todo: Performance: Generating GDI objects (CreateSolidBrush, CreatePen) inside a loop might impact performance, especially if the loop iterates frequently or handles a large number of triangles. It's more efficient to reuse existing GDI objects where possible.

    HDC hdc = GetDC(hwnd);

    // Clear window screen rect
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // Fill entire screen with specified background color
    HBRUSH hBackgroundBrush = CreateSolidBrush(backgroundColor);
    FillRect(hdcBuffer, &clientRect, hBackgroundBrush);
    DeleteObject(hBackgroundBrush);

    for (const auto& triangle : triangles) {
        // Create a region from the triangle's vertices
        HRGN hRgn = CreatePolygonRgn(triangle.points, 3, WINDING);

        // Fill the created region with the desired color
        //HBRUSH hBrush = CreateSolidBrush(RGB(20, 20, 20));
        HBRUSH hBrush1 = CreateSolidBrush(RGB(rand(), 0, 255));
        //FillRgn(hdcBuffer, hRgn, hBrush);

        // Draw the outline of the region with a different color (here: black)
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        SelectObject(hdcBuffer, hPen);
        FrameRgn(hdcBuffer, hRgn, hBrush1, 1, 1);

        // Clean up: delete the created region and brush to release resources
        DeleteObject(hRgn);
        DeleteObject(hBrush1);
        DeleteObject(hPen);
        // DeleteObject(hBrush);
    }
    
    BitBlt(hdc, 0, 0, screenWidth, screenHeight, hdcBuffer, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

bool WindowProcess() {
    MSG msg = {};
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    return msg.message != WM_QUIT;
}

// Window procedure to handle messages for the main window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEMOVE: {
            mousePos.x = LOWORD(lParam);
            mousePos.y = HIWORD(lParam);
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
    HWND hwnd = CreateWindowEx(
        0,                          // Optional window styles
        "3DRendererWindowClass",    // Window class
        "Laurin's sophisticated 3D Renderer", // Window title
        WS_OVERLAPPEDWINDOW,        // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 1080, 720, // Size and position
        NULL, NULL, hInstance, NULL // Parent window, menu, instance handle, additional app data
    );

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Init buffer todo: make sure to precisely understand
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    screenWidth = clientRect.right - clientRect.left;
    screenHeight = clientRect.bottom - clientRect.top;
    hdcBuffer = CreateCompatibleDC(NULL);
    hBitmap = CreateCompatibleBitmap(GetDC(hwnd), screenWidth, screenHeight);
    hOldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmap);

    // start main renderer process <-
    rendererMain(hwnd);

    // Release Buffer
    SelectObject(hdcBuffer, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBuffer);
    DeleteObject(hdcBuffer);

    DeleteObject(hwnd);

    return 0;
}

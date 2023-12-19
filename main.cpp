#include <windows.h>
#include <vector>

struct vector3d
{
    float x, y, z;
};

struct triangle
{
    vector3d vertices[3];
};

struct mesh
{
    mesh(std::initializer_list<triangle> initList) : triangles(initList) {}
    std::vector<triangle> triangles;
};

mesh meshCube = {

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

// Global variable for the mouse position
POINT mousePos = { 0, 0 };
COLORREF backgroundColor = RGB(0, 0, 0);

// Function to draw a filled triangle in the specified device context
void DrawFilledTriangle(HDC hdc) {
    // Specify the vertices of the triangle
    POINT points[3] = { { mousePos.x, mousePos.y }, { 200, 50 }, { 125, 200 } };

    // Create a region from the triangle's vertices
    HRGN hRgn = CreatePolygonRgn(points, 3, WINDING);

    // Fill the created region with the desired color (e.g., red)
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red color (adjust RGB values as needed)
    FillRgn(hdc, hRgn, hBrush);

    // Clean up: delete the created region and brush to release resources
    DeleteObject(hRgn);
    DeleteObject(hBrush);
}

// Window procedure to handle messages for the main window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Clear the background with the specified color
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            HBRUSH hBackgroundBrush = CreateSolidBrush(backgroundColor);
            FillRect(hdc, &clientRect, hBackgroundBrush);
            DeleteObject(hBackgroundBrush);

            DrawFilledTriangle(hdc);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEMOVE: {
            mousePos.x = LOWORD(lParam);
            mousePos.y = HIWORD(lParam);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TriangleWindowClass";
    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,                          // Optional window styles
        "TriangleWindowClass",      // Window class
        "Draw Triangle",           // Window title
        WS_OVERLAPPEDWINDOW,        // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, // Size and position
        NULL, NULL, hInstance, NULL // Parent window, menu, instance handle, additional app data
    );

    if (hwnd == NULL) {
        return 0;
    }

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

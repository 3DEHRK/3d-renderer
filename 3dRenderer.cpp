#include <windows.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

// 3D space structures
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

// User defined matrix type
class Matrix {  // optimization: make template
public:
    Matrix(int m_, int n_, const std::initializer_list<float> initList = {}) : m(m_), n(n_) {  // Create matrix
        size = m*n;
        if (n == 1) size++; // appending space if vector
        matrix = new float[size];

        if (initList.size() > m*n) throw std::length_error("Initializer list too long.");
        std::copy(initList.begin(), initList.end(), matrix);
    }
    Matrix(const Matrix& other) : m(other.m), n(other.n) {  // Copy constructor
        size = m*n;
        if (n == 1) size++;
        matrix = new float[size];

        for (int i = 0; i != m*n; ++i) {
            matrix[i] = other.matrix[i];
        }
    }
    Matrix(const Vector3D& vector) : m(3) {  // Create vector matrix by Vector3D
        size = m+1; // space for appending
        matrix = new float[size];
        matrix[0] = vector.x;
        matrix[1] = vector.y;
        matrix[2] = vector.z;
    }
    Matrix& operator=(const Matrix& other) {
        if (this == &other) return *this;
        if (m * n != other.m * other.n) throw std::out_of_range("Matrices are of unequal dimensions.");
        for (int i = 0; i != m*n; ++i) {
            matrix[i] = other.matrix[i];
        }
        return *this;
    }
    Matrix& operator=(const std::initializer_list<float> initList) {  // Assign raw values
        if (initList.size() > m*n) throw std::length_error("Initializer list too long.");
        std::copy(initList.begin(), initList.end(), matrix);
        return *this;
    }
    ~Matrix() {
        delete[] matrix;
    }

    Matrix operator*(const Matrix& other) {
        if (n != other.m) throw std::out_of_range("Matrices are of dimensions that cannot be multiplied.");
        Matrix product(m, other.n);
        for (int i1 = 0; i1 != m; ++i1) {
            for (int j2 = 0; j2 != other.n; ++j2) {
                float accu = 0;
                for (int t = 0; t != n; ++t) {
                    accu += get(i1, t) * other.get(t, j2);
                }
                product.set(i1, j2, accu);
            }
        }
        return product;
    }
    Matrix operator+(const Matrix& other) {
        if (m != other.m || n != other.n) throw std::out_of_range("Matrices are of unequal dimensions and cannot be added.");
        Matrix sum(m, n);
        for (int i = 0; i != m; ++i) {
            for (int j = 0; j != n; ++j) {
                sum.set(i, j, get(i, j) + other.get(i, j));
            }
        }
        return sum;
    }
    Matrix operator-(const Matrix& other) {
        if (m != other.m || n != other.n) throw std::out_of_range("Matrices are of unequal dimensions and cannot be subtracted.");
        Matrix difference(m, n);
        for (int i = 0; i != m; ++i) {
            for (int j = 0; j != n; ++j) {
                difference.set(i, j, get(i, j) - other.get(i, j));
            }
        }
        return difference;
    }

    void vectorAppend(float value) {
        if (n != 1) throw std::logic_error("This is not a vector and cannot be appended.");
        appendResizeCheck();
        matrix[m++] = value;
    }
    float vectorRemove() {
        if (n != 1) throw std::logic_error("This is not a vector elements cannot be removed.");
        if (m == 0) throw std::out_of_range("Vector is already empty.");
        return matrix[--m];
    }

    float get(int i_, int j_) const {
        if (i_ > m || j_ > n) throw std::out_of_range("Element is not in the matrix bounds.");
        return matrix[i_ * n + j_];
    }
    void set(int i_, int j_, float v) {
        if (i_ > m || j_ > n) throw std::out_of_range("Element is not in the matrix bounds.");
        matrix[i_ * n + j_] = v;
    }

    int mGet() const { return m; }
    int nGet() const { return n; }

private:
    float* matrix;
    int m = 1;
    int n = 1;
    int size = 0;

    void appendResizeCheck() {
        if (size < m + 1) {
            float* matrixNew = new float[m + 2];
            for (int i = 0; i != size; ++i) {
                matrixNew[i] = matrix[i];
            }
            delete[] matrix;
            matrix = matrixNew;
            size = m + 2;
        }
    }
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

UINT windowWidth = 1080;
UINT windowHeight = 720;
POINT mousePos = { 0, 0 };
float fieldOfView = 90.f;

void rendererMain() {
    while(WindowProcess()) {
        std::vector<Triangle2D> triangles;

        // --- TRANSFORMATION ---

        float y = (float) mousePos.x * 6 / (float) windowWidth; // y rotation
        Matrix rotationMatrixY(3, 3, {
            cosf(y),  0.f,  sinf(y),
            0.f,      1.f,      0.f,
            -sinf(y), 0.f,  cosf(y)
        });

        float x = (float) mousePos.y * 6 / (float) windowHeight; // x rotation
        Matrix rotationMatrixX(3, 3, {
            1.f,  0.f,          0.f,
            0.f,  cosf(x), -sinf(x),
            0.f,  sinf(x),  cosf(x)
        });

        Mesh3D meshCubeTransformed = meshCube;

        for (Triangle3D& triangle3dTransformed : meshCubeTransformed.triangles) {
            for (Vector3D& vector3d : triangle3dTransformed.vertices) {
                Matrix vector(vector3d);

                vector = rotationMatrixY * vector; // rotate y
                vector = rotationMatrixX * vector; // rotate x

                vector3d.x = vector.get(0,0);
                vector3d.y = vector.get(1,0);
                vector3d.z = vector.get(2,0);

                vector3d.z =+ 10.f; // apply distance to camera
            }
        }

        // --- PROJECTION ---

        float n = 0.1f; // near plane distance
        float f = 1000.f; // far plane distance
        float a = (float)windowHeight / (float)windowWidth; // viewing aspect ratio
        float v = 1.f / tanf(fieldOfView * 0.5f / 180.f * 3.14159f); // field of view scalar
        Matrix projectionMatrix(4, 4, {
            a*v,  0.f,  0.f,          0.f,
            0.f,  v,    0.f,          0.f,
            0.f,  0.f,  f/(f-n),      1.f,
            0.f,  0.f,  (-f*n)/(f-n), 0.f
        });

        for (const Triangle3D& triangle3d : meshCubeTransformed.triangles) {
            Triangle2D triangle2d;

            for (int i = 0; i != 3; ++i){
                Matrix vector(triangle3d.vertices[i]);

                vector.vectorAppend(1);
                vector = projectionMatrix * vector;

                float w = vector.vectorRemove();
                if (w == 0) w = n;
                Matrix normalizationMatrix(3, 3, {
                    1.f/w, 0.f,   0.f,
                    0.f,   1.f/w, 0.f,
                    0.f,   0.f,   1.f/w
                });
                
                vector = normalizationMatrix * vector;

                triangle2d.points[i].x = (vector.get(0,0) + 1.f) * 0.5f * (float)windowWidth; // scale vector onto window
                triangle2d.points[i].y = (vector.get(1,0) + 1.f) * 0.5f * (float)windowHeight; // scale vector onto window
            }
            triangles.push_back(triangle2d);
        }
        WindowDraw(triangles);
    }
}


// --- WIN32 & GDI: WINDOW MANAGEMENT ---

// General window variables
std::string windowTitle = "Laurin's 3D Renderer";
COLORREF backgroundColor = RGB(0, 0, 0);
// UINT windowWidth = 1080; (defined above)
// UINT windowHeight = 720; (defined above)

// POINT mousePos = { 0, 0 }; (defined above)
int frames = 0;
HWND hwnd = NULL;
HDC hdc = NULL;
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;

void WindowDraw(std::vector<Triangle2D> &triangles) {
    // Clear the screen with a background color
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
        FillRgn(hdcBuffer, hRgn, hBrush);

        // Draw triangle outline for debugging
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
        SelectObject(hdcBuffer, hPen);
        MoveToEx(hdcBuffer, triangle.points[0].x, triangle.points[0].y, NULL);
        LineTo(hdcBuffer, triangle.points[1].x, triangle.points[1].y);
        MoveToEx(hdcBuffer, triangle.points[1].x, triangle.points[1].y, NULL);
        LineTo(hdcBuffer, triangle.points[2].x, triangle.points[2].y);
        MoveToEx(hdcBuffer, triangle.points[2].x, triangle.points[2].y, NULL);
        LineTo(hdcBuffer, triangle.points[0].x, triangle.points[0].y);

        DeleteObject(hRgn);
        DeleteObject(hBrush);
        DeleteObject(hPen);
    }
    // Copy the buffer to screen
    BitBlt(hdc, 0, 0, windowWidth, windowHeight, hdcBuffer, 0, 0, SRCCOPY);
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

// Window procedure to handle messages for the window
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
        case WM_SIZE: {
            windowWidth = LOWORD(lParam);
            windowHeight = HIWORD(lParam);
            // Release buffer
            DeleteObject(hBitmap);
            DeleteDC(hdcBuffer);
            // Create new buffer
            hdcBuffer = CreateCompatibleDC(NULL);
            hBitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
            SelectObject(hdcBuffer, hBitmap);
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
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, // Size and position
        NULL, NULL, hInstance, NULL // Parent window, menu, instance handle, additional app data
    );
    ShowWindow(hwnd, nCmdShow);
    hdc = GetDC(hwnd);

    // Initialize buffer
    hdcBuffer = CreateCompatibleDC(NULL);
    hBitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
    SelectObject(hdcBuffer, hBitmap);

    // Set FPS Timer
    SetTimer(hwnd, 1, 1000, NULL);

    // *** Start main process ***
    rendererMain();

    DeleteObject(hBitmap);
    DeleteDC(hdcBuffer);
    DeleteDC(hdc);
    return 0;
}

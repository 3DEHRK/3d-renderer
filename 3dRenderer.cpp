#include <windows.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <iostream>


// --- SPACE STRUCTURES ---

struct Vector3D {
    float x, y, z;
};

struct Triangle3D {
    Vector3D& operator[](int i) { return vertices[i]; }
    Vector3D operator[](int i) const { return vertices[i]; }

    Vector3D vertices[3];
};

struct Mesh3D {
    Mesh3D() {}
    Mesh3D(std::initializer_list<Triangle3D> initList) : triangles(initList) {}

    std::vector<Triangle3D> triangles;
};

struct Triangle2D {
    Triangle2D() {}
    Triangle2D(bool isRendered) : isRendered(isRendered) {}

    POINT points[3];
    int shade;
    int r, g, b;
    float z;
    bool isRendered = true;
};


// --- MATRIX TYPE ---

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

    Matrix operator*(const Matrix& other) const {
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
    Matrix operator+(const Matrix& other) const {
        if (m != other.m || n != other.n) throw std::out_of_range("Matrices are of unequal dimensions and cannot be added.");
        Matrix sum(m, n);
        for (int i = 0; i != m; ++i) {
            for (int j = 0; j != n; ++j) {
                sum.set(i, j, get(i, j) + other.get(i, j));
            }
        }
        return sum;
    }
    Matrix operator-(const Matrix& other) const {
        if (m != other.m || n != other.n) throw std::out_of_range("Matrices are of unequal dimensions and cannot be subtracted.");
        Matrix difference(m, n);
        for (int i = 0; i != m; ++i) {
            for (int j = 0; j != n; ++j) {
                difference.set(i, j, get(i, j) - other.get(i, j));
            }
        }
        return difference;
    }
    Matrix T() const {
        Matrix transpose(n, m);
        for (int i = 0; i != m; ++i) {
            for (int j = 0; j != n; ++j) {
                transpose.set(j, i, get(i, j));
            }
        }
        return transpose;
    }
    float det() const {
        if (m != 2 || n != 2) throw std::out_of_range("Determinant can only be calculated on 2x2 matrix.");
        return get(0,0) * get(1,1) - get(0,1) * get(1,0);
    }
    float magnitude() const {
        if (n != 1) throw std::out_of_range("Magnitude can only be calculated on a vector.");
        float accu = 0;
        for (int i = 0; i != m; ++i) {
            accu += get(i,0)*get(i,0);
        }
        return sqrt(accu);
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
    float x() const { return get(0,0); }
    float y() const { return get(1,0); }
    float z() const { return get(2,0); }
    void x(float x) { set(0,0,x); }
    void y(float y) { set(1,0,y); }
    void z(float z) { set(2,0,z); }
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


// --- OBJ FILE LOADING ---

Mesh3D loadObj (const std::string path) {
    std::ifstream file(path);

    if(!file.is_open()) {  // show fallback cube if no file
        return {
            // South Face
            { -10.f, -10.f, -10.f,   -10.f, 10.f, -10.f,   10.f, 10.f, -10.f},
            { -10.f, -10.f, -10.f,   10.f, 10.f, -10.f,   10.f, -10.f, -10.f},
            // East face
            { 10.f, -10.f, -10.f,   10.f, 10.f, -10.f,   10.f, 10.f, 10.f },
            { 10.f, -10.f, -10.f,   10.f, 10.f, 10.f,   10.f, -10.f, 10.f },
            // North face
            { 10.f, -10.f, 10.f,   10.f, 10.f, 10.f,   -10.f, 10.f, 10.f },
            { 10.f, -10.f, 10.f,   -10.f, 10.f, 10.f,   -10.f, -10.f, 10.f },
            // West face
            { -10.f, -10.f, 10.f,   -10.f, 10.f, 10.f,   -10.f, 10.f, -10.f },
            { -10.f, -10.f, 10.f,   -10.f, 10.f, -10.f,   -10.f, -10.f, -10.f },
            // Top face
            { -10.f, 10.f, -10.f,   -10.f, 10.f, 10.f,   10.f, 10.f, 10.f },
            { -10.f, 10.f, -10.f,   10.f, 10.f, 10.f,   10.f, 10.f, -10.f },
            // Bottom face
            { 10.f, -10.f, 10.f,   -10.f, -10.f, 10.f,   -10.f, -10.f, -10.f },
            { 10.f, -10.f, -10.f,   10.f, -10.f, 10.f,   -10.f, -10.f, -10.f }
        };
    }

    // accumulate from reading file
    std::vector<Triangle3D> triangles;
    std::vector<Vector3D> vertices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefixToken;
        iss >> prefixToken;

        if(prefixToken == "v") {
            Vector3D vector3d;
            iss >> vector3d.x >> vector3d.y >> vector3d.z;
            vertices.push_back(vector3d);
        }

        if(prefixToken == "f") {
            Triangle3D triangle3d;
            std::string vertex[3];
            iss >> vertex[0] >> vertex[1] >> vertex[2];
            triangle3d[0] = vertices[std::stoi(vertex[0])-1]; // stoi() ignores /vt/vn
            triangle3d[1] = vertices[std::stoi(vertex[1])-1];
            triangle3d[2] = vertices[std::stoi(vertex[2])-1];
            triangles.push_back(triangle3d);
        }
    }
    file.close();

    Mesh3D mesh;
    for (Triangle3D triangle3d : triangles) {
        mesh.triangles.push_back(triangle3d);
    }
    return mesh;
}


// --- RENDERING ---

UINT windowWidth = 1080;
UINT windowHeight = 720;
POINT mousePos = { 0, 0 };

float fieldOfView = 90.f;
float distanceToCamera = 20.f;

Triangle2D renderTrianglePipeline(Triangle3D triangle3d) {
    Triangle2D triangle2d;
    triangle2d.r = triangle3d[0].x*17;
    triangle2d.g = triangle3d[0].y*17;
    triangle2d.b = triangle3d[0].z*17;

    // --- TRANSFORMATION ---

    float z = (float) mousePos.x * 10.f / (float) windowWidth; // z rotation
    Matrix rotationMatrixZ(3, 3, {
        cosf(z), -sinf(z),  0.f,
        sinf(z),  cosf(z),  0.f,
        0.f,          0.f,  1.f
    });

    float x = (float) mousePos.y * 10.f / (float) windowHeight; // x rotation
    Matrix rotationMatrixX(3, 3, {
        1.f,  0.f,          0.f,
        0.f,  cosf(x), -sinf(x),
        0.f,  sinf(x),  cosf(x)
    });

    for (Vector3D& vector3d : triangle3d.vertices) {
        Matrix vector(vector3d);

        vector = rotationMatrixX * vector; // rotate x
        vector = rotationMatrixZ * vector; // rotate z

        vector3d.x = vector.x();
        vector3d.y = vector.y();
        vector3d.z = vector.z();

        vector3d.z += distanceToCamera; // apply distance to camera
    }

    // approximate depth
    triangle2d.z = (triangle3d[0].z + triangle3d[1].z + triangle3d[2].z) / 3;

    // --- SHADING & VISIBILITY ---

    Matrix lightDirVector(3,1, {
        0.0f,
        0.0f,
        -0.75f
    });
    Matrix cameraLocVector(3,1, {
        0.0f,
        0.0f,
        0.0f
    });
    int ambientLight = 25;

    // calculate normals using cross product
    // shade and sort out based on similarity using dot product

    // Make relative lines from Triangle
    Matrix vectorA = Matrix(triangle3d[0]) - Matrix(triangle3d[1]);
    Matrix vectorB = Matrix(triangle3d[0]) - Matrix(triangle3d[2]);

    // Make component pairs for 3d cross product
    Matrix xPair(2,2,{vectorA.y(), vectorB.y(),
                        vectorA.z(), vectorB.z()});

    Matrix yPair(2,2,{vectorA.x(), vectorB.x(),
                        vectorA.z(), vectorB.z()});

    Matrix zPair(2,2,{vectorA.x(), vectorB.x(),
                        vectorA.y(), vectorB.y()});

    Matrix normal(3,1,{xPair.det(),
                        -yPair.det(),
                        zPair.det()});

    // normalize normal hihi
    float normalMagnitude = normal.magnitude();
    normal.x(normal.x() / normalMagnitude);
    normal.y(normal.y() / normalMagnitude);
    normal.z(normal.z() / normalMagnitude);

    // apply shading using dot product
    Matrix similarityShade = lightDirVector.T() * normal;
    triangle2d.shade = similarityShade.get(0,0) * (255 - ambientLight) + ambientLight;

    // remove hidden triangles using dot product
    Matrix cameraToTriangleVector(cameraLocVector + Matrix(triangle3d[0]));

    // normalize to unit vector
    float cttvMagnitude = cameraToTriangleVector.magnitude();
    cameraToTriangleVector.x(cameraToTriangleVector.x() / cttvMagnitude);
    cameraToTriangleVector.y(cameraToTriangleVector.y() / cttvMagnitude);
    cameraToTriangleVector.z(cameraToTriangleVector.z() / cttvMagnitude);

    Matrix similarityView = cameraToTriangleVector.T() * normal;
    if (similarityView.get(0,0) >= 0.f || triangle3d[0].z < 0.f)
        return Triangle2D(false);

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

    // project each vector and normalize them by z(w), creating perspective
    for (int i = 0; i != 3; ++i){
        Matrix vector(triangle3d[i]);

        vector.vectorAppend(1.f);
        vector = projectionMatrix * vector;

        float w = vector.vectorRemove();
        if (w == 0) w = n;
        Matrix normalizationMatrix(3, 3, {
            1.f/w, 0.f,   0.f,
            0.f,   1.f/w, 0.f,
            0.f,   0.f,   1.f/w
        });
        
        vector = normalizationMatrix * vector;

        triangle2d.points[i].x = (vector.x() + 1.f) * 0.5f * (float)windowWidth; // scale vector onto window
        triangle2d.points[i].y = (vector.y() + 1.f) * 0.5f * (float)windowHeight; // scale vector onto window
    }

    return triangle2d;
}


// --- MAIN FUNCTION ---

std::mutex renderingMutex;

void renderThread(const Mesh3D& mesh, std::vector<Triangle2D>& triangles2d, int start, int end) {
    for (int i = start; i < end; ++i) {
        const Triangle3D& triangle3d = mesh.triangles[i];

        const Triangle2D& triangle2d = renderTrianglePipeline(triangle3d);
        if (!triangle2d.isRendered)
            continue;

        std::lock_guard<std::mutex> lock(renderingMutex);
        triangles2d.push_back(triangle2d);
    }
}

bool WindowProcess();
void WindowDraw(const std::vector<Triangle2D> &triangles);
int totalTriangles = 0;

void rendererMain() {
    Mesh3D mesh = loadObj("demo.obj");
    totalTriangles = mesh.triangles.size();

    std::thread drawingThread = std::thread([]{});
    const int numThreads = std::thread::hardware_concurrency() - 1;

    while (WindowProcess()) {
        std::vector<Triangle2D> triangles2d;
        std::vector<std::thread> renderingThreads;
        const int trianglesPerThread = mesh.triangles.size() / numThreads;

        // Rrendering threads
        for (int i = 0; i < numThreads; ++i) {
            int start = i * trianglesPerThread;
            int end = (i == numThreads - 1) ? mesh.triangles.size() : (i + 1) * trianglesPerThread;
            renderingThreads.emplace_back(renderThread, std::ref(mesh), std::ref(triangles2d), start, end);
        }
        for (std::thread &renderingThread : renderingThreads) {
            renderingThread.join();
        }

        // introsort triangles by approximated depth for proper overlapping (painter's algorithm)
        std::sort(triangles2d.begin(), triangles2d.end(), [](const Triangle2D& triangle1, const Triangle2D& triangle2) -> bool {
            return triangle1.z > triangle2.z;
        });

        // Drawing thread
        drawingThread.join();
        drawingThread = std::thread(WindowDraw, triangles2d);
    }

    drawingThread.join();
}


// --- WIN32 & GDI: WINDOW MANAGEMENT ---

// General window variables
std::string windowTitle = "Laurin's 3D Renderer";
COLORREF backgroundColor = RGB(0, 0, 0);
int renderingMode = 0;
// UINT windowWidth = 1080; (defined above)
// UINT windowHeight = 720; (defined above)

// POINT mousePos = { 0, 0 }; (defined above)
int frames = 0;
int visibleTriangles = 0;
HWND hwnd = NULL;
HDC hdc = NULL;
HDC hdcBuffer = NULL;
HBITMAP hBitmap = NULL;

void WindowDraw(const std::vector<Triangle2D> &triangles) {
    // Clear the screen with a background color
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    HBRUSH hBackgroundBrush = CreateSolidBrush(backgroundColor);
    FillRect(hdcBuffer, &clientRect, hBackgroundBrush);
    DeleteObject(hBackgroundBrush);

    // Texture experiments
    HBITMAP bitmap = (HBITMAP)LoadImage(NULL, "texture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, bitmap);
    BITMAP bm;
    GetObject(bitmap, sizeof(bm), &bm);

    for (const Triangle2D& triangle : triangles) {
        POINT point1 = {200,200};
        POINT point2 = {100,200};
        POINT point3 = {300,300};
        Triangle2D tri;
        tri.points[0] = point1;
        tri.points[1] = point2;
        tri.points[2] = point3;
        tri = triangle;

        // rasterization (that isnt so good yet)

        std::sort(tri.points, tri.points + 3, [](POINT& a, POINT& b) -> bool {
            return a.y < b.y;
        });

        bool swapped = false;
        POINT xstart = tri.points[1];
        POINT xend = tri.points[2];
        if (xstart.x > xend.x) {
            std::swap(xstart, xend);
            swapped = true;
        }

        float slopeL = 0;
        float slopeR = 0;
        if (xstart.y - tri.points[0].y != 0)
            slopeL = (float)(tri.points[0].x - xstart.x) / (float)(xstart.y - tri.points[0].y);
        if (xend.y - tri.points[0].y != 0)
            slopeR = (float)(xend.x - tri.points[0].x) / (float)(xend.y - tri.points[0].y);

        //std::cout << slopeL << " " << slopeR << ". ";
        
        int lastEnd = tri.points[0].x;
        int lastStart = xstart.x;
        // cleanness: store progress in vars instead

        for (int y = tri.points[0].y; y < tri.points[1].y; y++) {
            for (int x = tri.points[0].x - slopeL * (y - tri.points[0].y); x < tri.points[0].x + slopeR * (y - tri.points[0].y); x++) {
                SetPixel(hdcBuffer, x, y, RGB(tri.shade, tri.shade, tri.shade));
            }
            lastStart = tri.points[0].x - slopeL * (y - tri.points[0].y);
            lastEnd = tri.points[0].x + slopeR * (y - tri.points[0].y);
        }

        // slope r or l that changes?
        if (!swapped) {
            if (xend.y - tri.points[1].y)
                slopeL = (float)(tri.points[1].x - xend.x) / (float)(xend.y - tri.points[1].y);
        } else {
            if (xstart.y - tri.points[1].y)
                slopeR = (float)(xstart.x - tri.points[1].x) / (float)(xstart.y - tri.points[1].y);
        }

        for (int y = tri.points[1].y; y < tri.points[2].y; y++) {
            for (int x = lastStart - slopeL * (y - tri.points[1].y); x < lastEnd + slopeR * (y - tri.points[1].y); x++) {
                SetPixel(hdcBuffer, x, y, RGB(tri.shade, tri.shade, tri.shade));
            }
        }

    }
    BitBlt(hdc, 0, 0, windowWidth, windowHeight, hdcBuffer, 0, 0, SRCCOPY);
    frames++;
    visibleTriangles = triangles.size();

    DeleteObject(hdcMem);
    DeleteObject(bitmap);
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
        case WM_KEYDOWN: {
            if (wParam == VK_UP)
                distanceToCamera -= 5.f;
            if (wParam == VK_DOWN)
                distanceToCamera += 5.f;
            if (wParam == VK_TAB)
                renderingMode++;
            return 0;
        }
        case WM_MOUSEMOVE: {
            mousePos.x = LOWORD(lParam);
            mousePos.y = HIWORD(lParam);
            return 0;
        }
        case WM_TIMER: {
            const std::string title = windowTitle + " | TRIS " + std::to_string(visibleTriangles) + "/" + std::to_string(totalTriangles) + " | FPS " + std::to_string(frames);
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
    //FreeConsole();

    // Initial placeholder screen
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(20,20,20));
    FillRect(hdc, &clientRect, hBackgroundBrush);
    DeleteObject(hBackgroundBrush);

    // Initialize buffer
    hdcBuffer = CreateCompatibleDC(NULL);
    hBitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
    SelectObject(hdcBuffer, hBitmap);

    // Set FPS Timer
    SetTimer(hwnd, 1, 1000, NULL);

    // Start main process
    rendererMain();

    DeleteObject(hBitmap);
    DeleteDC(hdcBuffer);
    DeleteDC(hdc);
    return 0;
}

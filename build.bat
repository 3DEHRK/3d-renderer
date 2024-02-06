@echo off

g++ -o 3dRenderer.exe 3dRenderer.cpp -O2 -static-libgcc -static-libstdc++ -lgdi32 -static -lpthread

if errorlevel 1 (
	pause
) else (
	start 3dRenderer.exe
)

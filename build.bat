@echo off

g++ -static-libgcc -static-libstdc++ -mwindows -o 3dRenderer.exe main.cpp matrices.cpp -lgdi32

if errorlevel 1 (
	echo Make sure Mingw32 is installed properly, check readme.md
	pause
) else (
	.\3dRenderer.exe
)

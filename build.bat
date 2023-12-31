@echo off

g++ -static-libgcc -static-libstdc++ -mwindows -o 3dRenderer.exe 3dRenderer.cpp -lgdi32

if errorlevel 1 (
	echo Make sure Mingw32 is installed and path variable set
	echo https://github.com/niXman/mingw-builds-binaries/releases
	pause
) else (
	.\3dRenderer.exe
)

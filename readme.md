# Prerequisites for Development

## 1. Setup MinGW Compiler (5 min)
### 1. Download MinGW 32-bit compiler from: https://www.sfml-dev.org/download/sfml/2.6.0/ so its version matches the SFML
### 2. Move to e.g. C:/mingw32
### 3. Open system environment variables and add C:/mingw32/bin to the Path variable

## 2. Install cmake or build via cmd:
### g++ -o bin/app.exe src/main.cpp -I SFML-2.6.0/include -L SFML-2.6.0/lib -lsfml-system -lsfml-window -lsfml-graphics

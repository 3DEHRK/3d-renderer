# Dev env setup guide: Using MinGW & SFML on Windows (10 min)

## Setup MinGW (5 min)
### 1. Download MinGW 32-bit compiler from: https://www.sfml-dev.org/download/sfml/2.6.0/ so its version matches the SFML
### 2. Move to e.g. C:/mingw32
### 3. Open system environment variables and add C:/mingw32/bin to the Path variable

## Setup SFML (3 min)
### 1. Download 32-bit SFML library: https://www.sfml-dev.org/download/sfml/2.6.0/
### 2. Move to e.g. C:/Users/laz/SFML-2.6.0
### 3. Copy DLLs from SFML-2.6.0/bin to the app.exe location 

## Setup compilation command (2 min)
### 1. Modify as needed
### g++ -o 'C:/Users/laz/VS code projects/app.exe' 'C:/Users/laz/VS code projects/main.cpp' -I C:/Users/laz/SFML-2.6.0/include -L C:/Users/laz/SFML-2.6.0/lib -lsfml-system -lsfml-window -lsfml-graphics

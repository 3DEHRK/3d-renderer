@echo off
echo Setting up CMake...
cmake -G "MinGW Makefiles" .
cmake .
echo CMake setup completed.
pause
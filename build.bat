@echo off
REM build command is at file end

>nul 2>&1 g++
if %errorlevel% neq 0 (
    echo Mingw32 g++ not found!
	
	net session >nul 2>&1
	if %errorlevel% neq 0 (
		powershell -command "& {Start-Process '%0' -Verb RunAs}"
		exit /b
	)

	echo Initiating Mingw32 installation...
	set "downloadURL=https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/i686-13.2.0-release-mcf-dwarf-ucrt-rt_v11-rev1.7z"
	set "downloadLocation=%TEMP%\download.7z"
	set "extractedFolder=%TEMP%\extracted"

	echo Downloading file from %downloadURL%...
	powershell -Command "(New-Object System.Net.WebClient).DownloadFile('%downloadURL%', '%downloadLocation%')"

	echo Extracting contents...
	if not exist "%extractedFolder%" mkdir "%extractedFolder%"
	"%ProgramFiles%\7-Zip\7z.exe" x "%downloadLocation%" -o"%extractedFolder%" -y

	echo Copying mingw32 folder to C:\mingw32...
	if not exist "C:\mingw32\" mkdir "C:\mingw32\"
	xcopy /E /I "%extractedFolder%\mingw32" "C:\mingw32\"

	echo Adding C:\mingw32\bin to the PATH...
	setx PATH "%PATH%;C:\mingw32\bin" /M > nul

	echo Cleaning up temporary files...
	del /F /Q "%downloadLocation%"
	rmdir /Q /S "%extractedFolder%"

	echo Mingw32 installation completed.
	pause

)

g++ -static-libgcc -static-libstdc++ -mwindows -o 3dRenderer.exe main.cpp matrices.cpp -lgdi32
.\3dRenderer.exe
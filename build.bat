@echo off
setlocal
cd /d "C:\Users\User\Desktop\x86_64-15.2.0-release-posix-seh-msvcrt-rt_v13-rev0\mingw64\bin"

g++ "%~dp0*.cpp" -o "%~dp0PROGRAM_FOLDER\3d_FluidSimMOD.exe" -Wall -lgdi32 -O2 -static -static-libgcc -static-libstdc++
if errorlevel 1 (
	pause
	exit /b 1
)
start "" /d "%~dp0PROGRAM_FOLDER" "%~dp0PROGRAM_FOLDER\3d_FluidSimMOD.exe"
exit /b 0
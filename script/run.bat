@echo off
set FILE_NAME=%1
set ARCH=%2
set ARCH=%ARCH:amd64=X64%
set ARCH=%ARCH:x86=X86%
set MODE=%3
set VC_VERSION=vc14
set OPENCV_VERSION=410
set OPENCV_VERSION_DOT=4.1.0

cd bin
set PATH=%PATH%;C:\Program Files (x86)\Windows Kits\10\bin\%ARCH%\ucrt
set PATH=%PATH%;..\opencv-%OPENCV_VERSION_DOT%\%ARCH%\%VC_VERSION%\bin

if "%MODE%" == "Run" (
    @echo ^> "%~n1.exe"
    call "%~n1.exe" && pause
) else if "%MODE%" == "Terminal" (
    call "%COMSPEC%" /k
)
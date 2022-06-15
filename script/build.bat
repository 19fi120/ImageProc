@echo off
set FILE_NAME=%1
set ARCH=%2
set DEBUG=%3
set MODE=%4
set VC_VERSION=vc14
set OPENCV_VERSION=410
set OPENCV_VERSION_DOT=4.1.0

cd bin
if "%ARCH%" == "amd64" (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
    )
) else if "%ARCH%" == "x86" (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars32.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
    )
)

echo ========== Build started: Configuration: %DEBUG% %ARCH% ==========

set ARCH=%ARCH:amd64=X64%
set ARCH=%ARCH:x86=X86%
set DEBUG=%DEBUG:Debug=d%
set DEBUG=%DEBUG:Release=%
echo ^> cl.exe /nologo /EHsc /MD%DEBUG% /I"..\opencv-%OPENCV_VERSION_DOT%\include" /source-charset:utf-8 %FILE_NAME% /link /MACHINE:%ARCH% "..\opencv-%OPENCV_VERSION_DOT%\%ARCH%\%VC_VERSION%\lib\opencv_world%OPENCV_VERSION%%DEBUG%.lib"
call cl.exe /nologo /EHsc /MD%DEBUG% /I"..\opencv-%OPENCV_VERSION_DOT%\include" /source-charset:utf-8 %FILE_NAME% /link /MACHINE:%ARCH% "..\opencv-%OPENCV_VERSION_DOT%\%ARCH%\%VC_VERSION%\lib\opencv_world%OPENCV_VERSION%%DEBUG%.lib"

if "%ERRORLEVEL%" == "0" (
    del "%~n1.obj"
    echo ========== Build: succeeded ==========
    if "%MODE%" == "Run" (
        cd ..\
        call .\script\boot.bat .\script\run.bat %1 %2 %4
    )
) else (
    echo ========== Build: failed ==========
)
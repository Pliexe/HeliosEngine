@echo off

IF "%~1" == "" GOTO PrtHelp
IF "%~1" == "/?" GOTO PrtHelp
IF "%~1" == "-?" GOTO PrtHelp
IF "%~1" == "/h" GOTO PrtHelp
IF "%~1" == "-h" GOTO PrtHelp
IF "%~1" == "/help" GOTO PrtHelp
IF "%~1" == "-help" GOTO PrtHelp

IF "%~1" == "compile" GOTO Compile

vendor\premake\bin\premake5.exe %1
GOTO Done

:PrtHelp

echo.
echo Usage: build.bat [command]
echo.
echo Commands:
echo.
echo   compile       - Compile the project
echo   gmake         - Generate GNU makefiles for POSIX, MinGW, and Cygwin
echo   gmake2        - Generate GNU makefiles for POSIX, MinGW, and Cygwin
echo   xcode4        - Generate Apple Xcode 4 project files
echo   codelite      - Generate CodeLite project files
echo   vs2015        - Generate Visual Studio 2015 project files
echo   vs2017        - Generate Visual Studio 2017 project files
echo   vs2019        - Generate Visual Studio 2019 project files
echo   vs2022        - Generate Visual Studio 2022 project files
echo.
echo   clean         - Clean project files
echo.
echo For additional information, see the Premake User Guide: https://premake.github.io/docs/
echo.

GOTO Done

:Compile

vendor\premake\bin\premake5.exe vs2022

if not defined DevEnvDir (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)

set soltionfile="HeliosEngine.sln"
msbuild %soltionfile% /t:Build /p:Configuration=Debug /p:Platform=x64

:Done
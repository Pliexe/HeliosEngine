set +v

premake5() {
    ./vendor/premake/bin/premake5 --cc=clang $@
}

prt_help()
{
    echo ''
    echo Usage: $0 [command]
    echo ''
    echo Commands:
    echo ''
    echo   compile       - Compile the project
    echo   gmake         - Generate GNU makefiles for POSIX, MinGW, and Cygwin
    echo   gmake2        - Generate GNU makefiles for POSIX, MinGW, and Cygwin
    echo   xcode4        - Generate Apple Xcode 4 project files
    echo   codelite      - Generate CodeLite project files
    echo   vscode        - Generate Visual Studio Code project files
    echo   vs2015        - Generate Visual Studio 2015 project files
    echo   vs2017        - Generate Visual Studio 2017 project files
    echo   vs2019        - Generate Visual Studio 2019 project files
    echo   vs2022        - Generate Visual Studio 2022 project files
    echo ''
    echo   clean         - Clean project files
    echo ''
    echo For additional information, see the Premake User Guide: https://premake.github.io/docs/
    echo ''
}

case $1 in
    compile)
        echo "Compiling..."
        premake5 gmake
        make
        ;;
    cmake)
        echo "Generating CMake project files..."
        premake5 cmake
        ;;
    gmake)
        echo "Generating GNU makefiles..."
        premake5 gmake
        ;;
    gmake2)
        echo "Generating GNU makefiles..."
        premake5 gmake2
        ;;
    xcode4)
        echo "Generating Apple Xcode 4 project files..."
        premake5 xcode4
        ;;
    codelite)
        echo "Generating CodeLite project files..."
        premake5 codelite
        ;;
    vs2015)
        echo "Generating Visual Studio 2015 project files..."
        premake5 vs2015
        ;;
    vs2017)
        echo "Generating Visual Studio 2017 project files..."
        premake5 vs2017
        ;;
    vs2019)
        echo "Generating Visual Studio 2019 project files..."
        premake5 vs2019
        ;;
    vs2022)
        echo "Generating Visual Studio 2022 project files..."
        premake5 vs2022
        ;;
    vscode)
        echo "Generating Visual Studio Code project files..."
        premake5 gmake2
        premake5 vscode
        code HeliosEngine.code-workspace
        ;;
    clean)
        echo "Cleaning project files..."
        premake5 clean
        ;;
    *)
        prt_help
        ;;
esac

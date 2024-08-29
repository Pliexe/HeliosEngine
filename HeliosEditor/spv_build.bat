@echo off
setlocal enabledelayedexpansion

:: Directory containing HLSL shader files
set SHADERS_DIR=.\Shaders

:: Output directory for SPIR-V files
set OUTPUT_DIR=.\CompiledShaders

:: Create output directory if it doesn't exist
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

:: Loop through all HLSL shader files
for %%F in (%SHADERS_DIR%\*VertexShader.hlsl) do (
    :: Extract the base name of the shader file
    set FILE_BASENAME=%%~nxF
    
    echo Compiling %%~nxF.hlsl to %OUTPUT_DIR%\%%~nF.spv using dxc

    :: Compile the shader to SPIR-V using dxc
    dxc -spirv -T vs_5_0 -E main %%F -Fo %OUTPUT_DIR%\%%~nF.spv
)

:: Loop through all HLSL shader files
for %%F in (%SHADERS_DIR%\*PixelShader.hlsl) do (
    :: Extract the base name of the shader file
    set FILE_BASENAME=%%~nxF
    
    :: Compile the shader to SPIR-V using dxc
    dxc -spirv -T ps_5_0 -E main %%F -Fo %OUTPUT_DIR%\%%~nF.spv
)

:: Loop through all HLSL shader files
for %%F in (%SHADERS_DIR%\*GeometryShader.hlsl) do (
    :: Extract the base name of the shader file
    set FILE_BASENAME=%%~nxF
    
    :: Compile the shader to SPIR-V using dxc
    dxc -spirv -T gs_5_0 -E main %%F -Fo %OUTPUT_DIR%\%%~nF.spv
)

echo Compilation completed.
pause

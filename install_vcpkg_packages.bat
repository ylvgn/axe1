@REM #change current directory to this file
%~d0
cd %~dp0

@set triplets=x64-windows
@set s_triplets=%triplets%-static
@set VCPKG_ROOT=externals\_vcpkg
@set vcpkg=%VCPKG_ROOT%\vcpkg

git clone https://github.com/microsoft/vcpkg.git %VCPKG_ROOT%

call %VCPKG_ROOT%\bootstrap-vcpkg.bat

%vcpkg% install libpng:%triplets% --recurse
%vcpkg% install libjpeg-turbo:%triplets% --recurse
%vcpkg% install fmt:%triplets% --recurse
%vcpkg% install eastl:%triplets% --recurse
%vcpkg% install nlohmann-json:%triplets% --recurse
%vcpkg% install imgui[docking-experimental]:%triplets% --recurse

@REM just for dl ninja.exe
%vcpkg% install vcpkg-tool-ninja:%triplets% --recurse

@REM spirv-cross, glslc, slangc in vulkan sdk folder: %VK_SDK_PATH%\bin
@REM %vcpkg% install spirv-cross:%triplets% --recurse
@REM %vcpkg% install shaderc:%triplets% --recurse

@REM dxc in windows sdk folder: C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64
@REM Notice: DO NOT Use Vulkan SDK folder's dxc.exe
@REM %vcpkg% install directx-dxc:%triplets% --recurse

@REM manually build in externals
@REM %vcpkg% install directx-headers:%triplets% --recurse

%vcpkg% install tracy:%triplets% --recurse

@pause
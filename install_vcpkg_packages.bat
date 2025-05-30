@REM #change current directory to this file
%~d0
cd %~dp0

@set triplets=x64-windows
@set VCPKG_ROOT=externals\_vcpkg
@set vcpkg=%VCPKG_ROOT%\vcpkg

git clone https://github.com/microsoft/vcpkg.git %VCPKG_ROOT%

call %VCPKG_ROOT%\bootstrap-vcpkg.bat

%vcpkg% install fmt:%triplets% --recurse
%vcpkg% install eastl:%triplets% --recurse
%vcpkg% install nlohmann-json:%triplets% --recurse
%vcpkg% install libpng:%triplets% --recurse
%vcpkg% install mozjpeg:%triplets% --recurse
%vcpkg% install vcpkg-tool-ninja:%triplets% --recurse
%vcpkg% install tracy:%triplets% --recurse
%vcpkg% install imgui[docking-experimental]:%triplets% --recurse
%vcpkg% install spirv-cross:%triplets% --recurse
%vcpkg% install shaderc:%triplets% --recurse
%vcpkg% install directx-dxc:%triplets% --recurse

@pause
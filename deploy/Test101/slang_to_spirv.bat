@REM #change current directory to this file
@%~d0
@cd %~dp0

@REM slangc: https://github.com/shader-slang/slang
@REM spirv-cross: https://github.com/KhronosGroup/SPIRV-Cross

@setlocal

@REM set slangc=C:\VulkanSDK\1.4.341.1\Bin\slangc.exe
@set slangc=%VK_SDK_PATH%\Bin\slangc.exe

@REM set spirv_cross=C:\VulkanSDK\1.4.341.1\Bin\spirv-cross.exe
@set spirv_cross=%VK_SDK_PATH%\Bin\spirv-cross.exe

@set SPIRV_PROFILE=spirv_1_4
@set VS_ENTRY=vs_main
@set PS_ENTRY=ps_main

@IF NOT EXIST %slangc% (
	@echo "'slangc' path not found error: please install vulkan sdk https://vulkan.lunarg.com/sdk/home"
	@pause
	@exit -1
)

@IF NOT EXIST %spirv_cross% (
	@echo "'spirv-cross' path not found error: lease install vulkan sdk https://vulkan.lunarg.com/sdk/home"
	@pause
	@exit -1
)


@call :SLANG2JSON %1

@REM @pause
@REM exit %ERRORLEVEL%
goto :eof

@REM helper function ===================================
:SLANG2SPIRV
	@set MY_SPV_OUTPUT=%1.bin

	%slangc% "%1" ^
		-target spirv ^
		-profile %SPIRV_PROFILE% ^
		-fvk-use-entrypoint-name ^
		-entry %VS_ENTRY% -stage vertex ^
		-entry %PS_ENTRY% -stage fragment ^
		-o "%MY_SPV_OUTPUT%"
	@echo off && call :ABORT_IF_ERROR
@exit /B %ERRORLEVEL%

:SPIRV2JSON
	@set MY_SPV_INPUT=%1.bin
	
	%spirv_cross% "%MY_SPV_INPUT%" ^
		--reflect ^
		--output "%MY_SPV_INPUT%.reflect"
	@echo off && call :ABORT_IF_ERROR
@exit /B %ERRORLEVEL%

:SLANG2JSON
	@call :SLANG2SPIRV %1
	@call :SPIRV2JSON %1
@exit /B %ERRORLEVEL%

:ABORT_IF_ERROR
	if %ERRORLEVEL% neq 0 (
		@goto :ERROR
	)
	@echo on
@exit /B %ERRORLEVEL%

:ERROR
    @echo on
    @echo Failed!!
	@pause
	@exit %ERRORLEVEL%
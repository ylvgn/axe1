
@call :SLANG2SPIRV Demo/hello_triangle.slang

@pause
@exit /B %ERRORLEVEL%

@REM #helper function ===================================
:SLANG2SPIRV
	@REM #change current directory to this file each call
	@%~d0
	@cd %~dp0
	@call .\..\..\slang_to_spirv.bat Assets/Shaders/%1
@exit /B %ERRORLEVEL%
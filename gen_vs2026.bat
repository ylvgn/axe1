@REM #change current directory to this file
@%~d0
@cd %~dp0

cmake -G "Visual Studio 18 2026" ^
	-B _build/vs2026-x64-windows ^
	.

@pause
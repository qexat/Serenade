@echo off
REM $Id$

REM Change this as you want
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat

REM Set current directory
cd %CD%

cl /Fe:Tool/configgen.exe Tool/configgen.c
Tool\configgen.exe config.h

REM Navigate to Serenade directory
cd Serenade

REM Compile each .c file except ffi_binding.c
for %%f in (*.c) do (
    if "%%f" neq "ffi_binding.c" (
        cl /c /D _AMD64_ /Fo:%%~nf.obj %%f
        if errorlevel 1 (
            goto :fail
        )
    )
)

:eof

cl /Fe:serenade.exe *.obj

:fail

REM Return to previous directory
cd ..

cl /LD binmod.c

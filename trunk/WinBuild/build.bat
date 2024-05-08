@echo off
REM $Id$
del winbuild.exe
REM Change as you want
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
cl /Fe:winbuild.exe winbuild.c
winbuild.exe
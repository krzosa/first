@echo off
call ../misc/compile_setup.bat

mkdir build
cd build
cl -Fe:bld.exe ../build_tool/main.cpp -FC -WX -W3 -wd4200 -diagnostics:column -nologo -Zi -D_CRT_SECURE_NO_WARNINGS
cd ..
build\bld.exe

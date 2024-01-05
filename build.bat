@echo off
call ../misc/compile_setup.bat

mkdir build
cd build
cl -Fe:bld.exe ../bld_main.cpp -WX -W3 -wd4200 -diagnostics:column -nologo -Zi -D_CRT_SECURE_NO_WARNINGS
cd ..
build\bld.exe


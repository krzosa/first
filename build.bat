@echo off
mkdir build
cd build
cl -Fe:bld.exe ../build_tool/main.cpp -FC -WX -W3 -wd4200 -diagnostics:column -nologo -Zi -D_CRT_SECURE_NO_WARNINGS
cd ..
rem build\bld.exe clear_cache
build\bld.exe
rem build\bld.exe cc=clang

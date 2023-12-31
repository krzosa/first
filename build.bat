@echo off
call ../misc/compile_setup.bat

set DEBUG=-Od -RTC1 -D_DEBUG -MTd -fsanitize=address
set RELEASE=-O2 -MT -DNDEBUG -GL
set COMMON=-MP -FC -Z7 -GF -Gm- -Oi -Zo -EHa- -GR-
set WRN=-WX -W3 -wd4200 -diagnostics:column -nologo -D_CRT_SECURE_NO_WARNINGS
set LINK_DEBUG=-NODEFAULTLIB:LIBCMT
set LINK_RELEASE=-opt:ref -opt:icf -ltcg

set DEBUG_LINE=%DEBUG% %WRN% %COMMON% -link -incremental:no %LINK_DEBUG%
set RELEASE_LINE=%RELEASE% %WRN% %COMMON% -link -incremental:no %LINK_RELEASE%

mkdir build
cd build
cl.exe  -Fe:cpp_debug.exe ../test/main.cpp %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
cl.exe  -Fe:cpp_release.exe ../test/main.cpp %RELEASE_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
cl.exe  -Fe:c_debug.exe ../test/main.c %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
cl.exe  -Fe:main_core_as_header.exe ../core.c ../test/main_core_as_header.cpp %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..

rem rtc1 - runtime error checks
rem gl - whole program optimizations

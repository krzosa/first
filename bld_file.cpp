#include "bld_lib.cpp"

Strs cc = ON_WINDOWS("cl") ON_MAC("clang") ON_LINUX("gcc");

int CompileFiles(Strs files) {
    int result = 0;
    Str exe = FilenameWithoutExt(files[0]);
    Str filestr = Merge(files);
    if (cc == "gcc") {
        result = OS_SystemF("g++ -o %Q.exe %Q -g -Wno-write-strings", exe, filestr);
    }
    else if (cc == "clang") {
        result = OS_SystemF("clang++ -o %Q.exe %Q -g -Wno-writable-strings", exe, filestr);
    }
    else {
        result = OS_SystemF("cl -Fe:%Q.exe %Q -Zi -WX -W3 -wd4200 -diagnostics:column -nologo -D_CRT_SECURE_NO_WARNINGS", exe, filestr);
    }
    return result;
}

int Main() {
    Strs files = ListDir("../test");
    CompileFiles({"../test/main_core_as_header.cpp", "../core.c"});
    For(files) {
        if (S8_Find(it, "test_"_s, 0, 0)) {
            CompileFiles(it);
        }
    }

    return 0;
}

#if 0 // msvc flag reference
int Main() {
    bool debug = true;
    bool release = !debug;
    bool use_std = false;

    Strs cc = "cl.exe";
    Strs flags = Split("-WX -W3 -wd4200 -diagnostics:column -nologo -Z7 -FC -GF -Gm- -Oi -Zo -D_CRT_SECURE_NO_WARNINGS");
    Strs link = Split("-link -incremental:no");
    Strs files = Split("../test/main_core_as_header.cpp ../core.c");
    // files += IfCodeWasModified("../core.c", "../core.obj"); // .o for linux!
    if (use_std) {
        flags += Split("-GR- -EHa-");
    }
    if (!use_std) {
        flags += Split("-EHsc");
    }
    if (release) {
        flags += Split("-O2 -MT -DNDEBUG -GL");
        link += Split("-opt:ref -opt:icf -ltcg");
    }
    if (debug) {
        flags += Split("-Od -D_DEBUG -MTd -fsanitize=address -MTd -RTC1");
        link += Split("-NODEFAULTLIB:LIBCMT");
    }

    Strs objs = {};
    For(files) {
        if (CodeWasModified(it)) {
            int error = Run(cc + it + flags + "-c" + link);
            if (error != 0) return error;
        }

        objs += S8_Format(Perm, "../build/%Q.obj", FilenameWithoutExt(it)); // .o for linux!
    }
    int error = Run(cc + objs + flags + link);
    return error;
}

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

cl.exe  -Fe:bld.exe ../bld_main.cpp %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
bld.exe
if %errorlevel% neq 0 exit /b %errorlevel%

cl.exe  -Fe:test_arena.exe ../test/test_arena.cpp %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
test_arena.exe
if %errorlevel% neq 0 exit /b %errorlevel%

cl.exe  -Fe:test_table.exe ../test/test_table.cpp %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
test_table.exe
if %errorlevel% neq 0 exit /b %errorlevel%

cl.exe  -Fe:test_array.exe ../test/test_array.cpp %DEBUG_LINE%
if %errorlevel% neq 0 exit /b %errorlevel%
test_array.exe
if %errorlevel% neq 0 exit /b %errorlevel%

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

#endif
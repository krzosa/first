#include "build_tool/build_lib.cpp"

void CompileFiles(Strs cc, Strs files);
int ReturnValue = 0;

int Main() {
    Strs cc = CMDLine.get("cc"_s, ON_WINDOWS("cl"_s) ON_MAC("clang"_s) ON_LINUX("gcc"_s));
    Strs files = ListDir("../tests");
    CompileFiles(cc, {"../tests/main_core_as_header.cpp", "../core_library/core.c"});
    For(files) {
        if (S8_Find(it, "test_"_s)) {
            CompileFiles(cc, it);
        }
    }

    return ReturnValue;
}

void CompileFiles(Strs cc, Strs files) {
    int result = 0;
    Str exe = FilenameWithoutExt(files[0]);
    Str filestr = Merge(files);
    if (cc == "gcc") {
        result = OS_SystemF("g++ -o %.*s.exe %.*s -g -Wno-write-strings -fsanitize=address", S8_Expand(exe), S8_Expand(filestr));
    }
    else if (cc == "clang") {
        result = OS_SystemF("clang++ -std=c++11 -o %.*s.exe %.*s -fdiagnostics-absolute-paths -g -Wno-writable-strings -fsanitize=address", S8_Expand(exe), S8_Expand(filestr));
    }
    else {
        result = OS_SystemF("cl -Fe:%.*s.exe %.*s -FC -Zi -WX -W3 -wd4200 -diagnostics:column -nologo -D_CRT_SECURE_NO_WARNINGS -fsanitize=address -RTC1", S8_Expand(exe), S8_Expand(filestr));
    }

    if (result == 0) {
        result = OS_SystemF(IF_WINDOWS_ELSE("", "./") "%.*s.exe", S8_Expand(exe));
    }
    else {
        ReturnValue = result;
    }
}

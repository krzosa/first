#include "build_tool/library.cpp"

void Compile(Strs cc, Str files);
int ReturnValue = 0;

int Main() {
    Strs cc = CMDLine.get("cc"_s, ON_WINDOWS("cl"_s) ON_MAC("clang"_s) ON_LINUX("gcc"_s));
    Compile(cc, "../tests/test_main.cpp");
    Compile(cc, "../tests/test_filesystem.c");

    return ReturnValue;
}

void Compile(Strs cc, Str file) {
    int result = 0;
    Str exe = FilenameWithoutExt(file);
    bool is_cpp = S8_EndsWith(file, ".cpp");
    if (cc == "gcc" && is_cpp) result = OS_SystemF("g++ -o %.*s.exe %.*s -g -Wno-write-strings -fsanitize=address", S8_Expand(exe), S8_Expand(file));
    else if (cc == "gcc" && !is_cpp) result = OS_SystemF("g -o %.*s.exe %.*s -g -Wno-write-strings -fsanitize=address", S8_Expand(exe), S8_Expand(file));
    else if (cc == "clang" && is_cpp) result = OS_SystemF("clang++ -std=c++11 -o %.*s.exe %.*s -fdiagnostics-absolute-paths -g -Wno-writable-strings -fsanitize=address", S8_Expand(exe), S8_Expand(file));
    else if (cc == "clang" && !is_cpp) result = OS_SystemF("clang -o %.*s.exe %.*s -fdiagnostics-absolute-paths -g -Wno-writable-strings -fsanitize=address", S8_Expand(exe), S8_Expand(file));
    else result = OS_SystemF("cl -Fe:%.*s.exe %.*s -FC -Zi -WX -W3 -wd4200 -diagnostics:column -nologo -D_CRT_SECURE_NO_WARNINGS -fsanitize=address -RTC1", S8_Expand(exe), S8_Expand(file));

    if (result == 0) result = OS_SystemF(IF_WINDOWS_ELSE("", "./") "%.*s.exe", S8_Expand(exe));
    else ReturnValue = result;
}

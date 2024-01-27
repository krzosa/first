#include "build_tool/library.cpp"

void Compile(S8_String cc, S8_String files);
int ReturnValue = 0;

int main(int argc, char **argv) {
    Array<S8_String> cmd = CMD_Make(argv, argc);
    S8_String cc = CMD_Get(cmd, "cc", IF_WINDOWS("cl") IF_MAC("clang") IF_LINUX("gcc"));

    Compile(cc, "../tests/test_thread.cpp");
    // Compile(cc, "../tests/test_main.cpp");
    // Compile(cc, "../tests/test_filesystem.c");

    return ReturnValue;
}

void Compile(S8_String cc, S8_String file) {
    int result = 0;

    S8_String name_no_ext = S8_GetNameNoExt(file);
    S8_String exe = Fmt("%.*s.exe", S8_Expand(name_no_ext));
    bool is_cpp = S8_EndsWith(file, ".cpp");

    if (cc == "cl") {

        Array<S8_String> flags = {Perm};
        flags += "/MP /Zi -D_CRT_SECURE_NO_WARNINGS";
        flags += "/FC /WX /W3 /wd4200 /diagnostics:column /nologo";
        flags += "/GF /Gm- /Oi";
        flags += "/GR- /EHa-";
        flags += "/D_DEBUG -RTC1 -Od";
        flags += Fmt("/Fe:%.*s", S8_Expand(exe));

        Array<S8_String> link = {Perm};
        link += "/link /incremental:no";

        result = Run(cc + file + flags + link);
    }
    else if (cc == "clang") {
        if (is_cpp) cc = "clang++";

        Array<S8_String> flags = {Perm};
        flags += "-g -Wno-write-strings";
        flags += "-fdiagnostics-absolute-paths";
        flags += "-fsanitize=address";
        if (is_cpp) flags += "-std=c++11";
        flags += Fmt("-o %.*s", S8_Expand(exe));

        result = Run(cc + file + flags);
    }
    else {
        IO_Assert(cc == "gcc");
        if (is_cpp) cc = "g++";

        Array<S8_String> flags = {Perm};
        flags += "-g -Wno-write-strings";
        flags += "-fsanitize=address";
        if (is_cpp) flags += "-std=c++11";
        flags += Fmt("-o %.*s", S8_Expand(exe));

        result = Run(cc + file + flags);
    }

    if (result == 0) result = OS_SystemF(IF_WINDOWS_ELSE("", "./") "%.*s", S8_Expand(exe));
    else ReturnValue = result;
}

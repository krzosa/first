#include "build_tool/library.cpp"

void Compile(S8_String cc, S8_String files);
int ReturnValue = 0;

int Main() {
    S8_String cc = CMDLine.get("cc", ON_WINDOWS("cl") ON_MAC("clang") ON_LINUX("gcc"));
    Compile(cc, "../tests/test_main.cpp");
    Compile(cc, "../tests/test_filesystem.c");

    return ReturnValue;
}

void Compile(S8_String cc, S8_String file) {
    int result = 0;

    S8_String name_no_ext = S8_GetNameNoExt(file);
    S8_String exe = Fmt("%.*s.exe", S8_Expand(name_no_ext));

    if (cc == "cl") {
        Array<S8_String> flags = Split("/MP /Zi -D_CRT_SECURE_NO_WARNINGS /GF /Gm- /Oi");
        flags += Split("/FC /WX /W3 /wd4200 /diagnostics:column /nologo");
        flags += Split("/GR- /EHa-");
        flags += Split("-RTC1");

        Array<S8_String> link = Split("/link /incremental:no");
        S8_String name = Fmt("/Fe:%.*s", S8_Expand(exe));

        result = Run(cc + file + name + flags + link);
    }
    else if (cc == "clang") {
        Array<S8_String> flags = Split("-g -Wno-write-strings -fdiagnostics-absolute-paths");
        flags += "-fsanitize=address";

        if (S8_EndsWith(file, ".cpp")) {
            cc = "clang++";
            flags += "-std=c++11";
        }

        S8_String name = Fmt("-o %.*s", S8_Expand(exe));
        result = Run(cc + file + name + flags);
    }
    else {
        IO_Assert(cc == "gcc");

        Array<S8_String> flags = Split("-g -Wno-write-strings");
        flags += "-fsanitize=address";

        if (S8_EndsWith(file, ".cpp")) {
            cc = "g++";
            flags += "-std=c++11";
        }

        S8_String name = Fmt("-o %.*s", S8_Expand(exe));
        result = Run(cc + file + name + flags);
    }

    if (result == 0) result = OS_SystemF(IF_WINDOWS_ELSE("", "./") "%.*s", S8_Expand(exe));
    else ReturnValue = result;
}

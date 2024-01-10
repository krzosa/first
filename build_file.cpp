#include "code/build_lib.cpp"

int CompileFiles(Strs cc, Strs files);

int Main() {
    Strs cc = CMDLine.get("cc"_s, ON_WINDOWS("cl"_s) ON_MAC("clang"_s) ON_LINUX("gcc"_s));
    Strs files = ListDir("../test");
    CompileFiles(cc, {"../test/main_core_as_header.cpp", "../core.c"});
    For(files) {
        if (S8_Find(it, "test_"_s, 0, 0)) {
            CompileFiles(cc, it);
        }
    }

    return 0;
}

int CompileFiles(Strs cc, Strs files) {
    int result = 0;
    Str exe = FilenameWithoutExt(files[0]);
    Str filestr = Merge(files);
    if (cc == "gcc") {
        result = OS_SystemF("g++ -o %Q.exe %Q -g -Wno-write-strings", exe, filestr);
    }
    else if (cc == "clang") {
        result = OS_SystemF("clang++ -std=c++11 -o %Q.exe %Q -g -Wno-writable-strings", exe, filestr);
    }
    else {
        result = OS_SystemF("cl -Fe:%Q.exe %Q -Zi -WX -W3 -wd4200 -diagnostics:column -nologo -D_CRT_SECURE_NO_WARNINGS", exe, filestr);
    }
    if (result == 0) result = OS_SystemF(IF_WINDOWS_ELSE("", "./") "%Q.exe", exe);
    return result;
}

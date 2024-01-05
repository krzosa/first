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
#endif
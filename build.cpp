#include "bld_lib.cpp"

int Main() {
    bool debug = true;
    bool release = !debug;
    bool use_std = false;

    Strs cc = "cl.exe";
    Strs flags = Split("-WX -W3 -wd4200 -diagnostics:column -nologo -Z7 -FC -GF -Gm- -Oi -Zo -D_CRT_SECURE_NO_WARNINGS");
    Strs link = Split("-link -incremental:no");
    Strs files = Split("../test/main_core_as_header.cpp ../core.c");
    // files += IfCodeWasModified("../core.c", "../core.obj");
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

        objs += S8_Format(Perm, "../build/%Q.obj", FilenameWithoutExt(it));
    }
    int error = Run(cc + objs + flags + link);
    return error;
}
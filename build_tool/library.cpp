#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "../core_library/core.c"

#define CL_Allocator MA_Arena *
#define CL_Allocate(a, s) MA_PushSizeNonZeroed(a, s)
#define CL_ASSERT IO_Assert
#define CL_VSNPRINTF stbsp_vsnprintf
#define CL_SNPRINTF stbsp_snprintf
#define AND_CL_STRING_TERMINATE_ON_NEW_LINE
#include "../standalone_libraries/clexer.c"

MA_Arena PernamentArena;
MA_Arena *Perm = &PernamentArena;
Table<S8_String> CMDLine;

#include "cache.cpp"
#include "easy_strings.cpp"

S8_String CL_Flags = "/MP /Zi /FC /WX /W3 /wd4200 /diagnostics:column /nologo -D_CRT_SECURE_NO_WARNINGS /GF /Gm- /Oi";
S8_String CL_Link = "/link /incremental:no";
S8_String CL_StdOff = "/GR- /EHa-";
S8_String CL_StdOn = "/EHsc";
S8_String CL_Debug = "-Od -D_DEBUG -fsanitize=address -RTC1";
S8_String CL_Release = "-O2 -MT -DNDEBUG -GL";
S8_String CL_ReleaseLink = "-opt:ref -opt:icf";
/*
/FC      = Print full paths in diagnostics
/Gm-     = Old feature, 'minimal compilation', in case it's not off by default
/GF      = Pools strings as read-only. If you try to modify strings under /GF, an application error occurs.
/Oi      = Replaces some function calls with intrinsic
/MP      = Multithreaded compilation
/GR-     = Disable runtime type information
/EHa-    = Disable exceptions
/EHsc    = Enable exceptions
/MT      = Link static libc. The 'd' means debug version
/MD      = Link dynamic libc. The 'd' means debug version
/GL      = Whole program optimization
/RTC1    = runtime error checks
/opt:ref = eliminates functions and data that are never referenced
/opt:icf = eliminates redundant 'COMDAT's
*/

S8_String Clang_Flags = "-fdiagnostics-absolute-paths -Wno-writable-strings";
S8_String Clang_Debug = "-fsanitize=address -g";
/*
-std=c++11
 */

S8_String GCC_Flags = "-Wno-write-strings";
S8_String GCC_Debug = "-fsanitize=address -g";

#ifndef BUILD_MAIN
int Main();

void BUILD_ReportError(S8_String it) {
    IO_FatalErrorf("Invalid command line argument syntax! Expected a key value pair!\n"
                   "Here is the wrong argument: %.*s\n"
                   "Here is a good example: bld.exe profile=release platform=windows\n",
                   S8_Expand(it));
}

int main(int argc, char **argv) {

    if (argc > 1) IO_Printf("Command line arguments:\n");
    for (int i = 1; i < argc; i += 1) {
        S8_String it = S8_MakeFromChar(argv[i]);

        int64_t idx = 0;
        if (S8_Seek(it, "="_s, 0, &idx)) {
            S8_String key = S8_GetPrefix(it, idx);
            S8_String value = S8_Skip(it, idx + 1);
            if (key.len == 0) BUILD_ReportError(it);
            if (value.len == 0) BUILD_ReportError(it);
            IO_Printf("[%d] %.*s = %.*s\n", i, S8_Expand(key), S8_Expand(value));

            CMDLine.put(key, value);
        }
        else BUILD_ReportError(it);
    }

    SRC_InitCache(Perm, S8_Lit("build_file.cache"));
    int result = Main();
    if (result == 0) SRC_SaveCache();
    return result;
}
#endif
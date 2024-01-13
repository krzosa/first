/*
----------------- CL.EXE -----------------
FLAGS        = /MP /Zi /FC /WX /W3 /wd4200 /diagnostics:column /nologo -D_CRT_SECURE_NO_WARNINGS /GF /Gm- /Oi
LINK         = /link /incremental:no
STD_OFF      = /GR- /EHa-
STD_ON       = /EHsc
DEBUG        = -Od -D_DEBUG -MDd -fsanitize=address -RTC1
DEBUG_LINK   = -NODEFAULTLIB:LIBCMT
RELEASE      = -O2 -MT -DNDEBUG -GL
RELEASE_LINK = -opt:ref -opt:icf
----------------- CL.EXE -----------------
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

----------------- CLANG -----------------
FLAGS = -fdiagnostics-absolute-paths -g -Wno-writable-strings
DEBUG = -fsanitize=address
*/

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

#include "cache.c"

//
//
//

using Str = S8_String;
struct Strs : Array<Str> {
    Strs() = default;
    Strs(char *str) {
        *this = {};
        this->add(S8_MakeFromChar(str));
    }
    Strs(char *a, char *b) {
        *this = {};
        this->add(S8_MakeFromChar(a));
        this->add(S8_MakeFromChar(b));
    }
    Strs(char *a, char *b, char *c) {
        *this = {};
        this->add(S8_MakeFromChar(a));
        this->add(S8_MakeFromChar(b));
        this->add(S8_MakeFromChar(c));
    }
    Strs(Str a) {
        *this = {};
        this->add(a);
    }
    Strs(Array<Str> a) { MA_MemoryCopy(this, &a, sizeof(a)); }
};

bool operator==(Strs a, char *b) {
    if (a.len != 1) return false;
    bool result = a[0] == S8_MakeFromChar(b);
    return result;
}

bool operator==(Strs a, const char *b) {
    if (a.len != 1) return false;
    bool result = a[0] == S8_MakeFromChar((char *)b);
    return result;
}

Strs operator+(Strs a, Strs b) {
    Strs c = {a.copy(*Perm)};
    c.add_array(b);
    return c;
}

Strs operator+(Strs a, Str b) {
    Strs c = {a.copy(*Perm)};
    c.add(b);
    return c;
}

Strs operator+(Str a, Strs b) {
    Strs c = {b.copy(*Perm)};
    c.add(a);
    return c;
}

Strs &operator+=(Strs &a, Strs b) {
    a.add_array(b);
    return a;
}

Strs &operator+=(Strs &a, char *str) {
    a.add(S8_MakeFromChar(str));
    return a;
}

Strs &operator+=(Strs &a, Str s) {
    a.add(s);
    return a;
}

Strs operator+(Strs a, char *b) {
    Strs c = {a.copy(*Perm)};
    c.add(S8_MakeFromChar(b));
    return c;
}

Strs operator+(char *a, Strs b) {
    Strs c = {b.copy(*Perm)};
    c.add(S8_MakeFromChar(a));
    return c;
}

Strs operator+(Str a, Str b) {
    Strs c = {};
    c.add(a);
    c.add(b);
    return c;
}

Strs operator+(Str a, char *b) {
    return a + S8_MakeFromChar(b);
}

//@todo: split on any whitespace instead!
Strs Split(char *str) {
    Str s = S8_MakeFromChar(str);
    S8_List list = S8_Split(Perm, s, S8_Lit(" "), 0);

    Strs result = {};
    S8_For(it, list) result.add(it->string);
    return result;
}

Str Merge(Strs list, Str separator = " "_s) {
    int64_t char_count = 0;
    For(list) char_count += it.len;
    if (char_count == 0) return {};
    int64_t node_count = list.len;

    int64_t base_size = (char_count + 1);
    int64_t sep_size = (node_count - 1) * separator.len;
    int64_t size = base_size + sep_size;
    char *buff = (char *)MA_PushSize(Perm, sizeof(char) * size);
    Str string = S8_Make(buff, 0);
    For(list) {
        IO_Assert(string.len + it.len <= size);
        MA_MemoryCopy(string.str + string.len, it.str, it.len);
        string.len += it.len;
        if (!list.is_last(it)) {
            MA_MemoryCopy(string.str + string.len, separator.str, separator.len);
            string.len += separator.len;
        }
    }
    IO_Assert(string.len == size - 1);
    string.str[size] = 0;
    return string;
}

S8_String FilenameWithoutExt(S8_String it) { return S8_SkipToLastSlash(S8_ChopLastPeriod(it)); }
bool CodeWasModified(char *str, char *artifact = 0) { return SRC_WasModified(S8_MakeFromChar(str), S8_MakeFromChar(artifact)); }
bool CodeWasModified(S8_String str, S8_String artifact = {}) { return SRC_WasModified(str, artifact); }
Strs IfCodeWasModified(char *cfile, char *objfile) {
    Strs result = {};
    S8_String s = S8_MakeFromChar(cfile);
    S8_String o = S8_MakeFromChar(objfile);
    if (SRC_WasModified(s, o)) {
        return cfile;
    }
    return objfile;
}
void MakeDir(char *str) { OS_MakeDir(S8_MakeFromChar(str)); }
void ChangeDir(char *str) { OS_SetWorkingDir(S8_MakeFromChar(str)); }
int Run(Strs s) {
    Str cmd = Merge(s);
    return OS_SystemF("%.*s", S8_Expand(cmd));
}

Strs ListDir(char *dir) {
    Strs result = {};
    for (OS_FileIter it = OS_IterateFiles(Perm, S8_MakeFromChar(dir)); OS_IsValid(it); OS_Advance(&it)) {
        result.add(S8_Copy(Perm, it.absolute_path));
    }
    return result;
}

void ReportError(S8_String it) {
    IO_FatalErrorf("Invalid command line argument syntax! Expected a key value pair!\n"
                   "Here is the wrong argument: %.*s\n"
                   "Here is a good example: bld.exe profile=release platform=windows\n",
                   S8_Expand(it));
}

#ifndef BUILD_MAIN
int Main();
int main(int argc, char **argv) {

    if (argc > 1) IO_Printf("Command line arguments:\n");
    for (int i = 1; i < argc; i += 1) {
        S8_String it = S8_MakeFromChar(argv[i]);

        int64_t idx = 0;
        if (S8_Seek(it, "="_s, 0, &idx)) {
            S8_String key = S8_GetPrefix(it, idx);
            S8_String value = S8_Skip(it, idx + 1);
            if (key.len == 0) ReportError(it);
            if (value.len == 0) ReportError(it);
            IO_Printf("[%d] %.*s = %.*s\n", i, S8_Expand(key), S8_Expand(value));

            CMDLine.put(key, value);
        }
        else ReportError(it);
    }

    SRC_InitCache(Perm, S8_Lit("build_file.cache"));
    int result = Main();
    if (result == 0) SRC_SaveCache();
    return result;
}
#endif
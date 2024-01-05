#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "core.c"

#define CL_Arena MA_Arena
#define CL_PushSize MA_PushSizeNonZeroed
#define CL_ASSERT IO_Assert
#define CL_VSNPRINTF stbsp_vsnprintf
#define CL_SNPRINTF stbsp_snprintf
#define AND_CL_STRING_TERMINATE_ON_NEW_LINE
#include "clexer.c"

#define SRC_CACHE_ENTRY_COUNT 1024
struct SRC_CacheEntry {
    uint64_t filepath_hash;
    uint64_t file_hash;
    uint64_t includes_hash;
    uint64_t total_hash;
};

struct SRC_Cache {
    int entry_cap;
    int entry_len;
    SRC_CacheEntry entries[SRC_CACHE_ENTRY_COUNT];
};

double SRC_Time;
SRC_Cache *SRC_InMemoryCache;
SRC_Cache *SRC_FromFileCache;
CL_ArenaTuple SRC_ArenaTuple;
S8_String SRC_CacheFilename;
MA_Arena PernamentArena;
MA_Arena *Perm = &PernamentArena;
CL_SearchPaths SRC_SearchPaths = {}; // @todo;

void SRC_InitCache(MA_Arena *arena, S8_String cachefilename) {
    SRC_CacheFilename = cachefilename;
    CL_InitDefaultTuple(&SRC_ArenaTuple);

    SRC_InMemoryCache = MA_PushStruct(arena, SRC_Cache);
    SRC_InMemoryCache->entry_cap = SRC_CACHE_ENTRY_COUNT;

    SRC_FromFileCache = MA_PushStruct(arena, SRC_Cache);
    SRC_FromFileCache->entry_cap = SRC_CACHE_ENTRY_COUNT;

    S8_String cache = OS_ReadFile(arena, SRC_CacheFilename);
    if (cache.len) MA_MemoryCopy(SRC_FromFileCache, cache.str, cache.len);
}

void SRC_SaveCache() {
    S8_String dump = S8_Make((char *)SRC_InMemoryCache, sizeof(SRC_Cache));
    OS_WriteFile(SRC_CacheFilename, dump);
}

SRC_CacheEntry *SRC_AddHash(uint64_t filepath, uint64_t file, uint64_t includes) {
    IO_Assert(SRC_InMemoryCache->entry_len + 1 < SRC_InMemoryCache->entry_cap);
    SRC_CacheEntry *result = SRC_InMemoryCache->entries + SRC_InMemoryCache->entry_len++;
    result->filepath_hash = filepath;
    result->file_hash = file;
    result->includes_hash = includes;
    result->total_hash = HashBytes(result, sizeof(uint64_t) * 3);
    return result;
}

SRC_CacheEntry *SRC_FindCache(SRC_Cache *cache, uint64_t filepath_hash) {
    for (int cache_i = 0; cache_i < cache->entry_len; cache_i += 1) {
        SRC_CacheEntry *it = cache->entries + cache_i;
        if (it->filepath_hash == filepath_hash) {
            return it;
        }
    }
    return 0;
}

SRC_CacheEntry *SRC_HashFile(S8_String file, char *parent_file) {
    char *resolved_file = CL_ResolveFilepath(Perm, &SRC_SearchPaths, file.str, parent_file, false);
    if (!resolved_file) {
        IO_Printf("Failed to resolve file: %s\n", file.str);
        return 0;
    }

    uint64_t filepath_hash = HashBytes(resolved_file, S8_Length(resolved_file));
    SRC_CacheEntry *entry = SRC_FindCache(SRC_InMemoryCache, filepath_hash);
    if (entry) return entry;

    CL_LexResult *first_lex = CL_LexFile(&SRC_ArenaTuple, resolved_file);
    IO_Assert(first_lex);
    uint64_t file_hash = HashBytes(first_lex->stream_begin, first_lex->stream - first_lex->stream_begin);
    uint64_t includes_hash = 13;

    CL_LexList list = CL_MakeLexList(first_lex);
    for (CL_IncludeIter iter = CL_IterateIncludes(&list); iter.filename; CL_GetNextInclude(&iter)) {
        if (iter.is_system_include) continue;

        S8_String file_it = S8_MakeFromChar(iter.filename);
        SRC_CacheEntry *cache = SRC_HashFile(file_it, resolved_file);
        if (!cache) {
            IO_Printf("Missing cache for: %s\n", file_it.str);
            continue;
        }

        includes_hash = HashMix(includes_hash, cache->total_hash);
    }

    SRC_CacheEntry *result = SRC_AddHash(filepath_hash, file_hash, includes_hash);
    return result;
}

bool SRC_WasModified(S8_String file, S8_String artifact_path) {
    double time_start = OS_GetTime();

    if (OS_FileExists(file) == false) {
        IO_Printf("FAILED File doesnt exist: %.*s\n", S8_Expand(file));
        exit(0);
    }
    if (OS_IsAbsolute(file) == false) {
        file = OS_GetAbsolutePath(Perm, file);
    }

    S8_String without_ext = S8_ChopLastPeriod(file);
    S8_String name_only = S8_SkipToLastSlash(without_ext);

    if (artifact_path.len == 0) artifact_path = S8_Format(Perm, "%.*s.%s", S8_Expand(name_only), IF_WINDOWS_ELSE("obj", "o"));
    bool modified = OS_FileExists(artifact_path) == false;

    SRC_CacheEntry *in_memory = SRC_HashFile(file, 0);
    IO_Assert(in_memory);

    if (modified == false) {
        SRC_CacheEntry *from_file = SRC_FindCache(SRC_FromFileCache, in_memory->filepath_hash);
        if (from_file == 0 || (in_memory->total_hash != from_file->total_hash)) {
            modified = true;
        }
    }

    SRC_Time = SRC_Time + (OS_GetTime() - time_start);

    return modified;
}

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
    return OS_SystemF("%Q", cmd);
}

Strs ListDir(char *dir) {
    Strs result = {};
    S8_List files = OS_ListDir(Perm, S8_MakeFromChar(dir), 0);
    S8_For(it, files) result.add(it->string);
    return result;
}

#ifndef BLD_MAIN
int Main();
int main() {
    SRC_InitCache(Perm, S8_Lit("bld_file.cache"));
    int result = Main();
    if (result == 0) SRC_SaveCache();
}
#endif
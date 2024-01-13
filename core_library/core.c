/*
@todo
- Rewrite OS_ListDir, no weird flags and conventions, [relative, absolute, is_dir]
- add c++ default args using macros
- Rework string flags
- String should use allocator instead of arena?
- Maybe remove completely null termination from S8_String, I think it might complicate and is error prone because one can sometimes assume it's null terminated and it isn't
- Maybe create a nice C++ binding for strings,
    - also add String Arrays and String Builder, temp allocators hook ins for nicer api
    - Iterating over unicode codepoints using For
- Redesign clexer because the Arena Tuple is hella weird
    - Maybe remove token array stuff, do standard stream thing, then you need one arena ...
        - Add filter stuff so we can get includes / comments / normal tokens etc.
*/

#include "core.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "../standalone_libraries/stb_sprintf.h"
#define IO_VSNPRINTF stbsp_vsnprintf
#define IO_SNPRINTF stbsp_snprintf
#include "../standalone_libraries/io.c"
#define MA_ASSERT(x) IO_Assert(x)
#include "../standalone_libraries/arena.c"
#define RE_ASSERT(x) IO_Assert(x)
#include "../standalone_libraries/regex.c"
#include "../standalone_libraries/unicode.c"
#define S8_VSNPRINTF stbsp_vsnprintf
#define S8_ALLOCATE(allocator, size) MA_PushSize(allocator, size)
#define S8_ASSERT(x) IO_Assert(x)
#define S8_MemoryCopy MA_MemoryCopy
#include "../standalone_libraries/string.c"
#define MU_ASSERT IO_Assert
#include "../standalone_libraries/multimedia.h"
#include "../standalone_libraries/hash.c"
#include "../standalone_libraries/load_library.c"
#include "filesystem.c"

// #if LANG_CPP
//     #include "string.cpp"
// #endif
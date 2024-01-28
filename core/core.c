/*
- I think it's okay to say that strings being null terminated should be mostly treated as NOT terminated but leave some
   leeway for big buffers and other such things. Just make sure to not relay on it because it's easier unless specified.
- Not sure if we should assume that strings should use allocators or arenas, for now it's arenas because I don't have other use cases
@todo
- Add compiler checking to make sure it's on the path and if it's not message
- Add file, line info to Arenas!
- Remove static buffers from filesystem, use scratch arenas, more secure, data corruption instead of control flow corruption
- Use allocators instead of concrete Arenas
- Add proper string arrays and utilities for build files
    - also add String Arrays and String Builder, temp allocators hook ins for nicer api
*/

#include "core.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "../standalone_libraries/stb_sprintf.h"
#define IO_VSNPRINTF stbsp_vsnprintf
#define IO_SNPRINTF stbsp_snprintf
#include "../standalone_libraries/io.c"
#define MA_Assertf(x, ...) IO_Assertf(x, __VA_ARGS__)
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

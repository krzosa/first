#include "core.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "standalone_modules/stb_sprintf.h"
#define IO_VSNPRINTF stbsp_vsnprintf
#define IO_SNPRINTF stbsp_snprintf
#include "standalone_modules/io.c"
#define MA_ASSERT(x) IO_Assert(x)
#include "standalone_modules/arena.c"
#define RE_ASSERT(x) IO_Assert(x)
#include "standalone_modules/regex.c"
#include "standalone_modules/unicode.c"
#define S8_VSNPRINTF stbsp_vsnprintf
#define S8_ALLOCATE(allocator, size) MA_PushSize(allocator, size)
#define S8_ASSERT(x) IO_Assert(x)
#define S8_MemoryCopy MA_MemoryCopy
#include "standalone_modules/string.c"
#define MU_ASSERT IO_Assert
#include "standalone_modules/multimedia.h"
#include "standalone_modules/hash.c"
#include "standalone_modules/load_library.c"
#include "filesystem/filesystem.c"

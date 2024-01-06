#include "core.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "code/stb_sprintf.h"
#define IO_VSNPRINTF stbsp_vsnprintf
#define IO_SNPRINTF stbsp_snprintf
#include "code/io.c"
#define MA_ASSERT(x) IO_Assert(x)
#include "code/arena.c"
#define RE_ASSERT(x) IO_Assert(x)
#include "code/regex.c"
#include "code/unicode.c"
#define S8_VSNPRINTF stbsp_vsnprintf
#define S8_ALLOCATE(allocator, size) MA_PushSize(allocator, size)
#define S8_ASSERT(x) IO_Assert(x)
#define S8_MemoryCopy MA_MemoryCopy
#include "code/string.c"
#define MU_ASSERT IO_Assert
#include "code/multimedia.h"
#include "code/hash.c"
#include "code/load_library.c"
#include "code/filesystem.c"

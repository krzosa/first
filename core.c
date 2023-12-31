#include "core.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#define IO_VSNPRINTF stbsp_vsnprintf
#define IO_SNPRINTF stbsp_snprintf
#include "io.c"
#define MA_ASSERT(x) IO_Assert(x)
#include "arena.c"
#define RE_ASSERT(x) IO_Assert(x)
#include "regex.c"
#include "unicode.c"
#define S8_VSNPRINTF stbsp_vsnprintf
#define S8_ALLOCATE(allocator, size) MA_PushSize(allocator, size)
#define S8_ASSERT(x) IO_Assert(x)
#define S8_MemoryCopy MA_MemoryCopy
#include "string.c"
#define MU_ASSERT IO_Assert
#include "multimedia.h"
#include "hash.c"
#include "load_library.c"
#include "filesystem.c"

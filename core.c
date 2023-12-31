#include "core.h"
#define S8_IMPLEMENTATION

#define IO_IMPLEMENTATION
#define IO_VSNPRINTF stbsp_vsnprintf
#define IO_SNPRINTF stbsp_snprintf
#include "io.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#define MA_IMPLEMENTATION
#define MA_ASSERT(x) IO_Assert(x)
#include "arena.h"

#define RE_IMPLEMENTATION
#define RE_ASSERT(x) IO_Assert(x)
#include "regex.h"

#define UTF_IMPLEMENTATION
#include "unicode.h"

#define S8_VSNPRINTF stbsp_vsnprintf
#define S8_ALLOCATE(allocator, size) MA_PushSize(allocator, size)
#define S8_ASSERT(x) IO_Assert(x)
#define S8_MemoryCopy MA_MemoryCopy
#include "string.h"

#define MU_IMPLEMENTATION
#define MU_ASSERT IO_Assert
#include "multimedia.h"
#include "hash.c"
#include "load_library.c"
#include "filesystem.c"

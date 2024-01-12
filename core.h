#pragma once
#include "standalone_modules/preproc_env.h"
#include "standalone_modules/stb_sprintf.h"
#include "standalone_modules/io.h"
#include "standalone_modules/arena.h"
#include "standalone_modules/string.h"
#include "standalone_modules/unicode.h"
#include "standalone_modules/hash.h"
#include "standalone_modules/linked_list.h"
#include "standalone_modules/regex.h"
#include "standalone_modules/multimedia.h"
#include "standalone_modules/load_library.h"
#include "filesystem/filesystem.h"

#ifdef __cplusplus
    #include "standalone_modules/defer.hpp"
    #define TABLE_ASSERT IO_Assert
    #define TABLE_Allocator M_Allocator
    #define TABLE_ALLOCATE(allocator, size) M_Alloc(allocator, size)
    #define TABLE_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
    #define TABLE_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "standalone_modules/table.hpp"
    #define ARRAY_ASSERT IO_Assert
    #define ARRAY_Allocator M_Allocator
    #define ARRAY_REALLOCATE(allocator, p, size, old_size) M_ReallocNonZeroed(allocator, p, size, old_size)
    #define ARRAY_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
    #define ARRAY_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "standalone_modules/array.hpp"
#endif

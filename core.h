#pragma once
#include "code/preproc_env.h"
#include "code/stb_sprintf.h"
#include "code/io.h"
#include "code/arena.h"
#include "code/string.h"
#include "code/unicode.h"
#include "code/hash.h"
#include "code/linked_list.h"
#include "code/regex.h"
#include "code/multimedia.h"
#include "code/load_library.h"
#include "code/filesystem.h"

#ifdef __cplusplus
    #include "code/defer.hpp"
    #define TABLE_ASSERT IO_Assert
    #define TABLE_Allocator M_Allocator
    #define TABLE_ALLOCATE(allocator, size) M_Alloc(allocator, size)
    #define TABLE_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
    #define TABLE_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "code/table.hpp"
    #define ARRAY_ASSERT IO_Assert
    #define ARRAY_Allocator M_Allocator
    #define ARRAY_REALLOCATE(allocator, p, size, old_size) M_ReallocNonZeroed(allocator, p, size, old_size)
    #define ARRAY_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
    #define ARRAY_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "code/array.hpp"
#endif

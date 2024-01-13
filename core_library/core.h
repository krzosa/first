#pragma once
#include "../standalone_libraries/preproc_env.h"
#include "../standalone_libraries/stb_sprintf.h"
#include "../standalone_libraries/io.h"
#include "../standalone_libraries/arena.h"
#include "../standalone_libraries/unicode.h"
#include "../standalone_libraries/string.h"
#include "../standalone_libraries/hash.h"
#include "../standalone_libraries/linked_list.h"
#include "../standalone_libraries/regex.h"
#include "../standalone_libraries/multimedia.h"
#include "../standalone_libraries/load_library.h"
#include "filesystem.h"

#if LANG_CPP
    #include "../standalone_libraries/defer.hpp"
    #define TABLE_ASSERT IO_Assert
    #define TABLE_Allocator M_Allocator
    #define TABLE_ALLOCATE(allocator, size) M_Alloc(allocator, size)
    #define TABLE_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
    #define TABLE_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "../standalone_libraries/table.hpp"
    #define ARRAY_ASSERT IO_Assert
    #define ARRAY_Allocator M_Allocator
    #define ARRAY_REALLOCATE(allocator, p, size, old_size) M_ReallocNonZeroed(allocator, p, size, old_size)
    #define ARRAY_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
    #define ARRAY_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "../standalone_libraries/array.hpp"
// #include "string.hpp"
#endif

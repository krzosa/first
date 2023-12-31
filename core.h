#pragma once
#include "preproc_env.h"
#include "stb_sprintf.h"
#include "io.h"
#include "arena.h"
#include "string.h"
#include "unicode.h"
#include "hash.h"
#include "linked_list.h"
#include "regex.h"
#include "multimedia.h"
#include "load_library.h"
#include "filesystem.h"

#ifdef __cplusplus
    #include "defer.hpp"
    #define TABLE_ASSERT IO_Assert
    #define TABLE_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "table.hpp"
    #define ARRAY_ASSERT IO_Assert
    #define ARRAY_ALLOCATOR_TYPE M_Allocator
    #define ARRAY_SET_DEFAULT_ALLOCATOR \
        if (!allocator.p) allocator = M_GetSystemAllocator();
    #include "array.hpp"
#endif

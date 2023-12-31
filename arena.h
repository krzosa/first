/*
## MA_Arena version 2
A public domain, single-header-file library that provides easy to use
arena data structure.

### Features
    - linear allocator (MA_Arena)
    - thread local scratch allocator (MA_Scratch) *optional*
    - allocator abstraction (M_Allocator) *overridable*
    - virtual memory abstraction (MV_Memory)


### Usage, do this in *one* C or C++ file:
```
#define MA_IMPLEMENTATION
#include "arena.h"
```

*/
#ifndef MA_HEADER
#define MA_HEADER
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MA_KIB(x) ((x##ull) * 1024ull)
#define MA_MIB(x) (MA_KIB(x) * 1024ull)
#define MA_GIB(x) (MA_MIB(x) * 1024ull)
#define MA_TIB(x) (MA_GIB(x) * 1024ull)

#ifndef MA_DEFAULT_RESERVE_SIZE
    #define MA_DEFAULT_RESERVE_SIZE MA_GIB(1)
#endif

#ifndef MA_DEFAULT_ALIGNMENT
    #define MA_DEFAULT_ALIGNMENT 8
#endif

#ifndef MA_COMMIT_ADD_SIZE
    #define MA_COMMIT_ADD_SIZE MA_MIB(4)
#endif

#ifndef MA_INHERIT_HOOK
    #define MA_INHERIT_HOOK
#endif

#ifndef MA_C_INHERIT_HOOK
    #define MA_C_INHERIT_HOOK M_Allocator allocator;
#endif

#ifndef MA_ZERO_IS_INITIALIZATION
    #define MA_ZERO_IS_INITIALIZATION 1
#endif

#ifndef MA_INIT_HOOK
    #define MA_INIT_HOOK(arena)               \
        arena->allocator.obj = (void *)arena; \
        arena->allocator.p = (M_AllocatorProc *)MA_AllocatorProc;
#endif

#ifndef MA_API
    #ifdef __cplusplus
        #define MA_API extern "C"
    #else
        #define MA_API
    #endif
#endif

#ifndef MA_THREAD_LOCAL
    #if defined(__cplusplus) && __cplusplus >= 201103L
        #define MA_THREAD_LOCAL thread_local
    #elif defined(__GNUC__)
        #define MA_THREAD_LOCAL __thread
    #elif defined(_MSC_VER)
        #define MA_THREAD_LOCAL __declspec(thread)
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
        #define MA_THREAD_LOCAL _Thread_local
    #elif defined(__TINYC__)
        #define MA_THREAD_LOCAL _Thread_local
    #else
        #error Couldnt figure out thread local, needs to be provided manually
    #endif
#endif

typedef enum M_AllocatorOp {
    M_AllocatorOp_Invalid,
    M_AllocatorOp_Allocate,
    M_AllocatorOp_Deallocate,
} M_AllocatorOp;

typedef struct MV_Memory MV_Memory;
typedef struct MA_Checkpoint MA_Checkpoint;
typedef struct MA_Arena MA_Arena;
typedef struct M_Allocator M_Allocator;
typedef void *M_AllocatorProc(void *allocator, M_AllocatorOp kind, void *p, size_t size);

struct M_Allocator {
    void *obj;
    void *(*p)(void *allocator, M_AllocatorOp kind, void *p, size_t size);
};

struct MV_Memory {
    size_t commit;
    size_t reserve;
    uint8_t *data;
};

struct MA_Arena MA_INHERIT_HOOK {
    MA_C_INHERIT_HOOK
    MV_Memory memory;
    int alignment;
    int saved_alignment;
    size_t len;

    size_t packed_array_element_size;
    size_t packed_array_begin;
};

struct MA_Checkpoint {
    MA_Arena *arena;
    size_t pos;
};

#define MA_PushArrayNonZeroed(a, T, c) (T *)MA_PushSizeNonZeroed(a, sizeof(T) * (c))
#define MA_PushStructNonZeroed(a, T) (T *)MA_PushSizeNonZeroed(a, sizeof(T))
#define MA_PushStruct(a, T) (T *)MA_PushSize(a, sizeof(T))
#define MA_PushArray(a, T, c) (T *)MA_PushSize(a, sizeof(T) * (c))
#define MA_PushStructCopy(a, T, p) (T *)MA_PushCopy(a, (p), sizeof(T))
#define MA_BeginPackedArray(arena, T) (T *)MA__BeginPackedArray(arena, sizeof(T))
#define MA_CheckpointScope(name, InArena) for (MA_Checkpoint name = MA_Save(InArena); name.arena; (MA_Load(name), name.arena = 0))

#define M_AllocStruct(a, T) (T *)M_Alloc((a), sizeof(T))
#define M_AllocArray(a, T, c) (T *)M_Alloc((a), sizeof(T) * (c))
#define M_AllocStructCopy(a, T, p) (T *)M_PushCopy(a, (p), sizeof(T))

#define MA_IS_POW2(x) (((x) & ((x)-1)) == 0)
#define MA_MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MA_MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MA_LENGTHOF(x) ((int64_t)((sizeof(x) / sizeof((x)[0]))))

#define MA_CLAMP_TOP(x, max) ((x) >= (max) ? (max) : (x))
#define MA_CLAMP_BOT(x, min) ((x) <= (min) ? (min) : (x))
#define MA_CLAMP(x, min, max) ((x) >= (max) ? (max) : (x) <= (min) ? (min) \
                                                                   : (x))
// clang-format off
MA_API void            MA_MemoryZero(void *p, size_t size);
MA_API void            MA_MemoryCopy(void *dst, void *src, size_t size);
MA_API size_t          MA_GetAlignOffset(size_t size, size_t align);
MA_API size_t          MA_AlignUp(size_t size, size_t align);
MA_API size_t          MA_AlignDown(size_t size, size_t align);
MA_API void            MA_DeallocateStub(MA_Arena *arena, void *p);
MA_API void            MA_PopToPos(MA_Arena *arena, size_t pos);
MA_API void *          MA_PopSize(MA_Arena *arena, size_t size);
MA_API void            MA_DeallocateArena(MA_Arena *arena);
MA_API void            MA_Reset(MA_Arena *arena);
MA_API void *          MA__BeginPackedArray(MA_Arena *arena, size_t element_size);
MA_API int             MA_EndPackedArray(MA_Arena *arena);
MA_API void            MA_SetAlignment(MA_Arena *arena, int alignment);
MA_API uint8_t *       MA_GetTop(MA_Arena *a);
MA_API void *          MA_PushSizeNonZeroed(MA_Arena *a, size_t size);
MA_API void *          MA_PushSize(MA_Arena *arena, size_t size);
MA_API void            MA_InitEx(MA_Arena *a, size_t reserve);
MA_API void            MA_Init(MA_Arena *a);
MA_API void            MA_MakeSureInitialized(MA_Arena *a);
MA_API MA_Arena *      MA_Bootstrap(void);
MA_API void            MA_InitFromBuffer(MA_Arena *arena, void *buffer, size_t size);
MA_API MA_Arena        MA_MakeFromBuffer(void *buffer, size_t size);
MA_API char *          MA_PushStringCopy(MA_Arena *arena, char *p, size_t size);
MA_API void *          MA_PushCopy(MA_Arena *arena, void *p, size_t size);
MA_API bool            MA_IsPointerInside(MA_Arena *arena, void *p);
MA_API MA_Arena        MA_PushArena(MA_Arena *arena, size_t size);
MA_API MA_Checkpoint   MA_Save(MA_Arena *arena);
MA_API void            MA_Load(MA_Checkpoint checkpoint);
MA_API MA_Checkpoint   MA_GetScratchEx(MA_Arena **conflicts, int conflict_count);
MA_API MA_Checkpoint   MA_GetScratch(void);
MA_API MA_Checkpoint   MA_GetScratch1(MA_Arena *conflict);
MA_API void *          MA_AllocatorProc(M_Allocator allocator, M_AllocatorOp kind, void *p, size_t size);

MA_API MV_Memory       MV_Reserve(size_t size);
MA_API bool            MV_Commit(MV_Memory *m, size_t commit);
MA_API void            MV_Deallocate(MV_Memory *m);
MA_API bool            MV_DecommitPos(MV_Memory *m, size_t pos);

MA_API void *          M_AllocNonZeroed(M_Allocator allocator, size_t size);
MA_API void *          M_Alloc(M_Allocator allocator, size_t size);
MA_API void *          M_AllocCopy(M_Allocator allocator, void *p, size_t size);
MA_API void            M_Dealloc(M_Allocator allocator, void *p);
MA_API M_Allocator     M_GetSystemAllocator(void);
// clang-format on

#ifndef MA_DISABLE_SCRATCH
extern MA_THREAD_LOCAL MA_Arena MA_ScratchArenaPool[];
    #define MA_ScratchScope(x) for (MA_Checkpoint x = MA_GetScratch(); x.arena; (MA_ReleaseScratch(x), x.arena = 0))
    #define MA_ReleaseScratch MA_Load

    #if defined(__cplusplus)
struct MA_Scratch {
    MA_Checkpoint checkpoint;
    MA_Scratch() { this->checkpoint = MA_GetScratch(); }
    ~MA_Scratch() { MA_Load(checkpoint); }
    operator MA_Arena *() { return checkpoint.arena; }

  private: // @Note: Disable copy constructors, cause its error prone
    MA_Scratch(MA_Scratch &arena);
    MA_Scratch(MA_Scratch &arena, MA_Scratch &a2);
};
    #endif // __cplusplus
#endif     // MA_DISABLE_SCRATCH
#endif     // MA_HEADER

#ifdef MA_IMPLEMENTATION

#ifndef MA_ASSERT
    #include <assert.h>
    #define MA_ASSERT(x) assert(x)
#endif

#ifndef MA_MemoryZero
    #include <string.h>
MA_API void MA_MemoryZero(void *p, size_t size) {
    memset(p, 0, size);
}
#endif

#ifndef MA_MemoryCopy
    #include <string.h>
MA_API void MA_MemoryCopy(void *dst, void *src, size_t size) {
    memcpy(dst, src, size);
}
#endif

#ifndef MA_CMalloc
    #include <stdlib.h>
    #define MA_CMalloc(x) malloc(x)
    #define MA_CFree(x) free(x)
#endif

#ifndef MA_FN
    #if defined(__GNUC__) || defined(__clang__)
        #define MA_FN __attribute__((unused)) static
    #else
        #define MA_FN static
    #endif
#endif

MA_API size_t MA_GetAlignOffset(size_t size, size_t align) {
    size_t mask = align - 1;
    size_t val = size & mask;
    if (val) {
        val = align - val;
    }
    return val;
}

MA_API size_t MA_AlignUp(size_t size, size_t align) {
    size_t result = size + MA_GetAlignOffset(size, align);
    return result;
}

MA_API size_t MA_AlignDown(size_t size, size_t align) {
    size += 1; // Make sure when align is 8 doesn't get rounded down to 0
    size_t result = size - (align - MA_GetAlignOffset(size, align));
    return result;
}

MA_FN uint8_t *MV__AdvanceCommit(MV_Memory *m, size_t *commit_size, size_t page_size) {
    size_t aligned_up_commit = MA_AlignUp(*commit_size, page_size);
    size_t to_be_total_commited_size = aligned_up_commit + m->commit;
    size_t to_be_total_commited_size_clamped_to_reserve = MA_CLAMP_TOP(to_be_total_commited_size, m->reserve);
    size_t adjusted_to_boundary_commit = to_be_total_commited_size_clamped_to_reserve - m->commit;
    MA_ASSERT(adjusted_to_boundary_commit && "Reached the virtual memory reserved boundary");
    *commit_size = adjusted_to_boundary_commit;

    if (adjusted_to_boundary_commit == 0) {
        return 0;
    }
    uint8_t *result = m->data + m->commit;
    return result;
}

MA_API void MA_DeallocateStub(MA_Arena *arena, void *p) {}

MA_API void MA_PopToPos(MA_Arena *arena, size_t pos) {
    pos = MA_CLAMP_TOP(pos, arena->len);
    arena->len = pos;
}

MA_API void *MA_PopSize(MA_Arena *arena, size_t size) {
    size = MA_CLAMP_TOP(size, arena->len);
    arena->len -= size;
    return arena->memory.data + arena->len;
}

MA_API void MA_DeallocateArena(MA_Arena *arena) {
    MV_Deallocate(&arena->memory);
}

MA_API void MA_Reset(MA_Arena *arena) {
    MA_PopToPos(arena, 0);
}

MA_FN size_t MA__AlignLen(MA_Arena *a) {
    size_t align_offset = a->alignment ? MA_GetAlignOffset((uintptr_t)a->memory.data + (uintptr_t)a->len, a->alignment) : 0;
    size_t aligned = a->len + align_offset;
    return aligned;
}

MA_API void *MA__BeginPackedArray(MA_Arena *arena, size_t element_size) {
    MA_ASSERT(arena->memory.data);
    arena->len = MA__AlignLen(arena);
    arena->saved_alignment = arena->alignment;
    arena->alignment = 0;
    arena->packed_array_begin = arena->len;
    arena->packed_array_element_size = element_size;
    void *result = arena->memory.data + arena->len;
    return result;
}

MA_API int MA_EndPackedArray(MA_Arena *arena) {
    arena->alignment = arena->saved_alignment;
    size_t different = (arena->len - arena->packed_array_begin);
    int result = (int)((arena->len - arena->packed_array_begin) / arena->packed_array_element_size);
    return result;
}

MA_API void MA_SetAlignment(MA_Arena *arena, int alignment) {
    arena->alignment = alignment;
}

MA_API uint8_t *MA_GetTop(MA_Arena *a) {
    MA_ASSERT(a->memory.data);
    return a->memory.data + a->len;
}

MA_API void *MA_PushSizeNonZeroed(MA_Arena *a, size_t size) {
    size_t align_offset = a->alignment ? MA_GetAlignOffset((uintptr_t)a->memory.data + (uintptr_t)a->len, a->alignment) : 0;
    size_t aligned_len = a->len + align_offset;
    size_t size_with_alignment = size + align_offset;

    if (a->len + size_with_alignment > a->memory.commit) {
        if (a->memory.reserve == 0) {
#if MA_ZERO_IS_INITIALIZATION
            MA_Init(a);
#else
            MA_ASSERT("Pushing on uninitialized arena");
#endif
        }
        bool result = MV_Commit(&a->memory, size_with_alignment + MA_COMMIT_ADD_SIZE);
        MA_ASSERT(result && "Failed to commit memory");
        (void)result;
    }

    uint8_t *result = a->memory.data + aligned_len;
    a->len += size_with_alignment;
    MA_ASSERT(a->len <= a->memory.commit);
    return (void *)result;
}

MA_API void *MA_PushSize(MA_Arena *arena, size_t size) {
    void *result = MA_PushSizeNonZeroed(arena, size);
    MA_MemoryZero(result, size);
    return result;
}

MA_API void MA_InitEx(MA_Arena *a, size_t reserve) {
    a->memory = MV_Reserve(reserve);
    a->alignment = MA_DEFAULT_ALIGNMENT;
    MA_INIT_HOOK(a);
}

MA_API void MA_Init(MA_Arena *a) {
    MA_InitEx(a, MA_DEFAULT_RESERVE_SIZE);
}

MA_API void MA_MakeSureInitialized(MA_Arena *a) {
    if (a->memory.data == 0) {
        MA_Init(a);
    }
}

MA_API MA_Arena *MA_Bootstrap(void) {
    MA_Arena bootstrap_arena = {0};
    MA_Arena *arena = MA_PushStruct(&bootstrap_arena, MA_Arena);
    *arena = bootstrap_arena;
    arena->allocator.obj = arena;
    return arena;
}

MA_API void MA_InitFromBuffer(MA_Arena *arena, void *buffer, size_t size) {
    arena->memory.data = (uint8_t *)buffer;
    arena->memory.commit = size;
    arena->memory.reserve = size;
    arena->alignment = MA_DEFAULT_ALIGNMENT;
    MA_INIT_HOOK(arena);
}

MA_API MA_Arena MA_MakeFromBuffer(void *buffer, size_t size) {
    MA_Arena arena;
    MA_MemoryZero(&arena, sizeof(arena));
    MA_InitFromBuffer(&arena, buffer, size);
    return arena;
}

MA_API char *MA_PushStringCopy(MA_Arena *arena, char *p, size_t size) {
    char *copy_buffer = (char *)MA_PushSizeNonZeroed(arena, size + 1);
    MA_MemoryCopy(copy_buffer, p, size);
    copy_buffer[size] = 0;
    return copy_buffer;
}

MA_API void *MA_PushCopy(MA_Arena *arena, void *p, size_t size) {
    void *copy_buffer = MA_PushSizeNonZeroed(arena, size);
    MA_MemoryCopy(copy_buffer, p, size);
    return copy_buffer;
}

MA_API bool MA_IsPointerInside(MA_Arena *arena, void *p) {
    uintptr_t pointer = (uintptr_t)p;
    uintptr_t start = (uintptr_t)arena->memory.data;
    uintptr_t stop = start + (uintptr_t)arena->len;
    bool result = pointer >= start && pointer < stop;
    return result;
}

MA_API MA_Arena MA_PushArena(MA_Arena *arena, size_t size) {
    MA_Arena result;
    MA_MemoryZero(&result, sizeof(result));
    result.memory.data = MA_PushArrayNonZeroed(arena, uint8_t, size);
    result.memory.commit = size;
    result.memory.reserve = size;
    result.alignment = arena->alignment;
    return result;
}

MA_API MA_Checkpoint MA_Save(MA_Arena *arena) {
    MA_Checkpoint result;
    result.pos = arena->len;
    result.arena = arena;
    return result;
}

MA_API void MA_Load(MA_Checkpoint checkpoint) {
    MA_PopToPos(checkpoint.arena, checkpoint.pos);
}

MA_API void *M_AllocNonZeroed(M_Allocator allocator, size_t size) {
    void *p = allocator.p(allocator.obj, M_AllocatorOp_Allocate, NULL, size);
    return p;
}

MA_API void *M_Alloc(M_Allocator allocator, size_t size) {
    void *p = allocator.p(allocator.obj, M_AllocatorOp_Allocate, NULL, size);
    MA_MemoryZero(p, size);
    return p;
}

MA_API void *M_AllocCopy(M_Allocator allocator, void *p, size_t size) {
    void *copy_buffer = M_AllocNonZeroed(allocator, size);
    MA_MemoryCopy(copy_buffer, p, size);
    return copy_buffer;
}

MA_API void M_Dealloc(M_Allocator allocator, void *p) {
    allocator.p(allocator.obj, M_AllocatorOp_Deallocate, p, 0);
}

MA_FN void *M_ClibAllocatorProc(void *allocator, M_AllocatorOp kind, void *p, size_t size) {
    if (kind == M_AllocatorOp_Allocate) {
        return MA_CMalloc(size);
    }

    if (kind == M_AllocatorOp_Deallocate) {
        MA_CFree(p);
        return NULL;
    }

    MA_ASSERT("MA_Arena invalid codepath");
    return NULL;
}

MA_API void *MA_AllocatorProc(M_Allocator allocator, M_AllocatorOp kind, void *p, size_t size) {
    if (kind == M_AllocatorOp_Allocate) {
        return MA_PushSizeNonZeroed((MA_Arena *)allocator.obj, size);
    }

    if (kind == M_AllocatorOp_Deallocate) {
        return NULL;
    }

    MA_ASSERT("MA_Arena invalid codepath");
    return NULL;
}

MA_API M_Allocator M_GetSystemAllocator(void) {
    M_Allocator allocator;
    allocator.obj = 0;
    allocator.p = M_ClibAllocatorProc;
    return allocator;
}

#ifndef MA_DISABLE_SCRATCH
MA_THREAD_LOCAL MA_Arena MA_ScratchArenaPool[4];

MA_API MA_Checkpoint MA_GetScratchEx(MA_Arena **conflicts, int conflict_count) {
    MA_Arena *unoccupied = 0;
    for (int i = 0; i < MA_LENGTHOF(MA_ScratchArenaPool); i += 1) {
        MA_Arena *from_pool = MA_ScratchArenaPool + i;
        unoccupied = from_pool;
        for (int conflict_i = 0; conflict_i < conflict_count; conflict_i += 1) {
            MA_Arena *from_conflict = conflicts[conflict_i];
            if (from_pool == from_conflict) {
                unoccupied = 0;
                break;
            }
        }

        if (unoccupied) {
            break;
        }
    }

    MA_ASSERT(unoccupied);
    MA_Checkpoint result = MA_Save(unoccupied);
    return result;
}

MA_API MA_Checkpoint MA_GetScratch(void) {
    MA_Checkpoint result = MA_Save(MA_ScratchArenaPool + 0);
    return result;
}

MA_API MA_Checkpoint MA_GetScratch1(MA_Arena *conflict) {
    MA_Arena *conflicts[] = {conflict};
    return MA_GetScratchEx(conflicts, 1);
}
#endif // MA_DISABLE_SCRATCH

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>

const size_t MV__WIN32_PAGE_SIZE = 4096;

MA_API MV_Memory MV_Reserve(size_t size) {
    MV_Memory result;
    MA_MemoryZero(&result, sizeof(result));
    size_t adjusted_size = MA_AlignUp(size, MV__WIN32_PAGE_SIZE);
    result.data = (uint8_t *)VirtualAlloc(0, adjusted_size, MEM_RESERVE, PAGE_READWRITE);
    MA_ASSERT(result.data && "Failed to reserve virtual memory");
    result.reserve = adjusted_size;
    return result;
}

MA_API bool MV_Commit(MV_Memory *m, size_t commit) {
    uint8_t *pointer = MV__AdvanceCommit(m, &commit, MV__WIN32_PAGE_SIZE);
    if (pointer) {
        void *result = VirtualAlloc(pointer, commit, MEM_COMMIT, PAGE_READWRITE);
        MA_ASSERT(result && "Failed to commit more memory");
        if (result) {
            m->commit += commit;
            return true;
        }
    }
    return false;
}

MA_API void MV_Deallocate(MV_Memory *m) {
    BOOL result = VirtualFree(m->data, 0, MEM_RELEASE);
    MA_ASSERT(result != 0 && "Failed to release MV_Memory");
}

MA_API bool MV_DecommitPos(MV_Memory *m, size_t pos) {
    size_t aligned = MA_AlignDown(pos, MV__WIN32_PAGE_SIZE);
    size_t adjusted_pos = MA_CLAMP_TOP(aligned, m->commit);
    size_t size_to_decommit = m->commit - adjusted_pos;
    if (size_to_decommit) {
        uint8_t *base_address = m->data + adjusted_pos;
        BOOL result = VirtualFree(base_address, size_to_decommit, MEM_DECOMMIT);
        if (result) {
            m->commit -= size_to_decommit;
            return true;
        }
    }
    return false;
}

#elif __unix__ // _WIN32
    #include <sys/mman.h>
    #define MV__UNIX_PAGE_SIZE 4096
MA_API MV_Memory MV_Reserve(size_t size) {
    MV_Memory result = {};
    size_t size_aligned = MA_AlignUp(size, MV__UNIX_PAGE_SIZE);
    result.data = (uint8_t *)mmap(0, size_aligned, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    MA_ASSERT(result.data && "Failed to reserve memory using mmap!!");
    if (result.data) {
        result.reserve = size_aligned;
    }
    return result;
}

MA_API bool MV_Commit(MV_Memory *m, size_t commit) {
    uint8_t *pointer = MV__AdvanceCommit(m, &commit, MV__UNIX_PAGE_SIZE);
    if (pointer) {
        int mprotect_result = mprotect(pointer, commit, PROT_READ | PROT_WRITE);
        MA_ASSERT(mprotect_result == 0 && "Failed to commit more memory using mmap");
        if (mprotect_result == 0) {
            m->commit += commit;
            return true;
        }
    }
    return false;
}

MA_API void MV_Deallocate(MV_Memory *m) {
    int result = munmap(m->data, m->reserve);
    MA_ASSERT(result == 0 && "Failed to release virtual memory using munmap");
}
#else
    #error "unhandled arena platform"
#endif // __unix__
#endif // MA_IMPL
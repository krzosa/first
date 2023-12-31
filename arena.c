#include "arena.h"
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
    #define MA_CRealloc(p, size) realloc(p, size)
#endif

#ifndef MA_StaticFunc
    #if defined(__GNUC__) || defined(__clang__)
        #define MA_StaticFunc __attribute__((unused)) static
    #else
        #define MA_StaticFunc static
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

MA_StaticFunc uint8_t *MV__AdvanceCommit(MV_Memory *m, size_t *commit_size, size_t page_size) {
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

MA_StaticFunc size_t MA__AlignLen(MA_Arena *a) {
    size_t align_offset = a->alignment ? MA_GetAlignOffset((uintptr_t)a->memory.data + (uintptr_t)a->len, a->alignment) : 0;
    size_t aligned = a->len + align_offset;
    return aligned;
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
    return arena;
}

MA_API void MA_InitFromBuffer(MA_Arena *arena, void *buffer, size_t size) {
    arena->memory.data = (uint8_t *)buffer;
    arena->memory.commit = size;
    arena->memory.reserve = size;
    arena->alignment = MA_DEFAULT_ALIGNMENT;
}

MA_API MA_Arena MA_MakeFromBuffer(void *buffer, size_t size) {
    MA_Arena arena;
    MA_MemoryZero(&arena, sizeof(arena));
    MA_InitFromBuffer(&arena, buffer, size);
    return arena;
}

MA_API MA_Arena MA_Create() {
    MA_Arena arena = {0};
    MA_Init(&arena);
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
    void *p = allocator.p(allocator.obj, M_AllocatorOp_Allocate, NULL, size, 0);
    return p;
}

MA_API void *M_Alloc(M_Allocator allocator, size_t size) {
    void *p = allocator.p(allocator.obj, M_AllocatorOp_Allocate, NULL, size, 0);
    MA_MemoryZero(p, size);
    return p;
}

MA_API void *M_AllocCopy(M_Allocator allocator, void *p, size_t size) {
    void *copy_buffer = M_AllocNonZeroed(allocator, size);
    MA_MemoryCopy(copy_buffer, p, size);
    return copy_buffer;
}

MA_API void M_Dealloc(M_Allocator allocator, void *p) {
    allocator.p(allocator.obj, M_AllocatorOp_Deallocate, p, 0, 0);
}

MA_API void *M_ReallocNonZeroed(M_Allocator allocator, void *p, size_t size, size_t old_size) {
    void *result = allocator.p(allocator.obj, M_AllocatorOp_Reallocate, p, size, old_size);
    // @todo: add old_size? because we can't zero
    return result;
}

MA_StaticFunc void *M_ClibAllocatorProc(void *allocator, M_AllocatorOp kind, void *p, size_t size, size_t old_size) {
    if (kind == M_AllocatorOp_Allocate) {
        return MA_CMalloc(size);
    }

    if (kind == M_AllocatorOp_Deallocate) {
        MA_CFree(p);
        return NULL;
    }

    if (kind == M_AllocatorOp_Reallocate) {
        return MA_CRealloc(p, size);
    }

    MA_ASSERT("MA_Arena invalid codepath");
    return NULL;
}

MA_API void *MA_AllocatorProc(void *allocator, M_AllocatorOp kind, void *p, size_t size, size_t old_size) {
    if (kind == M_AllocatorOp_Allocate) {
        return MA_PushSizeNonZeroed((MA_Arena *)allocator, size);
    }

    else if (kind == M_AllocatorOp_Reallocate) {
        void *new_p = MA_PushSizeNonZeroed((MA_Arena *)allocator, size);
        MA_MemoryCopy(new_p, p, old_size);
        return new_p;
    }

    else if (kind == M_AllocatorOp_Deallocate) {
        return NULL;
    }

    MA_ASSERT("MA_Arena invalid codepath");
    return NULL;
}

MA_API void *MA_ExclusiveAllocatorProc(void *allocator, M_AllocatorOp kind, void *p, size_t size, size_t old_size) {
    MA_Arena *arena = (MA_Arena *)allocator;
    if (kind == M_AllocatorOp_Reallocate) {
        if (size > arena->len) {
            size_t size_to_push = size - arena->len;
            MA_PushSizeNonZeroed(arena, size_to_push);
            return arena->memory.data;
        }
    }

    if (kind == M_AllocatorOp_Deallocate) {
        MA_DeallocateArena(arena);
        return NULL;
    }

    MA_ASSERT("MA_Arena invalid codepath");
    return NULL;
}

MA_API M_Allocator MA_GetExclusiveAllocator(MA_Arena *arena) {
    M_Allocator allocator = {arena, MA_ExclusiveAllocatorProc};
    return allocator;
}

MA_API M_Allocator MA_GetAllocator(MA_Arena *arena) {
    M_Allocator allocator = {arena, MA_AllocatorProc};
    return allocator;
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
#ifndef MA_HEADER
#define MA_HEADER
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MA_KIB(x) ((x##ull) * 1024ull)
#define MA_MIB(x) (MA_KIB(x) * 1024ull)
#define MA_GIB(x) (MA_MIB(x) * 1024ull)
#define MA_TIB(x) (MA_GIB(x) * 1024ull)

typedef struct MV_Memory MV_Memory;
typedef struct MA_Checkpoint MA_Checkpoint;
typedef struct MA_Arena MA_Arena;
typedef struct M_Allocator M_Allocator;

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
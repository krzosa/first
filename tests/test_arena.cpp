void TestBootstrapArenaClear() {
    MA_Arena *arena = MA_Bootstrap();
    IO_Assert(arena->base_len != 0);

    MA_PushSize(arena, 1024);
    MA_Reset(arena);

    int *vals = MA_PushArray(arena, int, 1024);
    for (int i = 0; i < 1024; i += 1) {
        vals[i] = i;
    }

    int *a = MA_PushStruct(arena, int);
    *a = 1;

    for (int i = 0; i < 1024; i += 1) {
        IO_Assert(vals[i] == i);
    }

    size_t len = arena->len;
    MA_PopSize(arena, 512);
    IO_Assert(len == arena->len + 512);
    ((char *)arena->memory.data)[arena->len - 1] = 0;

    MA_PopToPos(arena, 512);
    IO_Assert(arena->len == 512);
    ((char *)arena->memory.data)[arena->len - 1] = 0;
}

void TestBuffer() {
    char buffer[1024];
    MA_Arena buffer_arena = MA_MakeFromBuffer(buffer, 1024);
    void *result = MA_PushSize(&buffer_arena, 1024);
    IO_Assert(buffer == result);
    IO_Assert(buffer_arena.len == 1024);
    IO_Assert(buffer_arena.memory.commit == 1024);
    IO_Assert(buffer_arena.memory.reserve == 1024);
}

void TestCreateAllocate() {
    MA_Arena created_arena = MA_Create();
    IO_Assert(MA_GetTop(&created_arena) != 0);
    MA_PushSize(&created_arena, MA_MIB(64));
    IO_Assert(created_arena.len == MA_MIB(64));
    MA_DeallocateArena(&created_arena);
}

void TestBootstrap() {
    MA_Arena *arena = MA_Bootstrap();
    void *r0 = MA_PushSize(arena, 1024);
    void *r1 = MA_PushSize(arena, 1024);
    IO_Assert(arena->len >= 2048);
    IO_Assert(r0 != r1);
    MA_DeallocateArena(arena);
}

void TestBootstrapExclusive() {
    Array<int> v = {MA_BootstrapExclusive()};
    v.add(0);
    int *ptr = &v[0];
    for (int i = 1; i < 1000; i += 1) {
        IO_Assert(&v[0] == ptr);
        v.add(i);
    }
    int i = 0;
    For(v) IO_Assert(it == i++);
    v.dealloc();
}

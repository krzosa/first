#include "../core.c"

void TestScratch() {
    MA_Arena *scratch_arena_test = NULL;
    {
        MA_Scratch scratch;
        IO_Assert(scratch.checkpoint.arena->len == 0);
        IO_Assert(scratch.checkpoint.arena->memory.data == NULL);
        int *a = MA_PushStruct(scratch, int);
        IO_Assert(scratch.checkpoint.arena->memory.data);
        IO_Assert(scratch.checkpoint.arena->len == sizeof(int));
        IO_Assert(scratch.checkpoint.arena);
        scratch_arena_test = scratch.checkpoint.arena;

        int b = 10;
        IO_Assert(MA_IsPointerInside(scratch, a));
        IO_Assert(!MA_IsPointerInside(scratch, &b));
    }

    {
        MA_Scratch scratch;
        IO_Assert(scratch_arena_test == scratch.checkpoint.arena);
        IO_Assert(scratch.checkpoint.arena->len == 0);

        MA_Scratch scratch2(scratch.checkpoint);
        IO_Assert(scratch.checkpoint.arena != scratch2.checkpoint.arena);

        MA_Scratch scratch3(scratch.checkpoint, scratch2.checkpoint);
        IO_Assert(scratch3.checkpoint.arena != scratch2.checkpoint.arena);
    }
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

int main() {
    TestScratch();
    TestBuffer();
    TestCreateAllocate();
}
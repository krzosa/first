#include "../io.c"
#define MA_ASSERT(x) IO_Assert(x)
#include "../arena.c"

#include "../defer.hpp"
#define ARRAY_REALLOCATE(allocator, p, size, old_size) M_ReallocNonZeroed(allocator, p, size, old_size)
#define ARRAY_DEALLOCATE(allocator, p) M_Dealloc(allocator, p)
#define ARRAY_Allocator M_Allocator
#define ARRAY_SET_DEFAULT_ALLOCATOR \
    if (!allocator.p) allocator = M_GetSystemAllocator();
#include "../array.hpp"

void TestExclusiveArenaBackedArray() {
    MA_Scratch scratch;
    MA_Arena ex = MA_Create();
    Array<int> array = {MA_GetExclusiveAllocator(&ex)};
    Array<int *> ptrs = {scratch};
    array.reserve(16);
    ptrs.reserve(16);
    void *initial_p0 = array.data;
    void *initial_p1 = ptrs.data;

    for (int i = 0; i < 1000; i += 1) {
        array.add(i);
        ptrs.add(&array[i]);
    }
    for (int i = 0; i < 1000; i += 1) {
        IO_Assert(array[i] == i);
        IO_Assert(&array[i] == ptrs[i]);
    }

    int i = 0;
    For(array) {
        IO_Assert(it == i++);
    }
    IO_Assert(initial_p0 == array.data);
    IO_Assert(initial_p1 != ptrs.data);

    array.dealloc();
}

Array<int> GenArray() {
    Array<int> result = {M_GetSystemAllocator()};
    for (int i = 0; i < 100; i += 1) result.add(i);
    return result;
}

void TestRemoveForLoop() {
    Array<int> array = GenArray();
    IO_Assert(array.len == 100);
    IO_Assert(array[4] == 4);
    ForArrayRemovable(array) {
        ForArrayRemovablePrepare(array);
        if (it == 4) ForArrayRemovableDeclare();
    }
    IO_Assert(array[4] != 4);
    IO_Assert(array[4] == 5);
    IO_Assert(array[5] == 6);
    IO_Assert(array[3] == 3);
    IO_Assert(array.len == 99);
}

void TestBasic() {
    Array<int> array = GenArray();
    array.unordered_remove_index(40);
    IO_Assert(array.len == 99);
    IO_Assert(array[40] != 40);
    IO_Assert(array[40] == 99);

    array.ordered_remove_index(35);
    IO_Assert(array.len == 98);
    IO_Assert(array[35] == 36);

    array.ordered_remove_index(array[34]);
    IO_Assert(array.len == 97);
    IO_Assert(array[34] == 36);

    array.unordered_remove(array[30]);
    IO_Assert(array.len == 96);
    IO_Assert(array[30] != 30);
    IO_Assert(array[30] == 98);

    array.insert(101, 20);
    IO_Assert(array[20] == 101);
    IO_Assert(array[21] == 20);
    IO_Assert(array.len == 97);

    IO_Assert(array.contains(array[20]));
    IO_Assert(array.contains(array[96]));
    IO_Assert(array.contains(array[0]));
    IO_Assert(!array.contains(array.data[97]));

    IO_Assert(array.is_first(array[0]));
    IO_Assert(!array.is_first(array[1]));

    IO_Assert(!array.is_last(array[0]));
    IO_Assert(array.is_last(array[array.len - 1]));

    array.reset();
    IO_Assert(array.len == 0);
    array.dealloc();
}

void TestReverseLoop() {
    Array<int> array = GenArray();

    int i = 99;
    For(array.reverse()) {
        assert(it == i--);
    }

    array.dealloc();
}

int main() {
    TestExclusiveArenaBackedArray();
    TestRemoveForLoop();
    TestBasic();
    TestReverseLoop();

    return 0;
}
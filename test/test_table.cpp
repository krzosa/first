#include "../core.c"

void TestSimpleInsertAndIntegrity() {
    MA_Scratch scratch;
    Table<uint64_t> table = {scratch};
    table.reserve(64);

    for (uint64_t i = 0; i < 10000; i += 1) {
        table.insert(i, i);
    }
    for (uint64_t i = 0; i < 10000; i += 1) {
        uint64_t *v = table.get(i);
        IO_Assert(*v == i);
    }
    IO_Assert(table.len == 10000);
    IO_Assert(table.cap > table.len);
    IO_Assert(MA_IS_POW2(table.cap));
    table.remove(32);
    table.reset();
    table.dealloc();
}

void TestStrings() {
    struct Data {
        int a[32];
        int i;
    };

    Table<Data> table = {};
    table.puts("1", Data{{}, 1});
    table.puts("2", Data{{}, 2});
    table.puts("3", Data{{}, 3});

    IO_Assert(table.gets("1")->i == 1);
    IO_Assert(table.gets("2")->i == 2);
    IO_Assert(table.gets("3")->i == 3);
}

int main() {
    IO_Printf("test_table.cpp - ");
    TestSimpleInsertAndIntegrity();
    TestStrings();
    IO_Printf("DONE\n");
    return 0;
}
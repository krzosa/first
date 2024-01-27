#include "../core_library/core.cpp"
#include "test_table.cpp"
#include "test_array.cpp"
#include "test_arena.cpp"
#include "test_lexer.cpp"

int main() {
    TestSimpleInsertAndIntegrity();
    IO_Printf("TestSimpleInsertAndIntegrity() - DONE\n");
    TestStrings();
    IO_Printf("TestStrings() - DONE\n");

    TestExclusiveArenaBackedArray();
    IO_Printf("TestExclusiveArenaBackedArray() - DONE\n");
    TestRemoveForLoop();
    IO_Printf("TestRemoveForLoop() - DONE\n");
    TestBasic();
    IO_Printf("TestBasic() - DONE\n");
    TestReverseLoop();
    IO_Printf("TestReverseLoop() - DONE\n");
    TestCopy();
    IO_Printf("TestCopy() - DONE\n");

    TestBuffer();
    IO_Printf("TestBuffer() - DONE\n");
    TestCreateAllocate();
    IO_Printf("TestCreateAllocate() - DONE\n");
    TestBootstrap();
    IO_Printf("TestBootstrap() - DONE\n");
    TestBootstrapExclusive();
    IO_Printf("TestBootstrapExclusive() - DONE\n");
    TestBootstrapArenaClear();
    IO_Printf("TestBootstrapArenaClear() - DONE\n");

    TestClexer();
    IO_Printf("TestClexer() - DONE\n");

    // Unicode iteration over codepoints using For
    {
        S8_String s = "mrówka";

        bool found_two_byte = false;
        For(s) {
            if (it.utf8_codepoint_byte_size == 2) {
                found_two_byte = true;
                IO_Assert(it.i == 2);
            }
        }
        IO_Assert(found_two_byte);
    }

    // List iteration using For
    {
        MA_Scratch scratch;
        S8_List list = {};
        S8_AddF(scratch, &list, "1");
        S8_AddF(scratch, &list, "2");
        S8_AddF(scratch, &list, "3");
        S8_AddF(scratch, &list, "4");
        S8_AddF(scratch, &list, "5");

        int i = 0;
        For(list) {
            i += 1;
        }
        IO_Assert(i == 5);
    }
}

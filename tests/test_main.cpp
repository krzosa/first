#include "../core/core.cpp"
#include "test_table.cpp"
#include "test_array.cpp"
#include "test_arena.cpp"
#include "test_lexer.cpp"

int main() {
    TestSimpleInsertAndIntegrity();
    TestStrings();

    TestExclusiveArenaBackedArray();
    TestRemoveForLoop();
    TestBasic();
    TestReverseLoop();
    TestCopy();

    TestBuffer();
    TestCreateAllocate();
    TestBootstrap();
    TestBootstrapExclusive();
    TestBootstrapArenaClear();

    // @todo: github worker fails on this:
    TestClexer();

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

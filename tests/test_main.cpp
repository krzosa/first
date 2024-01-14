#include "../core_library/core.cpp"
#include "test_table.cpp"
#include "test_array.cpp"
#include "test_arena.cpp"

void TestClexer();
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

    TestClexer();

    // Unicode iteration over codepoints
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
}

#define CL_Allocator MA_Arena *
#define CL_Allocate(a, s) MA_PushSizeNonZeroed(a, s)
#define CL_ASSERT IO_Assert
#define CL_VSNPRINTF stbsp_vsnprintf
#define CL_SNPRINTF stbsp_snprintf
#include "../standalone_libraries/clexer.c"

void TestClexer() {
    MA_Scratch scratch;

    S8_String filename = "../standalone_libraries/clexer.c";
    S8_String file = OS_ReadFile(scratch, filename);
    CL_Lexer lexer = CL_Begin(scratch, file.str, filename.str);

    char buff[1024];
    for (;;) {
        CL_Token token = CL_Next(&lexer);
        if (token.kind == CL_EOF) break;
        if (token.kind != CL_PREPROC_INCLUDE) continue;

        CL_Stringify(buff, sizeof(buff), &token);
        // IO_Printf("%s\n", buff);
    }
}
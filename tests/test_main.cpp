#include "../core/core.cpp"
#include "test_table.cpp"
#include "test_array.cpp"
#include "test_arena.cpp"
#include "test_lexer.cpp"

// ./a.exe -scratch
// ./a.exe -quick
// ./a.exe -run_tests a.test b.test c.test
// ./a.exe --break_on_error
// ./a.exe -build=release
// ./a.exe --help
//
void TestCmdParser() {
    const char *argv[] = {
        "exe",
        "--build_scratch",
        "-tests",
        "a",
        "b",
        "c",
        "--things",
        "1234",
        "asdsa",
        "-build",
        "release",
    };
    int argc = MA_Lengthof(argv);

    MA_Checkpoint scratch              = MA_GetScratch();
    bool          build_scratch        = false;
    S8_List       test_list            = {0};
    S8_List       test_things          = {0};
    int           build_profile        = 0;
    const char   *build_profiles[]     = {"debug", "release"};
    int           build_profiles_count = MA_Lengthof(build_profiles);

    CmdParser p = MakeCmdParser(scratch.arena, argc, (char **)argv, "this is a test");
    AddBool(&p, &build_scratch, "build_scratch", "builds a sandbox where I experiment with things");
    AddList(&p, &test_list, "tests", "list of specific tests to run");
    AddList(&p, &test_things, "things", "list of things");
    AddEnum(&p, &build_profile, "build", "choose build profile", build_profiles, build_profiles_count);
    bool success = ParseCmd(scratch.arena, &p);
    IO_Assertf(success, "failed to parse cmd");
    IO_Assert(build_scratch);
    IO_Assert(test_list.node_count == 3);
    IO_Assert(test_things.node_count == 2);
    IO_Assert(build_profile == 1);
    MA_ReleaseScratch(scratch);
}

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
        S8_List    list = {};
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


#define CL_Allocator MA_Arena *
#define CL_Allocate(a, s) MA_PushSizeNonZeroed(a, s)
#define CL_ASSERT IO_Assert
#define CL_VSNPRINTF stbsp_vsnprintf
#define CL_SNPRINTF stbsp_snprintf
#include "../standalone_libraries/clexer.c"

void TestLexDir(S8_String dir) {
    MA_Scratch scratch_filenames;
    char buff[1024];
    char msg_buff[1024];
    for (OS_FileIter iter = OS_IterateFiles(scratch_filenames, dir); OS_IsValid(iter); OS_Advance(&iter)) {
        MA_Scratch scratch(scratch_filenames.checkpoint);

        S8_String file = OS_ReadFile(scratch, iter.absolute_path);
        if (!file.str) continue;
        CL_Lexer lexer = CL_Begin(scratch, file.str, iter.absolute_path.str);

        for (int i = 0;; i += 1) {
            CL_Token token = CL_Next(&lexer);
            if (token.kind == CL_EOF) break;

            if (lexer.errors) {
                for (CL_Message *it = lexer.first_message; it; it = it->next) {
                    CL_StringifyMessage(msg_buff, sizeof(msg_buff), it);
                    IO_Printf("%s\n", msg_buff);
                }
            }

            CL_Stringify(buff, sizeof(buff), &token);
        }
    }
}

void TestClexer() {
    TestLexDir("../standalone_libraries");
    TestLexDir("../core_library");
    TestLexDir("../build_tool");
    TestLexDir("../tests");
}
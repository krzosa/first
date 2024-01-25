
#define CL_Allocator MA_Arena *
#define CL_Allocate(a, s) MA_PushSizeNonZeroed(a, s)
#define CL_ASSERT IO_Assert
#define CL_VSNPRINTF stbsp_vsnprintf
#define CL_SNPRINTF stbsp_snprintf
#include "../standalone_libraries/clexer.c"

void TestClexer() {
    MA_Scratch scratch;
    for (OS_FileIter iter = OS_IterateFiles(scratch, "../standalone_libraries"); OS_IsValid(iter); OS_Advance(&iter)) {
        S8_String file = OS_ReadFile(scratch, iter.absolute_path);
        CL_Lexer lexer = CL_Begin(scratch, file.str, iter.absolute_path.str);

        char buff[1024];
        char msg_buff[1024];
        int token_count = 0;
        for (;;) {
            if (iter.filename == "preproc_env.h") {
                int a = 10;
            }
            CL_Token token = CL_Next(&lexer);
            if (token.kind == CL_EOF) break;

            if (lexer.errors) {
                for (CL_Message *it = lexer.first_message; it; it = it->next) {
                    CL_StringifyMessage(msg_buff, sizeof(msg_buff), it);
                    IO_Printf("%s\n", msg_buff);
                }
            }

            CL_Stringify(buff, sizeof(buff), &token);
            token_count += 1;
        }
        // IO_Printf("%.*s token count = %d\n", S8_Expand(iter.absolute_path), token_count);
    }

    S8_String filename = "../standalone_libraries/clexer.c";
}
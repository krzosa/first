#include "../core_library/core.c"

int main() {
    MA_Arena arena = {0};
    S8_String read_file_path = S8_Lit("../tests/data/read_file");

    // Read file test
    {
        S8_String file = OS_ReadFile(&arena, read_file_path);
        IO_Assert(S8_AreEqual(file, S8_Lit("OK"), 0));
    }

    // Write file test
    {
        S8_String path = S8_Lit("../tests/data/write_file");
        {
            S8_String data = S8_Lit("WRITE1 OK");
            OS_Result result = OS_WriteFile(path, data);
            IO_Assert(result == OS_SUCCESS);

            S8_String read = OS_ReadFile(&arena, path);
            IO_Assert(S8_AreEqual(read, data, 0));
        }
        S8_String data = S8_Lit("WRITE2 OK");

        OS_Result result = OS_WriteFile(path, data);
        IO_Assert(result == OS_SUCCESS);

        S8_String read = OS_ReadFile(&arena, path);
        IO_Assert(S8_AreEqual(read, data, 0));
    }

    // Append file test
    {
        S8_String path = S8_Lit("../tests/data/append_file");
        {
            S8_String data = S8_Lit("WRITE OK");
            OS_Result result = OS_WriteFile(path, data);
            IO_Assert(result == OS_SUCCESS);
        }
        S8_String data = S8_Lit(" APPEND OK");
        OS_Result result = OS_AppendFile(path, data);
        IO_Assert(result == OS_SUCCESS);

        S8_String read = OS_ReadFile(&arena, path);
        IO_Assert(S8_AreEqual(read, S8_Lit("WRITE OK APPEND OK"), 0));
    }

    IO_Assert(OS_FileExists(read_file_path));
    IO_Assert(!OS_FileExists(S8_Lit("121ffsadasd.random")));

    // Fetching dirs
    {
        S8_String exe_path = OS_GetExePath(&arena);
        S8_String dir_path = OS_GetExeDir(&arena);
        S8_String work_path = OS_GetWorkingDir(&arena);
        S8_String abs_path = OS_GetAbsolutePath(&arena, read_file_path);

        IO_Assert(OS_IsDir(dir_path));
        IO_Assert(!OS_IsFile(dir_path));

        IO_Assert(OS_IsFile(exe_path));
        IO_Assert(!OS_IsDir(exe_path));

        IO_Assert(OS_IsAbsolute(exe_path));
        IO_Assert(OS_IsAbsolute(dir_path));
        IO_Assert(OS_IsAbsolute(work_path));
        IO_Assert(OS_IsAbsolute(abs_path));

        IO_Assert(S8_Find(exe_path, S8_Lit("/test_filesystem"), 0, 0));
        IO_Assert(S8_Find(exe_path, S8_Lit("/build"), 0, 0));
        IO_Assert(S8_Find(dir_path, S8_Lit("/build"), 0, 0));
        IO_Assert(S8_Find(work_path, S8_Lit("/build"), 0, 0));
        IO_Assert(S8_Find(abs_path, S8_Lit("/tests/data"), 0, 0));
        IO_Assert(!S8_Find(abs_path, S8_Lit("../"), 0, 0));
    }

    // List dir test
    {
        S8_List list = OS_ListDir(&arena, S8_Lit("../tests"), 0);
        IO_Assert(list.node_count > 4);
        int dir_count = 0;
        S8_For(it, list) {
            if (OS_IsDir(it->string)) {
                IO_Assert(it->string.str[it->string.len - 1] == '/');
                dir_count += 1;
            }
            IO_Assert(S8_Find(it->string, S8_Lit("/tests"), 0, 0));
            IO_Assert(!S8_Find(it->string, S8_Lit("../tests"), 0, 0));
        }
        IO_Assert(dir_count > 0);

        // relative
        {
            S8_List list = OS_ListDir(&arena, S8_Lit("../tests"), OS_RELATIVE_PATHS);
            S8_For(it, list) {
                IO_Assert(S8_Find(it->string, S8_Lit("../tests"), 0, 0));
            }
        }
    }
}
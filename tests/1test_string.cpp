#include "../core_library/core.c"

int main() {
    MA_Scratch scratch;
    for (OS_FileIter it = OS_IterateFiles(scratch, "../"_s); OS_IsValid(it); OS_Advance(&it)) {
        IO_Printf("is_directory: %d\n", it.is_directory);
        IO_Printf("absolute_path: %.*s\n", S8_Expand(it.absolute_path));
        IO_Printf("relative_path: %.*s\n", S8_Expand(it.relative_path));
    }
}
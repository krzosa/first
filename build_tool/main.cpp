#define BUILD_MAIN
#include "library.cpp"

int main(int argument_count, char **arguments) {
    OS_MakeDir(S8_Lit("build"));
    OS_SetWorkingDir(S8_Lit("build"));
    S8_String working_dir = OS_GetWorkingDir(Perm);
    IO_Printf("WORKING DIR: %.*s\n", S8_Expand(working_dir));

    S8_String cc = ON_WINDOWS("cl"_s) ON_MAC("clang"_s) ON_LINUX("gcc"_s);
    S8_String cache_filename = "build_tool.cache"_s;

    S8_String cmdline_args = S8_MakeEmpty();
    for (int i = 1; i < argument_count; i += 1) {
        S8_String arg = S8_MakeFromChar(arguments[i]);
        if (arg == "clear_cache"_s) {
            OS_DeleteFile(cache_filename);
            break;
        }

        cmdline_args = S8_Format(Perm, "%.*s%.*s ", S8_Expand(cmdline_args), S8_Expand(arg));
    }

    SRC_InitCache(Perm, cache_filename);

    // Search for build file in the project directory
    S8_String build_file = {};
    {
        for (OS_FileIter it = OS_IterateFiles(Perm, S8_Lit("..")); OS_IsValid(it); OS_Advance(&it)) {
            if (S8_Seek(it.filename, S8_Lit("build_file.c"), S8_IgnoreCase)) {
                build_file = it.absolute_path;
            }
        }

        if (build_file.str == 0) {
            IO_Printf("IDLE Nothing to do, couldnt find build file in current dir: %.*s, exiting ... \n", S8_Expand(working_dir));
            return 0;
        }
    }

    S8_String name_no_ext = S8_GetNameNoExt(build_file);
    S8_String exe_name = S8_Format(Perm, "%.*s.exe", S8_Expand(name_no_ext));

    // Compile the build file only if code changed
    if (SRC_WasModified(build_file, exe_name)) {
        double time = OS_GetTime();
        int result = 0;
        if (cc == "cl"_s) {
            result = OS_SystemF("cl %.*s -Fe:%.*s -WX -W3 -wd4200 -diagnostics:column -nologo -Zi", S8_Expand(build_file), S8_Expand(exe_name));
        }
        else if (cc == "clang"_s) {
            result = OS_SystemF("clang++ -std=c++11 -Wno-writable-strings %.*s -o %.*s -g", S8_Expand(build_file), S8_Expand(exe_name));
        }
        else {
            IO_Assert(cc == "gcc"_s);
            result = OS_SystemF("gcc -Wno-write-strings %.*s -o %.*s -g", S8_Expand(build_file), S8_Expand(exe_name));
        }

        if (result != 0) {
            IO_Printf("FAILED compilation of the build file!\n");
            return 1;
        }
        time = OS_GetTime() - time;
        IO_Printf("TIME Build file compilation: %f\n", time);
    }

    // Run the build file
    double time = OS_GetTime();
    if (build_file.str) {
        int result = OS_SystemF(IF_WINDOWS_ELSE("", "./") "%.*s %.*s", S8_Expand(exe_name), S8_Expand(cmdline_args));
        if (result != 0) {
            printf("FAILED execution of the build file!\n");
            return 1;
        }
    }
    time = OS_GetTime() - time;
    IO_Printf("TIME total build file execution: %f\n", time);

    SRC_SaveCache();
    // OS_SetWorkingDir(S8_Lit(".."));
}

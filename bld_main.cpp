#define BLD_MAIN
#include "bld_lib.cpp"

int main(int argument_count, char **arguments) {
    OS_MakeDir(S8_Lit("build"));
    OS_SetWorkingDir(S8_Lit("build"));
    IO_Printf("WORKING DIR: %Q\n", OS_GetWorkingDir(Perm));

    S8_String cc = ON_WINDOWS("cl"_s) ON_MAC("clang"_s) ON_LINUX("gcc"_s);

    for (int i = 1; i < argument_count; i += 1) {
        S8_String arg = S8_MakeFromChar(arguments[i]);
        if (arg == "clear_cache"_s) OS_DeleteFile("bld_tool.cache"_s);
    }

    SRC_InitCache(Perm, S8_Lit("bld.cache"));

    // Search for build file in the project directory
    S8_String build_file = {0};
    {
        S8_List files_current_dir = OS_ListDir(Perm, S8_Lit(".."), 0);
        for (S8_Node *it = files_current_dir.first; it; it = it->next) {
            if (S8_Find(it->string, S8_Lit("bld_file.c"), S8_IGNORE_CASE, 0)) {
                build_file = it->string;
            }
        }

        if (build_file.str == 0) {
            IO_Printf("IDLE Nothing to do, couldnt find build file in current dir: %Q, exiting ... \n", OS_GetWorkingDir(Perm));
            return 0;
        }
    }

    S8_String a = S8_ChopLastPeriod(build_file);
    S8_String b = S8_SkipToLastSlash(a);
    S8_String exe_name = S8_Format(Perm, "%Q.exe", b);

    // @todo: add search path?
    // Compile the build file only if code changed
    if (SRC_WasModified(build_file, exe_name)) {
        double time = OS_GetTime();
        int result = 0;
        if (cc == "cl"_s) {
            result = OS_SystemF("cl %Q -Fe:%Q -WX -W3 -wd4200 -diagnostics:column -nologo -Zi", build_file, exe_name);
        }
        else if (cc == "clang"_s) {
            result = OS_SystemF("clang -Wno-writable-strings %Q -o %Q -g", build_file, exe_name);
        }
        else {
            IO_Assert(cc == "gcc"_s);
            result = OS_SystemF("gcc -Wno-write-strings %Q -o %Q -g", build_file, exe_name);
        }

        if (result != 0) {
            IO_Printf("FAILED compilation of the build file!\n");
            return result;
        }
        time = OS_GetTime() - time;
        IO_Printf("TIME Build file compilation: %f\n", time);
    }

    // Run the build file
    double time = OS_GetTime();
    if (build_file.str) {
#if OS_WINDOWS
        int result = OS_SystemF("%Q", exe_name);
#else
        int result = OS_SystemF("./%Q", exe_name);
#endif
        if (result != 0) {
            printf("FAILED execution of the build file!\n");
            return result;
        }
    }
    time = OS_GetTime() - time;
    IO_Printf("TIME total build file execution: %f\n", time);

    SRC_SaveCache();
    // OS_SetWorkingDir(S8_Lit(".."));
}

#include "../build_tool/library.cpp"
#include "../build_tool/process.cpp"

int main() {
    MA_Scratch scratch;
    S8_String working_dir = OS_GetWorkingDir(scratch);

    Array<S8_String> exes = {scratch};
    Array<TH_Process> proc = {scratch};
    Array<S8_String> files = {scratch};
    files.add("../../tests/test_main.cpp");
    files.add("../../tests/test_filesystem.c");

    For2(file, files) {
        S8_String name_no_ext = S8_GetNameNoExt(file);
        S8_String exe = Fmt("%.*s.exe", S8_Expand(name_no_ext));
        bool is_cpp = S8_EndsWith(file, ".cpp");

#if OS_WINDOWS
        // Setup CL Debug
        {
            S8_String cc = "cl";

            Array<S8_String> flags = {scratch};
            flags += "/MP /Zi -D_CRT_SECURE_NO_WARNINGS";
            flags += "/FC /WX /W3 /wd4200 /diagnostics:column /nologo";
            flags += "/GF /Gm- /Oi";
            flags += "/GR- /EHa-";
            flags += Fmt("/Fe:%.*s", S8_Expand(exe));

            Array<S8_String> debug = {scratch};
            debug += "/D_DEBUG -RTC1 -Od";

            Array<S8_String> release = {scratch};
            release += "-O2 -MT -DNDEBUG -GL";

            Array<S8_String> link = {scratch};
            link += "/link /incremental:no";

            S8_String dir_debug = Fmt("%.*s/%.*s_cl_debug_" OS_NAME, S8_Expand(working_dir), S8_Expand(name_no_ext));
            S8_String dir_release = Fmt("%.*s/%.*s_cl_release_" OS_NAME, S8_Expand(working_dir), S8_Expand(name_no_ext));

            OS_MakeDir(dir_debug);
            OS_MakeDir(dir_release);

            Array<S8_String> cmd_debug = cc + file + flags + debug + link;
            Array<S8_String> cmd_release = cc + file + flags + release + link;

            S8_String cmd_debug_m = Merge(cmd_debug);
            S8_String cmd_release_m = Merge(cmd_release);

            TH_Process p1 = TH_CreateProcess(cmd_debug_m, dir_debug);
            TH_Process p2 = TH_CreateProcess(cmd_release_m, dir_release);

            proc.add(p1);
            proc.add(p2);

            exes.add(Fmt("%.*s/%.*s", S8_Expand(dir_debug), S8_Expand(exe)));
            exes.add(Fmt("%.*s/%.*s", S8_Expand(dir_release), S8_Expand(exe)));
        }
#endif

        {
            S8_String cc = is_cpp ? "clang++" : "clang";

            Array<S8_String> flags = {scratch};
            flags += "-g -Wno-write-strings";
            flags += "-fdiagnostics-absolute-paths";
            flags += "-fsanitize=address";
            if (is_cpp) flags += "-std=c++11";
            flags += Fmt("-o %.*s", S8_Expand(exe));

            S8_String dir = Fmt("%.*s/%.*s_debug_clang_" OS_NAME, S8_Expand(working_dir), S8_Expand(name_no_ext));
            OS_MakeDir(dir);

            S8_String cmd = Merge(cc + file + flags);
            TH_Process p = TH_CreateProcess(cmd, dir);
            proc.add(p);
            exes.add(Fmt("%.*s/%.*s", S8_Expand(dir), S8_Expand(exe)));
        }

#if OS_LINUX
        {
            S8_String cc = is_cpp ? "g++" : "gcc";
            Array<S8_String> flags = {scratch};
            flags += "-g -Wno-write-strings";
            flags += "-fsanitize=address";
            if (is_cpp) flags += "-std=c++11";
            flags += Fmt("-o %.*s", S8_Expand(exe));

            S8_String dir = Fmt("%.*s/%.*s_debug_gcc_" OS_NAME, S8_Expand(working_dir), S8_Expand(name_no_ext));
            OS_MakeDir(dir);

            S8_String cmd = Merge(cc + file + flags);
            TH_Process p = TH_CreateProcess(cmd, dir);
            proc.add(p);
            exes.add(Fmt("%.*s/%.*s", S8_Expand(dir), S8_Expand(exe)));
        }
#endif
    }

    //
    // Wait for all compilation to finalize
    //
    int result = 0;
    For(proc) {
        int exit_code = TH_WaitForProcessExit(&it);
        if (exit_code != 0) result = exit_code;
    }

    //
    // Run exes if there were no errors
    //
    if (result == 0) {
        proc.reset();
        For(exes) {
            TH_Process p = TH_CreateProcess(it);
            proc.add(p);
        }

        For(proc) {
            int exit_code = TH_WaitForProcessExit(&it);
            if (exit_code != 0) result = exit_code;
        }
    }

    return result;
}
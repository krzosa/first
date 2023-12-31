#include "filesystem.h"

#define OS_LENGTHOF(x) ((int64_t)((sizeof(x) / sizeof((x)[0]))))

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <stdio.h>
    #include <stdlib.h>

OS_API bool OS_EnableTerminalColors(void) {
    // Enable color terminal output
    {
        // Set output mode to handle virtual terminal sequences
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (SetConsoleMode(hOut, dwMode)) {
                    return true;
                }
                else {
                    IO_Printf("Failed to enable colored terminal output C\n");
                }
            }
            else {
                IO_Printf("Failed to enable colored terminal output B\n");
            }
        }
        else {
            IO_Printf("Failed to enable colored terminal output A\n");
        }
    }
    return false;
}

OS_API bool OS_IsAbsolute(S8_String path) {
    bool result = path.len > 3 && CHAR_IsAlphabetic(path.str[0]) && path.str[1] == ':' && path.str[2] == '/';
    return result;
}

OS_API S8_String OS_GetExePath(MA_Arena *arena) {
    wchar_t wbuffer[1024];
    DWORD wsize = GetModuleFileNameW(0, wbuffer, OS_LENGTHOF(wbuffer));
    IO_Assert(wsize != 0);

    S8_String path = UTF_CreateStringFromWidechar(arena, wbuffer, wsize);
    S8_NormalizePath(path);
    return path;
}

OS_API S8_String OS_GetExeDir(MA_Arena *arena) {
    S8_String path = OS_GetExePath(arena);
    path = S8_ChopLastSlash(path);
    path.str[path.len] = 0;
    return path;
}

OS_API S8_String OS_GetWorkingDir(MA_Arena *arena) {
    wchar_t wbuffer[1024];
    DWORD wsize = GetCurrentDirectoryW(OS_LENGTHOF(wbuffer), wbuffer);
    IO_Assert(wsize != 0);
    IO_Assert(wsize < 1022);
    wbuffer[wsize++] = '/';
    wbuffer[wsize] = 0;

    S8_String path = UTF_CreateStringFromWidechar(arena, wbuffer, wsize);
    S8_NormalizePath(path);
    return path;
}

OS_API void OS_SetWorkingDir(S8_String path) {
    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), path.str, path.len);
    SetCurrentDirectoryW(wpath);
}

OS_API S8_String OS_GetAbsolutePath(MA_Arena *arena, S8_String relative) {
    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), relative.str, relative.len);
    wchar_t wpath_abs[1024];
    DWORD written = GetFullPathNameW((wchar_t *)wpath, OS_LENGTHOF(wpath_abs), wpath_abs, 0);
    if (written == 0)
        return S8_MakeEmpty();
    S8_String path = UTF_CreateStringFromWidechar(arena, wpath_abs, written);
    S8_NormalizePath(path);
    return path;
}

OS_API bool OS_FileExists(S8_String path) {
    wchar_t wbuff[1024];
    UTF_CreateWidecharFromChar(wbuff, OS_LENGTHOF(wbuff), path.str, path.len);
    DWORD attribs = GetFileAttributesW(wbuff);
    bool result = attribs == INVALID_FILE_ATTRIBUTES ? false : true;
    return result;
}

OS_API bool OS_IsDir(S8_String path) {
    wchar_t wbuff[1024];
    UTF_CreateWidecharFromChar(wbuff, OS_LENGTHOF(wbuff), path.str, path.len);
    DWORD dwAttrib = GetFileAttributesW(wbuff);
    return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

OS_API bool OS_IsFile(S8_String path) {
    wchar_t wbuff[1024];
    UTF_CreateWidecharFromChar(wbuff, OS_LENGTHOF(wbuff), path.str, path.len);
    DWORD dwAttrib = GetFileAttributesW(wbuff);
    bool is_file = (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0;
    return dwAttrib != INVALID_FILE_ATTRIBUTES && is_file;
}

OS_API double OS_GetTime(void) {
    static int64_t counts_per_second;
    if (counts_per_second == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        counts_per_second = freq.QuadPart;
    }

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    double result = (double)time.QuadPart / (double)counts_per_second;
    return result;
}

// @todo: I think we want a list with both relative + absolute + other things
//
// Returns directories with slash at the end '/'
// By default returns absolute paths
OS_API S8_List OS_ListDir(MA_Arena *arena, S8_String path, unsigned flags) {
    MA_Checkpoint scratch = MA_GetScratch1(arena);
    S8_List dirs_to_read = S8_MakeEmptyList();
    S8_List result = S8_MakeEmptyList();
    S8_AddNode(scratch.arena, &dirs_to_read, path);

    for (S8_Node *it = dirs_to_read.first; it; it = it->next) {
        wchar_t wbuff[1024];
        S8_String modified_path = S8_Format(scratch.arena, "%.*s\\*", (int)it->string.len, it->string.str);
        IO_Assert(modified_path.len < OS_LENGTHOF(wbuff));
        int64_t wsize = UTF_CreateWidecharFromChar(wbuff, OS_LENGTHOF(wbuff), modified_path.str, modified_path.len);
        IO_Assert(wsize);

        WIN32_FIND_DATAW ffd;
        HANDLE handle = FindFirstFileW(wbuff, &ffd);
        if (handle == INVALID_HANDLE_VALUE)
            continue;

        do {

            //
            // Skip '.' and '..'
            //
            if (ffd.cFileName[0] == '.') {
                if (ffd.cFileName[1] == '.') {
                    if (ffd.cFileName[2] == 0)
                        continue;
                }

                if (ffd.cFileName[1] == 0)
                    continue;
            }

            bool dir = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            S8_String filename = UTF_CreateStringFromWidechar(scratch.arena, ffd.cFileName, S8_WideLength(ffd.cFileName));
            S8_String rel_abs_path = S8_Format(scratch.arena, "%Q/%Q%Q", it->string, filename, dir ? S8_Lit("/") : S8_Lit(""));
            if (flags & OS_RELATIVE_PATHS) {
                S8_AddNode(arena, &result, rel_abs_path);
            }
            else {
                S8_String abs_path = OS_GetAbsolutePath(arena, rel_abs_path);
                S8_AddNode(arena, &result, abs_path);
            }

            if (dir && flags & OS_RECURSIVE) {
                S8_AddNode(scratch.arena, &dirs_to_read, rel_abs_path);
            }
        } while (FindNextFileW(handle, &ffd) != 0);

        DWORD error = GetLastError();
        if (error != ERROR_NO_MORE_FILES) {
            // Not sure what to do here hmmm
        }
        FindClose(handle);
    }

    MA_ReleaseScratch(scratch);
    return result;
}

OS_API OS_Result OS_MakeDir(S8_String path) {
    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), path.str, path.len);
    BOOL success = CreateDirectoryW(wpath, NULL);
    OS_Result result = OS_SUCCESS;
    if (success == 0) {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            result = OS_ALREADY_EXISTS;
        }
        else if (error == ERROR_PATH_NOT_FOUND) {
            result = OS_PATH_NOT_FOUND;
        }
        else {
            IO_Assert(0);
        }
    }
    return result;
}

OS_API OS_Result OS_CopyFile(S8_String from, S8_String to, bool overwrite) {
    wchar_t wfrom[1024];
    UTF_CreateWidecharFromChar(wfrom, OS_LENGTHOF(wfrom), from.str, from.len);

    wchar_t wto[1024];
    UTF_CreateWidecharFromChar(wto, OS_LENGTHOF(wto), to.str, to.len);

    BOOL fail_if_exists = !overwrite;
    BOOL success = CopyFileW(wfrom, wto, fail_if_exists);

    OS_Result result = OS_SUCCESS;
    if (success == FALSE)
        result = OS_FAILURE;
    return result;
}

OS_API OS_Result OS_DeleteFile(S8_String path) {
    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), path.str, path.len);
    BOOL success = DeleteFileW(wpath);
    OS_Result result = OS_SUCCESS;
    if (success == 0)
        result = OS_PATH_NOT_FOUND;
    return result;
}

OS_API OS_Result OS_DeleteDir(S8_String path, unsigned flags) {
    if (flags & OS_RECURSIVE) {
        MA_Checkpoint scratch = MA_GetScratch();
        S8_List list = OS_ListDir(scratch.arena, path, OS_RECURSIVE);
        S8_Node *dirs_to_remove = 0;
        for (S8_Node *it = list.first; it; it = it->next) {
            if (!S8_EndsWith(it->string, S8_Lit("/"), S8_IGNORE_CASE)) {
                OS_DeleteFile(it->string);
            }
            else {
                S8_Node *node = S8_CreateNode(scratch.arena, it->string);
                SLL_STACK_ADD(dirs_to_remove, node);
            }
        }
        for (S8_Node *it = dirs_to_remove; it; it = it->next) {
            OS_DeleteDir(it->string, OS_NO_FLAGS);
        }
        OS_Result result = OS_DeleteDir(path, OS_NO_FLAGS);
        MA_ReleaseScratch(scratch);
        return result;
    }
    else {
        wchar_t wpath[1024];
        UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), path.str, path.len);
        BOOL success = RemoveDirectoryW(wpath);
        OS_Result result = OS_SUCCESS;
        if (success == 0)
            result = OS_PATH_NOT_FOUND;
        return result;
    }
}

static OS_Result OS__WriteFile(S8_String path, S8_String data, bool append) {
    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), path.str, path.len);
    OS_Result result = OS_FAILURE;

    DWORD access = GENERIC_WRITE;
    DWORD creation_disposition = CREATE_ALWAYS;
    if (append) {
        access = FILE_APPEND_DATA;
        creation_disposition = OPEN_ALWAYS;
    }

    HANDLE handle = CreateFileW(wpath, access, 0, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle != INVALID_HANDLE_VALUE) {
        DWORD bytes_written = 0;
        IO_Assert(data.len == (DWORD)data.len); // @Todo: can only read 32 byte size files?
        BOOL error = WriteFile(handle, data.str, (DWORD)data.len, &bytes_written, NULL);
        if (error == TRUE) {
            if (bytes_written != data.len) {
                result = OS_SUCCESS;
            }
        }
        CloseHandle(handle);
    }
    else
        result = OS_PATH_NOT_FOUND;

    return result;
}

OS_API OS_Result OS_AppendFile(S8_String path, S8_String string) {
    return OS__WriteFile(path, string, true);
} // @untested

OS_API OS_Result OS_WriteFile(S8_String path, S8_String string) {
    return OS__WriteFile(path, string, false);
}

OS_API S8_String OS_ReadFile(MA_Arena *arena, S8_String path) {
    bool success = false;
    S8_String result = S8_MakeEmpty();
    MA_Checkpoint checkpoint = MA_Save(arena);

    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, OS_LENGTHOF(wpath), path.str, path.len);
    HANDLE handle = CreateFileW(wpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(handle, &file_size)) {
            if (file_size.QuadPart != 0) {
                result.len = (int64_t)file_size.QuadPart;
                result.str = (char *)MA_PushSizeNonZeroed(arena, result.len + 1);
                DWORD read;
                if (ReadFile(handle, result.str, (DWORD)result.len, &read, NULL)) { // @todo: can only read 32 byte size files?
                    if (read == result.len) {
                        success = true;
                        result.str[result.len] = 0;
                    }
                }
            }
        }
        CloseHandle(handle);
    }

    if (!success) {
        result = S8_MakeEmpty();
        MA_Load(checkpoint);
    }

    return result;
}

OS_API int OS_SystemF(const char *string, ...) {
    MA_Checkpoint scratch = MA_GetScratch();
    S8_FORMAT(scratch.arena, string, result);
    IO_Printf("Executing: %s\n", result.str);
    fflush(stdout);
    int error_code = system(result.str);
    MA_ReleaseScratch(scratch);
    return error_code;
}

OS_API int64_t OS_GetFileModTime(S8_String file) {
    FILETIME time = {0};
    WIN32_FIND_DATAW data;

    wchar_t wpath[1024];
    UTF_CreateWidecharFromChar(wpath, 1024, file.str, file.len);
    HANDLE handle = FindFirstFileW(wpath, &data);
    if (handle != INVALID_HANDLE_VALUE) {
        FindClose(handle);
        time = data.ftLastWriteTime;
    }
    else {
        return -1;
    }
    int64_t result = (int64_t)time.dwHighDateTime << 32 | time.dwLowDateTime;
    return result;
}

OS_API OS_Date OS_GetDate(void) {
    SYSTEMTIME local;
    GetLocalTime(&local);

    OS_Date result = {0};
    result.year = local.wYear;
    result.month = local.wMonth;
    result.day = local.wDay;
    result.hour = local.wHour;
    result.second = local.wSecond;
    result.milliseconds = local.wMilliseconds;
    return result;
}

#else
OS_API bool OS_EnableTerminalColors(void) { return true; }
OS_API bool OS_IsAbsolute(S8_String path) { return false; }
OS_API S8_String OS_GetExePath(MA_Arena *arena) {
    S8_String s = {0};
    return s;
}
OS_API S8_String OS_GetExeDir(MA_Arena *arena) {
    S8_String s = {0};
    return s;
}
OS_API S8_String OS_GetWorkingDir(MA_Arena *arena) {
    S8_String s = {0};
    return s;
}
OS_API void OS_SetWorkingDir(S8_String path) {}
OS_API S8_String OS_GetAbsolutePath(MA_Arena *arena, S8_String relative) {
    S8_String s = {0};
    return s;
}
OS_API bool OS_FileExists(S8_String path) { return false; }
OS_API bool OS_IsDir(S8_String path) { return false; }
OS_API bool OS_IsFile(S8_String path) { return false; }
OS_API double OS_GetTime(void) { return 0.0; }
OS_API S8_List OS_ListDir(MA_Arena *arena, S8_String path, unsigned flags) {
    S8_List s = {0};
    return s;
}
OS_API OS_Result OS_MakeDir(S8_String path) { return OS_FAILURE; }
OS_API OS_Result OS_CopyFile(S8_String from, S8_String to, bool overwrite) { return OS_FAILURE; }
OS_API OS_Result OS_DeleteFile(S8_String path) { return OS_FAILURE; }
OS_API OS_Result OS_DeleteDir(S8_String path, unsigned flags) { return OS_FAILURE; }
OS_API OS_Result OS_AppendFile(S8_String path, S8_String string) { return OS_FAILURE; }
OS_API OS_Result OS_WriteFile(S8_String path, S8_String string) { return OS_FAILURE; }
OS_API S8_String OS_ReadFile(MA_Arena *arena, S8_String path) {
    S8_String s = {0};
    return s;
}
OS_API int OS_SystemF(const char *string, ...) { return 0; }
OS_API int64_t OS_GetFileModTime(S8_String file) { return 0; }
OS_API OS_Date OS_GetDate(void) {
    OS_Date s = {0};
    return s;
}
#endif

OS_API S8_String UTF_CreateStringFromWidechar(MA_Arena *arena, wchar_t *wstr, int64_t wsize) {
    int64_t buffer_size = (wsize + 1) * 2;
    char *buffer = (char *)MA_PushSizeNonZeroed(arena, buffer_size);
    int64_t size = UTF_CreateCharFromWidechar(buffer, buffer_size, wstr, wsize);
    IO_Assert(size < buffer_size);
    return S8_Make(buffer, size);
}

OS_API S8_List S8_SplitOnRegex(MA_Arena *arena, S8_String string, S8_String regex, unsigned flags) {
    S8_List result = S8_MakeEmptyList();
    int64_t index = 0;

    char buff[4096];
    RE_Regex *re = RE2_Parse(buff, sizeof(buff), regex.str, regex.len);
    for (RE_Match match = RE3_Find(re, string.str, string.len); match.pos != -1; match = RE3_Find(re, string.str, string.len)) {
        S8_String before_match = S8_Make(string.str, match.pos);
        S8_String the_match = S8_Make(string.str + match.pos, match.size);
        if (before_match.len) S8_AddNode(arena, &result, before_match);
        if (flags & S8_SPLIT_INCLUSIVE) {
            if (the_match.len) S8_AddNode(arena, &result, the_match);
        }
        string = S8_Skip(string, match.pos + match.size);
    }
    S8_AddNode(arena, &result, string);
    return result;
}

OS_API S8_List OS_ListDirRegex(MA_Arena *arena, S8_String path, unsigned flags, char *regex) {
    S8_List result = S8_MakeEmptyList();

    char buff[4096];
    RE_Regex *re = RE1_Parse(buff, sizeof(buff), regex);
    S8_List files = OS_ListDir(arena, path, flags);
    for (S8_Node *it = files.first; it; it = it->next) {
        if (RE3_AreEqual(re, it->string.str, it->string.len)) {
            S8_AddNode(arena, &result, it->string);
        }
    }
    return result;
}

OS_API S8_String OS_ListDirRegexAsString(MA_Arena *arena, S8_String path, unsigned flags, char *regex) {
    S8_List files = OS_ListDirRegex(arena, path, flags, regex);
    S8_String files_str = S8_MergeWithSeparator(arena, files, S8_Lit(" "));
    return files_str;
}

OS_API bool OS_ExpandIncludesList(MA_Arena *arena, S8_List *out, S8_String filepath) {
    S8_String c = OS_ReadFile(arena, filepath);
    if (c.str == 0) return false;
    S8_String path = S8_ChopLastSlash(filepath);
    S8_String include = S8_Lit("#include \"");
    for (;;) {
        int64_t idx = -1;
        if (S8_Find(c, include, 0, &idx)) {
            S8_String str_to_add = S8_GetPrefix(c, idx);
            S8_AddNode(arena, out, str_to_add);
            S8_String save = c;
            c = S8_Skip(c, idx + include.len);

            S8_String filename = c;
            filename.len = 0;
            while (filename.str[filename.len] != '"' && filename.len < c.len) {
                filename.len += 1;
            }

            c = S8_Skip(c, filename.len + 1);
            S8_String inc_path = S8_Format(arena, "%.*s/%.*s", S8_Expand(path), S8_Expand(filename));
            if (!OS_ExpandIncludesList(arena, out, inc_path)) {
                S8_String s = S8_GetPrefix(save, save.len - c.len);
                S8_AddNode(arena, out, s);
            }
        }
        else {
            S8_AddNode(arena, out, c);
            break;
        }
    }
    return true;
}

OS_API S8_String OS_ExpandIncludes(MA_Arena *arena, S8_String filepath) {
    S8_List out = {0};
    S8_String result = {0};
    MA_ScratchScope(s) {
        OS_ExpandIncludesList(s.arena, &out, filepath);
        result = S8_Merge(arena, out);
    }
    return result;
}
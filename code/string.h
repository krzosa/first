#pragma once
#define S8_HEADER
#include <stdint.h>
#include <stdbool.h>

#ifndef S8_API
    #ifdef __cplusplus
        #define S8_API extern "C"
    #else
        #define S8_API
    #endif
#endif

#ifndef S8_Allocator
struct MA_Arena;
    #define S8_Allocator MA_Arena *
#endif

typedef struct S8_String S8_String;
typedef struct S8_Node S8_Node;
typedef struct S8_List S8_List;

struct S8_String {
    char *str;
    int64_t len;
};

struct S8_Node {
    S8_Node *next;
    S8_String string;
};

struct S8_List {
    int64_t node_count;
    int64_t char_count;
    S8_Node *first;
    S8_Node *last;
};

enum {
    S8_NO_FLAGS = 0,
    S8_IGNORE_CASE = 1,
    S8_SPLIT_INCLUSIVE = 4,
    S8_MATCH_FIND_LAST = 32,
};

#define S8_Lit(string) S8_Make((char *)string, sizeof(string) - 1)
#define S8_ConstLit(string) \
    { string, sizeof(string) - 1 }
#define S8_Expand(string) (int)(string).len, (string).str

#define S8_FORMAT(allocator, str, result)                 \
    va_list args1;                                        \
    va_start(args1, str);                                 \
    S8_String result = S8_FormatV(allocator, str, args1); \
    va_end(args1)

#define S8_For(it, x) for (S8_Node *it = (x).first; it; it = it->next)

#if defined(__cplusplus)
S8_API bool S8_AreEqual(S8_String a, S8_String b, unsigned ignore_case);
inline bool operator==(S8_String a, S8_String b) { return S8_AreEqual(a, b, false); }
inline S8_String operator""_s(const char *str, size_t size) { return {(char *)str, (int64_t)size}; }
#endif

S8_API char CHAR_ToLowerCase(char a);
S8_API char CHAR_ToUpperCase(char a);
S8_API bool CHAR_IsWhitespace(char w);
S8_API bool CHAR_IsAlphabetic(char a);
S8_API bool CHAR_IsIdent(char a);
S8_API bool CHAR_IsDigit(char a);
S8_API bool CHAR_IsAlphanumeric(char a);
S8_API bool S8_AreEqual(S8_String a, S8_String b, unsigned ignore_case);
S8_API bool S8_EndsWith(S8_String a, S8_String end, unsigned ignore_case);
S8_API bool S8_StartsWith(S8_String a, S8_String start, unsigned ignore_case);
S8_API S8_String S8_Make(char *str, int64_t len);
S8_API S8_String S8_Copy(S8_Allocator allocator, S8_String string);
S8_API void S8_NormalizePath(S8_String s);
S8_API S8_String S8_Chop(S8_String string, int64_t len);
S8_API S8_String S8_Skip(S8_String string, int64_t len);
S8_API S8_String S8_GetPostfix(S8_String string, int64_t len);
S8_API S8_String S8_GetPrefix(S8_String string, int64_t len);
S8_API S8_String S8_Slice(S8_String string, int64_t first_index, int64_t one_past_last_index);
S8_API S8_String S8_Trim(S8_String string);
S8_API S8_String S8_TrimEnd(S8_String string);
S8_API S8_String S8_ToLowerCase(S8_Allocator allocator, S8_String s);
S8_API S8_String S8_ToUpperCase(S8_Allocator allocator, S8_String s);
S8_API bool S8_Find(S8_String string, S8_String find, unsigned flags, int64_t *index_out);
S8_API S8_List S8_Split(S8_Allocator allocator, S8_String string, S8_String find, unsigned flags);
S8_API S8_String S8_MergeWithSeparator(S8_Allocator allocator, S8_List list, S8_String separator);
S8_API S8_String S8_Merge(S8_Allocator allocator, S8_List list);
S8_API S8_String S8_ReplaceAll(S8_Allocator allocator, S8_String string, S8_String replace, S8_String with, unsigned flags);
S8_API S8_List S8_FindAll(S8_Allocator allocator, S8_String string, S8_String find, unsigned flags);
S8_API S8_String S8_ChopLastSlash(S8_String s);
S8_API S8_String S8_ChopLastPeriod(S8_String s);
S8_API S8_String S8_SkipToLastSlash(S8_String s);
S8_API S8_String S8_SkipToLastPeriod(S8_String s);
S8_API bool S8_IsPointerInside(S8_String string, char *p);
S8_API S8_String S8_SkipToP(S8_String string, char *p);
S8_API S8_String S8_SkipPast(S8_String string, S8_String a);
S8_API int64_t S8_Length(char *string);
S8_API int64_t S8_WideLength(wchar_t *string);
S8_API S8_String S8_MakeFromChar(char *string);
S8_API S8_String S8_MakeEmpty(void);
S8_API S8_List S8_MakeEmptyList(void);
S8_API S8_String S8_FormatV(S8_Allocator allocator, const char *str, va_list args1);
S8_API S8_String S8_Format(S8_Allocator allocator, const char *str, ...);
S8_API S8_Node *S8_CreateNode(S8_Allocator allocator, S8_String string);
S8_API void S8_ReplaceNodeString(S8_List *list, S8_Node *node, S8_String new_string);
S8_API void S8_AddExistingNode(S8_List *list, S8_Node *node);
S8_API void S8_AddArray(S8_Allocator allocator, S8_List *list, char **array, int count);
S8_API void S8_AddArrayWithPrefix(S8_Allocator allocator, S8_List *list, char *prefix, char **array, int count);
S8_API S8_List S8_MakeList(S8_Allocator allocator, S8_String a);
S8_API S8_List S8_CopyList(S8_Allocator allocator, S8_List a);
S8_API S8_List S8_ConcatLists(S8_Allocator allocator, S8_List a, S8_List b);
S8_API S8_Node *S8_AddNode(S8_Allocator allocator, S8_List *list, S8_String string);
S8_API S8_String S8_AddF(S8_Allocator allocator, S8_List *list, const char *str, ...);

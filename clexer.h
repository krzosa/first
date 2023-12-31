#ifndef CL_HEADER
#define CL_HEADER
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef CL_PRIVATE_FUNCTION
    #if defined(__GNUC__) || defined(__clang__)
        #define CL_PRIVATE_FUNCTION __attribute__((unused)) static
    #else
        #define CL_PRIVATE_FUNCTION static
    #endif
#endif

#ifndef CL_API_FUNCTION
    #ifdef __cplusplus
        #define CL_API_FUNCTION extern "C"
    #else
        #define CL_API_FUNCTION
    #endif
#endif

#ifndef CL_INLINE
    #ifndef _MSC_VER
        #ifdef __cplusplus
            #define CL_INLINE inline
        #else
            #define CL_INLINE
        #endif
    #else
        #define CL_INLINE __forceinline
    #endif
#endif

#ifndef CL_Arena
    #define CL_Arena CL__Arena
typedef struct CL__Arena {
    char *buff;
    int len, cap;
} CL_Arena;
CL_PRIVATE_FUNCTION void *CL_PushSize(CL_Arena *arena, int size);
#else
    #define CL_CUSTOM_ARENA_TYPE
    #ifndef CL_PushSize
        #error If you use a custom Arena type, you need to implement CL_PushSize macro
    #endif
#endif

#ifndef AND_CL_STRING_TERMINATE_ON_NEW_LINE
    #define AND_CL_STRING_TERMINATE_ON_NEW_LINE &&*T->stream != '\n'
#endif

typedef enum CL_Kind CL_Kind;
enum CL_Kind {
    CL_EOF,
    CL_MUL,
    CL_DIV,
    CL_MOD,
    CL_LEFTSHIFT,
    CL_RIGHTSHIFT,
    CL_ADD,
    CL_SUB,
    CL_EQUALS,
    CL_LESSERTHEN,
    CL_GREATERTHEN,
    CL_LESSERTHEN_OR_EQUAL,
    CL_GREATERTHEN_OR_EQUAL,
    CL_NOTEQUALS,
    CL_BITAND,
    CL_BITOR,
    CL_BITXOR,
    CL_AND,
    CL_OR,
    CL_NEG,
    CL_NOT,
    CL_DECREMENT,
    CL_INCREMENT,
    CL_POSTDECREMENT,
    CL_POSTINCREMENT,
    CL_ASSIGN,
    CL_DIVASSIGN,
    CL_MULASSIGN,
    CL_MODASSIGN,
    CL_SUBASSIGN,
    CL_ADDASSIGN,
    CL_ANDASSIGN,
    CL_ORASSIGN,
    CL_XORASSIGN,
    CL_LEFTSHIFTASSIGN,
    CL_RIGHTSHIFTASSIGN,
    CL_OPENPAREN,
    CL_CLOSEPAREN,
    CL_OPENBRACE,
    CL_CLOSEBRACE,
    CL_OPENBRACKET,
    CL_CLOSEBRACKET,
    CL_COMMA,
    CL_MACRO_CONCAT,
    CL_PREPROC_STRINGIFY,
    CL_QUESTION,
    CL_THREEDOTS,
    CL_SEMICOLON,
    CL_DOT,
    CL_COLON,
    CL_TAG,
    CL_ARROW,
    CL_EXPRSIZEOF,
    CL_DOCCOMMENT,
    CL_COMMENT,
    CL_IDENTIFIER,
    CL_STRINGLIT,
    CL_CHARLIT,
    CL_ERROR,
    CL_FLOAT,
    CL_INT,
    CL_PREPROC_NULL,
    CL_PREPROC_DEFINE,
    CL_PREPROC_IFDEF,
    CL_PREPROC_IFNDEF,
    CL_PREPROC_INCLUDE,
    CL_PREPROC_ENDIF,
    CL_PREPROC_IF,
    CL_PREPROC_PRAGMA,
    CL_PREPROC_ERROR,
    CL_PREPROC_ELSE,
    CL_PREPROC_ELIF,
    CL_PREPROC_UNDEF,
    CL_KEYWORD_VOID,
    CL_KEYWORD_INT,
    CL_KEYWORD_CHAR,
    CL_KEYWORD_UNSIGNED,
    CL_KEYWORD_SIGNED,
    CL_KEYWORD_LONG,
    CL_KEYWORD_SHORT,
    CL_KEYWORD_DOUBLE,
    CL_KEYWORD_FLOAT,
    CL_KEYWORD__BOOL,
    CL_KEYWORD__COMPLEX,
    CL_KEYWORD__IMAGINARY,
    CL_KEYWORD_STATIC,
    CL_KEYWORD_AUTO,
    CL_KEYWORD_CONST,
    CL_KEYWORD_EXTERN,
    CL_KEYWORD_INLINE,
    CL_KEYWORD_REGISTER,
    CL_KEYWORD_RESTRICT,
    CL_KEYWORD_VOLATILE,
    CL_KEYWORD__THREAD_LOCAL,
    CL_KEYWORD__ATOMIC,
    CL_KEYWORD__NORETURN,
    CL_KEYWORD_STRUCT,
    CL_KEYWORD_UNION,
    CL_KEYWORD_ENUM,
    CL_KEYWORD_TYPEDEF,
    CL_KEYWORD_DEFAULT,
    CL_KEYWORD_BREAK,
    CL_KEYWORD_RETURN,
    CL_KEYWORD_SWITCH,
    CL_KEYWORD_IF,
    CL_KEYWORD_ELSE,
    CL_KEYWORD_FOR,
    CL_KEYWORD_WHILE,
    CL_KEYWORD_CASE,
    CL_KEYWORD_CONTINUE,
    CL_KEYWORD_DO,
    CL_KEYWORD_GOTO,
    CL_KEYWORD_SIZEOF,
    CL_KEYWORD__ALIGNAS,
    CL_KEYWORD__ALIGNOF,
    CL_KEYWORD__STATIC_ASSERT,
    CL_KEYWORD__GENERIC,
    CL_COUNT,
};

typedef enum CL_Fix CL_Fix;
enum CL_Fix {
    CL_FIX_NONE,
    CL_SUFFIX_U,
    CL_SUFFIX_UL,
    CL_SUFFIX_ULL,
    CL_SUFFIX_L,
    CL_SUFFIX_LL,
    CL_SUFFIX_F,
    CL_SUFFIX_FL,
    CL_PREFIX_U8,
    CL_PREFIX_U16,
    CL_PREFIX_U32,
    CL_PREFIX_L,
};

typedef uint16_t CL_Flag;
enum {
    CL_NONE,
    CL_HEX = 1,
    CL_DIGRAPH = 2,
    CL_INSIDE_OF_MACRO = 4,
    CL_SYSTEM_INCLUDE = 8,
    CL_WHITESPACE_BEFORE_TOKEN = 16,
};

typedef struct CL_Hideset CL_Hideset;
struct CL_Hideset {
    CL_Hideset *next;
    char *name;
};

typedef struct CL_Token CL_Token; // 64 bytes
struct CL_Token {
    // 16 bytes :( we want debug info etc.
    CL_Kind kind;
    CL_Flag flags;
    CL_Fix fix;

    // 8bytes
    uint32_t id;
    int len;
    char *str; // 8bytes

    // We dont store line_begin like I would normally cause the user could
    // override the line and file information using directives.
    // On error need to do search if I want nice error context.
    int line, column;    // 8bytes
    char *file;          // 8bytes
    CL_Hideset *hideset; // 8bytes

    union { // 8bytes
        double f64;
        uint64_t u64;
        char *intern;
        char *string_literal;
        struct CL_Message *error;
        CL_Token *comment_is_attached_to_token;
    };
};

typedef enum CL_MessageKind {
    CLM_ERROR,
    CLM_WARNING,
    CLM_TRACE,
} CL_MessageKind;

typedef struct CL_Message CL_Message;
struct CL_Message {
    CL_Message *next;
    CL_MessageKind kind;
    char *string;
    CL_Token token;
};

typedef struct CL_Tokens CL_Tokens;
struct CL_Tokens {
    CL_Token *data;
    int count;
};

typedef char CL_Intern;
typedef struct CL_InternEntry CL_InternEntry;
struct CL_InternEntry {
    CL_InternEntry *next;
    char *string;
    int len;
    uint64_t hash;
};

typedef struct CL_InternTable CL_InternTable;
struct CL_InternTable {
    CL_InternEntry *entries;
    int entry_count;
    int occupied_entry_count;
    CL_Arena *arena;
};

typedef struct CL_ArenaTuple CL_ArenaTuple;
struct CL_ArenaTuple {

    // @todo: Add TokenList and TokenNode, get rid of 1 arena ?
    CL_Arena *token;
    CL_Arena *other;
    union {
        CL_Arena *include;
        CL_Arena *macro_token;
    };
    union {
        CL_Arena *comment;
        CL_Arena *scratch2;
    };

    CL_Arena default_comment;
    CL_Arena default_token;
    CL_Arena default_include;
    CL_Arena default_other;
};

typedef struct CL_LexResult CL_LexResult;
struct CL_LexResult {
    CL_LexResult *next_result;

    CL_Tokens tokens;
    CL_Tokens includes;
    CL_Tokens comments;
    int attached_comment_index;

    CL_Message *first_message;
    CL_Message *last_message;
    int errors;

    char *stream;
    char *stream_begin;
    int line;
    int column;
    char *file;
    bool inside_of_macro;

    CL_ArenaTuple *arena;
};

typedef struct CL_SearchPaths CL_SearchPaths;
struct CL_SearchPaths {
    char **include_path;
    int include_path_count;

    char **system_include_path;
    int system_include_path_count;

    char *file_begin_to_ignore;
};

typedef struct CL_LexList CL_LexList;
struct CL_LexList {
    int count;
    CL_LexResult *first_result;
    CL_LexResult *last_result;
    CL_InternTable *intern_table;
    CL_SearchPaths search_paths;
};

typedef struct CL_IncludeIter CL_IncludeIter;
struct CL_IncludeIter {
    char *filename;
    bool is_system_include;
    bool inited_with_filename;

    CL_Token *include_token;

    int include_index;
    CL_LexResult *parent;
    CL_LexList *lex_list;

    CL_Arena *arena;
    CL_SearchPaths search_paths;
    bool resolve;
};

//
// Main API
//
CL_API_FUNCTION void CL_InitDefaultTuple(CL_ArenaTuple *tuple);
CL_API_FUNCTION CL_LexResult *CL_LexString(CL_ArenaTuple *arena, char *filename, char *string);
CL_API_FUNCTION CL_LexResult *CL_LexFile(CL_ArenaTuple *arena, char *filename);
CL_API_FUNCTION CL_LexList CL_LexRecursive(CL_ArenaTuple *arena, char *filename, CL_SearchPaths paths);

//
// Intern table
//
CL_API_FUNCTION void CL_InitInternTable(CL_Arena *arena, CL_InternTable *table, int size);
CL_API_FUNCTION CL_InternTable *CL_CreateInternTable(CL_Arena *arena, int size);
CL_API_FUNCTION CL_Intern *CL_InsertIntern(CL_InternTable *table, char *string, int len);
CL_API_FUNCTION void CL_InternResult(CL_InternTable *table, CL_LexResult *result);

//
// Include iteration and path resolution
//
CL_API_FUNCTION CL_IncludeIter CL_IterateIncludes(CL_LexList *list);
CL_API_FUNCTION CL_IncludeIter CL_IterateResolvedIncludes(CL_Arena *arena, CL_LexList *list, CL_SearchPaths search_paths);
CL_API_FUNCTION char *CL_ResolveFilepath(CL_Arena *arena, CL_SearchPaths *search_paths, char *filename, char *parent_file, bool is_system_include);
CL_API_FUNCTION bool CL_IsValidFile(CL_LexList *list, char *filename);
CL_API_FUNCTION void CL_GetNextInclude(CL_IncludeIter *iter);

// Token serialization
CL_API_FUNCTION void CL_StringifyMessage(char *buff, int buff_size, CL_Message *msg);
CL_API_FUNCTION void CL_PrintMessages(CL_LexResult *lex_result);
CL_API_FUNCTION void CL_Stringify(char *buff, int buff_size, CL_Token *token);
CL_API_FUNCTION void CL_PrintTokens(CL_Tokens tokens);
//
// Extended API for "manual" lexing with extended help
//
CL_API_FUNCTION void CL_ReportError(CL_LexResult *T, CL_Token *token, const char *string, ...);
CL_API_FUNCTION bool CL_EatWhitespace(CL_LexResult *T);
CL_API_FUNCTION void CL_SetTokenLength(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION void CL_TryToFinalizeToken(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION void CL_ParseCharLiteral(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION void CL_ParseString(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION void CL_IsIdentifierKeyword(CL_LexResult *ctx, CL_Token *token);
CL_API_FUNCTION void CL_LexMacroInclude(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION bool CL_LexMacro(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION CL_LexResult *CL_CreateLexingResult(CL_ArenaTuple *arena, char *filename, char *filecontent);
CL_API_FUNCTION void CL_PrepareToken(CL_LexResult *T, CL_Token *token, bool skipped_whitespace);
CL_API_FUNCTION void CL_DefaultTokenize(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION bool CL_IsComment(CL_Kind kind);
CL_API_FUNCTION void CL_InitNextToken(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION CL_Hideset *CL_CreateHideset(CL_Arena *arena, char *name);
CL_API_FUNCTION CL_Token *CL_AddNextToken(CL_LexResult *T);
CL_API_FUNCTION void CL_AddToken(CL_LexResult *T, CL_Token *token);
CL_API_FUNCTION CL_LexList CL_MakeLexList(CL_LexResult *l);
CL_API_FUNCTION CL_IncludeIter CL_IterateFileAndResolvedIncludes(CL_ArenaTuple *arena, char *filename, CL_SearchPaths search_paths);

//
// Token iteration and utilities
//
CL_INLINE int CL_StringLength(char *string) {
    int len = 0;
    while (*string++ != 0) len++;
    return len;
}

CL_INLINE bool CL_StringsAreEqual(char *a, int64_t alen, char *b, int64_t blen) {
    if (alen != blen) return false;
    for (int i = 0; i < alen; i += 1) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

CL_INLINE bool CL_IsIdentifier(CL_Token *token, char *str) {
    int str_len = CL_StringLength(str);
    bool result = token->kind == CL_IDENTIFIER && CL_StringsAreEqual(token->str, token->len, str, str_len);
    return result;
}

CL_INLINE bool CL_IsAssign(CL_Kind op) {
    bool result = op >= CL_ASSIGN && op <= CL_RIGHTSHIFTASSIGN;
    return result;
}

CL_INLINE bool CL_IsKeywordType(CL_Kind op) {
    bool result = op >= CL_KEYWORD_VOID && op <= CL_KEYWORD__IMAGINARY;
    return result;
}

CL_INLINE bool CL_IsKeywordTypeOrSpec(CL_Kind op) {
    bool result = op >= CL_KEYWORD_VOID && op <= CL_KEYWORD_TYPEDEF;
    return result;
}

CL_INLINE bool CL_IsMacro(CL_Kind kind) {
    /*print(f"bool result = kind >= CL_PREPROC_{meta.preproc_keywords[0].upper()} && kind <= CL_PREPROC_{meta.preproc_keywords[-1].upper()};")*/
    bool result = kind >= CL_PREPROC_DEFINE && kind <= CL_PREPROC_UNDEF;
    /*END*/
    return result;
}

CL_INLINE bool CL_IsKeyword(CL_Kind kind) {
    /*#print(f"bool result = kind >= CL_KEYWORD_{meta.keywords[0].upper()} && kind <= CL_KEYWORD_{meta.keywords[-1].upper()};")*/
    bool result = kind >= CL_KEYWORD_VOID && kind <= CL_KEYWORD__GENERIC;
    /*END*/
    return result;
}

CL_INLINE bool CL_IsKeywordOrIdent(CL_Kind kind) {
    bool result = CL_IsKeyword(kind) || kind == CL_IDENTIFIER;
    return result;
}

CL_Token CL_NullToken;
CL_INLINE CL_Token *CL_Next(CL_Tokens *tokens) {
    if (tokens->count > 0) {
        CL_Token *result = tokens->data;
        tokens->data += 1;
        tokens->count -= 1;
        return result;
    }
    return &CL_NullToken;
}

CL_INLINE CL_Token *CL_Get(CL_Tokens *tokens) {
    if (tokens->count > 0) {
        return tokens->data;
    }
    return &CL_NullToken;
}

CL_INLINE CL_Token *CL_Match(CL_Tokens *tokens, CL_Kind kind) {
    CL_Token *result = CL_Get(tokens);
    if (result->kind == kind) {
        CL_Token *next = CL_Next(tokens);
        return next;
    }
    return 0;
}

CL_INLINE CL_Token *CL_MatchIdentifier(CL_Tokens *tokens, char *str) {
    CL_Token *result = CL_Get(tokens);
    if (CL_IsIdentifier(result, str)) {
        CL_Token *next = CL_Next(tokens);
        return next;
    }
    return 0;
}

#endif // CL_HEADER

#ifdef CL_IMPLEMENTATION

#include <stdarg.h>

#ifndef CL_STRING_TO_DOUBLE
    #include <stdlib.h>
    #define CL_STRING_TO_DOUBLE(str, len) strtod(str, 0)
#endif

#ifndef CL_ASSERT
    #include <assert.h>
    #define CL_ASSERT(x) assert(x)
#endif

#ifndef CL_VSNPRINTF
    #include <stdio.h>
    #define CL_VSNPRINTF vsnprintf
#endif

#ifndef CL_SNPRINTF
    #include <stdio.h>
    #define CL_SNPRINTF snprintf
#endif

#ifndef CL_ReadFile
    #define CL_ReadFile CL__ReadFile
    #include <stdio.h>
CL_PRIVATE_FUNCTION char *CL_ReadFile(CL_Arena *arena, char *name) {
    char *result = 0;
    FILE *f = fopen(name, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        int len = ftell(f);
        fseek(f, 0, SEEK_SET);

        result = (char *)CL_PushSize(arena, len + 1);
        fread(result, len, 1, f);
        fclose(f);
        result[len] = 0;
    }

    return result;
}
#endif

#ifndef CL_FileExists
    #define CL_FileExists CL__FileExists
    #include <stdio.h>
CL_API_FUNCTION bool CL_FileExists(char *name) {
    bool result = false;
    FILE *f = fopen(name, "rb");
    if (f) {
        result = true;
        fclose(f);
    }
    return result;
}
#endif

#ifndef CL__HASH_BYTES
    #define CL__HASH_BYTES CL__HashBytes
// FNV HASH (1a?)
static uint64_t CL__HashBytes(void *p, int bytes) {
    uint8_t *p8 = (uint8_t *)p;
    uint64_t hash = (uint64_t)14695981039346656037ULL;
    for (int i = 0; i < bytes; i++) {
        hash = hash ^ (uint64_t)(p8[i]);
        hash = hash * (uint64_t)1099511628211ULL;
    }
    return hash;
}
#endif

#ifndef CL_CUSTOM_ARENA_TYPE
CL_PRIVATE_FUNCTION void *CL_PushSize(CL_Arena *arena, int size) {
    if (arena->len + size > arena->cap) {
        CL_ASSERT(!"CLEX: Not enough memory");
    }
    void *result = arena->buff + arena->len;
    arena->len += size;
    return result;
}
#endif

#ifdef __cplusplus
    #define CL_ZeroStruct() \
        {}
#else
    #define CL_ZeroStruct() \
        { 0 }
#endif

CL_PRIVATE_FUNCTION void CL__MemoryZero(void *p, size_t size) {
    uint8_t *p8 = (uint8_t *)p;
    while (size--) *p8++ = 0;
}

CL_PRIVATE_FUNCTION void CL__MemoryCopy(void *dst, const void *src, size_t size) {
    char *src8 = (char *)src;
    char *dst8 = (char *)dst;
    while (size--) *dst8++ = *src8++;
}

#define CL_PushArray(arena, T, size) (T *)CL__PushSizeZeroed(arena, sizeof(T) * (size))
#define CL_PushStruct(arena, T) CL_PushArray(arena, T, 1)
CL_PRIVATE_FUNCTION void *CL__PushSizeZeroed(CL_Arena *arena, int size) {
    void *result = CL_PushSize(arena, size);
    CL__MemoryZero(result, size);
    return result;
}

/*#
print("\nchar *CL_FixString[] = {")
for i in meta.fix: print(f"""  "{i if i != "FIX_NONE" else ""}", """)
print("};")
meta.gen_enum(meta.tokens, table_name="CL_KindString", table=True)
meta.gen_enum(meta.message_kinds, table_name="CL_MessageKindString", table=True)
*/

char *CL_FixString[] = {
    "",
    "SUFFIX_U",
    "SUFFIX_UL",
    "SUFFIX_ULL",
    "SUFFIX_L",
    "SUFFIX_LL",
    "SUFFIX_F",
    "SUFFIX_FL",
    "PREFIX_U8",
    "PREFIX_U16",
    "PREFIX_U32",
    "PREFIX_L",
};

char *CL_KindString[] = {
    "EOF",
    "*",
    "/",
    "%",
    "<<",
    ">>",
    "+",
    "-",
    "==",
    "<",
    ">",
    "<=",
    ">=",
    "!=",
    "&",
    "|",
    "^",
    "&&",
    "||",
    "~",
    "!",
    "--",
    "++",
    "--",
    "++",
    "=",
    "/=",
    "*=",
    "%=",
    "-=",
    "+=",
    "&=",
    "|=",
    "^=",
    "<<=",
    ">>=",
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",
    ",",
    "##",
    "#Stringify",
    "?",
    "...",
    ";",
    ".",
    ":",
    "TAG",
    "->",
    "SIZEOF",
    "DOCCOMMENT",
    "COMMENT",
    "IDENTIFIER",
    "STRING_LITERAL",
    "CHARACTER_LITERAL",
    "ERROR TOKEN",
    "FLOAT",
    "INT",
    "PREPROC_NULL",
    "PREPROC_DEFINE",
    "PREPROC_IFDEF",
    "PREPROC_IFNDEF",
    "PREPROC_INCLUDE",
    "PREPROC_ENDIF",
    "PREPROC_IF",
    "PREPROC_PRAGMA",
    "PREPROC_ERROR",
    "PREPROC_ELSE",
    "PREPROC_ELIF",
    "PREPROC_UNDEF",
    "KEYWORD_VOID",
    "KEYWORD_INT",
    "KEYWORD_CHAR",
    "KEYWORD_UNSIGNED",
    "KEYWORD_SIGNED",
    "KEYWORD_LONG",
    "KEYWORD_SHORT",
    "KEYWORD_DOUBLE",
    "KEYWORD_FLOAT",
    "KEYWORD__BOOL",
    "KEYWORD__COMPLEX",
    "KEYWORD__IMAGINARY",
    "KEYWORD_STATIC",
    "KEYWORD_AUTO",
    "KEYWORD_CONST",
    "KEYWORD_EXTERN",
    "KEYWORD_INLINE",
    "KEYWORD_REGISTER",
    "KEYWORD_RESTRICT",
    "KEYWORD_VOLATILE",
    "KEYWORD__THREAD_LOCAL",
    "KEYWORD__ATOMIC",
    "KEYWORD__NORETURN",
    "KEYWORD_STRUCT",
    "KEYWORD_UNION",
    "KEYWORD_ENUM",
    "KEYWORD_TYPEDEF",
    "KEYWORD_DEFAULT",
    "KEYWORD_BREAK",
    "KEYWORD_RETURN",
    "KEYWORD_SWITCH",
    "KEYWORD_IF",
    "KEYWORD_ELSE",
    "KEYWORD_FOR",
    "KEYWORD_WHILE",
    "KEYWORD_CASE",
    "KEYWORD_CONTINUE",
    "KEYWORD_DO",
    "KEYWORD_GOTO",
    "KEYWORD_SIZEOF",
    "KEYWORD__ALIGNAS",
    "KEYWORD__ALIGNOF",
    "KEYWORD__STATIC_ASSERT",
    "KEYWORD__GENERIC",
};

char *CL_MessageKindString[] = {
    "ERROR",
    "WARNING",
    "TRACE",
};
/*END*/

#define CL_DLL_QUEUE_ADD_MOD(f, l, node, next, prev) \
    do {                                             \
        if ((f) == 0) {                              \
            (f) = (l) = (node);                      \
            (node)->prev = 0;                        \
            (node)->next = 0;                        \
        }                                            \
        else {                                       \
            (l)->next = (node);                      \
            (node)->prev = (l);                      \
            (node)->next = 0;                        \
            (l) = (node);                            \
        }                                            \
    } while (0)
#define CL_DLL_QUEUE_ADD(f, l, node) CL_DLL_QUEUE_ADD_MOD(f, l, node, next, prev)

#define CL_SLL_QUEUE_ADD_MOD(f, l, n, next) \
    do {                                    \
        (n)->next = 0;                      \
        if ((f) == 0) {                     \
            (f) = (l) = (n);                \
        }                                   \
        else {                              \
            (l) = (l)->next = (n);          \
        }                                   \
    } while (0)
#define CL_SLL_QUEUE_ADD(f, l, n) CL_SLL_QUEUE_ADD_MOD(f, l, n, next)

#define CL__FORMAT(arena, string, result)                 \
    va_list args1, args2;                                 \
    va_start(args1, string);                              \
    va_copy(args2, args1);                                \
    int len = CL_VSNPRINTF(0, 0, string, args2);          \
    va_end(args2);                                        \
    char *result = (char *)CL_PushSize((arena), len + 1); \
    CL_VSNPRINTF(result, len + 1, string, args1);         \
    va_end(args1)

CL_API_FUNCTION void CL_ReportError(CL_LexResult *T, CL_Token *token, const char *string, ...) {
    CL__FORMAT(T->arena->other, string, message_string);
    CL_Message *result = CL_PushStruct(T->arena->other, CL_Message);
    result->kind = CLM_ERROR;
    result->string = (char *)string;
    CL_SLL_QUEUE_ADD(T->first_message, T->last_message, result);
    result->token = *token;
    T->errors += 1;
    token->kind = CL_ERROR;
    token->error = result;
#if TEST_DEBUG
    printf("%s:%d %s\n", token->file, token->line, string);
    __debugbreak();
#endif
}

CL_PRIVATE_FUNCTION char *CL_PushStringCopy(CL_Arena *arena, char *p, int size) {
    char *copy_buffer = (char *)CL_PushSize(arena, size + 1);
    CL__MemoryCopy(copy_buffer, p, size);
    copy_buffer[size] = 0;
    return copy_buffer;
}

CL_PRIVATE_FUNCTION CL_Token *CL_CopyToken(CL_Arena *arena, CL_Token *token) {
    CL_Token *copy_buffer = (CL_Token *)CL_PushSize(arena, sizeof(CL_Token));
    CL__MemoryCopy(copy_buffer, token, sizeof(CL_Token));
    return copy_buffer;
}

CL_API_FUNCTION void CL_StringifyMessage(char *buff, int buff_size, CL_Message *msg) {
    char *kind = CL_MessageKindString[msg->kind];
    CL_SNPRINTF(buff, buff_size, "%s:%d %15s %15s", msg->token.file, msg->token.line, kind, msg->string);
}

CL_API_FUNCTION void CL_Stringify(char *buff, int buff_size, CL_Token *token) {
    char *token_kind = "UNKNOWN";
    if (token->kind < CL_COUNT) token_kind = CL_KindString[token->kind];
    CL_SNPRINTF(buff, buff_size, "%s:%d %15s %15.*s", token->file, token->line, token_kind, token->len, token->str);
}

CL_API_FUNCTION void CL_PrintMessages(CL_LexResult *lex_result) {
    char buff[1024];
    for (CL_Message *it = lex_result->first_message; it; it = it->next) {
        CL_StringifyMessage(buff, sizeof(buff), it);
        printf("%s\n", buff);
    }
}

CL_API_FUNCTION void CL_PrintTokens(CL_Tokens tokens) {
    char buff[1024];
    for (int i = 0; i < tokens.count; i += 1) {
        CL_Stringify(buff, sizeof(buff), &tokens.data[i]);
        printf("%s\n", buff);
    }
}

CL_INLINE void CL_Advance(CL_LexResult *T) {
    if (*T->stream == '\n') {
        T->line += 1;
        T->column = 0;
    }
    else if (*T->stream == ' ') {
        T->column += 1;
    }
    else if (*T->stream == '\t') {
        T->column += 1;
    }
    else if (*T->stream == 0) {
        return;
    }
    T->stream += 1;
}

CL_INLINE bool CL_IsAlphabetic(char c) {
    bool result = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    return result;
}

CL_INLINE bool CL_IsNumeric(char c) {
    bool result = (c >= '0' && c <= '9');
    return result;
}

CL_INLINE bool CL_IsHexNumeric(char c) {
    bool result = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    return result;
}

CL_INLINE bool CL_IsWhitespace(char c) {
    bool result = c == ' ' || c == '\n' || c == '\r' || c == '\t';
    return result;
}

CL_INLINE bool CL_IsAlphanumeric(char c) {
    bool result = CL_IsAlphabetic(c) || CL_IsNumeric(c);
    return result;
}

CL_API_FUNCTION bool CL_EatWhitespace(CL_LexResult *T) {
    bool skipped = false;
    for (;;) {
        if (CL_IsWhitespace(*T->stream)) {
            if (*T->stream == '\n') T->inside_of_macro = false;
            CL_Advance(T);
            skipped = true;
        }
        else if (T->stream[0] == '\\' && T->stream[1] == '\n') {
            CL_Advance(T);
            CL_Advance(T);
            skipped = true;
        }
        else if (T->stream[0] == '\\' && T->stream[1] == '\r' && T->stream[2] == '\n') {
            CL_Advance(T);
            CL_Advance(T);
            CL_Advance(T);
            skipped = true;
        }
        else {
            break;
        }
    }
    return skipped;
}

CL_API_FUNCTION void CL_SetTokenLength(CL_LexResult *T, CL_Token *token) {
    intptr_t diff = T->stream - token->str;
    CL_ASSERT(diff < 2147483647);
    token->len = (int)diff;
}

CL_API_FUNCTION void CL_TryToFinalizeToken(CL_LexResult *T, CL_Token *token) {
    for (; T->attached_comment_index < T->comments.count; T->attached_comment_index += 1) {
        CL_Token *it = T->comments.data + T->attached_comment_index;
        it->comment_is_attached_to_token = token;
    }
    if (!token->len) {
        CL_SetTokenLength(T, token);
    }
    if (T->inside_of_macro) {
        token->flags |= CL_INSIDE_OF_MACRO;
    }
}

CL_PRIVATE_FUNCTION uint64_t CL_CharMapToNumber(char c) {
    switch (c) {
        case '0': return 0; break;
        case '1': return 1; break;
        case '2': return 2; break;
        case '3': return 3; break;
        case '4': return 4; break;
        case '5': return 5; break;
        case '6': return 6; break;
        case '7': return 7; break;
        case '8': return 8; break;
        case '9': return 9; break;
        case 'a':
        case 'A': return 10; break;
        case 'b':
        case 'B': return 11; break;
        case 'c':
        case 'C': return 12; break;
        case 'd':
        case 'D': return 13; break;
        case 'e':
        case 'E': return 14; break;
        case 'f':
        case 'F': return 15; break;
        default: return 255;
    }
}

CL_PRIVATE_FUNCTION uint64_t CL_ParseInteger(CL_LexResult *T, CL_Token *token, char *string, uint64_t len, uint64_t base) {
    CL_ASSERT(base >= 2 && base <= 16);
    uint64_t acc = 0;
    for (uint64_t i = 0; i < len; i++) {
        uint64_t num = CL_CharMapToNumber(string[i]);
        if (num >= base) {
            CL_ReportError(T, token, "Internal compiler error! Failed to parse a number");
            break;
        }
        acc *= base;
        acc += num;
    }
    return acc;
}

typedef struct CL_UTF32Result {
    uint32_t out_str;
    int advance;
    int error;
} CL_UTF32Result;

CL_PRIVATE_FUNCTION CL_UTF32Result CL_UTF8ToUTF32(char *c, int max_advance) {
    CL_UTF32Result result = CL_ZeroStruct();

    if ((c[0] & 0x80) == 0) { // Check if leftmost zero of first byte is unset
        if (max_advance >= 1) {
            result.out_str = c[0];
            result.advance = 1;
        }
        else result.error = 1;
    }

    else if ((c[0] & 0xe0) == 0xc0) {
        if ((c[1] & 0xc0) == 0x80) { // Continuation byte required
            if (max_advance >= 2) {
                result.out_str = (uint32_t)(c[0] & 0x1f) << 6u | (c[1] & 0x3f);
                result.advance = 2;
            }
            else result.error = 2;
        }
        else result.error = 2;
    }

    else if ((c[0] & 0xf0) == 0xe0) {
        if ((c[1] & 0xc0) == 0x80 && (c[2] & 0xc0) == 0x80) { // Two continuation bytes required
            if (max_advance >= 3) {
                result.out_str = (uint32_t)(c[0] & 0xf) << 12u | (uint32_t)(c[1] & 0x3f) << 6u | (c[2] & 0x3f);
                result.advance = 3;
            }
            else result.error = 3;
        }
        else result.error = 3;
    }

    else if ((c[0] & 0xf8) == 0xf0) {
        if ((c[1] & 0xc0) == 0x80 && (c[2] & 0xc0) == 0x80 && (c[3] & 0xc0) == 0x80) { // Three continuation bytes required
            if (max_advance >= 4) {
                result.out_str = (uint32_t)(c[0] & 0xf) << 18u | (uint32_t)(c[1] & 0x3f) << 12u | (uint32_t)(c[2] & 0x3f) << 6u | (uint32_t)(c[3] & 0x3f);
                result.advance = 4;
            }
            else result.error = 4;
        }
        else result.error = 4;
    }
    else result.error = 4;

    return result;
}

// @todo I think I should look at this again
CL_API_FUNCTION void CL_ParseCharLiteral(CL_LexResult *T, CL_Token *token) {
    token->kind = CL_CHARLIT;
    token->str = T->stream;
    while (*T->stream != '\'') {
        if (*T->stream == '\\') {
            CL_Advance(T);
        }
        if (*T->stream == 0) {
            CL_ReportError(T, token, "Unclosed character literal!");
            return;
        }
        CL_Advance(T);
    }
    CL_SetTokenLength(T, token);

    if (token->str[0] == '\\') {
        switch (token->str[1]) {
            case '\\': token->u64 = '\\'; break;
            case '\'': token->u64 = '\''; break;
            case '"': token->u64 = '"'; break;
            case 't': token->u64 = '\t'; break;
            case 'v': token->u64 = '\v'; break;
            case 'f': token->u64 = '\f'; break;
            case 'n': token->u64 = '\n'; break;
            case 'r': token->u64 = '\r'; break;
            case 'a': token->u64 = '\a'; break;
            case 'b': token->u64 = '\b'; break;
            case '0':
                token->u64 = '\0';
                break;
                // Octal constant
            case 'x':
            case 'X': CL_ASSERT(0); break; // Hex constant
            case 'u': CL_ASSERT(0); break; // Unicode constant
            default: {
                CL_ReportError(T, token, "Unknown escape code");
            }
        }
    }

    else {
        if (token->len > 4) {
            CL_ReportError(T, token, "This character literal has invalid format, it's too big");
            goto skip_utf_encode;
        }

        token->u64 = 0;
        int i = 0;

        for (; i < token->len;) {
            CL_UTF32Result result = CL_UTF8ToUTF32(token->str + i, (int)token->len);
            i += result.advance;
            token->u64 |= result.out_str << (8 * (token->len - i));
            if (result.error) {
                CL_ReportError(T, token, "This character literal couldnt be parsed as utf8");
                break;
            }
        }
        if (i != token->len) {
            CL_ReportError(T, token, "Character literal decode error");
        }
    }

skip_utf_encode:
    CL_Advance(T);
}

CL_PRIVATE_FUNCTION void CL_BufferWrite(char *buffer, int buffer_size, int *buffer_iter, char write) {
    if (*buffer_iter < buffer_size) {
        buffer[*buffer_iter] = write;
        *buffer_iter += 1;
    }
}

// @todo I think I should look at this again
// Idea: Maybe try to figure out size first and then write the string
CL_API_FUNCTION void CL_ParseString(CL_LexResult *T, CL_Token *token) {
    // @todo String builder here, we dont really want 4096 character limit
    int buffer_iter = 0;
    int buffer_size = 4096;
    char buffer[4096];

    token->kind = CL_STRINGLIT;
    // First we try to parse the string normally, we write contents to scratch memory.
    // Afterwards we try to seek if there are more consecutive strings. As the speak
    // says, those are one string, so we combine them nicely. Then after we have written
    // everything to the scratch buffer. We make a proper tight copy on the pernament
    // allocator.
combine_next_string_literal:
    while (*T->stream != '"' && *T->stream != 0 AND_CL_STRING_TERMINATE_ON_NEW_LINE) {
        if (*T->stream == '\\') {
            CL_Advance(T);
            switch (*T->stream) {
                case '\\': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\\'); break;
                case '\'': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\''); break;
                case '"': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '"'); break;
                case 't': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\t'); break;
                case 'f': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\f'); break;
                case 'n': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\n'); break;
                case 'v': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\v'); break;
                case 'r': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\r'); break;
                case 'a': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\a'); break;
                case 'b': CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\b'); break;
                case '0':
                    CL_BufferWrite(buffer, buffer_size, &buffer_iter, '\0');
                    break;

                    // Octal constant
                case 'x':
                case 'X': CL_ASSERT(0); break; // Hex constant
                case 'u': CL_ASSERT(0); break; // Unicode constant
            }
        }
        else {
            CL_BufferWrite(buffer, buffer_size, &buffer_iter, *T->stream);
        }

        CL_Advance(T);
    }
    CL_Advance(T);

    // Try to seek if there is a consecutive string.
    // If there is such string we try to combine it.
    char *seek_for_next_string = T->stream;
    while (CL_IsWhitespace(*seek_for_next_string)) {
        seek_for_next_string += 1;
    }

    if (*seek_for_next_string == '"') {
        seek_for_next_string += 1;
        while (T->stream != seek_for_next_string) CL_Advance(T);
        goto combine_next_string_literal;
    }

    int len = buffer_iter + 1;
    if (len > buffer_size) {
        len = buffer_size;
        CL_ReportError(T, token, "Truncated string! Reached 4096 character limit for string literal.");
    }

    token->string_literal = CL_PushStringCopy(T->arena->other, buffer, len);
}

CL_API_FUNCTION void CL_IsIdentifierKeyword(CL_LexResult *ctx, CL_Token *token) {
    if (token->len == 1) return;
    char *c = token->str;
    /*import meta
meta.gen_lex_keywords()*/
    switch (c[0]) {
        case 'v': {
            switch (c[1]) {
                case 'o': {
                    if (CL_StringsAreEqual(token->str, token->len, "void", 4)) {
                        token->kind = CL_KEYWORD_VOID;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "volatile", 8)) {
                        token->kind = CL_KEYWORD_VOLATILE;
                    }
                } break;
            }
        } break;
        case 'i': {
            switch (c[1]) {
                case 'n': {
                    if (CL_StringsAreEqual(token->str, token->len, "int", 3)) {
                        token->kind = CL_KEYWORD_INT;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "inline", 6)) {
                        token->kind = CL_KEYWORD_INLINE;
                    }
                } break;
                case 'f': {
                    if (CL_StringsAreEqual(token->str, token->len, "if", 2)) {
                        token->kind = CL_KEYWORD_IF;
                    }
                } break;
            }
        } break;
        case 'c': {
            switch (c[1]) {
                case 'h': {
                    if (CL_StringsAreEqual(token->str, token->len, "char", 4)) {
                        token->kind = CL_KEYWORD_CHAR;
                    }
                } break;
                case 'o': {
                    if (CL_StringsAreEqual(token->str, token->len, "const", 5)) {
                        token->kind = CL_KEYWORD_CONST;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "continue", 8)) {
                        token->kind = CL_KEYWORD_CONTINUE;
                    }
                } break;
                case 'a': {
                    if (CL_StringsAreEqual(token->str, token->len, "case", 4)) {
                        token->kind = CL_KEYWORD_CASE;
                    }
                } break;
            }
        } break;
        case 'u': {
            switch (c[1]) {
                case 'n': {
                    if (CL_StringsAreEqual(token->str, token->len, "unsigned", 8)) {
                        token->kind = CL_KEYWORD_UNSIGNED;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "union", 5)) {
                        token->kind = CL_KEYWORD_UNION;
                    }
                } break;
            }
        } break;
        case 's': {
            switch (c[1]) {
                case 'i': {
                    if (CL_StringsAreEqual(token->str, token->len, "signed", 6)) {
                        token->kind = CL_KEYWORD_SIGNED;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "sizeof", 6)) {
                        token->kind = CL_KEYWORD_SIZEOF;
                    }
                } break;
                case 'h': {
                    if (CL_StringsAreEqual(token->str, token->len, "short", 5)) {
                        token->kind = CL_KEYWORD_SHORT;
                    }
                } break;
                case 't': {
                    if (CL_StringsAreEqual(token->str, token->len, "static", 6)) {
                        token->kind = CL_KEYWORD_STATIC;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "struct", 6)) {
                        token->kind = CL_KEYWORD_STRUCT;
                    }
                } break;
                case 'w': {
                    if (CL_StringsAreEqual(token->str, token->len, "switch", 6)) {
                        token->kind = CL_KEYWORD_SWITCH;
                    }
                } break;
            }
        } break;
        case 'l': {
            switch (c[1]) {
                case 'o': {
                    if (CL_StringsAreEqual(token->str, token->len, "long", 4)) {
                        token->kind = CL_KEYWORD_LONG;
                    }
                } break;
            }
        } break;
        case 'd': {
            switch (c[1]) {
                case 'o': {
                    if (CL_StringsAreEqual(token->str, token->len, "double", 6)) {
                        token->kind = CL_KEYWORD_DOUBLE;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "do", 2)) {
                        token->kind = CL_KEYWORD_DO;
                    }
                } break;
                case 'e': {
                    if (CL_StringsAreEqual(token->str, token->len, "default", 7)) {
                        token->kind = CL_KEYWORD_DEFAULT;
                    }
                } break;
            }
        } break;
        case 'f': {
            switch (c[1]) {
                case 'l': {
                    if (CL_StringsAreEqual(token->str, token->len, "float", 5)) {
                        token->kind = CL_KEYWORD_FLOAT;
                    }
                } break;
                case 'o': {
                    if (CL_StringsAreEqual(token->str, token->len, "for", 3)) {
                        token->kind = CL_KEYWORD_FOR;
                    }
                } break;
            }
        } break;
        case '_': {
            switch (c[1]) {
                case 'B': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Bool", 5)) {
                        token->kind = CL_KEYWORD__BOOL;
                    }
                } break;
                case 'C': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Complex", 8)) {
                        token->kind = CL_KEYWORD__COMPLEX;
                    }
                } break;
                case 'I': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Imaginary", 10)) {
                        token->kind = CL_KEYWORD__IMAGINARY;
                    }
                } break;
                case 'T': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Thread_local", 13)) {
                        token->kind = CL_KEYWORD__THREAD_LOCAL;
                    }
                } break;
                case 'A': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Atomic", 7)) {
                        token->kind = CL_KEYWORD__ATOMIC;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "_Alignas", 8)) {
                        token->kind = CL_KEYWORD__ALIGNAS;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "_Alignof", 8)) {
                        token->kind = CL_KEYWORD__ALIGNOF;
                    }
                } break;
                case 'N': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Noreturn", 9)) {
                        token->kind = CL_KEYWORD__NORETURN;
                    }
                } break;
                case 'S': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Static_assert", 14)) {
                        token->kind = CL_KEYWORD__STATIC_ASSERT;
                    }
                } break;
                case 'G': {
                    if (CL_StringsAreEqual(token->str, token->len, "_Generic", 8)) {
                        token->kind = CL_KEYWORD__GENERIC;
                    }
                } break;
            }
        } break;
        case 'a': {
            switch (c[1]) {
                case 'u': {
                    if (CL_StringsAreEqual(token->str, token->len, "auto", 4)) {
                        token->kind = CL_KEYWORD_AUTO;
                    }
                } break;
            }
        } break;
        case 'e': {
            switch (c[1]) {
                case 'x': {
                    if (CL_StringsAreEqual(token->str, token->len, "extern", 6)) {
                        token->kind = CL_KEYWORD_EXTERN;
                    }
                } break;
                case 'n': {
                    if (CL_StringsAreEqual(token->str, token->len, "enum", 4)) {
                        token->kind = CL_KEYWORD_ENUM;
                    }
                } break;
                case 'l': {
                    if (CL_StringsAreEqual(token->str, token->len, "else", 4)) {
                        token->kind = CL_KEYWORD_ELSE;
                    }
                } break;
            }
        } break;
        case 'r': {
            switch (c[1]) {
                case 'e': {
                    if (CL_StringsAreEqual(token->str, token->len, "register", 8)) {
                        token->kind = CL_KEYWORD_REGISTER;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "restrict", 8)) {
                        token->kind = CL_KEYWORD_RESTRICT;
                    }
                    else if (CL_StringsAreEqual(token->str, token->len, "return", 6)) {
                        token->kind = CL_KEYWORD_RETURN;
                    }
                } break;
            }
        } break;
        case 't': {
            switch (c[1]) {
                case 'y': {
                    if (CL_StringsAreEqual(token->str, token->len, "typedef", 7)) {
                        token->kind = CL_KEYWORD_TYPEDEF;
                    }
                } break;
            }
        } break;
        case 'b': {
            switch (c[1]) {
                case 'r': {
                    if (CL_StringsAreEqual(token->str, token->len, "break", 5)) {
                        token->kind = CL_KEYWORD_BREAK;
                    }
                } break;
            }
        } break;
        case 'w': {
            switch (c[1]) {
                case 'h': {
                    if (CL_StringsAreEqual(token->str, token->len, "while", 5)) {
                        token->kind = CL_KEYWORD_WHILE;
                    }
                } break;
            }
        } break;
        case 'g': {
            switch (c[1]) {
                case 'o': {
                    if (CL_StringsAreEqual(token->str, token->len, "goto", 4)) {
                        token->kind = CL_KEYWORD_GOTO;
                    }
                } break;
            }
        } break;
    }
    /*END*/
}

CL_API_FUNCTION void CL_LexMacroInclude(CL_LexResult *T, CL_Token *token) {
    token->kind = CL_PREPROC_INCLUDE;
    while (*T->stream == ' ') CL_Advance(T);
    char end = 0;
    if (*T->stream == '"') {
        end = '"';
    }
    else if (*T->stream == '<') {
        end = '>';
        token->flags |= CL_SYSTEM_INCLUDE;
    }
    else {
        CL_ReportError(T, token, "Invalid include directive, file not specified");
        return;
    }
    CL_Advance(T);

    token->str = T->stream;
    while (*T->stream != end) {
        if (*T->stream == 0) {
            CL_ReportError(T, token, "Invalid include directive, reached end of file while reading filename");
        }
        if (*T->stream == '\n') {
            CL_ReportError(T, token, "Invalid include directive filename, got newline character while reading filename");
        }
        CL_Advance(T);
    }
    CL_SetTokenLength(T, token);
    CL_Advance(T);

    token->str = CL_PushStringCopy(T->arena->other, token->str, token->len);

    CL_Token *include_list_item = CL_CopyToken(T->arena->include, token);
    T->includes.count += 1;
    if (T->includes.data == 0) T->includes.data = include_list_item;
}

CL_API_FUNCTION bool CL_LexMacro(CL_LexResult *T, CL_Token *token) {
    while (*T->stream == ' ' || T->stream[0] == '\t') CL_Advance(T);
    token->str = T->stream;
    while (CL_IsAlphabetic(*T->stream)) CL_Advance(T);
    CL_SetTokenLength(T, token);

    /*import meta
  meta.gen_lex_preproc_keywords() Need to add END*/
    switch (*token->str) {
        case 'd':
            if (CL_StringsAreEqual(token->str, token->len, "define", 6)) {
                token->kind = CL_PREPROC_DEFINE;
            }
            break;

        case 'i':
            if (CL_StringsAreEqual(token->str, token->len, "ifdef", 5)) {
                token->kind = CL_PREPROC_IFDEF;
            }
            else if (CL_StringsAreEqual(token->str, token->len, "ifndef", 6)) {
                token->kind = CL_PREPROC_IFNDEF;
            }
            else if (CL_StringsAreEqual(token->str, token->len, "include", 7)) {
                token->kind = CL_PREPROC_INCLUDE;
                CL_LexMacroInclude(T, token);
            }
            else if (CL_StringsAreEqual(token->str, token->len, "if", 2)) {
                token->kind = CL_PREPROC_IF;
            }
            break;

        case 'e':
            if (CL_StringsAreEqual(token->str, token->len, "endif", 5)) {
                token->kind = CL_PREPROC_ENDIF;
            }
            else if (CL_StringsAreEqual(token->str, token->len, "error", 5)) {
                token->kind = CL_PREPROC_ERROR;
            }
            else if (CL_StringsAreEqual(token->str, token->len, "else", 4)) {
                token->kind = CL_PREPROC_ELSE;
            }
            else if (CL_StringsAreEqual(token->str, token->len, "elif", 4)) {
                token->kind = CL_PREPROC_ELIF;
            }
            break;

        case 'p':
            if (CL_StringsAreEqual(token->str, token->len, "pragma", 6)) {
                token->kind = CL_PREPROC_PRAGMA;
            }
            break;

        case 'u':
            if (CL_StringsAreEqual(token->str, token->len, "undef", 5)) {
                token->kind = CL_PREPROC_UNDEF;
            }
            break;
        default: return false;
    }
    return true;
}

CL_API_FUNCTION void CL_InitLexResult(CL_LexResult *T, CL_ArenaTuple *arena, char *filename, char *filecontent) {
    CL__MemoryZero(T, sizeof(CL_LexResult));
    T->arena = arena;
    T->stream = filecontent;
    T->stream_begin = filecontent;
    T->file = filename;
}

CL_API_FUNCTION CL_LexResult *CL_CreateLexingResult(CL_ArenaTuple *arena, char *filename, char *filecontent) {
    CL_LexResult *T = CL_PushStruct(arena->other, CL_LexResult);
    CL_InitLexResult(T, arena, filename, filecontent);
    return T;
}

// Skipped space here is for case #define Memes (a), this is not a function like macro because of space
static uint32_t CL_TokenID; // @todo: make it stable, thread local?
CL_API_FUNCTION void CL_PrepareToken(CL_LexResult *T, CL_Token *token, bool skipped_space) {
    CL__MemoryZero(token, sizeof(*token));
    token->str = T->stream;
    token->line = T->line;
    token->column = T->column;
    token->file = T->file;
    token->id = ++CL_TokenID;
    if (skipped_space) token->flags |= CL_WHITESPACE_BEFORE_TOKEN;
    CL_Advance(T);
}

CL_API_FUNCTION void CL_DefaultTokenize(CL_LexResult *T, CL_Token *token) {
    char *c = token->str;
    switch (*c) {
        case 0: break;
        case '(': token->kind = CL_OPENPAREN; break;
        case ')': token->kind = CL_CLOSEPAREN; break;
        case '{': token->kind = CL_OPENBRACE; break;
        case '}': token->kind = CL_CLOSEBRACE; break;
        case '[': token->kind = CL_OPENBRACKET; break;
        case ']': token->kind = CL_CLOSEBRACKET; break;
        case ',': token->kind = CL_COMMA; break;
        case '~': token->kind = CL_NEG; break;
        case '?': token->kind = CL_QUESTION; break;
        case ';': token->kind = CL_SEMICOLON; break;
        case '.': {
            token->kind = CL_DOT;
            if (T->stream[0] == '.' && T->stream[1] == '.') {
                CL_Advance(T);
                CL_Advance(T);
                token->kind = CL_THREEDOTS;
            }
        } break;
        case ':': {
            token->kind = CL_COLON;
        } break;
        case '/': {
            token->kind = CL_DIV;
            if (*T->stream == '/') {
                token->kind = CL_COMMENT;
                CL_Advance(T);

                while (*T->stream != '\n' && *T->stream != 0) {
                    CL_Advance(T);
                }
                CL_SetTokenLength(T, token);

                CL_Token *comment_token = CL_CopyToken(T->arena->comment, token);
                if (T->comments.data == 0) T->comments.data = comment_token;
                T->comments.count += 1;
            }
            else if (*T->stream == '*') {
                token->kind = CL_COMMENT;
                CL_Advance(T);
                for (;;) {
                    if (T->stream[0] == '*' && T->stream[1] == '/') {
                        break;
                    }
                    if (T->stream[0] == 0) {
                        CL_ReportError(T, token, "Unclosed block comment");
                        goto error_end_path;
                    }
                    CL_Advance(T);
                }
                token->str += 2;
                CL_SetTokenLength(T, token);
                CL_Advance(T);
                CL_Advance(T);

                CL_Token *comment_token = CL_CopyToken(T->arena->comment, token);
                if (T->comments.data == 0) T->comments.data = comment_token;
                T->comments.count += 1;
            }
            else if (*T->stream == '=') {
                token->kind = CL_DIVASSIGN;
                CL_Advance(T);
            }
        } break;
        case '#': {
            if (*T->stream == '#') {
                token->kind = CL_MACRO_CONCAT;
                CL_Advance(T);
            }
            else {
                bool is_macro_directive = CL_LexMacro(T, token);
                if (is_macro_directive) {
                    T->inside_of_macro = true;
                }
                else {
                    if (!T->inside_of_macro) {
                        CL_ReportError(T, token, "Invalid preprocessor directive");
                        goto error_end_path;
                    }

                    token->kind = CL_PREPROC_STRINGIFY;
                    token->str = T->stream;
                    while (*T->stream == '_' || CL_IsAlphanumeric(*T->stream))
                        CL_Advance(T);
                    CL_SetTokenLength(T, token);
                }
            }
        } break;
        case '>': {
            if (*T->stream == '=') {
                token->kind = CL_GREATERTHEN_OR_EQUAL;
                CL_Advance(T);
            }
            else if (*T->stream == '>') {
                CL_Advance(T);
                if (*T->stream == '=') {
                    CL_Advance(T);
                    token->kind = CL_RIGHTSHIFTASSIGN;
                }
                else {
                    token->kind = CL_RIGHTSHIFT;
                }
            }
            else {
                token->kind = CL_GREATERTHEN;
            }
        } break;
        case '<': {
            token->kind = CL_LESSERTHEN;
            if (*T->stream == '=') {
                token->kind = CL_LESSERTHEN_OR_EQUAL;
                CL_Advance(T);
            }
            else if (*T->stream == '<') {
                CL_Advance(T);
                if (*T->stream == '=') {
                    CL_Advance(T);
                    token->kind = CL_LEFTSHIFTASSIGN;
                }
                else {
                    token->kind = CL_LEFTSHIFT;
                }
            }
        } break;
        case '&': {
            if (*T->stream == '=') {
                token->kind = CL_ANDASSIGN;
                CL_Advance(T);
            }
            else if (*T->stream == '&') {
                token->kind = CL_AND;
                CL_Advance(T);
            }
            else {
                token->kind = CL_BITAND;
            }
        } break;
        case '-': {
            if (*T->stream == '-') {
                token->kind = CL_DECREMENT;
                CL_Advance(T);
            }
            else if (*T->stream == '=') {
                token->kind = CL_SUBASSIGN;
                CL_Advance(T);
            }
            else {
                token->kind = CL_SUB;
            }
        } break;
        case '+': {
            if (*T->stream == '+') {
                token->kind = CL_INCREMENT;
                CL_Advance(T);
            }
            else if (*T->stream == '=') {
                token->kind = CL_ADDASSIGN;
                CL_Advance(T);
            }
            else {
                token->kind = CL_ADD;
            }
        } break;
        case '|': {
            if (*T->stream == '|') {
                token->kind = CL_OR;
                CL_Advance(T);
            }
            else if (*T->stream == '=') {
                token->kind = CL_ORASSIGN;
                CL_Advance(T);
            }
            else {
                token->kind = CL_BITOR;
            }
        } break;
        case '=': {
            if (*T->stream != '=') {
                token->kind = CL_ASSIGN;
            }
            else {
                CL_Advance(T);
                token->kind = CL_EQUALS;
            }
        } break;
        case '!': {
            if (*T->stream != '=') {
                token->kind = CL_NOT;
            }
            else {
                CL_Advance(T);
                token->kind = CL_NOTEQUALS;
            }
        } break;
        case '*': {
            token->kind = CL_MUL;
            if (*T->stream == '=') {
                CL_Advance(T);
                token->kind = CL_MULASSIGN;
            }
        } break;
        case '%': {
            token->kind = CL_MOD;
            if (*T->stream == '=') {
                token->kind = CL_MODASSIGN;
                CL_Advance(T);
            }
        } break;
        case '^': {
            token->kind = CL_BITXOR;
            if (*T->stream == '=') {
                CL_Advance(T);
                token->kind = CL_XORASSIGN;
            }
        } break;
        case '"': {
            CL_ParseString(T, token);
        } break;
        case '\'': {
            CL_ParseCharLiteral(T, token);
        } break;
        case 'U': { // @todo Unicode32
            if (*T->stream == '"') {
                token->fix = CL_PREFIX_U32;
                CL_Advance(T);
                CL_ParseString(T, token);
            }
            else if (*T->stream == '\'') {
                token->fix = CL_PREFIX_U32;
                CL_Advance(T);
                CL_ParseCharLiteral(T, token);
            }
            else goto parse_regular_char;
        } break;
        case 'u': {                        // Unicode16
            if (*T->stream == '8') {       // Unicode8
                if (T->stream[1] == '"') { // U8 STRING
                    token->fix = CL_PREFIX_U8;
                    CL_Advance(T);
                    CL_Advance(T);
                    CL_ParseString(T, token);
                }
                else if (T->stream[1] == '\'') { // U8 CHAR
                    token->fix = CL_PREFIX_U8;
                    CL_Advance(T);
                    CL_Advance(T);
                    CL_ParseCharLiteral(T, token);
                }
                else goto parse_regular_char;
            }
            else if (*T->stream == '"') { // U16 STRING
                token->fix = CL_PREFIX_U16;
                CL_Advance(T);
                CL_ParseString(T, token);
            }
            else if (*T->stream == '\'') { // U16 CHAR
                CL_Advance(T);
                CL_ParseCharLiteral(T, token);
            }
            else goto parse_regular_char;
        }
        case 'L': { // Widechar
            if (*T->stream == '"') {
                token->fix = CL_PREFIX_L;
                CL_Advance(T);
                CL_ParseString(T, token); // @todo UTF16
            }
            else if (*T->stream == '\'') {
                token->fix = CL_PREFIX_L;
                CL_Advance(T);
                CL_ParseCharLiteral(T, token);
            }
            else goto parse_regular_char;
        } break;
        case 'A':
        case 'a':
        case 'B':
        case 'b':
        case 'C':
        case 'c':
        case 'D':
        case 'd':
        case 'E':
        case 'e':
        case 'F':
        case 'f':
        case 'G':
        case 'g':
        case 'H':
        case 'h':
        case 'I':
        case 'i':
        case 'J':
        case 'j':
        case 'K':
        case 'k':
        /*case 'L':*/ case 'l':
        case 'M':
        case 'm':
        case 'N':
        case 'n':
        case 'O':
        case 'o':
        case 'P':
        case 'p':
        case 'Q':
        case 'q':
        case 'R':
        case 'r':
        case 'S':
        case 's':
        case 'T':
        case 't':
        // case 'U': case 'u':
        case 'V':
        case 'v':
        case 'W':
        case 'w':
        case 'X':
        case 'x':
        case 'Y':
        case 'y':
        case 'Z':
        case 'z':
        case '_':
        parse_regular_char : {
            token->kind = CL_IDENTIFIER;
            while (*T->stream == '_' || CL_IsAlphanumeric(*T->stream)) {
                CL_Advance(T);
            }
            CL_SetTokenLength(T, token);
            CL_IsIdentifierKeyword(T, token);
        } break;
        case '0': {
            if (*T->stream == 'x' || *T->stream == 'X') {
                token->kind = CL_INT;
                token->flags |= CL_HEX;
                CL_Advance(T);
                while (CL_IsHexNumeric(*T->stream)) {
                    CL_Advance(T);
                }
                uint64_t len = T->stream - token->str;
                CL_ASSERT(len > 2);
                token->u64 = CL_ParseInteger(T, token, token->str + 2, len - 2, 16);
                break;
            }
        }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            token->kind = CL_INT;
            for (;;) {
                if (*T->stream == '.') {
                    if (token->kind == CL_FLOAT) {
                        CL_ReportError(T, token, "Failed to parse a floating point number, invalid format, found multiple '.'");
                    }

                    if (token->kind == CL_INT) {
                        token->kind = CL_FLOAT;
                    }
                }
                else if (CL_IsNumeric(*T->stream) == false) {
                    break;
                }
                CL_Advance(T);
            }

            if (token->kind == CL_INT) {
                uint64_t len = T->stream - token->str;
                CL_ASSERT(len > 0);
                token->u64 = CL_ParseInteger(T, token, token->str, len, 10);
            }

            else if (token->kind == CL_FLOAT) {
                token->f64 = CL_STRING_TO_DOUBLE(token->str, token->len);
            }

            else {
                CL_ASSERT(token->kind == CL_ERROR);
            }

            if (*T->stream == 'f' || *T->stream == 'F') {
                CL_Advance(T);
                token->fix = CL_SUFFIX_F;
            }

            else if (*T->stream == 'l' || *T->stream == 'L') {
                CL_Advance(T);
                token->fix = CL_SUFFIX_L;
                if (*T->stream == 'l' || *T->stream == 'L') {
                    CL_Advance(T);
                    token->fix = CL_SUFFIX_LL;
                    if (*T->stream == 'u' || *T->stream == 'U') {
                        CL_Advance(T);
                        token->fix = CL_SUFFIX_ULL;
                    }
                }
                else if (*T->stream == 'u' || *T->stream == 'U') {
                    CL_Advance(T);
                    token->fix = CL_SUFFIX_UL;
                }
            }

            else if (*T->stream == 'u' || *T->stream == 'U') {
                CL_Advance(T);
                token->fix = CL_SUFFIX_U;
                if (*T->stream == 'l' || *T->stream == 'L') {
                    CL_Advance(T);
                    token->fix = CL_SUFFIX_UL;
                    if (*T->stream == 'l' || *T->stream == 'L') {
                        CL_Advance(T);
                        token->fix = CL_SUFFIX_ULL;
                    }
                }
            }

        } break;

        default: {
            CL_Message *result = CL_PushStruct(T->arena->other, CL_Message);
            result->kind = CLM_WARNING;
            result->string = "Unhandled character, skipping ...";
            CL_SLL_QUEUE_ADD(T->first_message, T->last_message, result);
            result->token = *token;
            token->kind = CL_COMMENT;
        } break;
    }

error_end_path:;
}

CL_API_FUNCTION bool CL_IsComment(CL_Kind kind) {
    bool result = kind == CL_COMMENT && kind != CL_EOF;
    return result;
}

CL_API_FUNCTION void CL_InitNextToken(CL_LexResult *T, CL_Token *token) {
    // Skip comments, comments get allocated on perm and gathered on the Tokenizer.
    // First non comment token gets those comments attached.
    do {
        bool skipped = CL_EatWhitespace(T);
        CL_PrepareToken(T, token, skipped);
        CL_DefaultTokenize(T, token);
    } while (CL_IsComment(token->kind));
    CL_TryToFinalizeToken(T, token);
}

CL_API_FUNCTION void CL_AddToken(CL_LexResult *T, CL_Token *token) {
    if (!T->tokens.data) T->tokens.data = token;
    T->tokens.count += 1;
}

CL_API_FUNCTION void CL_AddTokenEx(CL_Arena *arena, CL_Tokens *tokens, CL_Token *token_to_add) {
    if (token_to_add->kind != CL_EOF) {
        CL_Token *token = CL_PushStruct(arena, CL_Token);
        *token = *token_to_add;
        if (!tokens->data) tokens->data = token;
        tokens->count += 1;
    }
}

CL_API_FUNCTION void CL_AddTokenList(CL_Arena *arena, CL_Tokens *main, CL_Tokens *tokens_to_add) {
    for (int i = 0; i < tokens_to_add->count; i += 1) {
        CL_Token *it = tokens_to_add->data + i;
        CL_AddTokenEx(arena, main, it);
    }
}

CL_API_FUNCTION CL_Token *CL_AddNextToken(CL_LexResult *T) {
    CL_Token *token = CL_PushStruct(T->arena->token, CL_Token);
    CL_InitNextToken(T, token);
    CL_AddToken(T, token);
    return token;
}

CL_API_FUNCTION void CL_LexStringEx(CL_LexResult *result) {
    CL_Token *token;
    do {
        token = CL_AddNextToken(result);
    } while (token->kind != CL_EOF);
}

CL_API_FUNCTION CL_LexResult *CL_LexString(CL_ArenaTuple *arena, char *filename, char *string) {
    CL_LexResult *result = CL_CreateLexingResult(arena, filename, string);
    CL_LexStringEx(result);
    return result;
}

CL_API_FUNCTION CL_LexResult *CL_LexFile(CL_ArenaTuple *arena, char *filename) {
    char *file = CL_ReadFile(arena->other, filename);
    CL_LexResult *result = 0;
    if (file) {
        result = CL_LexString(arena, filename, file);
    }
    return result;
}

CL_API_FUNCTION void CL_AddLexResult(CL_LexList *list, CL_LexResult *result) {
    if (result == 0) return;
    CL_SLL_QUEUE_ADD_MOD(list->first_result, list->last_result, result, next_result);
    list->count += 1;
}

CL_API_FUNCTION CL_LexList CL_MakeLexList(CL_LexResult *l) {
    CL_LexList result = CL_ZeroStruct();
    CL_AddLexResult(&result, l);
    return result;
}

CL_PRIVATE_FUNCTION void CL__SetIncludeToken(CL_IncludeIter *iter, CL_Token *token) {
    if (token) {
        iter->include_token = token;
        iter->filename = token->str;
        iter->is_system_include = token->flags & CL_SYSTEM_INCLUDE;
    }
    else {
        iter->include_token = 0;
        iter->filename = 0;
        iter->is_system_include = 0;
    }
}

CL_API_FUNCTION void CL_GetNextInclude(CL_IncludeIter *iter) {
    if (iter->inited_with_filename) {
        iter->parent = iter->lex_list->first_result;
        iter->inited_with_filename = false;
    }

    for (; iter->parent;) {
        iter->include_index += 1;
        if (iter->include_index >= iter->parent->includes.count) {
            iter->parent = iter->parent->next_result;
            CL__SetIncludeToken(iter, 0);
            iter->include_index = -1;
            continue;
        }

        CL_Token *it = iter->parent->includes.data + iter->include_index;
        CL__SetIncludeToken(iter, it);

        if (iter->resolve) {
            char *filename = CL_ResolveFilepath(iter->arena, &iter->search_paths, iter->filename, iter->parent->file, iter->is_system_include);
            if (CL_IsValidFile(iter->lex_list, filename)) {
                iter->filename = filename;
            }
            else {
                CL__SetIncludeToken(iter, 0);
                continue;
            }
        }

        return;
    }
}

CL_API_FUNCTION CL_IncludeIter CL_IterateFileAndResolvedIncludes(CL_ArenaTuple *arena, char *filename, CL_SearchPaths search_paths) {
    CL_IncludeIter result;
    CL__MemoryZero(&result, sizeof(CL_IncludeIter));
    result.lex_list = CL_PushStruct(arena->other, CL_LexList);
    if (CL_FileExists(filename)) {
        result.inited_with_filename = true;
        result.filename = filename;
    }
    result.include_index = -1;
    result.resolve = true;
    result.search_paths = search_paths;
    result.arena = arena->other;
    return result;
}

CL_API_FUNCTION CL_IncludeIter CL_IterateIncludes(CL_LexList *list) {
    CL_IncludeIter result;
    CL__MemoryZero(&result, sizeof(CL_IncludeIter));
    result.lex_list = list;
    result.parent = list->first_result;
    result.include_index = -1;
    CL_GetNextInclude(&result);
    return result;
}

CL_API_FUNCTION CL_IncludeIter CL_IterateResolvedIncludes(CL_Arena *arena, CL_LexList *list, CL_SearchPaths search_paths) {
    CL_IncludeIter result;
    CL__MemoryZero(&result, sizeof(CL_IncludeIter));
    result.lex_list = list;
    result.parent = list->first_result;
    result.include_index = -1;
    result.resolve = true;
    result.search_paths = search_paths;
    result.arena = arena;
    CL_GetNextInclude(&result);
    return result;
}

#define CL_IS_POW2(x) (((x) & ((x)-1)) == 0)
#define CL_WRAP_AROUND_POWER_OF_2(x, pow2) (((x) & ((pow2)-1llu)))

CL_API_FUNCTION void CL_InitInternTable(CL_Arena *arena, CL_InternTable *table, int size) {
    CL_ASSERT(CL_IS_POW2(size));
    table->arena = arena;
    table->entries = CL_PushArray(arena, CL_InternEntry, size);
    table->entry_count = size;
    table->occupied_entry_count = 0;
}

CL_API_FUNCTION CL_InternTable *CL_CreateInternTable(CL_Arena *arena, int size) {
    CL_InternTable *result = CL_PushStruct(arena, CL_InternTable);
    CL_InitInternTable(arena, result, size);
    return result;
}

CL_API_FUNCTION CL_Intern *CL_InsertIntern(CL_InternTable *table, char *string, int len) {
    CL_ASSERT(table->arena);
    uint64_t hash = CL__HASH_BYTES(string, len);
    if (hash == 0) hash += 1;

    uint64_t index = CL_WRAP_AROUND_POWER_OF_2(hash, table->entry_count);
    CL_InternEntry *it = table->entries + index;
    for (;;) {
        if (it->hash == 0) {
            it->string = CL_PushStringCopy(table->arena, string, len);
            it->len = len;
            it->hash = hash;
            table->occupied_entry_count += 1;
            return it->string;
        }
        else if (CL_StringsAreEqual(string, len, it->string, it->len)) {
            return it->string;
        }

        if (!it->next) {
            it->next = CL_PushStruct(table->arena, CL_InternEntry);
        }
        it = it->next;
    }
}

CL_API_FUNCTION void CL_InternResult(CL_InternTable *table, CL_LexResult *result) {
    for (int i = 0; i < result->tokens.count; i += 1) {
        CL_Token *it = result->tokens.data + i;
        if (it->kind == CL_IDENTIFIER) {
            it->intern = CL_InsertIntern(table, it->str, it->len);
        }
    }
}

CL_API_FUNCTION void CL_InternListEx(CL_InternTable *table, CL_LexList *list) {
    for (CL_LexResult *it = list->first_result; it; it = it->next_result) {
        CL_InternResult(table, it);
    }
}

CL_API_FUNCTION void CL_InternList(CL_Arena *arena, CL_LexList *list) {
    list->intern_table = CL_CreateInternTable(arena, 4096);
    CL_InternListEx(list->intern_table, list);
}

CL_PRIVATE_FUNCTION char *CL_ChopLastSlash(CL_Arena *arena, char *str) {
    int i = 0;
    int slash_pos = -1;
    while (str[i]) {
        if (str[i] == '/') {
            slash_pos = i;
        }
        i += 1;
    }

    char *result = str;
    if (slash_pos != -1) {
        result = CL_PushStringCopy(arena, str, slash_pos);
    }
    else {
        result = "./";
    }
    return result;
}

CL_PRIVATE_FUNCTION char *CL_JoinPath(CL_Arena *arena, char *a, char *b) {
    int alen = CL_StringLength(a);
    int blen = CL_StringLength(b);
    int additional_len = 0;

    if (alen && a[alen - 1] != '/') additional_len = 1;
    char *result = CL_PushArray(arena, char, alen + blen + 1 + additional_len);
    CL__MemoryCopy(result, a, alen);
    if (additional_len) result[alen++] = '/';
    CL__MemoryCopy(result + alen, b, blen);
    result[alen + blen] = 0;
    return result;
}

CL_PRIVATE_FUNCTION bool CL_IsAbsolutePath(char *path) {
#if _WIN32
    bool result = CL_IsAlphabetic(path[0]) && path[1] == ':' && path[2] == '/';
#else
    bool result = path[0] == '/';
#endif
    return result;
}

char *CL_SkipToLastSlash(char *p) {
    int last_slash = 0;
    for (int i = 0; p[i]; i += 1) {
        if (p[i] == '/') last_slash = i;
    }
    return p + last_slash;
}

CL_API_FUNCTION char *CL_ResolveFilepath(CL_Arena *arena, CL_SearchPaths *search_paths, char *filename, char *parent_file, bool is_system_include) {
    CL_SearchPaths null_search_paths = CL_ZeroStruct();
    if (search_paths == 0) search_paths = &null_search_paths;

    if (search_paths->file_begin_to_ignore) {
        char *name = CL_SkipToLastSlash(filename);
        int namelen = CL_StringLength(name);
        char *ignore = search_paths->file_begin_to_ignore;
        int ignorelen = CL_StringLength(ignore);
        if (namelen > ignorelen) {
            namelen = ignorelen;
        }
        if (CL_StringsAreEqual(name, namelen, search_paths->file_begin_to_ignore, ignorelen)) {
            return 0;
        }
    }

    if (CL_IsAbsolutePath(filename) && CL_FileExists(filename)) {
        return filename;
    }

    if (is_system_include) {
        for (int path_i = 0; path_i < search_paths->system_include_path_count; path_i += 1) {
            char *path_it = search_paths->system_include_path[path_i];
            char *file = CL_JoinPath(arena, path_it, filename);
            if (CL_FileExists(file)) {
                return file;
            }
        }
    }
    else {
        if (parent_file) {
            char *parent_dir = CL_ChopLastSlash(arena, parent_file);
            char *file = CL_JoinPath(arena, parent_dir, filename);
            if (CL_FileExists(file)) {
                return file;
            }
        }

        for (int path_i = 0; path_i < search_paths->include_path_count; path_i += 1) {
            char *path_it = search_paths->include_path[path_i];
            char *file = CL_JoinPath(arena, path_it, filename);
            if (CL_FileExists(file)) {
                return file;
            }
        }
    }
    return 0;
}

CL_API_FUNCTION bool CL_IsValidFile(CL_LexList *list, char *filename) {
    if (filename == 0) return false;
    int filename_len = CL_StringLength(filename);
    if (filename_len == 0) return false;

    for (CL_LexResult *it = list->first_result; it; it = it->next_result) {
        int file_len = CL_StringLength(it->file);
        if (CL_StringsAreEqual(filename, filename_len, it->file, file_len)) {
            return false;
        }
    }
    return true;
}

CL_API_FUNCTION CL_LexResult *CL_GetFile(CL_LexList *list, char *name) {
    for (CL_LexResult *it = list->first_result; it; it = it->next_result) {
        if (CL_StringsAreEqual(it->file, CL_StringLength(it->file), name, CL_StringLength(name))) {
            return it;
        }
    }
    return 0;
}

CL_API_FUNCTION void CL_InitDefaultTuple(CL_ArenaTuple *tuple) {
    CL__MemoryZero(tuple, sizeof(CL_ArenaTuple));
    tuple->comment = &tuple->default_comment;
    tuple->token = &tuple->default_token;
    tuple->include = &tuple->default_include;
    tuple->other = &tuple->default_other;
}

CL_API_FUNCTION CL_LexList CL_LexRecursive(CL_ArenaTuple *arena, char *filename, CL_SearchPaths paths) {
    CL_LexResult *first_file = CL_LexFile(arena, filename);
    CL_LexList result = CL_MakeLexList(first_file);
    result.search_paths = paths;
    for (CL_IncludeIter iter = CL_IterateResolvedIncludes(arena->other, &result, paths); iter.filename; CL_GetNextInclude(&iter)) {
        CL_LexResult *file = CL_LexFile(arena, iter.filename);
        CL_AddLexResult(&result, file);
    }
    return result;
}

#endif // CL_IMPLEMENTATION
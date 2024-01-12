#pragma once
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

typedef enum CL_Kind {
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
} CL_Kind;

typedef enum CL_Fix {
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
} CL_Fix;

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

CL_INLINE bool CL_StringsAreEqual(char *a, int64_t alen, const char *b, int64_t blen) {
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

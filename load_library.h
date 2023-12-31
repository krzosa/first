#ifndef LIB_LOAD_HEADER
#define LIB_LOAD_HEADER

typedef void *LIB_Library;

LIB_Library LIB_LoadLibrary(char *str);
void *LIB_LoadSymbol(LIB_Library lib, char *symbol);
bool LIB_UnloadLibrary(LIB_Library lib);

#ifndef LIB_EXPORT
    #define LIB_EXPORT __declspec(dllexport)
#endif

#endif
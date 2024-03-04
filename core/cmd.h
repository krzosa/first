enum CmdDeclKind {
    CmdDeclKind_Bool,
    CmdDeclKind_List,
    CmdDeclKind_Enum,
};

struct CmdDecl {
    CmdDeclKind       kind;
    S8_String         name;
    S8_String         help;

    bool *            bool_result;
    Array<S8_String> *list_result;

    int *             enum_result;
    const char **     enum_options;
    int               enum_option_count;

};

struct CmdParser {
    int            argc;
    char **        argv;
    Array<CmdDecl> decls;
};

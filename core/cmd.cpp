CmdParser MakeCmdParser(MA_Arena *arena, int argc, char **argv) {
    CmdParser result = {argc, argv};
    result.decls = {arena};
    return result;
}

void AddBool(CmdParser *p, bool *value, const char *name, const char *help) {
    p->decls.add({CmdDeclKind_Bool, S8_MakeFromChar((char *)name), S8_MakeFromChar((char *)help), value});
}

void AddList(CmdParser *p, Array<S8_String> *list, const char *name, const char *help) {
    p->decls.add({CmdDeclKind_List, S8_MakeFromChar((char *)name), S8_MakeFromChar((char *)help), 0, list});
}

void AddEnum(CmdParser *p, int *result, const char *name, const char *help, const char **enum_options, int enum_option_count) {
    CmdDecl *decl = p->decls.alloc();
    decl->kind = CmdDeclKind_Enum;
    decl->name = S8_MakeFromChar((char *)name);
    decl->help = S8_MakeFromChar((char *)help);
    decl->enum_result = result;
    decl->enum_options = enum_options;
    decl->enum_option_count = enum_option_count;;
}

CmdDecl *FindDecl(CmdParser *p, S8_String name) {
    For (p->decls) {
        if (it.name == name) {
            return &it;
        }
    }
    return NULL;
}

S8_String StrEnumValues(MA_Arena *arena, CmdDecl *decl) {
    S8_List list = {};
    S8_AddF(arena, &list, "[");
    for (int i = 0; i < decl->enum_option_count; i += 1) {
        S8_AddF(arena, &list, "%s", decl->enum_options[i]);
        if (i != decl->enum_option_count - 1) S8_AddF(arena, &list, "|");
    }
    S8_AddF(arena, &list, "]");
    return S8_Merge(arena, list);
}

void PrintCmdUsage(CmdParser *p) {
    MA_Scratch scratch;

    IO_Printf("\nhere are the supported commands:\n");
    For(p->decls) {
        if (it.kind == CmdDeclKind_List) {
            S8_String example = S8_Format(scratch, "-%.*s a b c", S8_Expand(it.name));
            IO_Printf("%-30.*s %.*s\n", S8_Expand(example), S8_Expand(it.help));
        } else if (it.kind == CmdDeclKind_Bool) {
            S8_String example = S8_Format(scratch, "-%.*s", S8_Expand(it.name));
            IO_Printf("%-30.*s %.*s\n", S8_Expand(example), S8_Expand(it.help));
        } else if (it.kind == CmdDeclKind_Enum) {
            S8_String enum_vals = StrEnumValues(scratch, &it);
            S8_String example = S8_Format(scratch, "-%.*s %.*s", S8_Expand(it.name), S8_Expand(enum_vals));
            IO_Printf("%-30.*s %.*s\n", S8_Expand(example), S8_Expand(it.help));
        } else IO_Todo();
    }
}

bool InvalidCmdArg(CmdParser *p, S8_String arg) {
    IO_Printf("invalid command line argument: %.*s\n", S8_Expand(arg));
    PrintCmdUsage(p);
    return false;
}

bool ParseCmd(CmdParser *p) {
    for (int i = 1; i < p->argc; i += 1) {
        S8_String arg = S8_MakeFromChar(p->argv[i]);

        if (arg == "--help" || arg == "-h" || arg == "-help") {
            PrintCmdUsage(p);
            return false;
        }

        if (arg.str[0] == '-') {
            arg = S8_Skip(arg, 1);
            if (arg.str[0] == '-') {
                arg = S8_Skip(arg, 1);
            }

            CmdDecl *decl = FindDecl(p, arg);
            if (!decl) return InvalidCmdArg(p, arg);

            if (decl->kind == CmdDeclKind_Bool) {
                *decl->bool_result = !*decl->bool_result;
            } else if (decl->kind == CmdDeclKind_Enum) {
                if (i + 1 >= p->argc) { IO_Printf("expected at least 1 argument after %.*s\n", S8_Expand(arg)); PrintCmdUsage(p); return false; }
                S8_String option_from_cmd = S8_MakeFromChar(p->argv[++i]);

                bool found_option = false;
                for (int i = 0; i < decl->enum_option_count; i += 1) {
                    S8_String option = S8_MakeFromChar((char *)decl->enum_options[i]);
                    if (option == option_from_cmd) {
                        *decl->enum_result = i;
                        found_option = true;
                        break;
                    }
                }

                if (!found_option) {
                    MA_Scratch scratch;
                    IO_Printf("expected one of the enum values: %.*s", S8_Expand(StrEnumValues(scratch, decl)));
                    IO_Printf(" got instead: %.*s\n", S8_Expand(option_from_cmd));
                    PrintCmdUsage(p);
                    return false;
                }

            } else if (decl->kind == CmdDeclKind_List) {
                if (i + 1 >= p->argc) { IO_Printf("expected at least 1 argument after %.*s\n", S8_Expand(arg)); PrintCmdUsage(p); return false; }

                i += 1;
                for (int counter = 0; i < p->argc; i += 1, counter += 1) {
                    S8_String arg = S8_MakeFromChar(p->argv[i]);
                    if (arg.str[0] == '-') {
                        if (counter == 0) { IO_Printf("expected at least 1 argument after %.*s\n", S8_Expand(arg)); PrintCmdUsage(p); return false; }
                        i -= 1;
                        break;
                    }

                    decl->list_result->add(arg);
                }
            } else IO_Todo();

        }
        else return InvalidCmdArg(p, arg);
    }
    return true;
}

// ./a.exe -scratch
// ./a.exe -quick
// ./a.exe -run_tests a.test b.test c.test
// ./a.exe --break_on_error
// ./a.exe -build=release
// ./a.exe --help
//
void TestCmdParser() {
    char *argv[] = {
        "exe",
        "--build_scratch",
        "-tests",
        "a", "b", "c",
        "--things", "1234", "asdsa",
        "-build", "release",
    };
    int argc = MA_Lengthof(argv);

    MA_Scratch scratch;
    bool build_scratch = false;
    Array<S8_String> test_list = {scratch};
    Array<S8_String> test_things = {scratch};
    int build_profile = 0;
    const char *build_profiles[] = {"debug", "release"};
    int build_profiles_count = MA_Lengthof(build_profiles);

    CmdParser p = MakeCmdParser(scratch, argc, argv);
    AddBool(&p, &build_scratch, "build_scratch", "builds a sandbox where I experiment with things");
    AddList(&p, &test_list, "tests", "list of specific tests to run");
    AddList(&p, &test_things, "things", "list of things");
    AddEnum(&p, &build_profile, "build", "choose build profile", build_profiles, build_profiles_count);
    bool success = ParseCmd(&p);
    IO_Assertf(success, "failed to parse cmd");
    IO_Assert(build_scratch);
    IO_Assert(test_list.len == 3);
    IO_Assert(test_list[2] == "c");
    IO_Assert(test_list[0] == "a");
    IO_Assert(test_things.len == 2);
    IO_Assert(build_profile == 1);
}

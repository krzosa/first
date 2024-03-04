
CmdParser MakeCmdParser(MA_Arena *arena, int argc, char **argv, const char *custom_help) {
    CmdParser result = {argc, argv, arena, custom_help};
    return result;
}

void AddBool(CmdParser *p, bool *result, const char *name, const char *help) {
    CmdDecl *decl     = MA_PushStruct(p->arena, CmdDecl);
    decl->kind        = CmdDeclKind_Bool;
    decl->name        = S8_MakeFromChar((char *)name);
    decl->help        = S8_MakeFromChar((char *)help);
    decl->bool_result = result;
    SLL_QUEUE_ADD(p->fdecl, p->ldecl, decl);
}

void AddList(CmdParser *p, S8_List *result, const char *name, const char *help) {
    CmdDecl *decl     = MA_PushStruct(p->arena, CmdDecl);
    decl->kind        = CmdDeclKind_List;
    decl->name        = S8_MakeFromChar((char *)name);
    decl->help        = S8_MakeFromChar((char *)help);
    decl->list_result = result;
    SLL_QUEUE_ADD(p->fdecl, p->ldecl, decl);
}

void AddEnum(CmdParser *p, int *result, const char *name, const char *help, const char **enum_options, int enum_option_count) {
    CmdDecl *decl           = MA_PushStruct(p->arena, CmdDecl);
    decl->kind              = CmdDeclKind_Enum;
    decl->name              = S8_MakeFromChar((char *)name);
    decl->help              = S8_MakeFromChar((char *)help);
    decl->enum_result       = result;
    decl->enum_options      = enum_options;
    decl->enum_option_count = enum_option_count;
    SLL_QUEUE_ADD(p->fdecl, p->ldecl, decl);
}

CmdDecl *FindDecl(CmdParser *p, S8_String name) {
    for (CmdDecl *it = p->fdecl; it; it = it->next) {
        if (S8_AreEqual(it->name, name, true)) {
            return it;
        }
    }
    return NULL;
}

S8_String StrEnumValues(MA_Arena *arena, CmdDecl *decl) {
    S8_List list = {0};
    S8_AddF(arena, &list, "[");
    for (int i = 0; i < decl->enum_option_count; i += 1) {
        S8_AddF(arena, &list, "%s", decl->enum_options[i]);
        if (i != decl->enum_option_count - 1) S8_AddF(arena, &list, "|");
    }
    S8_AddF(arena, &list, "]");
    return S8_Merge(arena, list);
}

void PrintCmdUsage(CmdParser *p) {
    IO_Printf("%s\nCommands:\n", p->custom_help);
    for (CmdDecl *it = p->fdecl; it; it = it->next) {
        IO_Printf("  ");
        if (it->kind == CmdDeclKind_List) {
            S8_String example = S8_Format(p->arena, "-%.*s a b c", S8_Expand(it->name));
            IO_Printf("%-30.*s %.*s\n", S8_Expand(example), S8_Expand(it->help));
        } else if (it->kind == CmdDeclKind_Bool) {
            S8_String example = S8_Format(p->arena, "-%.*s", S8_Expand(it->name));
            IO_Printf("%-30.*s %.*s\n", S8_Expand(example), S8_Expand(it->help));
        } else if (it->kind == CmdDeclKind_Enum) {
            S8_String enum_vals = StrEnumValues(p->arena, it);
            S8_String example = S8_Format(p->arena, "-%.*s %.*s", S8_Expand(it->name), S8_Expand(enum_vals));
            IO_Printf("%-30.*s %.*s\n", S8_Expand(example), S8_Expand(it->help));
        } else IO_Todo();
    }
}

bool InvalidCmdArg(CmdParser *p, S8_String arg) {
    IO_Printf("invalid command line argument: %.*s\n", S8_Expand(arg));
    return false;
}

bool ParseCmd(MA_Arena *arg_arena, CmdParser *p) {
    for (int i = 1; i < p->argc; i += 1) {
        S8_String arg = S8_MakeFromChar(p->argv[i]);

        if (S8_AreEqual(arg, S8_Lit("--help"), true) || S8_AreEqual(arg, S8_Lit("-h"), true) || S8_AreEqual(arg, S8_Lit("-help"), true)) {
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
                if (i + 1 >= p->argc) { IO_Printf("expected at least 1 argument after %.*s\n", S8_Expand(arg)); return false; }
                S8_String option_from_cmd = S8_MakeFromChar(p->argv[++i]);

                bool found_option = false;
                for (int i = 0; i < decl->enum_option_count; i += 1) {
                    S8_String option = S8_MakeFromChar((char *)decl->enum_options[i]);
                    if (S8_AreEqual(option, option_from_cmd, true)) {
                        *decl->enum_result = i;
                        found_option = true;
                        break;
                    }
                }

                if (!found_option) {
                    IO_Printf("expected one of the enum values: %.*s", S8_Expand(StrEnumValues(p->arena, decl)));
                    IO_Printf(" got instead: %.*s\n", S8_Expand(option_from_cmd));
                    return false;
                }

            } else if (decl->kind == CmdDeclKind_List) {
                if (i + 1 >= p->argc) { IO_Printf("expected at least 1 argument after %.*s\n", S8_Expand(arg)); return false; }

                i += 1;
                for (int counter = 0; i < p->argc; i += 1, counter += 1) {
                    S8_String arg = S8_MakeFromChar(p->argv[i]);
                    if (arg.str[0] == '-') {
                        if (counter == 0) { IO_Printf("expected at least 1 argument after %.*s\n", S8_Expand(arg)); return false; }
                        i -= 1;
                        break;
                    }

                    S8_AddNode(arg_arena, decl->list_result, arg);
                }
            } else IO_Todo();

        }
        else {
            if (p->require_one_standalone_arg && p->args.node_count == 0) {
                S8_AddNode(arg_arena, &p->args, arg);
            } else {
                return InvalidCmdArg(p, arg);
            }
        }
    }

    if (p->require_one_standalone_arg && p->args.node_count == 0) {
        PrintCmdUsage(p);
        return false;
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

    MA_Checkpoint scratch = MA_GetScratch();
    bool build_scratch = false;
    S8_List test_list = {0};
    S8_List test_things = {0};
    int build_profile = 0;
    const char *build_profiles[] = {"debug", "release"};
    int build_profiles_count = MA_Lengthof(build_profiles);

    CmdParser p = MakeCmdParser(scratch.arena, argc, argv, "this is a test");
    AddBool(&p, &build_scratch, "build_scratch", "builds a sandbox where I experiment with things");
    AddList(&p, &test_list, "tests", "list of specific tests to run");
    AddList(&p, &test_things, "things", "list of things");
    AddEnum(&p, &build_profile, "build", "choose build profile", build_profiles, build_profiles_count);
    bool success = ParseCmd(scratch.arena, &p);
    IO_Assertf(success, "failed to parse cmd");
    IO_Assert(build_scratch);
    IO_Assert(test_list.node_count == 3);
    IO_Assert(test_things.node_count == 2);
    IO_Assert(build_profile == 1);
    MA_ReleaseScratch(scratch);
}

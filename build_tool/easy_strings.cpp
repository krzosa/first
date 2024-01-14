S8_String Fmt(const char *str, ...) S8__PrintfFormat(1, 2);

Array<S8_String> operator+(Array<S8_String> a, Array<S8_String> b) {
    Array<S8_String> c = a.copy(*Perm);
    c.add_array(b);
    return c;
}

Array<S8_String> operator+(Array<S8_String> a, S8_String b) {
    Array<S8_String> c = a.copy(*Perm);
    c.add(b);
    return c;
}

Array<S8_String> operator+(S8_String a, Array<S8_String> b) {
    Array<S8_String> c = b.copy(*Perm);
    c.insert(a, 0);
    return c;
}

Array<S8_String> operator+(S8_String a, S8_String b) {
    Array<S8_String> c = {*Perm};
    c.add(a);
    c.add(b);
    return c;
}

Array<S8_String> &operator+=(Array<S8_String> &a, Array<S8_String> b) {
    a.add_array(b);
    return a;
}

Array<S8_String> &operator+=(Array<S8_String> &a, S8_String s) {
    a.add(s);
    return a;
}

//@todo: split on any whitespace instead!
Array<S8_String> Split(char *str) {
    S8_String s = S8_MakeFromChar(str);
    S8_List list = S8_Split(Perm, s, S8_Lit(" "), 0);

    Array<S8_String> result = {*Perm};
    S8_For(it, list) result.add(it->string);
    return result;
}

S8_String Merge(Array<S8_String> list, S8_String separator = " "_s) {
    int64_t char_count = 0;
    For(list) char_count += it.len;
    if (char_count == 0) return {};
    int64_t node_count = list.len;

    int64_t base_size = (char_count + 1);
    int64_t sep_size = (node_count - 1) * separator.len;
    int64_t size = base_size + sep_size;
    char *buff = (char *)MA_PushSize(Perm, sizeof(char) * size);
    S8_String string = S8_Make(buff, 0);
    For(list) {
        IO_Assert(string.len + it.len <= size);
        MA_MemoryCopy(string.str + string.len, it.str, it.len);
        string.len += it.len;
        if (!list.is_last(it)) {
            MA_MemoryCopy(string.str + string.len, separator.str, separator.len);
            string.len += separator.len;
        }
    }
    IO_Assert(string.len == size - 1);
    string.str[size] = 0;
    return string;
}

S8_String Fmt(const char *str, ...) {
    S8_FORMAT(Perm, str, str_fmt);
    return str_fmt;
}

bool CodeWasModified(S8_String str, S8_String artifact = {}) {
    return SRC_WasModified(str, artifact);
}

S8_String IfCodeWasModified(S8_String cfile, S8_String objfile) {
    Array<S8_String> result = {};
    if (SRC_WasModified(cfile, objfile))
        return cfile;
    return objfile;
}

int Run(Array<S8_String> s) {
    S8_String cmd = Merge(s);
    return OS_SystemF("%.*s", S8_Expand(cmd));
}

Array<S8_String> ListDir(char *dir) {
    Array<S8_String> result = {};
    for (OS_FileIter it = OS_IterateFiles(Perm, S8_MakeFromChar(dir)); OS_IsValid(it); OS_Advance(&it)) {
        result.add(S8_Copy(Perm, it.absolute_path));
    }
    return result;
}

#include "../core_library/core.c"

int main() {
    S8_String s = "mrówka";

    bool found_two_byte = false;
    For(s) {
        if (it.utf8_codepoint_byte_size == 2) {
            found_two_byte = true;
            IO_Assert(it.i == 2);
        }
    }
    IO_Assert(found_two_byte);
}
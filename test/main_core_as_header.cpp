#include "../core.h"

int main() {
    MA_Arena arena = {};
    int *a = MA_PushStruct(&arena, int);
    *a = 10;
    return 0;
}
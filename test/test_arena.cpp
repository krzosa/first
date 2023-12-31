#include "../io.c"

#define MA_ASSERT(x) IO_Assert(x)
#include "../arena.c"

int main() {
    MA_Scratch scratch;
    int *thing = MA_PushStruct(scratch, int);
    *thing = 10;

    MA_Arena *arena = MA_Bootstrap();
    float *thingf = MA_PushStruct(arena, float);
    *thingf = 10.0f;
}
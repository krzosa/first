#ifndef HASH_HEADER
#define HASH_HEADER
#include <stdint.h>

#ifndef HASH_API_FUNCTION
    #ifdef __cplusplus
        #define HASH_API_FUNCTION extern "C"
    #else
        #define HASH_API_FUNCTION
    #endif
#endif

//@begin gen_structs
typedef struct RandomSeed RandomSeed;
//@end gen_structs

struct RandomSeed {
    uint64_t a;
};

//@begin gen_api_funcs
HASH_API_FUNCTION uint64_t        HashBytes(void *data, uint64_t size);
HASH_API_FUNCTION RandomSeed      MakeRandomSeed(uint64_t value);
HASH_API_FUNCTION uint64_t        GetRandomU64(RandomSeed *state);
HASH_API_FUNCTION int             GetRandomRangeI(RandomSeed *seed, int first, int last_included);
HASH_API_FUNCTION double          GetRandomNormal(RandomSeed *series);
HASH_API_FUNCTION double          GetRandomNormalRange(RandomSeed *seed, double min, double max);
HASH_API_FUNCTION uint64_t        HashMix(uint64_t x, uint64_t y);
//@end gen_api_funcs

#define WRAP_AROUND_POWER_OF_2(x, pow2) (((x) & ((pow2)-1llu)))
static inline float GetRandomNormalF(RandomSeed *series) { return (float)GetRandomNormal(series); }
#endif

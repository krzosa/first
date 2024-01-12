#pragma once
#include <stdint.h>
/*
    Hash table implementation:
        Pointers to values
        Open adressing
        Linear Probing
        Power of 2
        Robin Hood hashing
        Resizes on high probe count (min max load factor)

        Hash 0 is reserved for empty hash table entry
*/
#ifndef TABLE_PRIVATE_FUNCTION
    #if defined(__GNUC__) || defined(__clang__)
        #define TABLE_PRIVATE_FUNCTION __attribute__((unused)) static
    #else
        #define TABLE_PRIVATE_FUNCTION static
    #endif
#endif

#ifndef TABLE_Allocator
#define TABLE_Allocator void *
#endif

#ifndef TABLE_ALLOCATE
    #include <stdlib.h>
    #define TABLE_ALLOCATE(allocator, size) malloc(size)
#endif

#ifndef TABLE_DEALLOCATE
    #include <stdlib.h>
    #define TABLE_DEALLOCATE(allocator, p) free(p)
#endif

#ifndef TABLE_ASSERT
    #include <assert.h>
    #define TABLE_ASSERT(x) assert(x)
#endif

// Example:
// #define TABLE_SET_DEFAULT_ALLOCATOR if (!allocator) allocator = global_heap;
#ifndef TABLE_SET_DEFAULT_ALLOCATOR
    #define TABLE_SET_DEFAULT_ALLOCATOR
#endif

#ifndef TABLE_HASH_BYTES
    #define TABLE_HASH_BYTES TABLE__HashBytes
TABLE_PRIVATE_FUNCTION uint64_t TABLE__HashBytes(void *data, unsigned size) {
    uint8_t *data8 = (uint8_t *)data;
    uint64_t hash = (uint64_t)14695981039346656037ULL;
    for (unsigned i = 0; i < size; i++) {
        hash = hash ^ (uint64_t)(data8[i]);
        hash = hash * (uint64_t)1099511628211ULL;
    }
    return hash;
}
#endif

#define TABLE__WRAP_AROUND_POWER_OF_2(x, pow2) (((x) & ((pow2)-1llu)))
#define TABLE__IS_POW2(x) (((x) & ((x)-1)) == 0)

TABLE_PRIVATE_FUNCTION int TABLE_CStringLen(char *str) {
    int i = 0;
    while(str[i]) i += 1;
    return i;
}

template <class Value>
struct Table {
    struct Entry {
        uint64_t hash;
        uint64_t key;
        size_t distance;
        Value value;
    };

    TABLE_Allocator allocator;
    size_t len, cap;
    Entry *values;

    static const size_t max_load_factor = 80;
    static const size_t min_load_factor = 50;
    static const size_t significant_distance = 8;

    // load factor calculation was rearranged
    // to get rid of division:
    //> 100 * len / cap = load_factor
    //> len * 100 = load_factor * cap
    inline bool reached_load_factor(size_t lfactor) {
        return (len + 1) * 100 >= lfactor * cap;
    }

    inline bool is_empty(Entry *entry) { return entry->hash == 0; }
    inline bool is_occupied(Entry *entry) { return entry->hash != 0; }

    void reserve(size_t size) {
        TABLE_ASSERT(size > cap && "New size is smaller then original size");
        TABLE_ASSERT(TABLE__IS_POW2(size));
        TABLE_SET_DEFAULT_ALLOCATOR;

        Entry *old_values = values;
        size_t old_cap = cap;

        values = (Entry *)TABLE_ALLOCATE(allocator, sizeof(Entry) * size);
        for (int i = 0; i < size; i += 1) values[i] = {};
        cap = size;

        TABLE_ASSERT(!(old_values == 0 && len != 0));
        if (len == 0) {
            if (old_values) TABLE_DEALLOCATE(allocator, old_values);
            return;
        }

        len = 0;
        for (size_t i = 0; i < old_cap; i += 1) {
            Entry *it = old_values + i;
            if (is_occupied(it)) {
                insert(it->key, it->value);
            }
        }
        TABLE_DEALLOCATE(allocator, old_values);
    }

    Entry *get_table_entry(uint64_t key) {
        if (len == 0) return 0;
        uint64_t hash = TABLE_HASH_BYTES(&key, sizeof(key));
        if (hash == 0) hash += 1;
        uint64_t index = TABLE__WRAP_AROUND_POWER_OF_2(hash, cap);
        uint64_t i = index;
        uint64_t distance = 0;
        for (;;) {
            Entry *it = values + i;
            if (distance > it->distance) {
                return 0;
            }

            if (it->hash == hash && it->key == key) {
                return it;
            }

            distance += 1;
            i = TABLE__WRAP_AROUND_POWER_OF_2(i + 1, cap);
            if (i == index) return 0;
        }
        TABLE_ASSERT(!"Invalid codepath");
    }

    void insert(uint64_t key, const Value &value) {
        if (reached_load_factor(max_load_factor)) {
            if (cap == 0) cap = 16; // 32 cause cap*2
            reserve(cap * 2);
        }

        uint64_t hash = TABLE_HASH_BYTES(&key, sizeof(key));
        if (hash == 0) hash += 1;
        uint64_t index = TABLE__WRAP_AROUND_POWER_OF_2(hash, cap);
        uint64_t i = index;
        Entry to_insert = {hash, key, 0, value};
        for (;;) {
            Entry *it = values + i;
            if (is_empty(it)) {
                *it = to_insert;
                len += 1;
                // If we have more then 8 consecutive items we try to resize
                if (to_insert.distance > 8 && reached_load_factor(min_load_factor)) {
                    reserve(cap * 2);
                }
                return;
            }
            if (it->hash == hash && it->key == key) {
                *it = to_insert;
                // If we have more then 8 consecutive items we try to resize
                if (to_insert.distance > 8 && reached_load_factor(min_load_factor)) {
                    reserve(cap * 2);
                }
                return;
            }

            // Robin hood hashing
            if (to_insert.distance > it->distance) {
                Entry temp = to_insert;
                to_insert = *it;
                *it = temp;
            }

            to_insert.distance += 1;
            i = TABLE__WRAP_AROUND_POWER_OF_2(i + 1, cap);
            TABLE_ASSERT(i != index && "Did a full 360 through a hash table, no good :( that shouldnt be possible");
        }
        TABLE_ASSERT(!"Invalid codepath");
    }

    void remove(uint64_t key) {
        Entry *entry = get_table_entry(key);
        entry->hash = 0;
        entry->distance = 0;
        len -= 1;
    }

    Value *get(uint64_t key) {
        Entry *v = get_table_entry(key);
        if (!v) return 0;
        return &v->value;
    }

    Value get(uint64_t key, Value default_value) {
        Entry *v = get_table_entry(key);
        if (!v) return default_value;
        return v->value;
    }

    Value *gets(char *str) {
        int len = TABLE_CStringLen(str);
        uint64_t hash = TABLE_HASH_BYTES(str, len);
        return get(hash);
    }

    Value gets(char *str, Value default_value) {
        int len = TABLE_CStringLen(str);
        uint64_t hash = TABLE_HASH_BYTES(str, len);
        return get(hash, default_value);
    }

    #ifdef S8_HEADER
    Value *get(S8_String s) {
        uint64_t hash = TABLE_HASH_BYTES(s.str, (unsigned)s.len);
        return get(hash);
    }

    Value get(S8_String s, Value default_value) {
        uint64_t hash = TABLE_HASH_BYTES(s.str, (unsigned)s.len);
        return get(hash, default_value);
    }

    void put(S8_String s, const Value &value) {
        uint64_t hash = TABLE_HASH_BYTES(s.str, (unsigned)s.len);
        insert(hash, value);
    }
    #endif

    void puts(char *str, const Value &value) {
        int len = TABLE_CStringLen(str);
        uint64_t hash = TABLE_HASH_BYTES(str, len);
        insert(hash, value);
    }

    void reset() {
        len = 0;
        for (size_t i = 0; i < cap; i += 1) {
            Entry *it = values + i;
            it->hash = 0;
        }
    }

    void dealloc() {
        TABLE_DEALLOCATE(allocator, values);
        len = 0;
        cap = 0;
        values = 0;
    }
};
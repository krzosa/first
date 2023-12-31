#pragma once

#ifndef ARRAY_REALLOCATE
    #include <stdlib.h>
    #define ARRAY_REALLOCATE(allocator, p, size, old_size) realloc(p, size)
    #define ARRAY_DEALLOCATE(allocator, p) free(p)
#endif

#ifndef ARRAY_ASSERT
    #include <assert.h>
    #define ARRAY_ASSERT(x) assert(x)
#endif

#ifndef ARRAY_MemoryMove
    #include <string.h>
    #define ARRAY_MemoryMove(dst, src, size) memmove(dst, src, size)
#endif

#ifndef ARRAY_Allocator
    #define ARRAY_Allocator void *
#endif

// Example:
// #define ARRAY_SET_DEFAULT_ALLOCATOR if (!allocator) allocator = global_heap;
#ifndef ARRAY_SET_DEFAULT_ALLOCATOR
    #define ARRAY_SET_DEFAULT_ALLOCATOR
#endif

// Iterating and removing elements
//
//     ForArrayRemovable(array) {
//         ForArrayRemovablePrepare(array);
//         if (it == 4) ForArrayRemovableDeclare();
//     }
//
#ifdef DEFER_HEADER
    #define ForArrayRemovable(a) for (int __i = 0; __i < (a).len; __i += 1)
    #define ForArrayRemovablePrepare(a) \
        auto &it = (a)[__i];            \
        bool remove_it = false;         \
        defer {                         \
            if (remove_it) {            \
                (a).ordered_remove(it); \
                __i -= 1;               \
            }                           \
        }
    #define ForArrayRemovableDeclare() (remove_it = true)
#endif

#ifndef For
    #define For2(it, array) for(auto &it : (array))
    #define For(array) For2(it, array)
#endif

template <class T>
struct Array {
    ARRAY_Allocator allocator;
    T *data;
    int cap, len;

    T &operator[](int index) {
        ARRAY_ASSERT(index >= 0 && index < len);
        return data[index];
    }
    T &operator[](long long index) {
        ARRAY_ASSERT(index >= 0 && index < len);
        return data[index];
    }

    bool is_first(T &item) { return &item == first(); }
    bool is_last(T &item) { return &item == last(); }

    bool contains(T &item) {
        bool result = &item >= data && &item < data + len;
        return result;
    }

    int get_index(T &item) {
        ARRAY_ASSERT((data <= &item) && ((data + len) > &item));
        size_t offset = &item - data;
        return (int)offset;
    }

    void add(T item) {
        try_growing();
        data[len++] = item;
    }

    // Struct needs to have 'value_to_sort_by' field
    void sorted_insert_decreasing(T item) {
        int insert_index = -1;
        For(*this) {
            if (it.value_to_sort_by <= item.value_to_sort_by) {
                insert_index = get_index(it);
                insert(item, insert_index);
                break;
            }
        }

        if (insert_index == -1) {
            add(item);
        }
    }

    void bounded_add(T item) {
        ARRAY_ASSERT(len + 1 <= cap);
        try_growing(); // in case of error
        data[len++] = item;
    }

    T *alloc(const T &item) {
        try_growing();
        T *ref = data + len++;
        *ref = item;
        return ref;
    }

    T *alloc() {
        try_growing();
        T *ref = data + len++;
        *ref = {};
        return ref;
    }

	T *alloc_multiple(int size) {
		try_growing_to_fit_item_count(size);
		T *result = data + len;
		len += size;
		return result;
	}

    void add_array(T *items, int item_count) {
        for (int i = 0; i < item_count; i += 1) {
            add(items[i]);
        }
    }

    void add_array(Array<T> items) {
        add_array(items.data, items.len);
    }

    void reserve(int size) {
        if (size > cap) {
            ARRAY_SET_DEFAULT_ALLOCATOR;

            void *p = ARRAY_REALLOCATE(allocator, data, size * sizeof(T), cap * sizeof(T));
            ARRAY_ASSERT(p);

            data = (T *)p;
            cap = size;
        }
    }

    void init(ARRAY_Allocator allocator, int size) {
        len = 0;
        cap = 0;
        data = 0;
        this->allocator = allocator;
        reserve(size);
    }

    void reset() {
        len = 0;
    }

    T pop() {
        ARRAY_ASSERT(len > 0);
        return data[--len];
    }

    void unordered_remove(T &item) { // DONT USE IN LOOPS !!!!
        ARRAY_ASSERT(len > 0);
        ARRAY_ASSERT(&item >= begin() && &item < end());
        item = data[--len];
    }

    void unordered_remove_index(int index) {
        ARRAY_ASSERT(index >= 0 && index < len);
        data[index] = data[--len];
    }

    int get_index(const T &item) {
        ptrdiff_t index = (ptrdiff_t)(&item - data);
        ARRAY_ASSERT(index >= 0 && index < len);
        // ARRAY_ASSERT(index > INT_MIN && index < INT_MAX);
        return (int)index;
    }

    void ordered_remove(T &item) { // DONT USE IN LOOPS !!!
        ARRAY_ASSERT(len > 0);
        ARRAY_ASSERT(&item >= begin() && &item < end());
        int index = get_index(item);
        ordered_remove_index(index);
    }

    void ordered_remove_index(int index) {
        ARRAY_ASSERT(index >= 0 && index < len);
        int right_len = len - index - 1;
        ARRAY_MemoryMove(data + index, data + index + 1, right_len * sizeof(T));
        len -= 1;
    }

    void insert(T item, int index) {
        if (index == len) {
            add(item);
            return;
        }

        ARRAY_ASSERT(index < len);
        ARRAY_ASSERT(index >= 0);

        try_growing();
        int right_len = len - index;
        ARRAY_MemoryMove(data + index + 1, data + index, sizeof(T) * right_len);
        data[index] = item;
        len += 1;
    }

    void dealloc() {
        if (data) ARRAY_DEALLOCATE(allocator, data);
        data = 0;
        len = cap = 0;
    }

    Array<T> exact_copy(ARRAY_Allocator allocator) {
        Array result = {};
        result.allocator = allocator;
        result.reserve(cap);

        ARRAY_MemoryMove(result.data, data, sizeof(T) * len);
        result.len = len;
        return result;
    }

    Array<T> tight_copy(ARRAY_Allocator allocator) {
        Array result = {};
        ARRAY_ALLOCATOR_CODE(result.allocator = allocator;)
        result.reserve(len);

        ARRAY_MemoryMove(result.data, data, sizeof(T) * len);
        result.len = len;
        return result;
    }

    T *first() {
        ARRAY_ASSERT(len > 0);
        return data;
    }
    T *last() {
        ARRAY_ASSERT(len > 0);
        return data + len - 1;
    }
    T *front() {
        ARRAY_ASSERT(len > 0);
        return data;
    }
    T *back() {
        ARRAY_ASSERT(len > 0);
        return data + len - 1;
    }
    T *begin() { return data; }
    T *end() { return data + len; }

    // for (auto it = integers.begin(), end = integers.end(); it != end; ++it)
    struct Reverse_Iter {
        T *data;
        Array<T> *arr;

        Reverse_Iter operator++(int) {
            Reverse_Iter ret = *this;
            data -= 1;
            return ret;
        }
        Reverse_Iter &operator++() {
            data -= 1;
            return *this;
        }

        T &operator*() { return data[0]; }
        T *operator->() { return data; }

        friend bool operator==(const Reverse_Iter &a, const Reverse_Iter &b) { return a.data == b.data; };
        friend bool operator!=(const Reverse_Iter &a, const Reverse_Iter &b) { return a.data != b.data; };

        Reverse_Iter begin() { return Reverse_Iter{arr->end() - 1, arr}; }
        Reverse_Iter end() { return Reverse_Iter{arr->begin() - 1, arr}; }
    };

    Reverse_Iter reverse() { return {end() - 1, this}; }

	void try_growing() {
		if (len + 1 > cap) {
			int new_size = cap * 2;
			if (new_size < 16) new_size = 16;

			reserve(new_size);
		}
	}

	void try_growing_to_fit_item_count(int item_count) {
		if (len + item_count > cap) {
			int new_size = (cap + item_count) * 2;
			if (new_size < 16) new_size = 16;
			reserve(new_size);
		}
	}
};

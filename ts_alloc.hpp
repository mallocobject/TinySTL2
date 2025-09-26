#ifndef TS_ALLOC_HPP
#define TS_ALLOC_HPP

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

namespace TS
{
#define THROW_BAD_ALLOC                                                                            \
    std::cout << "out of memory" << std::endl;                                                     \
    throw std::bad_alloc()

template <typename T> inline void construct(T *p)
{
    new (p) T();
}

template <typename T, typename U> inline void construct(T *p, const U &val)
{
    new (p) T(val);
}

template <typename T, typename U> inline void construct(T *p, U &&val)
{
    new (p) T(std::move(val));
}

// template <typename T, typename U> inline void reconstruct(T *p, const U &val)
// {
//     p->~T();
//     new (p) T(val);
// }

// template <typename T, typename U> inline void reconstruct(T *p, U &&val)
// {
//     p->~T();
//     new (p) T(std::forward<U>(val));
// }

// // Specialization for std::initializer_list to avoid lifetime issues
// template <typename T, typename U>
// inline void construct(T *p, std::initializer_list<U> il)
// {
//     new (p) T(il);
// }

// template <typename T, typename... Args> inline void construct(T *p, Args &&...args)
// {
//     std::cerr << "Constructing at " << p << " with " << sizeof...(args) << " arguments\n";
//     new (p) T(std::forward<Args>(args)...);
// }

template <typename T> inline void destroy(T *p)
{
    p->~T();
}

template <int inst> class malloc_alloc_template
{

  public:
    using pointer = void *;
    using size_type = std::size_t;

  public:
    static void *allocate(std::size_t size)
    {
        void *result = malloc(size);
        if (nullptr == result)
        {
            // 暂时如此处理
            THROW_BAD_ALLOC;
        }
        return result;
    }

    static void *reallocate(void *p, std::size_t new_size)
    {
        void *result = realloc(p, new_size);
        if (nullptr == result)
        {
            // 暂时如此处理
            THROW_BAD_ALLOC;
        }
        return result;
    }

    static void deallocate(void *p, size_type)
    {
        free(p);
    }

  protected:
    static pointer oom_allocate(size_type size);
    static pointer oom_reallocate(pointer p, size_type new_size);
    static void (*malloc_alloc_oom_hander)();
};

template <int inst> void (*malloc_alloc_template<inst>::malloc_alloc_oom_hander)() = nullptr;

template <int inst> void *malloc_alloc_template<inst>::oom_allocate(std::size_t size)
{
    void (*my_malloc_hander)() = nullptr;
    void *result;
    while (true)
    {
        my_malloc_hander = malloc_alloc_oom_hander;
        if (nullptr == my_malloc_hander)
        {
            oom_allocate(size);
        }
        (*my_malloc_hander)();
        result = malloc(size);
        if (result)
        {
            return result;
        }
    }
}

template <int inst> void *malloc_alloc_template<inst>::oom_reallocate(void *p, std::size_t new_size)
{
    void (*my_malloc_hander)() = nullptr;
    void *result;
    while (true)
    {
        my_malloc_hander = malloc_alloc_oom_hander;
        if (nullptr == my_malloc_hander)
        {
            oom_reallocate(p, new_size);
        }
        (*my_malloc_hander)();
        result = realloc(p, new_size);
        if (result)
        {
            return result;
        }
    }
}

using malloc_alloc = malloc_alloc_template<0>;

template <typename T, class Alloc> class simple_alloc
{
  public:
    static T *allocate(std::size_t count)
    {
        return 0 == count ? nullptr : (T *)Alloc::allocate(count * sizeof(T));
    }

    static T *allocate()
    {
        return (T *)Alloc::allocate(sizeof(T));
    }

    static T *reallocate(T *p, std::size_t count)
    {
        T *result = nullptr;
        if (0 == count)
        {
            Alloc::deallocate(p);
        }
        else
        {
            result = (T *)Alloc::reallocate(p, count * sizeof(T));
        }
        return result;
    }

    // static void deallocate(T *p)
    // {
    //     Alloc::deallocate(p, sizeof(T));
    // }

    static void deallocate(T *p, std::size_t count = 1)
    {
        if (0 != count)
        {
            Alloc::deallocate(p, count * sizeof(T));
        }
    }
};

enum
{
    ALIGN = 8
};
enum
{
    MAX_BYTES = 128
};
enum
{
    NFREELISTS = 16
};

template <bool threads, int inst> class deafault_alloc_template
{
  public:
    using pointer = void *;
    using size_type = std::size_t;

  public:
    static pointer allocate(size_type size)
    {
        void *result = nullptr;
        if (size > MAX_BYTES)
        {
            result = malloc_alloc::allocate(size);
        }
        else
        {
            Obj *free_space = free_list[free_list_index(size)];
            if (nullptr == free_space)
            {
                result = refiil(round_up(size));
            }
            else
            {
                free_list[free_list_index(size)] = free_space->free_list_link;
                result = free_space;
            }
        }
        return result;
    }

    static void deallocate(pointer p, size_type size)
    {
        if (size > MAX_BYTES)
        {
            malloc_alloc::deallocate(p, size);
        }
        else
        {
            Obj *q = (Obj *)p;
            q->free_list_link = free_list[free_list_index(size)];
            free_list[free_list_index(size)] = q;
        }
    }

    static pointer reallocate(pointer p, size_type old_size, size_type new_size);

  protected:
    static size_type round_up(size_type bytes)
    {
        return (bytes + ALIGN - 1) & (~(ALIGN - 1)); // key function
    }

    union Obj {
        Obj *free_list_link;
        char *client_data;
    };

    static size_type free_list_index(size_type bytes)
    {
        return (bytes + ALIGN - 1) / ALIGN - 1;
    }

    static pointer refiil(size_type size);

    static char *chunk_alloc(size_type size, size_type &count);

  protected:
    static Obj *free_list[]; // 二级指针
    static char *start_free;
    static char *end_free;
    static size_type heap_size;
};

template <bool threads, int inst>
typename deafault_alloc_template<threads, inst>::Obj
    *deafault_alloc_template<threads, inst>::free_list[NFREELISTS];
template <bool threads, int inst>
char *deafault_alloc_template<threads, inst>::start_free = nullptr;
template <bool threads, int inst> char *deafault_alloc_template<threads, inst>::end_free = nullptr;
// 统计从系统申请的空间
template <bool threads, int inst> std::size_t deafault_alloc_template<threads, inst>::heap_size = 0;
// 初始化为0->nullptr

template <bool threads, int inst>
inline bool operator==(const deafault_alloc_template<threads, inst> &,
                       const deafault_alloc_template<threads, inst> &)
{
    return true;
}

template <bool threads, int inst>
void *deafault_alloc_template<threads, inst>::reallocate(pointer p, size_type old_size,
                                                         size_type new_size)
{
    if (old_size > MAX_BYTES && new_size > MAX_BYTES)
    {
        return realloc(p, new_size);
    }
    else if (round_up(old_size) == round_up(new_size))
    {
        return p;
    }
    else
    {
        void *result = allocate(new_size);
        size_type size_copy = old_size > new_size ? new_size : old_size;
        memcpy(result, p, size_copy);
        deallocate(p, old_size);
        return result;
    }
}

template <bool threads, int inst>
void *deafault_alloc_template<threads, inst>::refiil(size_type size)
{
    size_type count = 20; // 默认创建为每种空间二十块区
    char *chunk = chunk_alloc(size, count);
    // chunk_alloc有可能更改count
    if (1 == count)
    {
        return chunk;
    }
    void *result = chunk;
    Obj *cur_obj = (Obj *)(chunk + size);
    free_list[free_list_index(size)] = cur_obj;
    for (size_type i = 2; i < count; i++)
    {
        Obj *next_obj = (Obj *)((char *)cur_obj + size);
        cur_obj->free_list_link = next_obj;
        cur_obj = next_obj;
    }
    cur_obj->free_list_link = nullptr;
    return result;
}

template <bool threads, int inst>
char *deafault_alloc_template<threads, inst>::chunk_alloc(size_type size, size_type &count)
{
    char *result = nullptr;
    size_type bytes_left = end_free - start_free;
    size_type bytes_total = size * count;

    if (bytes_left >= bytes_total)
    {
        result = start_free;
        start_free += bytes_total;
        return result;
    }
    else if (bytes_left >= size)
    {
        count = bytes_left / size;
        result = start_free;
        start_free += size * count;
        return result;
    }
    else
    {
        size_type bytes_to_get = 2 * bytes_total + round_up(heap_size >> 4); // 避免线性增长
        if (bytes_left > 0)
        {
            size_type aligned_size = round_up(bytes_left);
            if (aligned_size <= MAX_BYTES)
            {
                Obj *free_space = free_list[free_list_index(bytes_left)];
                ((Obj *)start_free)->free_list_link = free_space;
                free_list[free_list_index(bytes_left)] = (Obj *)start_free;
            }
        }
        start_free = (char *)malloc(bytes_to_get);
        if (nullptr == start_free)
        {
            for (size_type i = size; i <= MAX_BYTES; i += ALIGN)
            {
                Obj *free_space = free_list[free_list_index(i)];
                if (nullptr != free_space)
                {
                    free_list[free_list_index(i)] = free_space->free_list_link;
                    start_free = (char *)free_space;
                    end_free = start_free + i;
                    return chunk_alloc(size, count);
                }
            }
            end_free = nullptr;
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, count);
    }
}

using alloc = deafault_alloc_template<false, 0>;

} // namespace TS

#endif
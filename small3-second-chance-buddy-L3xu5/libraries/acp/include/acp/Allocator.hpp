#ifndef ACP_ALLOCATOR_HPP
#define ACP_ALLOCATOR_HPP

#include <cstddef>
#include <cstdint>
#include <utility>

#include "acp/Pool.hpp"

class AllocatorWithPool: private PoolAllocator {
public:
    AllocatorWithPool(std::size_t minPower, std::size_t maxPower);

    template <class T, class... Args>
    T *create(Args &&...args) {
        auto *ptr = allocate(sizeof(T));
        return new (ptr) T(std::forward<Args>(args)...);
    }

    template <class T>
    void destroy(void *ptr) {
        static_cast<T *>(ptr)->~T();
        deallocate(ptr);
    }
};

#endif  // ACP_ALLOCATOR_HPP

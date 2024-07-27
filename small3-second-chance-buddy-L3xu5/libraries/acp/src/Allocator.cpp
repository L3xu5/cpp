#include "acp/Allocator.hpp"

AllocatorWithPool::AllocatorWithPool(const std::size_t minPower, const std::size_t maxPower)
    : PoolAllocator(minPower, maxPower) {}

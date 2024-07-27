#include "acp/Pool.hpp"

#include <cstdlib>

std::size_t evalCurPower(std::size_t sz) {
    std::size_t curPower = 0;
    while (sz > 1 << curPower) {
        curPower++;
    }
    return curPower;
}

void PoolAllocator::eraseFromFreeNodes(PoolAllocator::Node* node) {
    freeNodes[node->power].erase(node);
    if (freeNodes[node->power].empty())
        freeNodes.erase(node->power);
}

PoolAllocator::Node::Node(void* addr, PoolAllocator::Node* twin, std::size_t power, bool used)
    : addr(addr), twin(twin), power(power), used(used) {}

PoolAllocator::PoolAllocator(const std::size_t minPower, const std::size_t maxPower)
    : minPower(minPower)
     {
    void* addr = std::malloc(1 << maxPower);
    pool = {{addr, Node(addr, nullptr, maxPower, false)}};
    freeNodes = {{maxPower, {&(pool.begin()->second)}}};
}

PoolAllocator::~PoolAllocator() {
    std::free(const_cast<void*>(pool.begin()->first));
}

void* PoolAllocator::allocate(std::size_t const sz) {
    std::size_t curPower = evalCurPower(sz);
    curPower             = std::max(minPower, curPower);
    auto availableNodes  = freeNodes.lower_bound(curPower);
    if (availableNodes == freeNodes.end())
        throw std::bad_alloc{};
    auto toSplit           = *availableNodes->second.begin();
    void* addr             = toSplit->addr;
    std::size_t startPower = toSplit->power;
    while (toSplit->power > curPower) {
        toSplit->power--;
        void* splitterAddr = static_cast<std::byte*>(addr) + (1 << toSplit->power);
        pool.emplace(splitterAddr, Node(splitterAddr, toSplit, toSplit->power, false));
        Node* splitter = &pool.find(splitterAddr)->second;
        if (!freeNodes.contains(toSplit->power)) {
            freeNodes.insert({toSplit->power, {splitter}});
        } else {
            freeNodes[toSplit->power].insert(splitter);
        }
    }
    toSplit->used = true;
    pool.emplace(addr, *toSplit);
    freeNodes[startPower].erase(toSplit);
    if (freeNodes[startPower].empty())
        freeNodes.erase(startPower);
    return addr;
}

void PoolAllocator::deallocate(void const* ptr) {
    auto node = pool.find(ptr);
    while (true) {
        auto next = std::next(node);
        eraseFromFreeNodes(&node->second);
        if (next != pool.end() && next->second.twin == node->second && !next->second.used &&
            node->second->power == next->second->power) {
            eraseFromFreeNodes(next->second);
            node->second->power++;
            delete next->second;
            pool.erase(next);
        } else if (node != pool.begin()) {
            auto prev = std::prev(node);
            if (node->second->twin == prev->second && !prev->second->used &&
                node->second->power == prev->second->power) {
                eraseFromFreeNodes(prev->second);
                prev->second->power++;
                delete node->second;
                pool.erase(node);
                node = prev;
            } else
                break;
        } else
            break;
    }
    node->second->used = false;
    if (freeNodes.contains(node->second->power))
        freeNodes[node->second->power].insert(node->second);
    else
        freeNodes[node->second->power] = {node->second};
}

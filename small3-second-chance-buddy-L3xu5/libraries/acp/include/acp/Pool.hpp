#ifndef ACP_POOL_HPP
#define ACP_POOL_HPP

#include <cstddef>
#include <map>
#include <new>
#include <unordered_set>

class PoolAllocator {
public:
    PoolAllocator(std::size_t minPower, std::size_t maxPower);

    void* allocate(std::size_t sz);

    void deallocate(const void* ptr);
    ~PoolAllocator();
    class Node {
    public:
        Node(void* addr, Node* twin, std::size_t power, bool used);
        void* addr;
        Node* twin;
        std::size_t power;
        bool used;
    };

private:
    std::size_t minPower;
    std::map<std::size_t, std::unordered_set<Node*>> freeNodes;
    std::map<void const*, Node> pool;
    void eraseFromFreeNodes(Node* node);
};
#endif  // ACP_POOL_HPP

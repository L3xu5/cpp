#ifndef ACP_CACHE_HPP
#define ACP_CACHE_HPP

#include <cstddef>
#include <deque>
#include <new>
#include <ostream>

template <class Key, class KeyProvider, class Allocator>
class Cache {
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&...alloc_args)
        : m_max_size(cache_size), m_alloc(std::forward<AllocArgs>(alloc_args)...) {}

    std::size_t size() const { return queue.size(); }

    bool empty() const { return queue.empty(); }

    template <class T>
    T &get(const Key &key);

    std::ostream &print(std::ostream &strm) const;

    friend std::ostream &operator<<(std::ostream &strm, const Cache &cache) { return cache.print(strm); }

    ~Cache() {
        for (auto &elem : queue) {
            m_alloc.template destroy<KeyProvider>(elem.first);
        }
    }

private:
    const std::size_t m_max_size;
    Allocator m_alloc;
    std::deque<std::pair<KeyProvider *, bool>> queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T &Cache<Key, KeyProvider, Allocator>::get(const Key &key) {
    for (auto &[elem, used] : queue) {
        if (*elem == key) {
            used = true;
            return static_cast<T &>(*elem);
        }
    }
    if (queue.size() < m_max_size) {
        queue.emplace_front(m_alloc.template create<T>(key), false);
        return static_cast<T &>(*queue.front().first);
    }
    std::pair<KeyProvider *, bool> &back = queue.back();
    while (back.second) {
        queue.pop_back();
        queue.emplace_front(back.first, false);
        back = queue.back();
    }
    queue.pop_back();
    m_alloc.template destroy<KeyProvider>(back.first);
    queue.emplace_front(m_alloc.template create<T>(key), false);
    return static_cast<T &>(*queue.front().first);
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream &Cache<Key, KeyProvider, Allocator>::print(std::ostream &strm) const {
    return strm << "<empty>\n";
}

#endif  // ACP_CACHE_HPP

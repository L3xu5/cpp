#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

class Scapegoat {
public:
    [[nodiscard]] bool contains(int value) const noexcept;
    bool insert(int value);
    bool remove(int value);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] std::vector<int> values() const noexcept;

    ~Scapegoat();

private:
    struct Node;
    Node *root = nullptr;
    Node *find(Node *node, int value) const;
    void values(Node *node, std::vector<Node *> &vector) const;
    float a = 1.5;
    Node *rebuild(Node *node);
    Node *build(std::vector<Node *>::iterator begin, int l, int r, Node *parent);
    Node *insert(Node *node, int value, Node *parent);
    Node *findScapegoat(Node *node) const;
    void rebuildIfNeeded(Node *node);
    void updateSizes(Node *node, bool mode) const;
};

#endif

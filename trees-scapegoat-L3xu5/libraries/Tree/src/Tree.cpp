#include "tree/Tree.hpp"

#include "algorithm"

struct Scapegoat::Node {
    int key;
    Node *left;
    Node *right;
    Node *parent;
    int size;
    Node(int key, Node *left = nullptr, Node *right = nullptr, Node *parent = nullptr, int size = 1)
        : key(key), left(left), right(right), parent(parent), size(size) {}

    ~Node() {
        delete left;
        delete right;
    }
};

bool Scapegoat::contains(int value) const noexcept {
    return Scapegoat::find(this->root, value) != nullptr;
}

void Scapegoat::updateSizes(Node *node, bool mode) const {
    while (node != nullptr) {
        if (mode)
            node->size++;
        else
            node->size--;
        node = node->parent;
    }
}

Scapegoat::Node *Scapegoat::find(Node *node, int value) const {
    if (node == nullptr || node->key == value)
        return node;
    if (node->left != nullptr && value < node->key)
        return find(node->left, value);
    else if (node->right != nullptr && value > node->key)
        return find(node->right, value);
    return nullptr;
}

bool Scapegoat::insert(int value) {
    if (this->root == nullptr) {
        this->root = new Node(value);
    } else {
        Node *node = insert(this->root, value, nullptr);
        if (node == nullptr)
            return false;
        updateSizes(node->parent, true);
        rebuildIfNeeded(node);
    }
    return true;
}

void Scapegoat::rebuildIfNeeded(Node *node) {
    Node *scapegoat = findScapegoat(node);
    if (scapegoat == nullptr)
        return;
    Node *scapegoatFather = scapegoat->parent;
    Node *newNode         = rebuild(scapegoat);
    if (scapegoatFather == nullptr)
        this->root = newNode;
    else if (scapegoatFather->key > scapegoat->key)
        scapegoatFather->left = newNode;
    else
        scapegoatFather->right = newNode;
    if (newNode != nullptr)
        newNode->parent = scapegoatFather;
}

Scapegoat::Node *Scapegoat::insert(Node *node, int value, Node *parent) {
    if (node == nullptr) {
        Node *newNode = new Node(value, nullptr, nullptr, parent);
        if (parent != nullptr) {
            if (parent->key > newNode->key)
                parent->left = newNode;
            else
                parent->right = newNode;
        }
        return newNode;
    }
    if (node->key == value)
        return nullptr;
    if (node->key > value)
        return insert(node->left, value, node);
    else
        return insert(node->right, value, node);
}

Scapegoat::Node *Scapegoat::findScapegoat(Node *node) const {
    if (node == nullptr)
        return nullptr;
    int leftSize  = node->left != nullptr ? node->left->size : 0;
    int rightSize = node->right != nullptr ? node->right->size : 0;
    if (leftSize + rightSize < this->a * leftSize || leftSize + rightSize < this->a * rightSize)
        return node;
    return findScapegoat(node->parent);
}

bool Scapegoat::remove(int value) {
    Node *node = find(this->root, value);
    if (node == nullptr)
        return false;
    if (node == this->root) {
        if (node->right == nullptr && node->left == nullptr) {
            this->root = nullptr;
            delete node;
        } else if (node->right == nullptr) {
            this->root         = node->left;
            this->root->parent = nullptr;
            delete node;
            rebuildIfNeeded(this->root);
        } else if (node->left == nullptr) {
            this->root         = node->right;
            this->root->parent = nullptr;
            delete node;
            rebuildIfNeeded(this->root);
        } else {
            Node *leftmost = node->right;
            while (leftmost->left != nullptr)
                leftmost = leftmost->left;
            this->root->key = leftmost->key;
            if (leftmost == node->right)
                leftmost->parent->right = leftmost->right;
            else
                leftmost->parent->left = leftmost->right;
            if (leftmost->right != nullptr)
                leftmost->right->parent = leftmost->parent;
            updateSizes(leftmost->parent, false);
            rebuildIfNeeded(leftmost->parent);
            delete leftmost;
        }
    } else {
        if (node->right == nullptr && node->left == nullptr) {
            if (node->key < node->parent->key)
                node->parent->left = nullptr;
            else
                node->parent->right = nullptr;
            updateSizes(node->parent, false);
            rebuildIfNeeded(node->parent);
            node->left = nullptr;
            delete node;
        } else if (node->right == nullptr) {
            if (node->key < node->parent->key)
                node->parent->left = node->left;
            else
                node->parent->right = node->left;
            node->left->parent = node->parent;
            updateSizes(node->parent, false);
            rebuildIfNeeded(node->parent);
            node->left = nullptr;
            delete node;
        } else if (node->left == nullptr) {
            if (node->key < node->parent->key)
                node->parent->left = node->right;
            else
                node->parent->right = node->right;
            node->right->parent = node->parent;
            updateSizes(node->parent, false);
            rebuildIfNeeded(node->parent);
            node->right = nullptr;
            delete node;
        } else {
            Node *leftmost = node->right;
            while (leftmost->left != nullptr)
                leftmost = leftmost->left;
            node->key = leftmost->key;
            if (leftmost == node->right)
                leftmost->parent->right = leftmost->right;
            else
                leftmost->parent->left = leftmost->right;
            if (leftmost->right != nullptr) {
                leftmost->right->parent = leftmost->parent;
                leftmost->right         = nullptr;
            }
            updateSizes(leftmost->parent, false);
            rebuildIfNeeded(leftmost->parent);
            delete leftmost;
        }
    }
    return true;
}

std::size_t Scapegoat::size() const noexcept {
    return this->root->size;
}

bool Scapegoat::empty() const noexcept {
    return this->root == nullptr;
}

std::vector<int> Scapegoat::values() const noexcept {
    std::vector<Scapegoat::Node *> nodes{};
    nodes.reserve(root->size);
    Scapegoat::values(root, nodes);
    std::vector<int> values(nodes.size());
    std::transform(nodes.begin(), nodes.end(), values.begin(), [](Node *node) { return node->key; });
    return values;
}

void Scapegoat::values(Node *node, std::vector<Node *> &vector) const {
    if (node == nullptr) {
        return;
    }
    values(node->left, vector);
    vector.push_back(node);
    values(node->right, vector);
}

Scapegoat::Node *Scapegoat::rebuild(Node *node) {
    std::vector<Node *> nodes = {};
    nodes.reserve(root->size);
    Scapegoat::values(node, nodes);
    return build(nodes.begin(), 0, nodes.size(), nullptr);
}

Scapegoat::Node *Scapegoat::build(std::vector<Node *>::iterator begin, int l, int r, Node *parent) {
    if (l >= r)
        return nullptr;
    int m        = l + (r - l) / 2;
    Node *node   = *(begin + m);
    node->parent = parent;
    node->right  = build(begin, m + 1, r, node);
    node->left   = build(begin, l, m, node);
    node->size = (node->right != nullptr ? node->right->size : 0) + (node->left != nullptr ? node->left->size : 0) + 1;
    return node;
}

Scapegoat::~Scapegoat() {
    delete root;
}

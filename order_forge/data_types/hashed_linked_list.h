#pragma once

#include "types.h"

#include "pool_allocator.h"

namespace of {
template<typename Id, typename T>
class Node {
    using NodeIdentifier = Id;
    using NodeType = T;

public:
    Node(NodeIdentifier& id, NodeType& item) : id(id), item(item) {}

    NodeIdentifier id;
    NodeType item;
    Node* left{nullptr};
    Node* right{nullptr};

    void* operator new(const size_t size) {
        return SingleTonWrapper<PoolAllocator<Node>>::get_instance().allocate(size);
    }

    void operator delete(void* ptr, size_t size) {
        return SingleTonWrapper<PoolAllocator<Node>>::get_instance().deallocate(static_cast<Node*>(ptr), size);
    }
};

template<typename Id, typename T>
class HashLinkedList {
public:
    using Item = T;
    using ItemRef = T&;
    using ItemPtr = T*;
    using NodeType = Node<Id, T>;
    using NodeTypePtr = NodeType*;
    using NodeTypeRef = NodeType&;
    using NodeMap = tsl::robin_map<Id, NodeTypePtr>;

    void pop() { pop_front(); }
    size_t size() const { return node_map_.size(); }
    bool empty() const { return node_map_.empty(); }
    ItemRef front() { return front_->item; }
    ItemRef back() { return back_->item; }

    struct Iterator {
        Iterator(NodeTypePtr ptr) : ptr(ptr) {};
        NodeTypeRef operator++() { return *(ptr = ptr->right); }
        bool operator==(const Iterator& itr) const { return itr.ptr == ptr; }
        NodeTypeRef operator*() { return *ptr; }
        NodeTypePtr ptr;
    };

    Iterator begin() { return {front_}; }

    Iterator end() { return {nullptr}; }
    bool contains(Id& id) { return node_map_.contains(id); }

    ItemPtr find(Id& id) {
        if (auto it = node_map_.find(id); it != node_map_.end()) {
            return &it->second->item;
        }
        return nullptr;
    }

    void push(Id& id, Item& node_item) {
        // copy
        auto* node_ptr = new NodeType(id, node_item);
        if (node_map_.empty()) {
            front_ = node_ptr;
        } else {
            back_->right = node_ptr;
            node_ptr->left = back_;
        }
        node_map_.emplace(id, node_ptr);
        back_ = node_ptr;
    };

    void remove_node(NodeTypePtr node_ptr) {
        if (node_ptr == front_) {
            pop_front();
        } else if (node_ptr == back_) {
            pop_back();
        } else {
            pop_middle(node_ptr);
        }
    }

    void remove(Id& id) {
        if (auto it = node_map_.find(id); it != node_map_.end()) {
            remove_node(it->second);
        }
    }

    HashLinkedList() = default;
    HashLinkedList(HashLinkedList&) = delete;
    HashLinkedList& operator=(HashLinkedList&) = delete;
    HashLinkedList(HashLinkedList&&) = delete;
    HashLinkedList& operator=(HashLinkedList&&) = delete;

    ~HashLinkedList() {
        while (front_) {
            pop_front();
        }
    }

private:
    NodeTypePtr front_{nullptr};
    NodeTypePtr back_{nullptr};
    NodeMap node_map_;

    void pop_front() {
        if (!front_)
            return;

        node_map_.erase(front_->id);
        auto* temp = front_;
        front_ = front_->right;
        if (front_) {
            front_->left = nullptr;
        } else {
            back_ = nullptr;
        }
        delete temp;
    }

    void pop_back() {
        if (!back_)
            return;

        node_map_.erase(back_->id);
        auto* temp = back_;
        back_ = back_->left;
        if (back_) {
            back_->right = nullptr;
        } else {
            front_ = nullptr;
        }
        delete temp;
    }

    void pop_middle(NodeTypePtr node_ptr) {
        node_map_.erase(node_ptr->id);
        node_ptr->left->right = node_ptr->right;
        node_ptr->right->left = node_ptr->left;
        delete node_ptr;
    }
};
} // namespace of

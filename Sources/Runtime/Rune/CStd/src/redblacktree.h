#pragma once

#include <rune/allocator.h>
#include <rune/memory.h>
#include <rune/sortedmap.h>
#include <rune/sortedset.h>
#include <rune/types.h>

#include <string.h>

typedef enum
{
    RBT_COLOR_BLACK = 0,
    RBT_COLOR_RED = 1,
} RbtColor;

typedef struct RbtNodeHeader
{
    struct RbtNodeHeader* parent;
    struct RbtNodeHeader* left;
    struct RbtNodeHeader* right;
    RbtColor color;
} RbtNodeHeader;

typedef struct
{
    RuneSortedSet* set;
    uint64 node_size;
} SetTree;

typedef struct
{
    RuneSortedMap* map;
    uint64 node_size;
} MapTree;

static inline int redblacktree_compare_values(void const* a, void const* b, RuneCompareFn compare_fn, usize size)
{
    if (compare_fn != nullptr)
    {
        return compare_fn(a, b);
    }

    int8_t const result = rune_memory_compare(a, b, size);
    return result < 0 ? -1 : result > 0 ? 1
                                        : 0;
}

static inline bool redblacktree_allocator_ready(RuneAllocator const* allocator)
{
    return rune_allocator_is_valid(allocator);
}

static inline void redblacktree_ensure_allocator(RuneAllocator const** allocator)
{
    if (!redblacktree_allocator_ready(*allocator))
    {
        *allocator = rune_get_default_allocator();
    }
}

static inline uint64 redblacktree_element_offset(void)
{
    return rune_align_up(sizeof(RbtNodeHeader), RUNE_MAX_ALIGN);
}

static inline usize redblacktree_set_node_size(usize elementSize)
{
    return redblacktree_element_offset() + elementSize;
}

static inline uint64 redblacktree_map_node_size(usize keySize, usize valueSize)
{
    uint64 const keyOffset = redblacktree_element_offset();
    uint64 const valueOffset = rune_align_up(keyOffset + keySize, RUNE_MAX_ALIGN);
    return valueOffset + valueSize;
}

static inline uint8* redblacktree_node_value(RbtNodeHeader* node)
{
    return (uint8*)(node) + redblacktree_element_offset();
}

static inline uint8 const* redblacktree_node_value_const(RbtNodeHeader const* node)
{
    return (uint8 const*)(node) + redblacktree_element_offset();
}

static inline uint8* redblacktree_map_key(RbtNodeHeader* node)
{
    return redblacktree_node_value(node);
}

static inline uint8 const* redblacktree_map_key_const(RbtNodeHeader const* node)
{
    return redblacktree_node_value_const(node);
}

static inline uint8* redblacktree_map_value(RbtNodeHeader* node, usize keySize)
{
    uint64 const keyOffset = redblacktree_element_offset();
    return (uint8*)(node) + rune_align_up(keyOffset + keySize, RUNE_MAX_ALIGN);
}

static inline uint8 const* redblacktree_map_value_const(RbtNodeHeader const* node, usize keySize)
{
    uint64 const keyOffset = redblacktree_element_offset();
    return (uint8 const*)(node) + rune_align_up(keyOffset + keySize, RUNE_MAX_ALIGN);
}

static inline RbtNodeHeader* redblacktree_minimum_node(RbtNodeHeader* node)
{
    while (node != nullptr && node->left != nullptr)
    {
        node = node->left;
    }

    return node;
}

static inline RbtNodeHeader* redblacktree_maximum_node(RbtNodeHeader* node)
{
    while (node != nullptr && node->right != nullptr)
    {
        node = node->right;
    }

    return node;
}

static inline RbtNodeHeader* redblacktree_successor_node(RbtNodeHeader* node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->right != nullptr)
    {
        return redblacktree_minimum_node(node->right);
    }

    RbtNodeHeader* parent = node->parent;
    while (parent != nullptr && node == parent->right)
    {
        node = parent;
        parent = parent->parent;
    }

    return parent;
}

static inline void redblacktree_rotate_left(RbtNodeHeader** root, RbtNodeHeader* node)
{
    RbtNodeHeader* right = node->right;
    node->right = right->left;
    if (right->left != nullptr)
    {
        right->left->parent = node;
    }

    right->parent = node->parent;
    if (node->parent == nullptr)
    {
        *root = right;
    }
    else if (node == node->parent->left)
    {
        node->parent->left = right;
    }
    else
    {
        node->parent->right = right;
    }

    right->left = node;
    node->parent = right;
}

static inline void redblacktree_rotate_right(RbtNodeHeader** root, RbtNodeHeader* node)
{
    RbtNodeHeader* left = node->left;
    node->left = left->right;
    if (left->right != nullptr)
    {
        left->right->parent = node;
    }

    left->parent = node->parent;
    if (node->parent == nullptr)
    {
        *root = left;
    }
    else if (node == node->parent->right)
    {
        node->parent->right = left;
    }
    else
    {
        node->parent->left = left;
    }

    left->right = node;
    node->parent = left;
}

static inline void redblacktree_insert_fixup(RbtNodeHeader** root, RbtNodeHeader* node)
{
    while (node->parent != nullptr && node->parent->color == RBT_COLOR_RED)
    {
        RbtNodeHeader* parent = node->parent;
        RbtNodeHeader* grandparent = parent->parent;
        if (grandparent == nullptr)
        {
            break;
        }

        if (parent == grandparent->left)
        {
            RbtNodeHeader* uncle = grandparent->right;

            if (uncle != nullptr && uncle->color == RBT_COLOR_RED)
            {
                parent->color = RBT_COLOR_BLACK;
                uncle->color = RBT_COLOR_BLACK;
                grandparent->color = RBT_COLOR_RED;
                node = grandparent;
                continue;
            }

            if (node == parent->right)
            {
                node = parent;
                redblacktree_rotate_left(root, node);
                parent = node->parent;
            }

            parent->color = RBT_COLOR_BLACK;
            grandparent->color = RBT_COLOR_RED;
            redblacktree_rotate_right(root, grandparent);
        }
        else
        {
            RbtNodeHeader* uncle = grandparent->left;

            if (uncle != nullptr && uncle->color == RBT_COLOR_RED)
            {
                parent->color = RBT_COLOR_BLACK;
                uncle->color = RBT_COLOR_BLACK;
                grandparent->color = RBT_COLOR_RED;
                node = grandparent;
                continue;
            }

            if (node == parent->left)
            {
                node = parent;
                redblacktree_rotate_right(root, node);
                parent = node->parent;
            }

            parent->color = RBT_COLOR_BLACK;
            grandparent->color = RBT_COLOR_RED;
            redblacktree_rotate_left(root, grandparent);
        }
    }

    (*root)->color = RBT_COLOR_BLACK;
}

static inline void redblacktree_transplant(RbtNodeHeader** root, RbtNodeHeader* oldNode, RbtNodeHeader* newNode)
{
    if (oldNode->parent == nullptr)
    {
        *root = newNode;
    }
    else if (oldNode == oldNode->parent->left)
    {
        oldNode->parent->left = newNode;
    }
    else
    {
        oldNode->parent->right = newNode;
    }

    if (newNode != nullptr)
    {
        newNode->parent = oldNode->parent;
    }
}

static inline void redblacktree_delete_fixup(RbtNodeHeader** root, RbtNodeHeader* node, RbtNodeHeader* parent)
{
    while ((node == nullptr || node->color == RBT_COLOR_BLACK) && node != *root)
    {
        if (parent == nullptr)
        {
            break;
        }

        if (node == parent->left)
        {
            RbtNodeHeader* sibling = parent->right;

            if (sibling != nullptr && sibling->color == RBT_COLOR_RED)
            {
                sibling->color = RBT_COLOR_BLACK;
                parent->color = RBT_COLOR_RED;
                redblacktree_rotate_left(root, parent);
                sibling = parent->right;
            }

            if (sibling == nullptr ||
                ((sibling->left == nullptr || sibling->left->color == RBT_COLOR_BLACK) &&
                 (sibling->right == nullptr || sibling->right->color == RBT_COLOR_BLACK)))
            {
                if (sibling != nullptr)
                {
                    sibling->color = RBT_COLOR_RED;
                }

                node = parent;
                parent = node->parent;
                continue;
            }

            if (sibling->right == nullptr || sibling->right->color == RBT_COLOR_BLACK)
            {
                if (sibling->left != nullptr)
                {
                    sibling->left->color = RBT_COLOR_BLACK;
                }

                sibling->color = RBT_COLOR_RED;
                redblacktree_rotate_right(root, sibling);
                sibling = parent->right;
            }

            if (sibling != nullptr)
            {
                sibling->color = parent->color;
            }

            parent->color = RBT_COLOR_BLACK;

            if (sibling != nullptr && sibling->right != nullptr)
            {
                sibling->right->color = RBT_COLOR_BLACK;
            }

            redblacktree_rotate_left(root, parent);
            node = *root;
            break;
        }
        else
        {
            RbtNodeHeader* sibling = parent->left;

            if (sibling != nullptr && sibling->color == RBT_COLOR_RED)
            {
                sibling->color = RBT_COLOR_BLACK;
                parent->color = RBT_COLOR_RED;
                redblacktree_rotate_right(root, parent);
                sibling = parent->left;
            }

            if (sibling == nullptr ||
                ((sibling->right == nullptr || sibling->right->color == RBT_COLOR_BLACK) &&
                 (sibling->left == nullptr || sibling->left->color == RBT_COLOR_BLACK)))
            {
                if (sibling != nullptr)
                {
                    sibling->color = RBT_COLOR_RED;
                }

                node = parent;
                parent = node->parent;
                continue;
            }

            if (sibling->left == nullptr || sibling->left->color == RBT_COLOR_BLACK)
            {
                if (sibling->right != nullptr)
                {
                    sibling->right->color = RBT_COLOR_BLACK;
                }

                sibling->color = RBT_COLOR_RED;
                redblacktree_rotate_left(root, sibling);
                sibling = parent->left;
            }

            if (sibling != nullptr)
            {
                sibling->color = parent->color;
            }

            parent->color = RBT_COLOR_BLACK;

            if (sibling != nullptr && sibling->left != nullptr)
            {
                sibling->left->color = RBT_COLOR_BLACK;
            }

            redblacktree_rotate_right(root, parent);
            node = *root;
            break;
        }
    }

    if (node != nullptr)
    {
        node->color = RBT_COLOR_BLACK;
    }
}

static inline RbtNodeHeader* redblacktree_allocate_node(RuneAllocator const* allocator, uint64 nodeSize)
{
    void* memory = rune_allocator_allocate(
        allocator,
        (usize)(nodeSize),
        RUNE_MAX_ALIGN);

    if (memory == nullptr)
    {
        return nullptr;
    }

    memset(memory, 0, nodeSize);
    RbtNodeHeader* node = (RbtNodeHeader*)(memory);
    node->color = RBT_COLOR_RED;
    return node;
}

static inline void redblacktree_free_node(RuneAllocator const* allocator, RbtNodeHeader* node)
{
    if (node != nullptr && redblacktree_allocator_ready(allocator))
    {
        rune_allocator_deallocate(allocator, node);
    }
}

static inline void redblacktree_clear_node_subtree(RuneAllocator const* allocator, RbtNodeHeader* node)
{
    if (node == nullptr)
    {
        return;
    }

    redblacktree_clear_node_subtree(allocator, node->left);
    redblacktree_clear_node_subtree(allocator, node->right);
    redblacktree_free_node(allocator, node);
}

// MARK: - SetTree functions

static inline SetTree set_tree_init(RuneSortedSet* self)
{
    SetTree t = {0};
    t.set = self;
    if (self != nullptr && self->element_size > 0)
    {
        t.node_size = redblacktree_set_node_size(self->element_size);
    }
    return t;
}

static inline RbtNodeHeader** set_tree_root_ptr(SetTree* t)
{
    return (RbtNodeHeader**)(&t->set->root);
}

static inline RbtNodeHeader* set_tree_root(SetTree const* t)
{
    return (RbtNodeHeader*)(t->set->root);
}

static inline RbtNodeHeader* set_tree_find_node(SetTree const* t, void const* element)
{
    RbtNodeHeader* node = set_tree_root(t);
    while (node != nullptr)
    {
        int const order = redblacktree_compare_values(
            redblacktree_node_value(node), element, t->set->compare_fn, t->set->element_size);

        if (order == 0)
        {
            return node;
        }

        node = order < 0 ? node->right : node->left;
    }

    return nullptr;
}

static inline void set_tree_insert_bytes(SetTree* t, void const* element)
{
    RuneSortedSet* set = t->set;
    if (set == nullptr || element == nullptr || t->node_size == 0)
    {
        return;
    }

    redblacktree_ensure_allocator(&set->allocator);

    RbtNodeHeader* existing = set_tree_find_node(t, element);
    if (existing != nullptr)
    {
        rune_memory_copy(element, redblacktree_node_value(existing), set->element_size);
        return;
    }

    RbtNodeHeader* node = redblacktree_allocate_node(set->allocator, t->node_size);
    if (node == nullptr)
    {
        return;
    }

    rune_memory_copy(element, redblacktree_node_value(node), set->element_size);

    RbtNodeHeader* parent = nullptr;
    RbtNodeHeader* current = set_tree_root(t);
    while (current != nullptr)
    {
        parent = current;
        int const order = redblacktree_compare_values(
            redblacktree_node_value(current), element, set->compare_fn, set->element_size);
        current = order < 0 ? current->right : current->left;
    }

    node->parent = parent;
    if (parent == nullptr)
    {
        *set_tree_root_ptr(t) = node;
    }
    else if (redblacktree_compare_values(redblacktree_node_value(parent), element, set->compare_fn, set->element_size) > 0)
    {
        parent->left = node;
    }
    else
    {
        parent->right = node;
    }

    redblacktree_insert_fixup(set_tree_root_ptr(t), node);
    ++set->length;
}

static inline void set_tree_remove_bytes(SetTree* t, void const* element)
{
    RuneSortedSet* set = t->set;
    if (set == nullptr || element == nullptr)
    {
        return;
    }

    RbtNodeHeader* node = set_tree_find_node(t, element);
    if (node == nullptr)
    {
        return;
    }

    RbtNodeHeader* fixNode = nullptr;
    RbtNodeHeader* fixParent = node->parent;
    RbtColor removedColor = node->color;

    if (node->left == nullptr)
    {
        fixNode = node->right;
        redblacktree_transplant(set_tree_root_ptr(t), node, node->right);
    }
    else if (node->right == nullptr)
    {
        fixNode = node->left;
        redblacktree_transplant(set_tree_root_ptr(t), node, node->left);
    }
    else
    {
        RbtNodeHeader* successor = redblacktree_minimum_node(node->right);
        removedColor = successor->color;
        fixNode = successor->right;
        fixParent = successor->parent;

        if (successor->parent == node)
        {
            if (fixNode != nullptr)
            {
                fixNode->parent = successor;
            }

            fixParent = successor;
        }
        else
        {
            redblacktree_transplant(set_tree_root_ptr(t), successor, successor->right);
            successor->right = node->right;
            if (successor->right != nullptr)
            {
                successor->right->parent = successor;
            }
        }

        redblacktree_transplant(set_tree_root_ptr(t), node, successor);
        successor->left = node->left;

        if (successor->left != nullptr)
        {
            successor->left->parent = successor;
        }

        successor->color = node->color;
    }

    redblacktree_free_node(set->allocator, node);
    --set->length;

    if (removedColor == RBT_COLOR_BLACK)
    {
        redblacktree_delete_fixup(set_tree_root_ptr(t), fixNode, fixParent);
    }
}

static inline void set_tree_clear_nodes(SetTree* t)
{
    RuneSortedSet* set = t->set;
    if (set == nullptr || set_tree_root(t) == nullptr)
    {
        return;
    }

    redblacktree_clear_node_subtree(set->allocator, set_tree_root(t));
    *set_tree_root_ptr(t) = nullptr;
    set->length = 0;
}

static inline void set_tree_copy_from(SetTree* dst, SetTree const* src)
{
    if (dst->set == nullptr || src->set == nullptr)
    {
        return;
    }

    RbtNodeHeader* node = redblacktree_minimum_node(set_tree_root((SetTree*)(src)));
    while (node != nullptr)
    {
        set_tree_insert_bytes(dst, redblacktree_node_value(node));
        node = redblacktree_successor_node(node);
    }
}

// MARK: - MapTree functions

static inline MapTree map_tree_init(RuneSortedMap* self)
{
    MapTree t = {0};
    t.map = self;
    if (self != nullptr && self->key_size > 0 && self->value_size > 0)
    {
        t.node_size = redblacktree_map_node_size(self->key_size, self->value_size);
    }
    return t;
}

static inline RbtNodeHeader** map_tree_root_ptr(MapTree* t)
{
    return (RbtNodeHeader**)(&t->map->root);
}

static inline RbtNodeHeader* map_tree_root(MapTree const* t)
{
    return (RbtNodeHeader*)(t->map->root);
}

static inline RbtNodeHeader* map_tree_find_node(MapTree const* t, void const* key)
{
    RbtNodeHeader* node = map_tree_root(t);
    while (node != nullptr)
    {
        int const order = redblacktree_compare_values(
            redblacktree_map_key(node), key, t->map->compare_fn, t->map->key_size);

        if (order == 0)
        {
            return node;
        }

        node = order < 0 ? node->right : node->left;
    }

    return nullptr;
}

static inline void map_tree_insert_entry(MapTree* t, void const* key, void const* value)
{
    RuneSortedMap* map = t->map;
    if (map == nullptr || key == nullptr || value == nullptr || t->node_size == 0)
    {
        return;
    }

    redblacktree_ensure_allocator(&map->allocator);

    RbtNodeHeader* existing = map_tree_find_node(t, key);
    if (existing != nullptr)
    {
        rune_memory_copy(value, redblacktree_map_value(existing, map->key_size), map->value_size);
        return;
    }

    RbtNodeHeader* node = redblacktree_allocate_node(map->allocator, t->node_size);
    if (node == nullptr)
    {
        return;
    }

    rune_memory_copy(key, redblacktree_map_key(node), map->key_size);
    rune_memory_copy(value, redblacktree_map_value(node, map->key_size), map->value_size);

    RbtNodeHeader* parent = nullptr;
    RbtNodeHeader* current = map_tree_root(t);
    while (current != nullptr)
    {
        parent = current;
        int const order = redblacktree_compare_values(redblacktree_map_key(current), key, map->compare_fn, map->key_size);
        current = order < 0 ? current->right : current->left;
    }

    node->parent = parent;
    if (parent == nullptr)
    {
        *map_tree_root_ptr(t) = node;
    }
    else if (redblacktree_compare_values(redblacktree_map_key(parent), key, map->compare_fn, map->key_size) > 0)
    {
        parent->left = node;
    }
    else
    {
        parent->right = node;
    }

    redblacktree_insert_fixup(map_tree_root_ptr(t), node);
    ++map->length;
}

static inline void map_tree_remove_key(MapTree* t, void const* key)
{
    RuneSortedMap* map = t->map;
    if (map == nullptr || key == nullptr)
    {
        return;
    }

    RbtNodeHeader* node = map_tree_find_node(t, key);
    if (node == nullptr)
    {
        return;
    }

    RbtNodeHeader* fixNode = nullptr;
    RbtNodeHeader* fixParent = node->parent;
    RbtColor removedColor = node->color;

    if (node->left == nullptr)
    {
        fixNode = node->right;
        redblacktree_transplant(map_tree_root_ptr(t), node, node->right);
    }
    else if (node->right == nullptr)
    {
        fixNode = node->left;
        redblacktree_transplant(map_tree_root_ptr(t), node, node->left);
    }
    else
    {
        RbtNodeHeader* successor = redblacktree_minimum_node(node->right);
        removedColor = successor->color;
        fixNode = successor->right;
        fixParent = successor->parent;

        if (successor->parent == node)
        {
            if (fixNode != nullptr)
            {
                fixNode->parent = successor;
            }

            fixParent = successor;
        }
        else
        {
            redblacktree_transplant(map_tree_root_ptr(t), successor, successor->right);
            successor->right = node->right;
            if (successor->right != nullptr)
            {
                successor->right->parent = successor;
            }
        }

        redblacktree_transplant(map_tree_root_ptr(t), node, successor);
        successor->left = node->left;

        if (successor->left != nullptr)
        {
            successor->left->parent = successor;
        }

        successor->color = node->color;
    }

    redblacktree_free_node(map->allocator, node);
    --map->length;

    if (removedColor == RBT_COLOR_BLACK)
    {
        redblacktree_delete_fixup(map_tree_root_ptr(t), fixNode, fixParent);
    }
}

static inline void map_tree_clear_nodes(MapTree* t)
{
    RuneSortedMap* map = t->map;
    if (map == nullptr || map_tree_root(t) == nullptr)
    {
        return;
    }

    redblacktree_clear_node_subtree(map->allocator, map_tree_root(t));
    *map_tree_root_ptr(t) = nullptr;
    map->length = 0;
}

static inline void map_tree_copy_from(MapTree* dst, MapTree const* src)
{
    if (dst->map == nullptr || src->map == nullptr)
    {
        return;
    }

    RbtNodeHeader* node = redblacktree_minimum_node(map_tree_root((MapTree*)(src)));
    while (node != nullptr)
    {
        map_tree_insert_entry(dst, redblacktree_map_key(node), redblacktree_map_value(node, src->map->key_size));
        node = redblacktree_successor_node(node);
    }
}

// MARK: - Clear helpers

static inline void redblacktree_clear_set(RuneSortedSet* set)
{
    if (set == nullptr)
    {
        return;
    }

    if (set->root != nullptr && set->element_size > 0)
    {
        SetTree tree = set_tree_init(set);
        set_tree_clear_nodes(&tree);
    }

    set->root = nullptr;
    set->length = 0;
    set->element_size = 0;
}

static inline void redblacktree_clear_map(RuneSortedMap* map)
{
    if (map == nullptr)
    {
        return;
    }

    if (map->root != nullptr && map->key_size > 0 && map->value_size > 0)
    {
        MapTree tree = map_tree_init(map);
        map_tree_clear_nodes(&tree);
    }

    map->root = nullptr;
    map->length = 0;
    map->key_size = 0;
    map->value_size = 0;
}
